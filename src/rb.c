#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rb.h"

RBNode* init_rb_nil() {
    RBNode *nil = (RBNode*)malloc(sizeof(RBNode));
    nil->color = BLACK;
    nil->left = nil->right = nil->parent = nil;
    return nil;
}

RBNode* insert_rb(RBNode *root, RBNode *nil, Player player, TreeStats *stats, char *log_buffer) {
    /* TODO: implementar insercao com fixup RB */
    (void)player; (void)stats;
    strcpy(log_buffer, "RB: insercao ainda nao implementada.");
    return root;
}

void print_rb_hierarchical(RBNode *root, RBNode *nil, int space) {
    /* TODO: implementar exibicao hierarquica */
    (void)root; (void)nil; (void)space;
    printf("\n  [Arvore RB ainda nao implementada]\n");
}

void display_rb_ranking(RBNode *root, RBNode *nil) {
    /* TODO: implementar exibicao do ranking */
    (void)root; (void)nil;
    printf("  [Arvore RB ainda nao implementada]\n");
}

void free_rb(RBNode *root, RBNode *nil) {
    if (root == nil || root == NULL) return;
    free_rb(root->left, nil);
    free_rb(root->right, nil);
    free(root);
}