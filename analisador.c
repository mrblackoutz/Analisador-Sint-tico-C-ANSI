/* 
    Projeto 2 – Analisador Sintático Recursivo Descendente
    Disciplina: Linguagens Formais e Autômatos – 2025-1
    
    Alunos:
        - Felipe Ujvari Gasparino de Sousa - 10418415
        - Gustavo Nascimento Siqueira - 10419057
        - Thomaz de Souza Scopel - 10417183
        - Vinicius Cappatti - 10418266
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1000

char lookahead;

/*
 * Assinaturas (mantidas conforme o exemplo do professor):
 * As funções recebem o array de entrada 'palavra' e o índice '*pos'
 * por referência, e devolvem 1 (sucesso) ou 0 (falha).
 */
int E(char palavra[], int *pos);
int X(char palavra[], int *pos);
int K(char palavra[], int *pos);
int T(char palavra[], int *pos);
int Z(char palavra[], int *pos);
int F(char palavra[], int *pos);
int N(char palavra[], int *pos);
int D(char palavra[], int *pos);

/*
 * Função para exibir o contexto de erro,
 * mostrando a expressão inteira e marcando a posição com um '^'.
 */
void imprimeContextoErro(const char *palavra, int pos) {
    printf("\n  %s\n", palavra);
    printf("  ");
    for(int i = 0; i < pos; i++) {
        printf(" ");
    }
    printf("^\n");
}

/*
 * Função para sinalizar erro fatal.
 * Imprime a posição, o caractere inesperado, o que se esperava e encerra o programa.
 */
void sinalizaErro(const char *palavra, int pos, const char *msgEsperado) {
    printf("\nERRO DE SINTAXE na posição %d:", pos);
    if (pos < (int)strlen(palavra)) {
        printf("\nCaractere inesperado: '%c'", palavra[pos]);
    } else {
        printf("\nFim inesperado da expressão.");
    }
    if (msgEsperado != NULL && strlen(msgEsperado) > 0) {
        printf("\nEsperava: %s", msgEsperado);
    }
    imprimeContextoErro(palavra, pos);
    exit(1);
}

int match(char t, char palavra[], int *pos) {
    if (lookahead == t) {
        (*pos)++;
        lookahead = palavra[*pos];
        return 1;
    } else {
        char msg[100];
        snprintf(msg, sizeof(msg), "'%c'", t);
        sinalizaErro(palavra, *pos, msg);
        return 0; // só chegaria aqui se não encerrasse no sinalizaErro
    }
}

/*
 * Gramática a ser reconhecida:
 *
 * E -> X $
 * X -> T K
 * K -> + T K | - T K | ε
 * T -> F Z
 * Z -> * F Z | / F Z | ε
 * F -> ( X ) | - N | N
 * N -> [0-9] D
 * D -> [0-9] D | ε
 */

/* E -> X $ */
int E(char palavra[], int *pos) {
    if (!X(palavra, pos)) return 0;
    if (!match('$', palavra, pos)) return 0;
    return 1;
}

/* X -> T K */
int X(char palavra[], int *pos) {
    if (!T(palavra, pos)) return 0;
    if (!K(palavra, pos)) return 0;
    return 1;
}

/* K -> + T K | - T K | ε */
int K(char palavra[], int *pos) {
    if (lookahead == '+') {
        if (!match('+', palavra, pos)) return 0;
        if (!T(palavra, pos))         return 0;
        if (!K(palavra, pos))         return 0;
        return 1;
    } else if (lookahead == '-') {
        if (!match('-', palavra, pos)) return 0;
        if (!T(palavra, pos))         return 0;
        if (!K(palavra, pos))         return 0;
        return 1;
    }
    // Caso não seja '+' nem '-', pode ser ε se lookahead ∈ FOLLOW(K) = { '$', ')' }.
    if (lookahead == '$' || lookahead == ')') {
        return 1; // Produção ε
    }
    sinalizaErro(palavra, *pos, "'+', '-' ou fim de expressão ( ) ou $ )");
    return 0;
}

/* T -> F Z */
int T(char palavra[], int *pos) {
    if (!F(palavra, pos)) return 0;
    if (!Z(palavra, pos)) return 0;
    return 1;
}

/* Z -> * F Z | / F Z | ε */
int Z(char palavra[], int *pos) {
    if (lookahead == '*') {
        if (!match('*', palavra, pos)) return 0;
        if (!F(palavra, pos))         return 0;
        if (!Z(palavra, pos))         return 0;
        return 1;
    } else if (lookahead == '/') {
        if (!match('/', palavra, pos)) return 0;
        if (!F(palavra, pos))         return 0;
        if (!Z(palavra, pos))         return 0;
        return 1;
    }
    // Caso contrário, ε se lookahead ∈ FOLLOW(Z) = { '$', ')', '+', '-' }.
    if (lookahead == '$' || lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; // ε
    }
    sinalizaErro(palavra, *pos, "'*', '/' ou fim de termo ($, ), +, -)");
    return 0;
}

/* F -> ( X ) | - N | N */
int F(char palavra[], int *pos) {
    if (lookahead == '(') {
        if (!match('(', palavra, pos)) return 0;
        if (!X(palavra, pos))         return 0;
        if (!match(')', palavra, pos)) return 0;
        return 1;
    } else if (lookahead == '-') {
        if (!match('-', palavra, pos)) return 0;
        if (!N(palavra, pos))         return 0;
        return 1;
    } else {
        if (!N(palavra, pos)) return 0;
        return 1;
    }
}

/* N -> [0-9] D */
int N(char palavra[], int *pos) {
    if (isdigit(lookahead)) {
        // match(lookahead, ...) para casar o dígito atual
        if (!match(lookahead, palavra, pos)) return 0;
        // Em seguida, chama D
        if (!D(palavra, pos)) return 0;
        return 1;
    }
    sinalizaErro(palavra, *pos, "dígito [0-9]");
    return 0;
}

/* D -> [0-9] D | ε */
int D(char palavra[], int *pos) {
    if (isdigit(lookahead)) {
        if (!match(lookahead, palavra, pos)) return 0;
        if (!D(palavra, pos)) return 0;
        return 1;
    }
    // ε se lookahead ∈ FOLLOW(D) = { '*', '/', '$', ')', '+', '-' }
    if (lookahead == '*' || lookahead == '/' || lookahead == '$' ||
        lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; 
    }
    sinalizaErro(palavra, *pos, "dígito ou fim de número (ex: +, -, ), $, *, /)");
    return 0;
}

int main() {
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo entrada.txt\n");
        return 1;
    }

    char palavra[MAX];
    if (fgets(palavra, MAX, arquivo) == NULL) {
        printf("Arquivo vazio ou erro de leitura.\n");
        fclose(arquivo);
        return 1;
    }
    fclose(arquivo);

    // Remove eventual quebra de linha do final
    size_t len = strlen(palavra);
    if (len > 0 && palavra[len - 1] == '\n') {
        palavra[len - 1] = '\0';
        len--;
    }

    // Se não terminar com '$', a expressão está incorreta 
    if (len == 0 || palavra[len - 1] != '$') {
        printf("ERRO: A expressão não termina com '$'.\n");
        return 1;
    }

    int pos = 0;
    lookahead = palavra[pos];

    printf("Expressão a ser analisada: %s\n", palavra);

    if (E(palavra, &pos)) {
        // Se consumiu tudo, pos deve estar em len
        if (pos == (int)strlen(palavra)) {
            printf("\nExpressão reconhecida com sucesso!\n\n");
        } else {
            // Algo sobrou após o $
            printf("\nAtenção: Sobrou texto após o fim da expressão!\n");
            printf("Pos = %d, Tamanho total = %lu.\n", pos, strlen(palavra));
        }
    } else {
        // Em caso de falha, provavelmente sinalizaErro já foi chamado,
        // mas deixamos aqui só pra consistência.
        sinalizaErro(palavra, pos, "Símbolo inicial E");
    }

    return 0;
}