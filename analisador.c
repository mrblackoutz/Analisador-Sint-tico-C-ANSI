/*
    Projeto 2 – Analisador Sintático Recursivo Descendente
    Disciplina: Linguagens Formais e Autômatos – 2025-1
    Alunos:
        - Felipe Ujvari Gasparino de Sousa - 10418415
        - Gustavo Nascimento Siqueira - 1041XXXX
        - Vinicius Capacci - 1041XXXX
        - Thomaz de Sousa - 1041XXXX
          
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1000

char entrada[MAX];
int pos = 0;
char token;

void erro(const char *mensagem) {
    printf("  ❌ Erro: %s na posição %d (caractere '%c')\n", mensagem, pos + 1, entrada[pos]);
    exit(1);
}

void proximo() {
    token = entrada[pos++];
}

void match(char esperado) {
    if (token == esperado) {
        proximo();
    } else {
        char msg[50];
        sprintf(msg, "Esperado '%c'", esperado);
        erro(msg);
    }
}

// E → X $
void E() {
    X();
    if (token != '$') {
        erro("Esperado fim da expressão ('$')");
    }
}

// X → T K
void X() {
    T();
    K();
}

// K → + T K | - T K | ε
void K() {
    while (token == '+' || token == '-') {
        proximo();
        T();
    }
}

// T → F Z
void T() {
    F();
    Z();
}

// Z → * F Z | / F Z | ε
void Z() {
    while (token == '*' || token == '/') {
        proximo();
        F();
    }
}

// F → (X) | N | -N
void F() {
    if (token == '(') {
        proximo();
        X();
        match(')');
    } else if (token == '-') {
        proximo();
        N();
    } else if (isdigit(token)) {
        N();
    } else {
        erro("Esperado número, parêntese ou sinal negativo");
    }
}

// N → [0-9] D
void N() {
    if (!isdigit(token)) {
        erro("Esperado dígito");
    }
    proximo();
    D();
}

// D → [0-9] D | ε
void D() {
    while (isdigit(token)) {
        proximo();
    }
}

void analisar(char *linha) {
    pos = 0;

    // Limpar e preparar string
    int len = strlen(linha);
    if (linha[len - 1] == '\n') linha[len - 1] = '\0';

    char expressao[MAX];
    strcpy(expressao, linha);
    strcat(expressao, "$");

    strcpy(entrada, expressao);
    printf("➡️  Analisando: %s\n", linha);

    proximo();
    E();
    printf("  ✅ Expressão válida!\n\n");
}

int main() {
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo entrada.txt\n");
        return 1;
    }

    char linha[MAX];
    while (fgets(linha, MAX, arquivo)) {
        if (linha[0] == '\n') continue;
        analisar(linha);
    }

    fclose(arquivo);
    return 0;
}
