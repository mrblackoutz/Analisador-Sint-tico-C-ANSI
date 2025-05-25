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
 * K -> + T K | - T K | 'e'
 * T -> F Z
 * Z -> * F Z | / F Z | 'e'
 * F -> ( X ) | N | - N
 * N -> [0-9] D
 * D -> [0-9] D | 'e'
 * 
 * 'e' é a palavra vazia
 * 
 * Conjuntos FIRST:
 *
 * FIRST(E) = { (, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, - }
 * FIRST(X) = { (, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, - }
 * FIRST(K) = { +, -, ε }
 * FIRST(T) = { (, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, - }
 * FIRST(Z) = { , /, ε }
 * FIRST(F) = { (, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, - }
 * FIRST(N) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
 * FIRST(D) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ε }
 * 
 * Conjuntos FOLLOW:
 * FOLLOW(E) = { $ }
 * FOLLOW(X) = { $, ) }
 * FOLLOW(K) = { $, ) }
 * FOLLOW(T) = { $, ), +, - }
 * FOLLOW(Z) = { $, ), +, - }
 * FOLLOW(F) = { , /, $, ), +, - }
 * FOLLOW(N) = { , /, $, ), +, -, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
 * FOLLOW(D) = { , /, $, ), +, -, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
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

/* K -> + T K | - T K | 'e' */
int K() {
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
    // K -> 'e'
    // Se o lookahead não for nem '+' nem '-', então K pode ser palavra vazia.
    // Isso é determinado pelo FOLLOW(K).
    // FOLLOW(K) = { $, ) }
    else if (lookahead == '$' || lookahead == ')') {
        return 1; // Produção epsilon
    }

    // Se chegou aqui, é um erro, pois o lookahead não corresponde a nenhum FIRST de K,
    // nem a um caractere no FOLLOW(K) que permitiria epsilon.
    // No entanto, para produções epsilon, o retorno 1 já cobre o caso.
    // Se nenhum dos casos anteriores (que consomem entrada) funcionou, e o lookahead está no FOLLOW(K),
    // significa que a produção epsilon foi usada.
    // Se o lookahead não é '+', '-', '$' ou ')', então é um erro.
    
    trataErro();    // Não deveria ser alcançado se usou a palavra vazia corretamente
    return 0;       // Não deveria ser alcançado
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

/* Z -> * F Z | / F Z | 'e' */
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
    // Z -> 'e'
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
        // Precisamos verificar se o caractere atual é um dígito e então avançar.
        if (lookahead >= '0' && lookahead <= '9') { // Verifica se é um dígito válido
             lookahead = input_string[++current_pos]; // Avança o lookahead
             if (D()) {
                 return 1;
             }
        }
    }
    return 0; // Se não for um dígito
}

/* D -> [0-9] D | 'e' */
int D() {
    if (isdigit(lookahead)) {
        // Se o lookahead for um dígito, o match é feito para o dígito
        if (lookahead >= '0' && lookahead <= '9') {
             lookahead = input_string[++current_pos];
             if (D()) {
                 return 1;
             }
        }
    }
    // D -> 'e'
    // Se o lookahead não for um dígito, então D pode ser epsilon.
    // Isso é determinado pelo FOLLOW(D).
    // FOLLOW(D) = { *, /, $, ), +, -, 0-9 }
    else if (lookahead == '*' || lookahead == '/' || lookahead == '$' ||
             lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; // Produção epsilon
    }

    // Se chegou aqui, é um erro, pois o lookahead não corresponde a nenhum FIRST de D (dígito),
    // nem a um caractere no FOLLOW(D) que permitiria epsilon.
    trataErro();
    return 0; // Não deve ser alcançado
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