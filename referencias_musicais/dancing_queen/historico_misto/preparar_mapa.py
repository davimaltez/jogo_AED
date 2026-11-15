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


def normalizar(colunas):
    return colunas / np.maximum(np.linalg.norm(colunas, axis=0), 1e-9)


def analisar_audio():
    caminho = RAIZ / 'assets/musicas/dancing_queen/audio.mp3'
    bruto = subprocess.check_output(['ffmpeg', '-v', 'error', '-i', str(caminho),
                                    '-ac', '1', '-ar', '11025', '-f', 'f32le', '-'])
    audio = np.frombuffer(bruto, dtype='<f4')
    passo, janela = 551, 4096
    quadros = np.lib.stride_tricks.sliding_window_view(np.pad(audio, (janela//2, janela//2)), janela)[::passo]
    espectro = np.abs(np.fft.rfft(quadros * np.hanning(janela), axis=1)).T
    frequencias = np.fft.rfftfreq(janela, 1/11025)
    # Soma energia em faixas estreitas de semitom; realca componentes tonais.
    alturas = np.arange(36, 96)
    energia_notas = np.empty((len(alturas), espectro.shape[1]))
    for indice, altura in enumerate(alturas):
        frequencia = 440 * 2**((altura-69)/12)
        faixa = np.abs(12*np.log2(np.maximum(frequencias, 1)/frequencia)) < 0.45
        energia_notas[indice] = np.max(espectro[faixa], axis=0)
    energia_notas = np.log1p(energia_notas)
    cromas = np.zeros((12, espectro.shape[1]))
    for indice, altura in enumerate(alturas):
        cromas[altura % 12] += energia_notas[indice]
    cromas -= np.min(cromas, axis=0)
    return normalizar(cromas), energia_notas, np.arange(espectro.shape[1])*passo/11025


def cromas_midi(notas, tempos, canais=None):
    cromas = np.zeros((12, len(tempos)))
    for nota in notas:
        if nota['canal'] == 10 or (canais and nota['canal'] not in canais):
            continue
        inicio, fim = np.searchsorted(tempos, [nota['inicio'], nota['fim']])
        cromas[nota['altura'] % 12, inicio:max(inicio+1, fim)] += nota['intensidade']/127
    return normalizar(cromas)


def ajustar_ancoras(referencia, tempos_midi, cromas, tempos, centros, deslocamento):
    ancoras = []
    for centro in centros:
        janela = (tempos_midi >= centro-4) & (tempos_midi <= centro+4)
        melhor = (-1, deslocamento)
        for ajuste in np.arange(deslocamento-0.3, deslocamento+0.301, 0.01):
            indices = np.clip(np.searchsorted(tempos, tempos_midi[janela]*1.004+ajuste), 0, len(tempos)-1)
            similaridade = float(np.mean(np.sum(referencia[:, janela]*cromas[:, indices], axis=0)))
            if similaridade > melhor[0]:
                melhor = similaridade, float(ajuste)
        ancoras.append([float(centro), float(centro*1.004+melhor[1]), melhor[0]])
    return np.array(ancoras)


def converter_tempo(instante, ancoras):
    deslocamentos = ancoras[:, 1] - ancoras[:, 0]*1.004
    return float(instante*1.004 + np.interp(instante, ancoras[:, 0], deslocamentos))


def gerar_eventos(notas, ancoras, repeticao, energia, tempos):
    eventos = []
    # Canal 4: linha melodica; canais 3 e 11: partes de piano do arranjo.
    for origem, minimo, maximo, pontos in [('principal', 6.49, 214.0, ancoras),
                                         ('repeticao_final', 109.3607, 127.7, repeticao)]:
        for nota in notas:
            if nota['canal'] not in (3, 4, 11) or not minimo <= nota['inicio'] < maximo:
                continue
            if nota['canal'] != 4 and nota['altura'] < 60:
                continue
            inicio = converter_tempo(nota['inicio'], pontos)
            fim = converter_tempo(nota['fim'], pontos)
            if origem == 'principal':
                fim = min(fim, 208.9)
                if inicio >= 208.9: continue
            if inicio < 0: continue
            eventos.append(dict(tempo=inicio, fim=fim, altura=nota['altura'],
                                voz=nota['canal']==4, origem=origem, canal=nota['canal']))
    # Fechamento difere do MIDI. Usa ataques tonais observados no registro agudo,
    # sem esticar o outro arranjo para preencher a gravacao. Nao e separacao de fontes.
    novidades = np.maximum(0, np.diff(energia[24:], axis=1, prepend=energia[24:, :1]))
    envelope = novidades.sum(axis=0)
    regiao = (tempos >= 227.6) & (tempos < 241.5)
    limiar = np.quantile(envelope[regiao], 0.60)
    ultimo = 0.0
    for i in np.flatnonzero(regiao)[1:-1]:
        if envelope[i] < limiar or envelope[i] < envelope[i-1] or envelope[i] < envelope[i+1]:
            continue
        instante = float(tempos[i])
        if instante-ultimo < 0.17: continue
        altura = int(np.argmax(novidades[:, i])) + 60
        eventos.append(dict(tempo=instante, fim=instante, altura=altura, voz=False,
                            origem='ataque_tonal_final', canal=None))
        ultimo = instante
    return sorted(eventos, key=lambda nota: nota['tempo'])


def adaptar_cinco_pistas(eventos):
    # Agrupa acordes do MIDI e reduz cada ataque a ate duas teclas.
    grupos = []
    for nota in eventos:
        if not grupos or nota['tempo']-grupos[-1][0]['tempo'] > 0.035:
            grupos.append([])
        grupos[-1].append(nota)
    mapa = []
    ultima_pista = [None]*5
    ultimo_ataque = -1.0
    for grupo in grupos:
        instante = min(nota['tempo'] for nota in grupo)
        vozes = [nota for nota in grupo if nota['voz']]
        pianos = sorted([nota for nota in grupo if not nota['voz']], key=lambda nota: nota['altura'])
        if not vozes and instante-ultimo_ataque < 0.125:
            continue
        candidatas = vozes[:1]
        if pianos:
            candidatas.append(pianos[-1])
            # Acordes de piano somente nos ataques espacados, evitando excesso de teclas.
            if not vozes and len(pianos)>2 and instante-ultimo_ataque >= 0.28:
                candidatas.append(pianos[0])
        usadas = set()
        for nota in candidatas:
            ocupadas = [p for p,n in enumerate(ultima_pista) if p not in usadas and n and
                        n['tempo']+n['duracao']>instante-0.09]
            if len(ocupadas)+len(usadas) >= 2:
                if not nota['voz']: continue
                # Prioriza a proxima silaba; encurta a cauda anterior em vez de sobrepor teclas.
                for pista in ocupadas:
                    anterior = ultima_pista[pista]
                    anterior['duracao'] = max(0, instante-0.12-anterior['tempo'])
                    if anterior['duracao'] < 0.4: anterior['duracao'] = 0
                ocupadas = []
            preferida = int(np.clip(round((nota['altura']-64)/(2.25 if nota['voz'] else 7)), 0, 4))
            livres = [p for p in range(5) if p not in usadas and p not in ocupadas and
                      (ultima_pista[p] is None or instante-ultima_pista[p]['tempo'] >= 0.16)]
            if not livres: continue
            pista = min(livres, key=lambda p: abs(p-preferida))
            duracao = min(3.0, nota['fim']-instante)
            if duracao < (0.45 if nota['voz'] else 0.65): duracao = 0.0
            evento = dict(tempo=round(instante,3), pista=pista, duracao=round(max(0,duracao),3),
                          origem=nota['origem'], canal=nota['canal'], altura=nota['altura'])
            mapa.append(evento)
            ultima_pista[pista] = evento
            usadas.add(pista)
        if usadas: ultimo_ataque = instante
    # Mantem espaco para soltar e apertar de novo, inclusive apos arredondamento.
    for pista in range(5):
        sequencia = [nota for nota in mapa if nota['pista']==pista]
        for anterior, seguinte in zip(sequencia, sequencia[1:]):
            anterior['duracao'] = round(min(anterior['duracao'], max(0,seguinte['tempo']-anterior['tempo']-0.12)),3)
            if anterior['duracao'] < 0.4: anterior['duracao'] = 0
    return mapa


def gerar():
    # Os trechos foram estudados para estes arquivos, nao para qualquer versao da musica.
    esperados = {
        RAIZ/'assets/musicas/dancing_queen/audio.mp3': 'b3973408cd9033d44f7f2551ccc1c5df9f235a1c1a9c870b002322b0ca5a4cab',
        PASTA/'abba_dancing_queen_3381.mid': 'dbb2a6064fcdc852cfccafb7a3defeaea59415b26810d3d5d0970e8cb0a85bda'}
    for caminho, esperado in esperados.items():
        if hashlib.sha256(caminho.read_bytes()).hexdigest() != esperado:
            raise ValueError(f'{caminho.name} mudou: refazer o alinhamento antes de gerar.')
    notas, programas = ler_midi(PASTA / 'abba_dancing_queen_3381.mid')
    for canal, programa in programas.items():
        eventos = [nota for nota in notas if nota['canal'] == canal]
        if eventos:
            print(canal, programa, len(eventos), 'primeiro', round(eventos[0]['inicio'], 3),
                  'ultimo', round(eventos[-1]['fim'], 3), 'alturas',
                  min(n['altura'] for n in eventos), max(n['altura'] for n in eventos))
    cromas, energia, tempos = analisar_audio()
    tempos_midi = np.arange(0, max(n['fim'] for n in notas)+0.1, 0.1)
    referencia = cromas_midi(notas, tempos_midi)
    ancoras = ajustar_ancoras(referencia, tempos_midi, cromas, tempos, np.arange(10,211,8), -6.3)
    repeticao = ajustar_ancoras(referencia, tempos_midi, cromas, tempos, [109.36,113,117,121,124], 99.23)
    mapa = adaptar_cinco_pistas(gerar_eventos(notas, ancoras, repeticao, energia, tempos))
    linhas = ['# Dancing Queen - dificil 5/5; adaptacao de teclado e melodia vocal.',
              '# Tempos relativos ao MP3 original; formato: tempo pista duracao.',
              '# Alinhamento automatico por trechos; fechamento por ataques tonais.',
              '# Conferencia auditiva humana pendente. Ver referencias_musicais/dancing_queen/RELATORIO.md.',
              '# Pistas: 0=D 1=F 2=G 3=J 4=K. Duracao zero: nota comum.']
    linhas += [f"{nota['tempo']:.3f} {nota['pista']} {nota['duracao']:.3f}" for nota in mapa]
    destino = PASTA / 'notas_dificil_candidato.txt'
    destino.write_text('\n'.join(linhas)+'\n', encoding='utf-8')
    relatorio = dict(audio_sha256=hashlib.sha256((RAIZ/'assets/musicas/dancing_queen/audio.mp3').read_bytes()).hexdigest(),
                     midi_sha256=hashlib.sha256((PASTA/'abba_dancing_queen_3381.mid').read_bytes()).hexdigest(),
                     ancoras_principal=ancoras.tolist(), ancoras_repeticao=repeticao.tolist(),
                     total=len(mapa), sustentacoes=sum(n['duracao']>0 for n in mapa),
                     primeiro=mapa[0]['tempo'], ultimo=max(n['tempo']+n['duracao'] for n in mapa),
                     eventos=mapa)
    (PASTA/'mapa_dificil_auditoria.json').write_text(json.dumps(relatorio,indent=2)+'\n',encoding='utf-8')
    print('Mapa:', {k:v for k,v in relatorio.items() if k not in ('eventos','ancoras_principal','ancoras_repeticao')})


if __name__ == '__main__':
    gerar()
