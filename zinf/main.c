// ---------- BIBLIOTECAS ----------
#include <raylib.h>    // Biblioteca grafica para criar janelas, desenhar, etc.
#include <stdio.h>     // Entrada e saida padrao (printf, scanf, FILE, etc.)
#include <stdlib.h>    // Funcoes gerais (como rand, malloc)
#include <string.h>    // Para manipulacao de strings
#include <time.h>     // Para utilizar unix time stamp

// ---------- CONSTANTES ----------
#define LARGURA 1200         // Largura da janela
#define ALTURA 800           // Altura da area de jogo (sem contar a barra superior)
#define ALTURA_BARRA 60      // Altura da barra de cima
#define LINHAS 16            // Quantidade de linhas do mapa
#define COLUNAS 24           // Quantidade de colunas do mapa
#define LADO (LARGURA / COLUNAS) // Tamanho de cada celula do mapa
#define MAX_SCORES 5         // Quantos highscores serao armazenados
#define MONSTROS 10          // Quantos monstros podem ter por mapa   

// ---------- STRUCTS ----------
// Guarda uma posicao no mapa (coordenadas)
typedef struct {
    int x, y;
} vetor;

// Representa o jogador
typedef struct {
    vetor jogadorLoc;
    char orientacao;
    int pts;     // Pontuacao
    int vida;    // Vidas
    int espada;  // Tem espada ou nao (0 ou 1)
} personagem;

// Representa os monstros
typedef struct {
    vetor monstroLoc;
    char orientacao;
    int pts;
    int vida; 
}monstro;


// Define os estados/telas do jogo
typedef enum GameScreen { MENU = 0, GAMEPLAY, HIGHSCORES, EXIT } GameScreen;

// Estrutura que guarda o nome e a pontuacao de um jogador
typedef struct {
    char nome[20];
    int pontos;
} Highscore;

// ------------------ FUNCOES ----------------
// ---------- FUNCOES DE HIGHSCORES ----------

// Carrega highscores de um arquivo .txt
void carregarHighscores(Highscore highscores[MAX_SCORES]) {
    FILE *arquivo = fopen("highscores.txt", "r");
    if (arquivo) {
        for (int i = 0; i < MAX_SCORES; i++) {
            fscanf(arquivo, "%s %d", highscores[i].nome, &highscores[i].pontos);
        }
        fclose(arquivo);
    } else {
        // Se o arquivo nao existir, inicializa com valores padrao
        for (int i = 0; i < MAX_SCORES; i++) {
            highscores[i].pontos = 0;
            strcpy(highscores[i].nome, "---");
        }
    }
}

// Salva os highscores no arquivo .txt
void salvarHighscores(Highscore highscores[MAX_SCORES]) {
    FILE *arquivo = fopen("highscores.txt", "w");
    for (int i = 0; i < MAX_SCORES; i++) {
        fprintf(arquivo, "%s %d\n", highscores[i].nome, highscores[i].pontos);
    }
    fclose(arquivo);
}

// Insere uma nova pontuacao e ordena os scores do maior para o menor
void inserirHighscore(const char* nome, int pontos, Highscore highscores[MAX_SCORES]) {
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

// ---------- FUNCOES DO MAPA/JOGO ----------
// Sorteia um valor inteiro entre min e max e retorna ele
int sorteia(int min, int max){
    return rand() % (max - min) + min;
}

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

// Define a posicao inicial do jogador com base na letra 'J' no mapa
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

// Verifica se o jogador pode se mover para a direcao desejada
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

// Move o jogador somando dx e dy a posicao atual
void move(int dx, int dy, int *x, int *y) {
    *x += dx;
    *y += dy;
}

// Le o teclado e atualiza a posicao do jogador
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

//Inicializa os monstros
int iniciaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[]){
    int k = 0;
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            if(mapa[i][j] == 'M'){
                mobs[k].monstroLoc.x = j * LADO;
                mobs[k].monstroLoc.y = i * LADO + ALTURA_BARRA;
                mobs[k].pts = sorteia(1, 10);
                mobs[k].vida = 1;
                k++;

            }
        }
    }
    return k;
}

// Atualiza a posicao dos monstros
int atualizaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[MONSTROS], int len){
    for(int i = 0; i < len; i++){
        int *x = &mobs[i].monstroLoc.x;
        int *y = &mobs[i].monstroLoc.y;

        int dx = sorteia(-1, 1);
        int dy = sorteia(-1, 1);
        if(!deveMover(mapa, *x, *y, dx, dy)){
            move(dx, dy, x, y);
        }

        DrawRectangle(mobs[i].monstroLoc.x, mobs[i].monstroLoc.y, LADO, LADO, BLACK);
    }
    return 0;
}
// ---------- FUNCAO PRINCIPAL ----------
int main(void) {
    const int screenWidth = LARGURA;
    const int screenHeight = ALTURA + ALTURA_BARRA;
    monstro mobs[MONSTROS] = {0};
    int qtdMonstros = 0;
    Highscore highscores[MAX_SCORES]; // Vetor que guarda os 5 melhores scores

    carregarHighscores(highscores); // Carrega os highscores no inicio
    srand(time(NULL));  // Define a seed da função rand como unix time stamp

    InitWindow(screenWidth, screenHeight, "ZINF");
    SetTargetFPS(60);  // Limita a 60 frames por segundo

    GameScreen currentScreen = MENU;  // Comeca no menu

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
                    qtdMonstros = iniciaMonstros(mapa, mobs);
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
                atualizaMonstros(mapa, mobs, qtdMonstros);
                // ESC retorna ao menu e salva pontuacao
                if (IsKeyPressed(KEY_ESCAPE)) {
                    if (!pontuacaoSalva) {
                        inserirHighscore("Jogador", jogador.pts, highscores);
                        salvarHighscores(highscores);
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
