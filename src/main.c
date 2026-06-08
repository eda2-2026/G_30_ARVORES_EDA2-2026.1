#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "player.h"
#include "avl.h"
#include "rb.h"

#define MAX_PLAYERS 5000

typedef enum { VIEW_AVL, VIEW_RB } ActiveView;

int main() {
    Player player_list[MAX_PLAYERS];
    int player_count = 0;

    AVLNode *avl_root = NULL;
    RBNode *rb_nil  = init_rb_nil();
    RBNode *rb_root = rb_nil;

    TreeStats avl_stats = {0, 0, 0, 0.0};
    TreeStats rb_stats  = {0, 0, 0, 0.0};

    ActiveView current_view = VIEW_AVL;
    char rotation_log[200] = "Nenhuma alteracao estrutural recente.";

    player_count = load_players("ranking.txt", player_list, MAX_PLAYERS);
    for (int i = 0; i < player_count; i++) {
        avl_root = insert_avl(avl_root, player_list[i], &avl_stats, rotation_log);
        rb_root  = insert_rb(rb_root, rb_nil, player_list[i], &rb_stats, rotation_log);
    }

    int choice;
    do {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        printf("========================================================\n");
        printf("  SISTEMA DE RANKING DE JOGADORES - COPA DO MUNDO\n");
        printf("========================================================\n");
        printf("VISAO ATUAL: ");
        if (current_view == VIEW_AVL) printf("[ ARVORE AVL ]\n");
        else                          printf("[ ARVORE RUBRO-NEGRA ]\n");
        printf("--------------------------------------------------------\n");

        printf("Estrutura Hierarquica:\n\n");
        if (current_view == VIEW_AVL) print_avl_tree(avl_root);
        else                          print_rb_hierarchical(rb_root, rb_nil, 0);

        printf("\n--------------------------------------------------------\n");
        printf("STATUS: %s\n", rotation_log);
        printf("--------------------------------------------------------\n");
        printf("1. Cadastrar Novo Jogador\n");
        printf("2. Exibir Tabela de Ranking (Ordem Decrescente)\n");
        printf("3. Alternar Estrutura Visualizada\n");
        printf("4. Simular Insercao em Massa (1000 jogadores)\n");
        printf("5. Exibir Painel Comparativo de Estatisticas\n");
        printf("6. Salvar dados e Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &choice);

        if (choice == 1) {
            if (player_count >= MAX_PLAYERS) {
                printf("Capacidade maxima atingida!\n");
                continue;
            }
            Player p;
            printf("Digite o ID do jogador: ");
            scanf("%d", &p.id);
            printf("Digite o nome do jogador: ");
            scanf("%49s", p.name);
            printf("Digite a pontuacao na Copa: ");
            scanf("%d", &p.score);

            player_list[player_count++] = p;

            clock_t start = clock();
            avl_root = insert_avl(avl_root, p, &avl_stats, rotation_log);
            avl_stats.execution_time += (double)(clock() - start) / CLOCKS_PER_SEC;

            start = clock();
            rb_root = insert_rb(rb_root, rb_nil, p, &rb_stats, rotation_log);
            rb_stats.execution_time += (double)(clock() - start) / CLOCKS_PER_SEC;

        } else if (choice == 2) {
            printf("\n--- CLASSIFICACAO DA COPA DO MUNDO ---\n");
            if (current_view == VIEW_AVL) display_avl_ranking(avl_root);
            else                          display_rb_ranking(rb_root, rb_nil);
            printf("\nPressione ENTER para voltar...");
            getchar(); getchar();

        } else if (choice == 3) {
            current_view = (current_view + 1) % 2;
            strcpy(rotation_log, "Visao da estrutura alternada.");

        } else if (choice == 4) {
            printf("Inserindo 1000 jogadores aleatorios...\n");
            for (int i = 0; i < 1000; i++) {
                if (player_count >= MAX_PLAYERS) break;
                Player p;
                p.id = 100 + player_count;
                sprintf(p.name, "Jogador_%d", player_count);
                p.score = rand() % 10000;
                player_list[player_count++] = p;
                avl_root = insert_avl(avl_root, p, &avl_stats, rotation_log);
                rb_root  = insert_rb(rb_root, rb_nil, p, &rb_stats, rotation_log);
            }
            strcpy(rotation_log, "Carga em massa finalizada! Confira o Painel de Estatisticas.");

        } else if (choice == 5) {
            printf("\n=======================================================\n");
            printf("  PAINEL DE PERFORMANCE COMPARATIVA DAS ARVORES\n");
            printf("=======================================================\n");
            printf("Metrica            | AVL          | Rubro-Negra  \n");
            printf("-------------------------------------------------------\n");
            printf("Comparacoes        | %-12d | %-12d\n", avl_stats.comparisons, rb_stats.comparisons);
            printf("Rotacoes Efetuadas | %-12d | %-12d\n", avl_stats.rotations, rb_stats.rotations);
            printf("Tempo Exec. (s)    | %-12.6f | %-12.6f\n", avl_stats.execution_time, rb_stats.execution_time);
            printf("=======================================================\n");
            printf("Pressione ENTER para voltar...");
            getchar(); getchar();
        }

    } while (choice != 6);

    save_players("ranking.txt", player_list, player_count);
    free_avl(avl_root);
    free_rb(rb_root, rb_nil);
    free(rb_nil);

    return 0;
}