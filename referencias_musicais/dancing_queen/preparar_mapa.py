"""Ferramenta offline de autoria; nao participa do jogo em C.

Le o MIDI sem bibliotecas adicionais. NumPy e FFmpeg sao usados somente
para analisar a gravacao durante a preparacao do mapa.
"""
import collections
import json
import pathlib
import struct
import subprocess
import hashlib
import numpy as np

PASTA = pathlib.Path(__file__).resolve().parent
RAIZ = PASTA.parent.parent


def ler_midi(caminho):
    dados = caminho.read_bytes()
    assert dados[:4] == b'MThd'
    formato, trilhas, resolucao = struct.unpack('>HHH', dados[8:14])
    assert formato == 0 and trilhas == 1 and resolucao < 32768
    posicao = 8 + int.from_bytes(dados[4:8], 'big')
    assert dados[posicao:posicao+4] == b'MTrk'
    posicao += 8
    tempo_tick = 0
    segundos = 0.0
    microssegundos = 500000
    status_anterior = None
    programas = {}
    abertas = collections.defaultdict(collections.deque)
    notas = []

    def ler_variavel():
        nonlocal posicao
        valor = 0
        while True:
            byte = dados[posicao]
            posicao += 1
            valor = (valor << 7) | (byte & 127)
            if byte < 128:
                return valor

    while posicao < len(dados):
        delta = ler_variavel()
        tempo_tick += delta
        segundos += delta * microssegundos / resolucao / 1000000
        status = dados[posicao]
        if status >= 128:
            posicao += 1
            if status < 240:
                status_anterior = status
        else:
            status = status_anterior
        if status == 255:
            tipo = dados[posicao]
            posicao += 1
            tamanho = ler_variavel()
            conteudo = dados[posicao:posicao+tamanho]
            posicao += tamanho
            if tipo == 81:
                microssegundos = int.from_bytes(conteudo, 'big')
        elif status in (240, 247):
            tamanho = ler_variavel()
            posicao += tamanho
        else:
            tipo, canal = status >> 4, (status & 15) + 1
            tamanho = 1 if tipo in (12, 13) else 2
            valores = dados[posicao:posicao+tamanho]
            posicao += tamanho
            if tipo == 12:
                programas[canal] = valores[0] + 1
            if tipo == 9 and valores[1] > 0:
                abertas[canal, valores[0]].append((segundos, valores[1]))
            if tipo == 8 or (tipo == 9 and valores[1] == 0):
                fila = abertas[canal, valores[0]]
                if fila:
                    inicio, intensidade = fila.popleft()
                    notas.append(dict(inicio=inicio, fim=segundos, canal=canal,
                                      altura=valores[0], intensidade=intensidade))
    assert not any(abertas.values()), 'MIDI com notas sem fim'
    return sorted(notas, key=lambda nota: nota['inicio']), programas


def analisar_ataques():
    # Resolucao temporal de cerca de 10 ms; processamento feito somente offline.
    bruto = subprocess.check_output(['ffmpeg', '-v', 'error', '-i',
        str(RAIZ/'assets/musicas/dancing_queen/audio.mp3'), '-ac', '1', '-ar',
        '11025', '-f', 'f32le', '-'])
    audio = np.frombuffer(bruto, dtype='<f4')
    janela, passo = 1024, 110
    quadros = np.lib.stride_tricks.sliding_window_view(
        np.pad(audio, (janela//2, janela//2)), janela)[::passo]
    frequencias = np.fft.rfftfreq(janela, 1/11025)
    alturas = np.arange(48, 101)
    faixas = []
    for altura in alturas:
        frequencia = 440 * 2**((altura-69)/12)
        faixa = np.flatnonzero(abs(12*np.log2(np.maximum(frequencias,1)/frequencia)) < 0.48)
        if not len(faixa): faixa = [np.argmin(abs(frequencias-frequencia))]
        faixas.append(faixa)
    energia = np.zeros((len(alturas), len(quadros)), dtype=np.float32)
    # Lotes pequenos evitam manter o espectrograma inteiro na memoria.
    for inicio in range(0, len(quadros), 512):
        espectro = np.abs(np.fft.rfft(quadros[inicio:inicio+512]*np.hanning(janela), axis=1))
        for indice, faixa in enumerate(faixas):
            energia[indice, inicio:inicio+512] = espectro[:, faixa].max(axis=1)
    energia = np.log1p(energia)
    ataques = np.maximum(0, energia - np.pad(energia[:, :-2], ((0,0),(2,0)), mode='edge'))
    ataques /= np.maximum(np.percentile(ataques,98,axis=1,keepdims=True),0.01)
    return np.minimum(ataques,3), np.arange(len(quadros))*passo/11025


def ajustar_relogio(notas, canal, ataques, tempos):
    # Ajusta apenas notas de piano; voz, baixo e bateria nao entram na referencia.
    referencia = [n for n in notas if n['canal']==canal and
                  65 <= n['altura'] <= 100 and 8 < n['inicio'] < 210]
    inicios = np.array([n['inicio'] for n in referencia])
    alturas = np.array([n['altura']-48 for n in referencia])
    melhor = (-1, 1.004, -6.35)
    for escala in np.arange(1.002,1.006,0.0001):
        for deslocamento in np.arange(-6.6,-6.1,0.005):
            indices = np.searchsorted(tempos, inicios*escala+deslocamento)
            valor = float(ataques[alturas,indices].mean())
            if valor > melhor[0]: melhor = valor, float(escala), float(deslocamento)
    return dict(valor_ataques=melhor[0], escala=melhor[1], deslocamento=melhor[2])


def grupos_piano(notas):
    grupos = []
    # Agrupa acordes dentro de cada canal, antes de procurar seu ataque no audio.
    for canal in (3,11):
        anteriores = []
        for nota in notas:
            if nota['canal'] != canal or nota['altura'] < 60: continue
            if not anteriores or nota['inicio']-anteriores[-1][0]['inicio'] > 0.035:
                anteriores.append([])
            anteriores[-1].append(nota)
        grupos.extend(anteriores)
    return grupos


def localizar_ataque(grupo, previsto, ataques, tempos):
    indices = np.flatnonzero((tempos >= previsto-0.045) & (tempos <= previsto+0.045))
    if not len(indices): return None
    alturas = [n['altura']-48 for n in grupo]
    valores = ataques[np.ix_(alturas,indices)].mean(axis=0)
    melhor = int(np.argmax(valores))
    if valores[melhor] < 0.25: return None
    # O pico de crescimento vem de uma janela centrada, nao de um detector de fonte isolada.
    return float(tempos[indices[melhor]]), float(valores[melhor])


def criar_eventos(notas, relogios, ataques, tempos):
    eventos = []
    for grupo in grupos_piano(notas):
        origem = grupo[0]['inicio']
        canal = grupo[0]['canal']
        relogio = relogios[canal]
        secoes = []
        if origem < 214:
            secoes.append(('piano_principal', origem*relogio['escala']+relogio['deslocamento']))
        if 109.3607 <= origem < 127.7:
            secoes.append(('piano_repeticao', origem*1.004+99.23))
        for secao, previsto in secoes:
            if secao == 'piano_principal' and previsto >= 208.9: continue
            localizado = localizar_ataque(grupo, previsto, ataques, tempos)
            if localizado is None: continue
            instante, forca = localizado
            for nota in grupo:
                duracao = max(0, nota['fim']-nota['inicio'])*relogio['escala']
                eventos.append(dict(tempo=instante, fim=instante+duracao,
                    altura=nota['altura'], canal=canal, origem=secao,
                    tempo_midi=nota['inicio'], ataque=forca))
    return sorted(eventos, key=lambda nota: nota['tempo'])


def adaptar_cinco_pistas(eventos):
    grupos = []
    for nota in eventos:
        if not grupos or nota['tempo']-grupos[-1][0]['tempo'] > 0.025: grupos.append([])
        grupos[-1].append(nota)
    mapa, ultimas = [], [None]*5
    ultimo_ataque = -1.0
    for grupo in grupos:
        instante = min(n['tempo'] for n in grupo)
        if instante-ultimo_ataque < 0.125: continue
        ordenadas = sorted(grupo, key=lambda n:n['altura'])
        selecionadas = [ordenadas[-1]]
        if len(set(n['altura'] for n in grupo)) >= 3 and instante-ultimo_ataque >= 0.28:
            selecionadas.append(ordenadas[0])
        usadas = set()
        for nota in selecionadas:
            ocupadas = [p for p,n in enumerate(ultimas) if p not in usadas and n and
                        n['tempo']+n['duracao'] > instante-0.09]
            if len(ocupadas)+len(usadas) >= 2: continue
            preferida = int(np.clip(round((nota['altura']-64)/7),0,4))
            livres = [p for p in range(5) if p not in ocupadas and p not in usadas and
                      (ultimas[p] is None or instante-ultimas[p]['tempo'] >= 0.16)]
            if not livres: continue
            pista = min(livres,key=lambda p:abs(p-preferida))
            duracao = min(2.0,nota['fim']-instante)
            if duracao < 0.65: duracao = 0
            evento = dict(tempo=round(instante,3), pista=pista, duracao=round(duracao,3),
                canal=nota['canal'], origem=nota['origem'], altura=nota['altura'],
                tempo_midi=round(nota['tempo_midi'],6), ataque=round(nota['ataque'],6))
            mapa.append(evento)
            ultimas[pista] = evento
            usadas.add(pista)
        if usadas: ultimo_ataque = instante
    for pista in range(5):
        sequencia = [n for n in mapa if n['pista']==pista]
        for atual, seguinte in zip(sequencia, sequencia[1:]):
            atual['duracao'] = round(min(atual['duracao'],max(0,seguinte['tempo']-atual['tempo']-0.12)),3)
            if atual['duracao'] < 0.4: atual['duracao'] = 0
    return mapa


def gerar():
    esperados = {
        RAIZ/'assets/musicas/dancing_queen/audio.mp3': 'b3973408cd9033d44f7f2551ccc1c5df9f235a1c1a9c870b002322b0ca5a4cab',
        PASTA/'abba_dancing_queen_3381.mid': 'dbb2a6064fcdc852cfccafb7a3defeaea59415b26810d3d5d0970e8cb0a85bda'}
    for caminho, esperado in esperados.items():
        if hashlib.sha256(caminho.read_bytes()).hexdigest() != esperado:
            raise ValueError(f'{caminho.name} mudou: refazer o alinhamento.')
    notas, programas = ler_midi(PASTA/'abba_dancing_queen_3381.mid')
    assert programas[3] == programas[11] == 2
    ataques, tempos = analisar_ataques()
    relogios = {canal: ajustar_relogio(notas,canal,ataques,tempos) for canal in (3,11)}
    mapa = adaptar_cinco_pistas(criar_eventos(notas,relogios,ataques,tempos))
    assert all(n['canal'] in (3,11) for n in mapa)
    linhas = ['# Dancing Queen - somente piano/teclado (canais MIDI 3 e 11).',
              '# Sem melodia vocal e sem preenchimento por ataques de instrumentos desconhecidos.',
              '# Alinhamento por ataques do piano; revisao auditiva ainda necessaria.',
              '# Final sem referencia de piano confiavel fica sem notas; audio continua ate o fim.',
              '# Formato: tempo pista duracao. Pistas: 0=D 1=F 2=G 3=J 4=K.']
    linhas += [f"{n['tempo']:.3f} {n['pista']} {n['duracao']:.3f}" for n in mapa]
    (PASTA/'notas_dificil_candidato.txt').write_text(chr(10).join(linhas)+chr(10),encoding='utf-8')
    relatorio = dict(versao='somente_teclado',relogios_por_canal=relogios,
        audio_sha256=esperados[RAIZ/'assets/musicas/dancing_queen/audio.mp3'],
        midi_sha256=esperados[PASTA/'abba_dancing_queen_3381.mid'],
        total=len(mapa),sustentacoes=sum(n['duracao']>0 for n in mapa),
        primeiro=mapa[0]['tempo'],ultimo=max(n['tempo']+n['duracao'] for n in mapa),eventos=mapa)
    (PASTA/'mapa_dificil_auditoria.json').write_text(json.dumps(relatorio,indent=2)+chr(10),encoding='utf-8')
    print(json.dumps({k:v for k,v in relatorio.items() if k!='eventos'},indent=2))


if __name__ == '__main__':
    gerar()
