#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Função auxiliar para obter a altura de um nó de forma segura (trata nós nulos)
static int get_height(AVLNode *n) {
    return n ? n->height : 0;
}

// Retorna o fator de balanceamento: altura(esquerda) - altura(direita)
static int get_balance(AVLNode *n) {
    return n ? get_height(n->left) - get_height(n->right) : 0;
}

static int max_val(int a, int b) {
    return (a > b) ? a : b;
}

// Função de comparação unificada (Critério Copa do Mundo: Pontuação e depois ID)
static int compare_avl_players(Player p1, Player p2) {
    if (p1.score != p2.score) {
        return p1.score - p2.score; // Negativo se p1 < p2, Positivo se p1 > p2
    }
    return p1.id - p2.id;
}

// Rotação Simples à Direita (Caso LL)
static AVLNode* rotate_right(AVLNode *y, TreeStats *stats, char *log_buffer) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    // Executa a rotação estrutural
    x->right = y;
    y->left = T2;

    // Recalcula as alturas dos nós afetados
    y->height = max_val(get_height(y->left), get_height(y->right)) + 1;
    x->height = max_val(get_height(x->left), get_height(x->right)) + 1;

    stats->rotations++;
    strcpy(log_buffer, "ROTAÇÃO: Rotação Direita executada com sucesso!");
    return x;
}

// Rotação Simples à Esquerda (Caso RR)
static AVLNode* rotate_left(AVLNode *x, TreeStats *stats, char *log_buffer) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    // Executa a rotação estrutural
    y->left = x;
    x->right = T2;

    // Recalcula as alturas dos nós afetados
    x->height = max_val(get_height(x->left), get_height(x->right)) + 1;
    y->height = max_val(get_height(y->left), get_height(y->right)) + 1;

    stats->rotations++;
    strcpy(log_buffer, "ROTAÇÃO: Rotação Esquerda executada com sucesso!");
    return y;
}

// Inserção Completa na AVL com verificação de 4 Casos de Desbalanceamento
AVLNode* insert_avl(AVLNode *root, Player player, TreeStats *stats, char *log_buffer) {
    // 1. Inserção normal
    if (root == NULL) {
        AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
        node->player = player;
        node->left = node->right = NULL;
        node->height = 1; // Novo nó começa com altura igual a 1
        return node;
    }

    stats->comparisons++;
    if (compare_avl_players(player, root->player) < 0) {
        root->left = insert_avl(root->left, player, stats, log_buffer);
    } else {
        root->right = insert_avl(root->right, player, stats, log_buffer);
    }

    // 2. Atualiza a altura do nó pai atual
    root->height = 1 + max_val(get_height(root->left), get_height(root->right));

    // 3. Obtém o fator de balanceamento para verificar se há desvio das regras AVL
    int balance = get_balance(root);

    // Se o nó ficar desbalanceado, existem 4 casos possíveis:

    // Caso 1: Esquerda-Esquerda (LL) -> Requer Rotação Direita Simples
    if (balance > 1 && compare_avl_players(player, root->left->player) < 0) {
        return rotate_right(root, stats, log_buffer);
    }

    // Caso 2: Direita-Direita (RR) -> Requer Rotação Esquerda Simples
    if (balance < -1 && compare_avl_players(player, root->right->player) > 0) {
        return rotate_left(root, stats, log_buffer);
    }

    // Caso 3: Esquerda-Direita (LR) -> Requer Rotação Esquerda no Filho + Direita no Pai
    if (balance > 1 && compare_avl_players(player, root->left->player) > 0) {
        root->left = rotate_left(root->left, stats, log_buffer);
        strcpy(log_buffer, "ROTAÇÃO: Rotação Dupla Esquerda-Direita aplicada!");
        return rotate_right(root, stats, log_buffer);
    }

    // Caso 4: Direita-Esquerda (RL) -> Requer Rotação Direita no Filho + Esquerda no Pai
    if (balance < -1 && compare_avl_players(player, root->right->player) < 0) {
        root->right = rotate_right(root->right, stats, log_buffer);
        strcpy(log_buffer, "ROTAÇÃO: Rotação Dupla Direita-Esquerda aplicada!");
        return rotate_left(root, stats, log_buffer);
    }

    // Retorna o ponteiro do nó inalterado se ele já estiver balanceado
    return root;
}

void print_avl_hierarchical(AVLNode *root, int space) {
    if (root == NULL) return;
    space += 8;
    
    print_avl_hierarchical(root->right, space);
    
    printf("\n");
    for (int i = 8; i < space; i++) printf(" ");
    // Exibe ID, pontuação e a altura calculada do nó
    printf("[%d] P:%d (H:%d)\n", root->player.id, root->player.score, root->height);
    
    print_avl_hierarchical(root->left, space);
}

void display_avl_ranking(AVLNode *root) {
    if (root == NULL) return;
    display_avl_ranking(root->right);
    printf("ID: %03d | Nome: %-15s | Pontuação: %d\n", root->player.id, root->player.name, root->player.score);
    display_avl_ranking(root->left);
}

void free_avl(AVLNode *root) {
    if (root == NULL) return;
    free_avl(root->left);
    free_avl(root->right);
    free(root);
}

// ─── Desenho de arvore AVL no terminal ───────────────────────────────────────
// No exibe apenas o ID entre colchetes: [001]
// Com ate ~30 nos a arvore cabe em terminais padrao (160+ colunas)

#define AVL_NODE_W 7   // largura da caixa: "[" + 3 digitos + "]" + padding

typedef struct AVLPrintBuf {
    char **lines;
    int width;
    int height;
} AVLPrintBuf;

static AVLPrintBuf avl_make_buf(int h, int w) {
    AVLPrintBuf b; b.height = h; b.width = w;
    b.lines = (char**)malloc(h * sizeof(char*));
    for (int i = 0; i < h; i++) {
        b.lines[i] = (char*)malloc(w + 2);
        memset(b.lines[i], ' ', w);
        b.lines[i][w] = '\0';
    }
    return b;
}

static void avl_free_buf(AVLPrintBuf b) {
    for (int i = 0; i < b.height; i++) free(b.lines[i]);
    free(b.lines);
}

static void avl_paste(AVLPrintBuf *dst, AVLPrintBuf src, int row, int col) {
    for (int i = 0; i < src.height; i++) {
        int r = row + i; if (r >= dst->height) break;
        for (int j = 0; j < src.width; j++) {
            int c = col + j; if (c >= dst->width) break;
            if (src.lines[i][j] != ' ') dst->lines[r][c] = src.lines[i][j];
        }
    }
}

static void avl_write(AVLPrintBuf *b, int row, int col, const char *s) {
    for (int i = 0; s[i] && col + i < b->width; i++)
        b->lines[row][col + i] = s[i];
}

static int avl_subtree_width(AVLNode *node) {
    if (!node) return 0;
    int lw = avl_subtree_width(node->left);
    int rw = avl_subtree_width(node->right);
    int ch = lw + rw + (lw > 0 && rw > 0 ? 3 : (lw > 0 || rw > 0 ? 2 : 0));
    return ch > AVL_NODE_W ? ch : AVL_NODE_W;
}

static int avl_subtree_height(AVLNode *node) {
    if (!node) return 0;
    int lh = avl_subtree_height(node->left);
    int rh = avl_subtree_height(node->right);
    int ch = lh > rh ? lh : rh;
    return ch == 0 ? 3 : ch + 4;
}

static AVLPrintBuf build_avl_buf(AVLNode *node) {
    if (!node) { AVLPrintBuf b = avl_make_buf(1, 1); return b; }

    int lw = avl_subtree_width(node->left);
    int rw = avl_subtree_width(node->right);
    int total_w = lw + rw + (lw > 0 && rw > 0 ? 3 : (lw > 0 || rw > 0 ? 2 : 0));
    if (total_w < AVL_NODE_W) total_w = AVL_NODE_W;

    int lh = node->left  ? avl_subtree_height(node->left)  : 0;
    int rh = node->right ? avl_subtree_height(node->right) : 0;
    int child_h = lh > rh ? lh : rh;
    int total_h = 3 + (child_h > 0 ? child_h + 1 : 0);

    AVLPrintBuf buf = avl_make_buf(total_h, total_w + 2);

    // Caixa: linha 0 "+-----+", linha 1 "| 001 |", linha 2 "+-----+"
    char label[8];
    snprintf(label, sizeof(label), " %03d ", node->player.id);
    int box_w = (int)strlen(label) + 2;  // 7
    int box_col = (total_w - box_w) / 2;

    char top[10];
    top[0] = '+';
    for (int i = 1; i < box_w - 1; i++) top[i] = '-';
    top[box_w - 1] = '+'; top[box_w] = '\0';

    avl_write(&buf, 0, box_col, top);

    char mid[10];
    snprintf(mid, sizeof(mid), "|%s|", label);
    avl_write(&buf, 1, box_col, mid);

    avl_write(&buf, 2, box_col, top);

    int node_center = box_col + box_w / 2;

    if (node->left) {
        AVLPrintBuf lb = build_avl_buf(node->left);
        int lc = lw / 2;
        for (int c = lc + 1; c < node_center; c++) buf.lines[3][c] = '_';
        buf.lines[3][lc] = '/';
        avl_paste(&buf, lb, 4, 0);
        avl_free_buf(lb);
    }

    if (node->right) {
        AVLPrintBuf rb = build_avl_buf(node->right);
        int right_start = lw > 0 ? lw + 3 : lw + 2;
        int rc = right_start + rw / 2;
        if (node->left) {
            int lc = lw / 2;
            for (int c = lc + 1; c < node_center; c++)
                if (buf.lines[3][c] == ' ') buf.lines[3][c] = '_';
            if (buf.lines[3][lc] == ' ') buf.lines[3][lc] = '/';
        }
        for (int c = node_center + 1; c < rc; c++) buf.lines[3][c] = '_';
        buf.lines[3][rc] = '\\';
        avl_paste(&buf, rb, 4, right_start);
        avl_free_buf(rb);
    }

    if ((node->left || node->right) && buf.lines[3][node_center] == ' ')
        buf.lines[3][node_center] = '|';

    return buf;
}

void print_avl_tree(AVLNode *root) {
    if (!root) { printf("\n  [Arvore AVL vazia]\n"); return; }
    printf("  (cada no exibe o ID do jogador. Use a opcao 2 para ver nome e pontuacao)\n\n");
    AVLPrintBuf buf = build_avl_buf(root);
    for (int i = 0; i < buf.height; i++) printf("  %s\n", buf.lines[i]);
    avl_free_buf(buf);
}