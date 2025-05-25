Projeto 2 – Analisador Sintático Recursivo Descendente  
Disciplina: Linguagens Formais e Autômatos – 2025-1  
Instituição: [Nome da Instituição, se necessário]

Autores:
- Felipe Ujvari Gasparino de Sousa - 10418415
- Gustavo Nascimento Siqueira - 10419057
- Thomaz de Souza Scopel - 10417183
- Vinicius Cappatti - 10418266

------------------------------------------------------------
📌 Objetivo do Projeto:
------------------------------------------------------------
Implementar em linguagem C (padrão ANSI) um analisador sintático recursivo descendente, baseado em uma gramática fornecida, para validar expressões numéricas compostas pelos operadores:
- Adição (+)
- Subtração (-) — unário e binário
- Multiplicação (*)
- Divisão (/)
- Parênteses e números inteiros

------------------------------------------------------------
📚 Gramática Implementada:
------------------------------------------------------------
A gramática considerada no analisador segue o seguinte formato:

    E → X $
    X → T K
    K → + T K | - T K | ε
    T → F Z
    Z → * F Z | / F Z | ε
    F → ( X ) | N | - N
    N → [0-9] D
    D → [0-9] D | ε

------------------------------------------------------------
🛠️ Como Compilar e Executar:
------------------------------------------------------------

1. Abrir o projeto no **Dev-C++** (ou outro compilador C ANSI).
2. Certifique-se de que o arquivo `entrada.txt` esteja na mesma pasta do executável.
3. Compilar o código `analisador.c`.
4. Executar o programa.
5. O programa lerá cada linha do `entrada.txt`, analisará a expressão e indicará se ela é **válida ou inválida**.

------------------------------------------------------------
📂 Estrutura Esperada do Arquivo entrada.txt:
------------------------------------------------------------
Cada linha deve conter **uma expressão numérica**.

Exemplo de testes:

    -(4+3)*5$       ← inválida pois a gramática não aceita que se tenha um '-' seguido de '('
    5+6*2$
    5+6*2           ← inválida
    (1+2)*(3-4)/5$
    9$
    -8+4$
    (2+(3*4))/5$
    ((7))$
    3+*4$           ← inválida
    5+$             ← inválida
    -)$             ← inválida

------------------------------------------------------------
🧪 Saída Esperada:
------------------------------------------------------------
O arquivo lê a palavra contida no arquivo `entrada.txt` e retorna

```
Palavra <palavra_de_entrada> reconhecida com sucesso!
```

em caso de palavra aceita ou

```
ERRO DE SINTAXE na posicao <posicao_do_erro>. Caractere inesperado: <caractere_causador_do_erro>
```

em caso de uma entrada que não seja aceita pela gramática.
------------------------------------------------------------
📌 Observações Finais:
------------------------------------------------------------
- O projeto respeita os nomes dos não-terminais da gramática.
- O código foi testado com múltiplas expressões válidas e inválidas.
- O analisador lida com precedência de operadores e parênteses corretamente.

------------------------------------------------------------

