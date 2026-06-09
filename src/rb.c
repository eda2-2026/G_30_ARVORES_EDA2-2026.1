#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rb.h"

/* ─── Nó sentinela NIL ───────────────────────────────────────────────────── */

RBNode* init_rb_nil() {
    RBNode *nil = (RBNode*)malloc(sizeof(RBNode));
    nil->color  = BLACK;
    nil->left   = nil->right = nil->parent = nil;
    nil->player.id    = 0;
    nil->player.score = 0;
    nil->player.name[0] = '\0';
    return nil;
}

/* ─── Comparação (mesmo critério da AVL: score desc, id asc) ─────────────── */

static int cmp_rb(Player a, Player b) {
    if (a.score != b.score) return a.score - b.score;
    return a.id - b.id;
}

/* ─── Rotações ───────────────────────────────────────────────────────────── */

static void rotate_left_rb(RBNode **root, RBNode *nil, RBNode *x,
                            TreeStats *stats, char *log_buffer) {
    RBNode *y = x->right;
    x->right  = y->left;

    if (y->left != nil) y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == nil)              *root    = y;
    else if (x == x->parent->left)    x->parent->left  = y;
    else                               x->parent->right = y;

    y->left   = x;
    x->parent = y;

    stats->rotations++;
    strcpy(log_buffer, "ROTAÇÃO RB: Rotação Esquerda executada com sucesso!");
}

static void rotate_right_rb(RBNode **root, RBNode *nil, RBNode *y,
                             TreeStats *stats, char *log_buffer) {
    RBNode *x = y->left;
    y->left   = x->right;

    if (x->right != nil) x->right->parent = y;

    x->parent = y->parent;
    if (y->parent == nil)              *root    = x;
    else if (y == y->parent->left)    y->parent->left  = x;
    else                               y->parent->right = x;

    x->right  = y;
    y->parent = x;

    stats->rotations++;
    strcpy(log_buffer, "ROTAÇÃO RB: Rotação Direita executada com sucesso!");
}

/* ─── Fixup após inserção ────────────────────────────────────────────────── */

static void insert_fixup(RBNode **root, RBNode *nil, RBNode *z,
                         TreeStats *stats, char *log_buffer) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right; /* tio */
            if (y->color == RED) {
                /* Caso 1: tio vermelho → recolorir */
                z->parent->color          = BLACK;
                y->color                  = BLACK;
                z->parent->parent->color  = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    /* Caso 2: z é filho direito → rotação esquerda no pai */
                    z = z->parent;
                    rotate_left_rb(root, nil, z, stats, log_buffer);
                }
                /* Caso 3: z é filho esquerdo → recolorir + rotação direita */
                z->parent->color         = BLACK;
                z->parent->parent->color = RED;
                rotate_right_rb(root, nil, z->parent->parent, stats, log_buffer);
                strcpy(log_buffer, "ROTAÇÃO RB: Dupla (Esq→Dir) aplicada com sucesso!");
            }
        } else {
            /* Espelho: pai é filho direito do avô */
            RBNode *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color          = BLACK;
                y->color                  = BLACK;
                z->parent->parent->color  = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right_rb(root, nil, z, stats, log_buffer);
                }
                z->parent->color         = BLACK;
                z->parent->parent->color = RED;
                rotate_left_rb(root, nil, z->parent->parent, stats, log_buffer);
                strcpy(log_buffer, "ROTAÇÃO RB: Dupla (Dir→Esq) aplicada com sucesso!");
            }
        }
    }
    (*root)->color = BLACK;
}

/* ─── Inserção pública ───────────────────────────────────────────────────── */

RBNode* insert_rb(RBNode *root, RBNode *nil, Player player,
                  TreeStats *stats, char *log_buffer) {
    /* Cria novo nó */
    RBNode *z    = (RBNode*)malloc(sizeof(RBNode));
    z->player    = player;
    z->color     = RED;
    z->left      = z->right = z->parent = nil;

    /* BST insert */
    RBNode *y = nil;
    RBNode *x = root;
    while (x != nil) {
        y = x;
        stats->comparisons++;
        if (cmp_rb(z->player, x->player) < 0) x = x->left;
        else                                    x = x->right;
    }
    z->parent = y;
    if (y == nil)                          root     = z;
    else if (cmp_rb(z->player, y->player) < 0) y->left  = z;
    else                                        y->right = z;

    insert_fixup(&root, nil, z, stats, log_buffer);
    return root;
}

/* ─── Exibição do ranking (in-order decrescente) ─────────────────────────── */

void display_rb_ranking(RBNode *root, RBNode *nil) {
    if (root == nil || root == NULL) return;
    display_rb_ranking(root->right, nil);
    printf("ID: %03d | Nome: %-15s | Pontuação: %d\n",
           root->player.id, root->player.name, root->player.score);
    display_rb_ranking(root->left, nil);
}

/* ─── Liberação de memória ───────────────────────────────────────────────── */

void free_rb(RBNode *root, RBNode *nil) {
    if (root == nil || root == NULL) return;
    free_rb(root->left,  nil);
    free_rb(root->right, nil);
    free(root);
}

/* ══════════════════════════════════════════════════════════════════════════ *
 *  Desenho gráfico da árvore RB no terminal                                 *
 *  Estilo idêntico à AVL: caixas +---+, linhas com / e \                    *
 *  Cada nó exibe: [R:001] ou [B:001]  (cor + ID)                            *
 * ══════════════════════════════════════════════════════════════════════════ */

#define RB_NODE_W 9   /* "[R:001]" = 7 chars + 1 espaço cada lado */

typedef struct RBPrintBuf {
    char **lines;
    int width;
    int height;
} RBPrintBuf;

static RBPrintBuf rb_make_buf(int h, int w) {
    RBPrintBuf b; b.height = h; b.width = w;
    b.lines = (char**)malloc(h * sizeof(char*));
    for (int i = 0; i < h; i++) {
        b.lines[i] = (char*)malloc(w + 2);
        memset(b.lines[i], ' ', w);
        b.lines[i][w] = '\0';
    }
    return b;
}

static void rb_free_buf(RBPrintBuf b) {
    for (int i = 0; i < b.height; i++) free(b.lines[i]);
    free(b.lines);
}

static void rb_paste(RBPrintBuf *dst, RBPrintBuf src, int row, int col) {
    for (int i = 0; i < src.height; i++) {
        int r = row + i; if (r >= dst->height) break;
        for (int j = 0; j < src.width; j++) {
            int c = col + j; if (c >= dst->width) break;
            if (src.lines[i][j] != ' ') dst->lines[r][c] = src.lines[i][j];
        }
    }
}

static void rb_write(RBPrintBuf *b, int row, int col, const char *s) {
    for (int i = 0; s[i] && col + i < b->width; i++)
        b->lines[row][col + i] = s[i];
}

static int rb_subtree_width(RBNode *node, RBNode *nil) {
    if (!node || node == nil) return 0;
    int lw = rb_subtree_width(node->left,  nil);
    int rw = rb_subtree_width(node->right, nil);
    int ch = lw + rw + (lw > 0 && rw > 0 ? 3 : (lw > 0 || rw > 0 ? 2 : 0));
    return ch > RB_NODE_W ? ch : RB_NODE_W;
}

static int rb_subtree_height(RBNode *node, RBNode *nil) {
    if (!node || node == nil) return 0;
    int lh = rb_subtree_height(node->left,  nil);
    int rh = rb_subtree_height(node->right, nil);
    int ch = lh > rh ? lh : rh;
    return ch == 0 ? 3 : ch + 4;
}

static RBPrintBuf build_rb_buf(RBNode *node, RBNode *nil) {
    if (!node || node == nil) {
        RBPrintBuf b = rb_make_buf(1, 1); return b;
    }

    int lw = rb_subtree_width(node->left,  nil);
    int rw = rb_subtree_width(node->right, nil);
    int total_w = lw + rw + (lw > 0 && rw > 0 ? 3 : (lw > 0 || rw > 0 ? 2 : 0));
    if (total_w < RB_NODE_W) total_w = RB_NODE_W;

    int lh = (node->left  != nil) ? rb_subtree_height(node->left,  nil) : 0;
    int rh = (node->right != nil) ? rb_subtree_height(node->right, nil) : 0;
    int child_h = lh > rh ? lh : rh;
    int total_h = 3 + (child_h > 0 ? child_h + 1 : 0);

    RBPrintBuf buf = rb_make_buf(total_h, total_w + 2);

    /* Label: "R:001" ou "B:001" */
    char label[10];
    snprintf(label, sizeof(label), " %c:%03d ",
             node->color == RED ? 'R' : 'B', node->player.id);
    int box_w  = (int)strlen(label) + 2;  /* +2 para os '|' */
    int box_col = (total_w - box_w) / 2;

    char top[12];
    top[0] = '+';
    for (int i = 1; i < box_w - 1; i++) top[i] = '-';
    top[box_w - 1] = '+'; top[box_w] = '\0';

    rb_write(&buf, 0, box_col, top);

    char mid[12];
    snprintf(mid, sizeof(mid), "|%s|", label);
    rb_write(&buf, 1, box_col, mid);

    rb_write(&buf, 2, box_col, top);

    int node_center = box_col + box_w / 2;

    if (node->left != nil) {
        RBPrintBuf lb = build_rb_buf(node->left, nil);
        int lc = lw / 2;
        for (int c = lc + 1; c < node_center; c++) buf.lines[3][c] = '_';
        buf.lines[3][lc] = '/';
        rb_paste(&buf, lb, 4, 0);
        rb_free_buf(lb);
    }

    if (node->right != nil) {
        RBPrintBuf rb2 = build_rb_buf(node->right, nil);
        int right_start = lw > 0 ? lw + 3 : lw + 2;
        int rc = right_start + rw / 2;
        for (int c = node_center + 1; c < rc; c++) buf.lines[3][c] = '_';
        buf.lines[3][rc] = '\\';
        rb_paste(&buf, rb2, 4, right_start);
        rb_free_buf(rb2);
    }

    if ((node->left != nil || node->right != nil) && buf.lines[3][node_center] == ' ')
        buf.lines[3][node_center] = '|';

    return buf;
}

void print_rb_hierarchical(RBNode *root, RBNode *nil, int space) {
    (void)space; /* não usamos mais o modo indentado */
    if (!root || root == nil) {
        printf("\n  [Arvore Rubro-Negra vazia]\n");
        return;
    }
    printf("  (R = Vermelho, B = Preto | cada no exibe cor:ID. Use opcao 2 para nome e pontuacao)\n\n");
    RBPrintBuf buf = build_rb_buf(root, nil);
    for (int i = 0; i < buf.height; i++) printf("  %s\n", buf.lines[i]);
    rb_free_buf(buf);
}