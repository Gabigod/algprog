#include <raylib.h>    // Biblioteca gr�fica para criar janelas, desenhar, etc.
#include <stdio.h>     // Entrada e sa�da padr�o (printf, scanf, FILE, etc.)
#include <stdlib.h>    // Fun��es gerais (como rand, malloc)
#include <string.h>    // Para manipula��o de strings

// ---------- CONSTANTES ----------
#define LARGURA 1200         // Largura da janela
#define ALTURA 800           // Altura da �rea de jogo (sem contar a barra superior)
#define ALTURA_BARRA 60      // Altura da barra de cima
#define LINHAS 16            // Quantidade de linhas do mapa
#define COLUNAS 24           // Quantidade de colunas do mapa
#define LADO (LARGURA / COLUNAS) // Tamanho de cada c�lula do mapa
#define MAX_SCORES 5         // Quantos highscores ser�o armazenados

// ---------- STRUCTS ----------

// Estrutura que guarda o nome e a pontua��o de um jogador
typedef struct {
    char nome[20];
    int pontos;
} Highscore;

// Vetor global que guarda os 5 melhores scores
Highscore highscores[MAX_SCORES];

// ---------- FUN��ES DE HIGHSCORES ----------

// Carrega highscores de um arquivo .txt
void carregarHighscores() {
    FILE *arquivo = fopen("highscores.txt", "r");
    if (arquivo) {
        for (int i = 0; i < MAX_SCORES; i++) {
            fscanf(arquivo, "%s %d", highscores[i].nome, &highscores[i].pontos);
        }
        fclose(arquivo);
    } else {
        // Se o arquivo n�o existir, inicializa com valores padr�o
        for (int i = 0; i < MAX_SCORES; i++) {
            highscores[i].pontos = 0;
            strcpy(highscores[i].nome, "---");
        }
    }
}

// Salva os highscores no arquivo .txt
void salvarHighscores() {
    FILE *arquivo = fopen("highscores.txt", "w");
    for (int i = 0; i < MAX_SCORES; i++) {
        fprintf(arquivo, "%s %d\n", highscores[i].nome, highscores[i].pontos);
    }
    fclose(arquivo);
}

// Insere uma nova pontua��o e ordena os scores do maior para o menor
void inserirHighscore(const char* nome, int pontos) {
    Highscore novo;
    strncpy(novo.nome, nome, 19);
    novo.nome[19] = '\0';
    novo.pontos = pontos;

    highscores[MAX_SCORES - 1] = novo;

    // Ordena os scores usando "bubble sort" invertido
    for (int i = MAX_SCORES - 1; i > 0; i--) {
        if (highscores[i].pontos > highscores[i - 1].pontos) {
            Highscore temp = highscores[i];
            highscores[i] = highscores[i - 1];
            highscores[i - 1] = temp;
        }
    }
}

// ---------- STRUCTS DO JOGO ----------

// Guarda uma posi��o no mapa (coordenadas)
typedef struct {
    int x, y;
} vetor;

// Representa o jogador
typedef struct {
    vetor jogadorLoc;
    char orientacao;
    int pts;     // Pontua��o
    int vida;    // Vidas
    int espada;  // Tem espada ou n�o (0 ou 1)
} personagem;

// Define os estados/telas do jogo
typedef enum GameScreen { MENU = 0, GAMEPLAY, HIGHSCORES, EXIT } GameScreen;

// ---------- FUN��ES DO MAPA/JOGO ----------

// Carrega o mapa a partir de um arquivo texto e preenche uma matriz
int carrega_mapa(char mapa[LINHAS][COLUNAS], char* nome) {
    FILE* arquivo = fopen(nome, "r");
    if (arquivo == NULL) return 1;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            int c;
            do {
                c = fgetc(arquivo);
                if (c == EOF) { fclose(arquivo); return 1; }
            } while (c == '\n');
            mapa[i][j] = (char)c;
        }
    }

    fclose(arquivo);
    return 0;
}

// Desenha os elementos do mapa na tela com base nos caracteres do arquivo
void IniciaMapa(char mapa[LINHAS][COLUNAS]) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            switch (mapa[i][j]) {
                case 'P': DrawRectangle(j * LADO, i * LADO + ALTURA_BARRA, LADO, LADO, BROWN); break;
                case 'V': DrawRectangle(j * LADO, i * LADO + ALTURA_BARRA, LADO, LADO, MAROON); break;
                case 'E': DrawRectangle(j * LADO, i * LADO + ALTURA_BARRA, LADO, LADO, GRAY); break;
            }
        }
    }
}

// Define a posi��o inicial do jogador com base na letra 'J' no mapa
int IniciaPosicaoJogador(char matriz[LINHAS][COLUNAS], personagem* jogador) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (matriz[i][j] == 'J') {
                jogador->jogadorLoc.x = j * LADO;
                jogador->jogadorLoc.y = i * LADO + ALTURA_BARRA;
                return 0;
            }
        }
    }
    return 1;
}

// Verifica se o jogador pode se mover para a dire��o desejada
int deveMover(char mapa[LINHAS][COLUNAS], int x, int y, int dx, int dy) {
    int novoX = x + dx;
    int novoY = y + dy;

    if (novoX < 0 || novoX + LADO > LARGURA) return 1;
    if (novoY < ALTURA_BARRA || novoY + LADO > ALTURA + ALTURA_BARRA) return 1;

    int lin = (novoY - ALTURA_BARRA) / LADO;
    int col = novoX / LADO;

    if (mapa[lin][col] == 'P') return 1;

    return 0;
}

// Move o jogador somando dx e dy � posi��o atual
void move(int dx, int dy, int *x, int *y) {
    *x += dx;
    *y += dy;
}

// L� o teclado e atualiza a posi��o do jogador
int AtualizaJogador(personagem* jogador, char matriz[LINHAS][COLUNAS]) {
    if (IsKeyPressed(KEY_RIGHT) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, 0))
        move(LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
    if (IsKeyPressed(KEY_LEFT) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, -LADO, 0))
        move(-LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
    if (IsKeyPressed(KEY_UP) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, -LADO))
        move(0, -LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
    if (IsKeyPressed(KEY_DOWN) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, LADO))
        move(0, LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);

    DrawRectangle(jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, LADO, GREEN);
    return 0;
}

// ---------- FUN��O PRINCIPAL ----------
int main(void) {
    const int screenWidth = LARGURA;
    const int screenHeight = ALTURA + ALTURA_BARRA;

    carregarHighscores(); // Carrega os highscores no in�cio

    InitWindow(screenWidth, screenHeight, "ZINF");
    SetTargetFPS(60);  // Limita a 60 frames por segundo

    GameScreen currentScreen = MENU;  // Come�a no menu

    char mapa[LINHAS][COLUNAS];
    personagem jogador;
    int pontuacaoSalva = 0;

    // ---------- LOOP PRINCIPAL ----------
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Troca entre telas
        switch (currentScreen) {
            case MENU:
                DrawRectangle(0, 0, screenWidth, ALTURA_BARRA, BLACK);
                DrawText("MENU PRINCIPAL", 400, 100, 30, BLACK);
                DrawText("1 - Jogar", 450, 160, 20, DARKGRAY);
                DrawText("2 - Ver Highscores", 450, 200, 20, DARKGRAY);
                DrawText("3 - Sair", 450, 240, 20, DARKGRAY);

                if (IsKeyPressed(KEY_ONE)) {
                    carrega_mapa(mapa, "mapa.txt");
                    jogador.vida = 3;
                    jogador.espada = 0;
                    jogador.orientacao = 'S';
                    jogador.pts = GetRandomValue(0, 999); // Exemplo de pontos
                    IniciaPosicaoJogador(mapa, &jogador);
                    pontuacaoSalva = 0;
                    currentScreen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_TWO)) {
                    currentScreen = HIGHSCORES;
                } else if (IsKeyPressed(KEY_THREE)) {
                    currentScreen = EXIT;
                }
                break;

            case GAMEPLAY:
                DrawRectangle(0, 0, screenWidth, ALTURA_BARRA, BLACK);
                IniciaMapa(mapa);
                AtualizaJogador(&jogador, mapa);

                // ESC retorna ao menu e salva pontua��o
                if (IsKeyPressed(KEY_ESCAPE)) {
                    if (!pontuacaoSalva) {
                        inserirHighscore("Jogador", jogador.pts);
                        salvarHighscores();
                        pontuacaoSalva = 1;
                    }
                    currentScreen = MENU;
                }
                break;

            case HIGHSCORES:
                DrawRectangle(0, 0, screenWidth, ALTURA_BARRA, DARKGRAY);
                DrawText("HIGHSCORES", 400, 80, 30, BLACK);
                for (int i = 0; i < MAX_SCORES; i++) {
                    char texto[50];
                    sprintf(texto, "%d. %s - %d", i + 1, highscores[i].nome, highscores[i].pontos);
                    DrawText(texto, 400, 130 + i * 30, 20, BLACK);
                }
                DrawText("Pressione ESC para voltar", 400, 350, 20, RED);
                if (IsKeyPressed(KEY_ESCAPE)) currentScreen = MENU;
                break;

            case EXIT:
                CloseWindow(); // Fecha a janela
                return 0;
        }

        EndDrawing(); // Termina o frame
    }

    CloseWindow(); // Fecha ao sair do loop
    return 0;
}
