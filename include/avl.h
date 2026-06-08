#ifndef AVL_H
#define AVL_H
#include "player.h"

typedef struct AVLNode {
    Player player;
    struct AVLNode *left;
    struct AVLNode *right;
    int height; // Armazena a altura para calcular o balanceamento de forma eficiente
} AVLNode;

// Funções principais de manipulação da árvore AVL
AVLNode* insert_avl(AVLNode *root, Player player, TreeStats *stats, char *log_buffer);
void print_avl_hierarchical(AVLNode *root, int space);
void display_avl_ranking(AVLNode *root);
void free_avl(AVLNode *root);

#endif
// Desenho bonito da arvore AVL no terminal
void print_avl_tree(AVLNode *root);