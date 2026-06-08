#ifndef RB_H
#define RB_H
#include "player.h"

typedef enum { RED, BLACK } NodeColor;

typedef struct RBNode {
    Player player;
    NodeColor color;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
} RBNode;

// Inicializa o nó sentinela nulo (NIL) que é sempre PRETO
RBNode* init_rb_nil();
RBNode* insert_rb(RBNode *root, RBNode *nil, Player player, TreeStats *stats, char *log_buffer);
void print_rb_hierarchical(RBNode *root, RBNode *nil, int space);
void display_rb_ranking(RBNode *root, RBNode *nil);
void free_rb(RBNode *root, RBNode *nil);

#endif