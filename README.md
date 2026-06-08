# G_30_ARVORES_2026.1

## Informações do Projeto

| Campo | Detalhes |
|-------|----------|
| **Disciplina** | Estruturas de Dados 2 |
| **Atividade** | Trabalho 2 — Árvores |
| **Linguagem** | C |
| **Tema** | Sistema de Ranking de Jogadores usando Árvore AVL e Árvore Rubro-Negra |

## Alunos

| Nome | Matrícula |
|------|-----------|
| Patrick Anderson Carvalho dos Santos | 211030620 |
| Vinicius Castelo Ferreira | 200062883 |

---

## Sobre

Este projeto foi desenvolvido como estudo prático das estruturas de árvores balanceadas AVL e Rubro-Negra, com o objetivo de observar seu funcionamento, suas características de balanceamento e a diferença de comportamento entre elas. Para tornar a implementação mais interessante, o sistema foi construído no contexto de um ranking de jogadores de futebol, aproveitando o clima de Copa do Mundo: cada jogador possui ID, nome e pontuação, e as duas árvores são mantidas em paralelo, permitindo cadastrar jogadores, exibir o ranking em ordem decrescente e acompanhar em tempo real as rotações, recolorações e estatísticas de cada estrutura como número de comparações, rotações realizadas e tempo de execução evidenciando na prática como AVL e Rubro-Negra garantem buscas eficientes e balanceamento mesmo com grandes volumes de dados.

---

## Arquitetura

```
trabalho2/
├── include/
│   ├── player.h     ← Struct Player, TreeStats e protótipos de I/O
│   ├── avl.h        ← Definições e protótipos da Árvore AVL
│   └── rb.h         ← Definições e protótipos da Árvore Rubro-Negra
├── src/
│   ├── main.c       ← Menu principal e controle do programa
│   ├── player.c     ← Leitura e escrita de jogadores em arquivo
│   ├── avl.c        ← Implementação da Árvore AVL
│   └── rb.c         ← Implementação da Árvore Rubro-Negra
├── ranking.txt      ← Arquivo de persistência dos jogadores
└── README.md
```

---

## Estrutura Base

### Estruturas de Dados

#### `Player` (registro do jogador)
```c
typedef struct {
    int id;
    char name[50];
    int score;
} Player;
```

#### `TreeStats` (estatísticas de desempenho)
```c
typedef struct {
    int height;
    int rotations;
    int comparisons;
    double execution_time;
} TreeStats;
```

#### `AVLNode` (nó da árvore AVL)
```c
typedef struct AVLNode {
    Player player;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;
```

#### `RBNode` (nó da árvore Rubro-Negra)
```c
typedef struct RBNode {
    Player player;
    NodeColor color;       // RED ou BLACK
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
} RBNode;
```

### Funções Implementadas — AVL

| Função | Descrição | Complexidade |
|--------|-----------|:------------:|
| `insert_avl()` | Insere jogador e rebalanceia com rotações | O(log n) |
| `print_avl_tree()` | Exibe a árvore hierarquicamente no terminal | O(n) |
| `display_avl_ranking()` | Lista jogadores em ordem decrescente de pontuação | O(n) |
| `free_avl()` | Desaloca todos os nós da árvore | O(n) |

### Funções Implementadas — Rubro-Negra

| Função | Descrição | Complexidade |
|--------|-----------|:------------:|
| `init_rb_nil()` | Cria o nó sentinela NIL (sempre preto) | O(1) |
| `insert_rb()` | Insere jogador e aplica fixup de coloração | O(log n) |
| `display_rb_ranking()` | Lista jogadores em ordem decrescente de pontuação | O(n) |
| `free_rb()` | Desaloca todos os nós da árvore | O(n) |

### Critério de Ordenação

Ambas as árvores ordenam pelos mesmos critérios, em cascata:

1. **Pontuação** (score) — critério primário
2. **ID** — critério de desempate

### Visualização no Terminal

A visualização exibe apenas o **ID** de cada jogador dentro dos nós para manter a legibilidade. Para ver nome e pontuação completos, use a opção **2 — Tabela de Ranking** no menu.

```
                +-----+
                | 008 |
               /       \
          +-----+     +-----+
          | 016 |     | 004 |
          +-----+     +-----+
```

O log de status exibe em tempo real qual rotação foi executada e quais nós foram afetados. Na Árvore AVL, são identificados os quatro casos clássicos de desbalanceamento: Esquerda-Esquerda (LL), Direita-Direita (RR), Esquerda-Direita (LR) e Direita-Esquerda (RL), cada um resolvido com a rotação correspondente. Na Árvore Rubro-Negra, o log indica qual caso do fixup foi aplicado após a inserção, seja uma recoloração dos nós ou uma rotação simples e dupla, mantendo as propriedades de coloração da árvore.

---

## Compilação

> Os comandos abaixo devem ser executados dentro da pasta `src/`.

**Compilar:**
```bash
gcc main.c player.c avl.c rb.c -I../include -o ranking_copa
```

**Executar:**
```bash
./ranking_copa
```

**Compilar e executar:**
```bash
gcc main.c player.c avl.c rb.c -I../include -o ranking_copa && ./ranking_copa
```

---

## Observações

- O painel de estatísticas (opção 5) exibe comparações, rotações e tempo de execução de cada estrutura lado a lado.
- A simulação em massa (opção 4) insere 1000 jogadores aleatórios para teste de estresse das estruturas.