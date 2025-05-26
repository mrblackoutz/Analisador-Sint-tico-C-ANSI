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

/*
 * Gramática a ser reconhecida (usaremos E como símbolo inicial):
 *
 * E -> X $
 * 
 * X -> T K
 * 
 * K -> + T K 
 *    | - T K
 *    | ε    (palavra vazia)
 * 
 * T -> F Z
 * 
 * Z -> * F Z
 *    | / F Z
 *    | ε
 * 
 * F -> ( X )
 *    | - N
 *    | N
 * 
 * N -> [0-9] D
 * 
 * D -> [0-9] D
 *    | ε
 */

// Variável global para armazenar o símbolo atual (lookahead).
char lookahead;

int match(char t, char palavra[], int *pos) {
    if (lookahead == t) {
        (*pos)++;
        lookahead = palavra[*pos];
        return 1;
    }
    return 0;
}

// --------------------------------------------------------------------
// Tratamento de erro simples (pode ser melhorado para indicar posição).
// --------------------------------------------------------------------
void trataErro(const char *palavra, int pos) {
    printf("\nERRO DE SINTAXE na posição %d", pos);
    printf("\nCaractere inesperado: '%c'\n", palavra[pos]);
}

int E(char palavra[], int *pos);
int X(char palavra[], int *pos);
int K(char palavra[], int *pos);
int T(char palavra[], int *pos);
int Z(char palavra[], int *pos);
int F(char palavra[], int *pos);
int N(char palavra[], int *pos);
int D(char palavra[], int *pos);

// --------------------------------------------------------------------
// E -> X $
// --------------------------------------------------------------------
int E(char palavra[], int *pos) {
    // Tenta derivar X
    if (!X(palavra, pos)) {
        return 0;
    }

    // Tenta casar '$'
    if (!match('$', palavra, pos)) {
        return 0;
    }

    // Se chegou aqui, derivou com sucesso X e encontrou '$'
    return 1;
}

// --------------------------------------------------------------------
// X -> T K
// --------------------------------------------------------------------
int X(char palavra[], int *pos) {
    if (!T(palavra, pos)) {
        return 0;
    }
    if (!K(palavra, pos)) { 
        return 0;
    }
    return 1;
}

// --------------------------------------------------------------------
// K -> + T K
//    | - T K
//    | ε
// --------------------------------------------------------------------
int K(char palavra[], int *pos) {
    // FIRST(K) = { '+', '-' } U {ε} 
    // FOLLOW(K) = { '$', ')' } (pois K aparece após T, dentro de X -> T K)
    //
    // Verifica se o lookahead é '+' ou '-'
    if (lookahead == '+') {
        if (!match('+', palavra, pos)) {
            return 0;
        }
        if (!T(palavra, pos)) {
            return 0;
        }
        if (!K(palavra, pos)) {
            return 0;
        }
        return 1;
    } else if (lookahead == '-') {
        if (!match('-', palavra, pos)) {
            return 0;
        }
        if (!T(palavra, pos)) {
            return 0;
        }
        if (!K(palavra, pos)) {
            return 0;
        }
        return 1;
    }

    // Caso não seja '+' nem '-', pode ser ε (nada). Então retornamos 1
    // se o lookahead estiver em FOLLOW(K): { '$', ')' }.
    if (lookahead == '$' || lookahead == ')') {
        // Produção epsilon
        return 1;
    }

    // Caso contrário, erro de sintaxe
    return 0;
}

// --------------------------------------------------------------------
// T -> F Z
// --------------------------------------------------------------------
int T(char palavra[], int *pos) {
    if (!F(palavra, pos)) {
        return 0;
    }
    if (!Z(palavra, pos)) {
        return 0;
    }
    return 1;
}

// --------------------------------------------------------------------
// Z -> * F Z
//    | / F Z
//    | ε
// --------------------------------------------------------------------
int Z(char palavra[], int *pos) {
    // FIRST(Z) = { '*', '/' } U {ε}
    // FOLLOW(Z) = { '$', ')', '+', '-' }
    //
    // Verifica se é '*' ou '/'
    if (lookahead == '*') {
        if (!match('*', palavra, pos)) {
            return 0;
        }
        if (!F(palavra, pos)) {
            return 0;
        }
        if (!Z(palavra, pos)) {
            return 0;
        }
        return 1;
    } else if (lookahead == '/') {
        if (!match('/', palavra, pos)) {
            return 0;
        }
        if (!F(palavra, pos)) {
            return 0;
        }
        if (!Z(palavra, pos)) {
            return 0;
        }
        return 1;
    }

    // Se não é '*' nem '/', pode ser ε no FOLLOW(Z): { '$', ')', '+', '-' }
    if (lookahead == '$' || lookahead == ')' || lookahead == '+' || lookahead == '-') {
        // Produção epsilon
        return 1;
    }

    return 0;
}

// --------------------------------------------------------------------
// F -> ( X ) 
//    | - N
//    | N
// --------------------------------------------------------------------
int F(char palavra[], int *pos) {
    if (lookahead == '(') {
        // F -> ( X )
        if (!match('(', palavra, pos)) {
            return 0;
        }
        if (!X(palavra, pos)) {
            return 0;
        }
        if (!match(')', palavra, pos)) {
            return 0;
        }
        return 1;
    } 
    else if (lookahead == '-') {
        // F -> - N
        if (!match('-', palavra, pos)) {
            return 0;
        }
        if (!N(palavra, pos)) {
            return 0;
        }
        return 1;
    }
    else {
        // F -> N
        if (isdigit(lookahead)) {
            if (!N(palavra, pos)) {
                return 0;
            }
            return 1;
        }
        return 0;
    }
}

// --------------------------------------------------------------------
// N -> [0-9] D
// --------------------------------------------------------------------
int N(char palavra[], int *pos) {
    if (isdigit(lookahead)) {
        // Casa um dígito
        if (!match(lookahead, palavra, pos)) {
            return 0;
        }
        // Em seguida, chama D
        if (!D(palavra, pos)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

// --------------------------------------------------------------------
// D -> [0-9] D
//    | ε
// --------------------------------------------------------------------
int D(char palavra[], int *pos) {
    // FIRST(D) = { [0-9], ε }
    // FOLLOW(D) = { '*', '/', '$', ')', '+', '-' }
    //
    if (isdigit(lookahead)) {
        // Casa o dígito
        if (!match(lookahead, palavra, pos)) {
            return 0;
        }
        // Continua recursivamente
        if (!D(palavra, pos)) {
            return 0;
        }
        return 1;
    }

    // Se não for dígito, pode ser ε se lookahead estiver em FOLLOW(D)
    if (lookahead == '*' || lookahead == '/' || lookahead == '$' || 
        lookahead == ')' || lookahead == '+' || lookahead == '-') {
        return 1; // Produção epsilon
    }

    return 0;
}

// --------------------------------------------------------------------
// Função principal
// --------------------------------------------------------------------
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

    // Remover possível quebra de linha do final:
    size_t len = strlen(palavra);
    if (len > 0 && palavra[len - 1] == '\n') {
        palavra[len - 1] = '\0';
        len--;
    }

    // Se não houver '$' no final, garantimos que tenha.
    if (len > 0 && palavra[len - 1] != '$') {
        palavra[len]   = '$';
        palavra[len+1] = '\0';
    }

    // Posição inicial do analisador
    int pos = 0;

    // Inicializa o lookahead
    lookahead = palavra[pos];

    printf("Expressão a ser analisada: %s\n", palavra);

    // Chama o símbolo inicial E
    if (E(palavra, &pos)) {
        // Verifica se realmente consumiu tudo até o '$'
        printf("\nExpressão reconhecida com sucesso!\n\n");
    } else {
        // Se E() retornar 0, pode ser que houve falha em algum match()
        // ou o lookahead não ficou em '$' no final.
        trataErro(palavra, pos);
    }

    return 0;
}