#include <stdio.h>
#include <stdlib.h>
#include "player.h"

void save_players(const char *filename, Player *players, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao abrir o arquivo para salvar!\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%d\n", players[i].id, players[i].name, players[i].score);
    }
    fclose(file);
    printf("Dados salvos com sucesso em %s!\n", filename);
}

int load_players(const char *filename, Player *players, int max_capacity) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0; // Arquivo não existe ou não pôde ser aberto
    }
    int count = 0;
    while (count < max_capacity && fscanf(file, "%d,%[^,],%d\n", &players[count].id, players[count].name, &players[count].score) == 3) {
        count++;
    }
    fclose(file);
    return count;
}