#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int id;
    char name[50];
    int score;
} Player;

// Estatisticas acumuladas de desempenho de cada arvore
typedef struct {
    int height;
    int rotations;
    int comparisons;
    double execution_time;
} TreeStats;

// Funcoes para salvar e carregar os dados em arquivo de texto simples (estilo CSV)
void save_players(const char *filename, Player *players, int count);
int load_players(const char *filename, Player *players, int max_capacity);

#endif