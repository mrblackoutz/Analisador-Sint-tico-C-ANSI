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
#include <ctype.h> // Para isdigit

#define MAX 1000 // Tamanho maximo da linha do arquivo

/*
 * Gramática:
 * E -> X $
 * X -> T K
 * K -> + T K | - T K | ε
 * T -> F Z
 * Z -> * F Z | / F Z | ε
 * F -> ( X ) | N | - N
 * N -> [0-9] D
 * D -> [0-9] D | ε
 */

char lookahead;     // Variável global para o lookahead
char *input_string; // Ponteiro para a string de entrada
int current_pos;    // Posição atual na string de entrada

// Protótipos das funções para cada não-terminal
int E();
int X();
int K();
int T();
int Z();
int F();
int N();
int D();

// Função para avançar o lookahead se o caractere atual corresponder ao terminal esperado
int match(char t) {
    if (lookahead == t) {
        lookahead = input_string[++current_pos];
        return 1;
    }
    return 0;
}

// Função para tratar erros de sintaxe
void trataErro() {
    printf("\n\nERRO DE SINTAXE na posicao %d. Caractere inesperado: '%c'\n\n", current_pos, lookahead);
    exit(1); // Encerra o programa em caso de erro grave
}

// Implementação das funções para cada não-terminal (análise sintática)

/* E -> X $ */
int E() {
    if (X()) {
        if (match('$')) {
            return 1;
        }
    }
    return 0;
}

/* X -> T K */
int X() {
    if (T()) {
        if (K()) {
            return 1;
        }
    }
    return 0;
}

/* K -> + T K | - T K | ε */
int K() {
    // Salva o estado atual para possível backtrack (se necessário, embora não estritamente aqui)
    // Para esta gramática, o FIRST/FOLLOW já nos guia.
    int saved_pos = current_pos;
    char saved_lookahead = lookahead;

    if (lookahead == '+') {
        if (match('+')) {
            if (T()) {
                if (K()) {
                    return 1;
                }
            }
        }
    } else if (lookahead == '-') {
        if (match('-')) {
            if (T()) {
                if (K()) {
                    return 1;
                }
            }
        }
    }
    // K -> ε
    // Se o lookahead não for nem '+' nem '-', então K pode ser epsilon.
    // Isso é determinado pelo FOLLOW(K).
    // FOLLOW(K) = { $, ) }
    else if (lookahead == '$' || lookahead == ')') {
        return 1; // Produção epsilon
    }

    // Se chegou aqui, é um erro, pois o lookahead não corresponde a nenhum FIRST de K,
    // nem a um caractere no FOLLOW(K) que permitiria epsilon.
    // No entanto, para produções epsilon, o retorno 1 já cobre o caso.
    // A lógica de "else return 0" deve ser mais cuidadosa para não "falhar" epsilon indevidamente.
    // Se nenhum dos casos anteriores (que consomem entrada) funcionou, e o lookahead está no FOLLOW(K),
    // significa que a produção epsilon foi usada.

    // Se chegamos aqui e não retornamos 1, significa que o lookahead não é válido para K.
    // A gramática tem produções epsilon condicionais, então o else final não deve ser um simples 'return 0'
    // se o lookahead estiver no FOLLOW(K).
    // A estrutura do parser preditivo impede o backtrack.
    // Se o lookahead não é '+', '-', '$' ou ')', então é um erro.
    trataErro(); // Should not be reached if epsilon is handled correctly
    return 0; // Should not be reached
}

/* T -> F Z */
int T() {
    if (F()) {
        if (Z()) {
            return 1;
        }
    }
    return 0;
}

/* Z -> * F Z | / F Z | ε */
int Z() {
    int saved_pos = current_pos;
    char saved_lookahead = lookahead;

    if (lookahead == '*') {
        if (match('*')) {
            if (F()) {
                if (Z()) {
                    return 1;
                }
            }
        }
    } else if (lookahead == '/') {
        if (match('/')) {
            if (F()) {
                if (Z()) {
                    return 1;
                }
            }
        }
    }
    // Z -> ε
    // Se o lookahead não for nem '*' nem '/', então Z pode ser epsilon.
    // Isso é determinado pelo FOLLOW(Z).
    // FOLLOW(Z) = { $, ), +, - }
    else if (lookahead == '$' || lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; // Produção epsilon
    }

    trataErro(); // Should not be reached if epsilon is handled correctly
    return 0; // Should not be reached
}

/* F -> ( X ) | N | - N */
int F() {
    if (lookahead == '(') {
        if (match('(')) {
            if (X()) {
                if (match(')')) {
                    return 1;
                }
            }
        }
    } else if (isdigit(lookahead)) { // F -> N
        if (N()) {
            return 1;
        }
    } else if (lookahead == '-') { // F -> - N
        if (match('-')) {
            if (N()) {
                return 1;
            }
        }
    }
    return 0; // Se nenhum dos casos acima for satisfeito
}

/* N -> [0-9] D */
int N() {
    if (isdigit(lookahead)) {
        // Se o lookahead for um dígito, o match é feito para o dígito
        // Note: A função match foi projetada para um único terminal.
        // Precisamos verificar se o caractere atual é um dígito e então avançar.
        char digit_char = lookahead; // Salva o dígito para verificar
        if (digit_char >= '0' && digit_char <= '9') { // Verifica se é um dígito válido
             lookahead = input_string[++current_pos]; // Avança o lookahead
             if (D()) {
                 return 1;
             }
        }
    }
    return 0; // Se não for um dígito
}

/* D -> [0-9] D | ε */
int D() {
    // Salva o estado atual para possível backtrack (se necessário)
    int saved_pos = current_pos;
    char saved_lookahead = lookahead;

    if (isdigit(lookahead)) {
        // Se o lookahead for um dígito, o match é feito para o dígito
        char digit_char = lookahead;
        if (digit_char >= '0' && digit_char <= '9') {
             lookahead = input_string[++current_pos];
             if (D()) {
                 return 1;
             }
        }
    }
    // D -> ε
    // Se o lookahead não for um dígito, então D pode ser epsilon.
    // Isso é determinado pelo FOLLOW(D).
    // FOLLOW(D) = { *, /, $, ), +, -, 0-9 }
    // O 0-9 no FOLLOW(D) é para casos como '123+...', onde o '+' segue '123'
    // e '3' é o último dígito de 'D'. No nosso `isdigit(lookahead)` já pegamos os 0-9.
    // O que nos importa é que o lookahead seja um dos símbolos do FOLLOW(D)
    // e *não* um dígito para que a produção epsilon seja escolhida.
    else if (lookahead == '*' || lookahead == '/' || lookahead == '$' ||
             lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; // Produção epsilon
    }

    // Se chegou aqui, é um erro, pois o lookahead não corresponde a nenhum FIRST de D (dígito),
    // nem a um caractere no FOLLOW(D) que permitiria epsilon.
    trataErro();
    return 0; // Should not be reached
}


int main() {

    FILE *arquivo_entrada = fopen("entrada.txt", "r");
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo entrada.txt\n");
        return 1;
    }

    char linha[MAX];
    fgets(linha, MAX, arquivo_entrada);

    input_string = linha;
    current_pos = 0;

    printf("Analisando a palavra: %s\n", input_string);

    lookahead = input_string[current_pos];

    if (E()) {
        printf("\nPalavra \"%s\" reconhecida com sucesso!\n\n", input_string);
    } else {
        trataErro(); // Isso já é chamado internamente, mas aqui é para o caso de E() retornar 0
    }

    fclose(arquivo_entrada);
    return 0;
}