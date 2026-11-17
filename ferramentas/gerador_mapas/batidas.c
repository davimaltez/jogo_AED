#include "gerador.h"
#include <math.h>

GradeBatidas detectar_batidas(const Ataques *harmonicos, const Ataques *percussivos) {
    GradeBatidas grade = {0};
    int quantidade = harmonicos->quadros, ataques = 0;
    for (int i = 0; i < quantidade; i++) {
        if (harmonicos->picos[i] > 0 || percussivos->picos[i] > 0) ataques++;
    }
    if (ataques < MINIMO_ATAQUES_BATIDA) return grade;
    float passo = (float)PASSO_FFT / TAXA_ANALISE;
    int menor = (int)ceilf(60 / (BPM_MAXIMO * passo));
    int maior = (int)floorf(60 / (BPM_MINIMO * passo));
    float melhor = 0;
    int atraso_escolhido = 0;
    for (int atraso = menor; atraso <= maior && atraso < quantidade / 2; atraso++) {
        double produto = 0, energia_atual = 0, energia_anterior = 0;
        for (int i = atraso; i < quantidade; i++) {
            float atual = harmonicos->fluxo[i] + percussivos->fluxo[i];
            float anterior = harmonicos->fluxo[i - atraso] + percussivos->fluxo[i - atraso];
            produto += (double)atual * anterior;
            energia_atual += (double)atual * atual;
            energia_anterior += (double)anterior * anterior;
        }
        float correlacao = energia_atual > 0 && energia_anterior > 0
            ? (float)(produto / sqrt(energia_atual * energia_anterior)) : 0;
        float bpm = 60 / (atraso * passo);
        float prioridade = correlacao;
        if (bpm >= BPM_PREFERIDO_MINIMO && bpm <= BPM_PREFERIDO_MAXIMO) prioridade *= BONUS_BPM_PREFERIDO;
        if (prioridade > melhor) {
            melhor = prioridade; atraso_escolhido = atraso; grade.confianca = correlacao;
        }
    }
    if (grade.confianca < CONFIANCA_BATIDA || !atraso_escolhido) return (GradeBatidas){0};
    float melhor_fase = -1;
    for (int fase = 0; fase < atraso_escolhido; fase++) {
        float soma = 0;
        for (int i = fase; i < quantidade; i += atraso_escolhido) soma += harmonicos->fluxo[i] + percussivos->fluxo[i];
        if (soma > melhor_fase) { melhor_fase = soma; grade.fase = fase * passo; }
    }
    grade.periodo = atraso_escolhido * passo;
    return grade;
}

float alinhar_grade(float tempo, GradeBatidas grade) {
    if (grade.periodo <= 0) return tempo;
    float proximo = grade.fase + roundf((tempo - grade.fase) / grade.periodo) * grade.periodo;
    float tolerancia = TOLERANCIA_GRADE_QUADROS * PASSO_FFT / TAXA_ANALISE;
    return proximo >= 0 && fabsf(proximo - tempo) <= tolerancia ? proximo : tempo;
}
