// ---------- BIBLIOTECAS ----------
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// ---------- CONSTANTES ----------
#define LARGURA 1200
#define ALTURA 800
#define ALTURA_BARRA 60
#define LINHAS 16
#define COLUNAS 24
#define LADO (LARGURA / COLUNAS)
#define MAX_SCORES 5
#define MONSTROS 10
#define MAX_MAPAS 99




// ---------- STRUCTS ----------
typedef struct {
    int x, y;
} vetor;

typedef struct {
    vetor jogadorLoc;
    char orientacao;
    int pts;
    int vida;
    int espada;
    float tempoProximoMovimento;
} personagem;

typedef struct {
    vetor monstroLoc;
    char orientacao;
    int pts;
    int vida;
    float tempoProximoMovimento;
} monstro;

typedef enum GameScreen { MENU = 0, GAMEPLAY, HIGHSCORES, PAUSE, EXIT, VICTORY, ENTER_HIGHSCORE } GameScreen;

typedef struct {
    char nome[20];
    int pontos;
} Highscore;

// ------------------ PROTÓTIPOS DE FUNÇÕES ----------------
int carrega_mapa(char mapa[LINHAS][COLUNAS], const char* nome);
int IniciaPosicaoJogador(char matriz[LINHAS][COLUNAS], personagem* jogador);
int iniciaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[]);


int carregarFase(int fase, char mapa[LINHAS][COLUNAS], personagem* jogador, monstro mobs[MONSTROS], int* numMonstros, vetor* posicaoInicial) {
    char nomeMapa[20];
    sprintf(nomeMapa, "mapa%d.txt", fase);

    // Se o mapa não puder ser carregado, retorna falha
    if (carrega_mapa(mapa, nomeMapa) != 0) {
        return 1; // Falha
    }

    jogador->vida = 3;
    jogador->espada = 0;
    jogador->orientacao = 'S';
    jogador->tempoProximoMovimento = 0;
    IniciaPosicaoJogador(mapa, jogador);
    *posicaoInicial = jogador->jogadorLoc;
    *numMonstros = iniciaMonstros(mapa, mobs);

    return 0;
}

void verificaVidaColetada(char mapa[LINHAS][COLUNAS], personagem *jogador) {
    int lin = (jogador->jogadorLoc.y - ALTURA_BARRA) / LADO;
    int col = jogador->jogadorLoc.x / LADO;
    if (mapa[lin][col] == 'V') {
        jogador->vida++;
        mapa[lin][col] = '.';
    }
}

void carregarHighscores(Highscore highscores[MAX_SCORES]) {
    FILE *arquivo = fopen("ranking.bin", "rb");
    if (arquivo) {
        fread(highscores, sizeof(Highscore), MAX_SCORES, arquivo);
        fclose(arquivo);
    } else {
        for (int i = 0; i < MAX_SCORES; i++) {
            highscores[i].pontos = 0;
            strcpy(highscores[i].nome, "---");
        }
    }
}

void verificaEspadaColetada(char mapa[LINHAS][COLUNAS], personagem *jogador) {
    int lin = (jogador->jogadorLoc.y - ALTURA_BARRA) / LADO;
    int col = jogador->jogadorLoc.x / LADO;
    if (mapa[lin][col] == 'E') {
        jogador->espada = 1;
        mapa[lin][col] = '.';
    }
}

void salvarHighscores(Highscore highscores[MAX_SCORES]) {
    FILE *arquivo = fopen("ranking.bin", "wb");
    if (arquivo) {
        fwrite(highscores, sizeof(Highscore), MAX_SCORES, arquivo);
        fclose(arquivo);
    }
}

void inserirHighscore(const char* nome, int pontos, Highscore highscores[MAX_SCORES]) {
    Highscore novo;
    strncpy(novo.nome, nome, 19);
    novo.nome[19] = '\0';
    novo.pontos = pontos;
    highscores[MAX_SCORES - 1] = novo;
    for (int i = MAX_SCORES - 1; i > 0; i--) {
        if (highscores[i].pontos > highscores[i - 1].pontos) {
            Highscore temp = highscores[i];
            highscores[i] = highscores[i - 1];
            highscores[i - 1] = temp;
        }
    }
}

int sorteia(int min, int max) {
    return rand() % (max - min) + min;
}

int carrega_mapa(char mapa[LINHAS][COLUNAS], const char* nome) {
    // Constrói o caminho completo para a pasta mapas
    char path[100];
    sprintf(path, "mapas/%s", nome);
    
    FILE* arquivo = fopen(path, "r");
    if (arquivo == NULL) return 1;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            int c;
            do {
                c = fgetc(arquivo);
                if (c == EOF) { fclose(arquivo); return 1; }
            } while (c == '\n' || c == '\r');
            mapa[i][j] = (char)c;
        }
    }
    fclose(arquivo);
    return 0;
}

void IniciaMapa(char mapa[LINHAS][COLUNAS], Texture2D texturaParede, Texture2D texturaChao, Texture2D texturaVida, Texture2D texturaEspada) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            Vector2 posicao = { (float)(j * LADO), (float)(i * LADO + ALTURA_BARRA) };
            float escalaChao = (float)LADO / texturaChao.width;
            float escalaParede = (float)LADO / texturaParede.width;
            float escalaVida = (float)LADO / texturaVida.width;
            float escalaEspada = (float)LADO / texturaEspada.width;

            switch (mapa[i][j]) {
                case 'P': DrawTextureEx(texturaParede, posicao, 0.0f, escalaParede, WHITE); break;
                case '.': DrawTextureEx(texturaChao, posicao, 0.0f, escalaChao, WHITE); break;
                case 'V':
                    DrawTextureEx(texturaChao, posicao, 0.0f, escalaChao, WHITE);
                    DrawTextureEx(texturaVida, posicao, 0.0f, escalaVida, WHITE);
                    break;
                case 'E':
                    DrawTextureEx(texturaChao, posicao, 0.0f, escalaChao, WHITE);
                    DrawTextureEx(texturaEspada, posicao, 0.0f, escalaEspada, WHITE);
                    break;
            }
        }
    }
}

int IniciaPosicaoJogador(char matriz[LINHAS][COLUNAS], personagem* jogador) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (matriz[i][j] == 'J') {
                jogador->jogadorLoc.x = j * LADO;
                jogador->jogadorLoc.y = i * LADO + ALTURA_BARRA;
                matriz[i][j] = '.';
                return 0;
            }
        }
    }
    return 1;
}

int deveMover(char mapa[LINHAS][COLUNAS], int x, int y, int dx, int dy) {
    int novoX = x + dx;
    int novoY = y + dy;
    if (novoX < 0 || novoX >= LARGURA) return 1;
    if (novoY < ALTURA_BARRA || novoY >= ALTURA + ALTURA_BARRA) return 1;
    int lin = (novoY - ALTURA_BARRA) / LADO;
    int col = novoX / LADO;
    if (mapa[lin][col] == 'P') return 1;
    return 0;
}

void move(int dx, int dy, int *x, int *y) {
    *x += dx;
    *y += dy;
}

int AtualizaJogador(personagem* jogador, char matriz[LINHAS][COLUNAS], Texture2D jogadorNorte, Texture2D jogadorSul, Texture2D jogadorLeste, Texture2D jogadorOeste) {
    float deltaTime = GetFrameTime();
    jogador->tempoProximoMovimento -= deltaTime;
    if (jogador->tempoProximoMovimento <= 0) {
        int moveu = 0;
        if ((IsKeyDown(KEY_RIGHT))) {
            if (!deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, 0)) {
                move(LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
                jogador->orientacao = 'L';
                moveu = 1;
            }
        } else if ((IsKeyDown(KEY_LEFT))) {
             if (!deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, -LADO, 0)) {
                move(-LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
                jogador->orientacao = 'O';
                moveu = 1;
            }
        } else if ((IsKeyDown(KEY_UP))) {
            if (!deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, -LADO)) {
                move(0, -LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
                jogador->orientacao = 'N';
                moveu = 1;
            }
        } else if ((IsKeyDown(KEY_DOWN))) {
            if (!deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, LADO)) {
                move(0, LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
                jogador->orientacao = 'S';
                moveu = 1;
            }
        }
        if (moveu) {
            jogador->tempoProximoMovimento = 0.12f;
        }
    }
    
    Texture2D texturaAtual;
    float escalaJogador;
    switch (jogador->orientacao) {
        case 'N': texturaAtual = jogadorNorte; escalaJogador = 0.7f * (float)LADO / jogadorNorte.width; break;
        case 'S': texturaAtual = jogadorSul; escalaJogador = 0.7f * (float)LADO / jogadorSul.width; break;
        case 'L': texturaAtual = jogadorLeste; escalaJogador = 0.6f * (float)LADO / jogadorLeste.width; break;
        case 'O': texturaAtual = jogadorOeste; escalaJogador = 0.6f * (float)LADO / jogadorOeste.width; break;
        default: texturaAtual = jogadorSul; escalaJogador = 0.7f * (float)LADO / jogadorSul.width; break;
    }
    DrawTextureEx(texturaAtual, (Vector2){(float)jogador->jogadorLoc.x, (float)jogador->jogadorLoc.y}, 0.0f, escalaJogador, WHITE);
    
    if (jogador->espada == 1 && IsKeyPressed(KEY_J)) { // Espada desenhada apenas no ataque
        int dx = 0, dy = 0;
        switch(jogador->orientacao) {
            case 'N': dy = -1; break;
            case 'S': dy = 1; break;
            case 'L': dx = 1; break;
            case 'O': dx = -1; break;
        }
        for (int i = 1; i <= 3; i++) {
            DrawRectangle(jogador->jogadorLoc.x + i * dx * LADO, jogador->jogadorLoc.y + i * dy * LADO, LADO, LADO, Fade(RED, 0.4f));
        }
    }
    return 0;
}

int iniciaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[]) {
    int k = 0;
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (mapa[i][j] == 'M') {
                mobs[k].monstroLoc.x = j * LADO;
                mobs[k].monstroLoc.y = i * LADO + ALTURA_BARRA;
                mobs[k].orientacao = 'S';
                mobs[k].vida = 1;
                mobs[k].tempoProximoMovimento = sorteia(1, 5) / 10.0f;
                k++;
                mapa[i][j] = '.';
            }
        }
    }
    return k;
}

int atualizaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[MONSTROS], int len, vetor jogadorLoc) {
    float deltaTime = GetFrameTime();
    for (int i = 0; i < len; i++) {
        if (mobs[i].vida <= 0) continue;

        mobs[i].tempoProximoMovimento -= deltaTime;
        if (mobs[i].tempoProximoMovimento <= 0.0f) {
            mobs[i].tempoProximoMovimento = 0.4f;

            int dx = 0, dy = 0;
            
            // Converte as coordenadas de pixel para grade
            int monstroCol = mobs[i].monstroLoc.x / LADO;
            int monstroLin = (mobs[i].monstroLoc.y - ALTURA_BARRA) / LADO;
            int jogadorCol = jogadorLoc.x / LADO;
            int jogadorLin = (jogadorLoc.y - ALTURA_BARRA) / LADO;
            
            // Calcula a Distância de Manhattan
            int distancia = abs(monstroCol - jogadorCol) + abs(monstroLin - jogadorLin);

            // Se estiver perto, persegue. Senão, move-se aleatoriamente.
            if (distancia <= 3) {
                int diffX = jogadorLoc.x - mobs[i].monstroLoc.x;
                int diffY = jogadorLoc.y - mobs[i].monstroLoc.y;

                // Move-se no eixo com maior distância primeiro para contornar paredes simples
                if (abs(diffX) > abs(diffY)) {
                    if (diffX > 0) dx = LADO; else dx = -LADO;
                } else {
                    if (diffY > 0) dy = LADO; else dy = -LADO;
                }

                // Se o caminho primário estiver bloqueado, tenta o secundário
                if (deveMover(mapa, mobs[i].monstroLoc.x, mobs[i].monstroLoc.y, dx, dy)) {
                    if (abs(diffX) > abs(diffY)) { // Tentou X, falhou, agora tenta Y
                        dx = 0;
                        if (diffY > 0) dy = LADO; else if (diffY < 0) dy = -LADO;
                    } else { // Tentou Y, falhou, agora tenta X
                        dy = 0;
                        if (diffX > 0) dx = LADO; else if (diffX < 0) dx = -LADO;
                    }
                }
            } else {
                // Comportamento aleatório original
                int direcao = sorteia(0, 5);
                switch (direcao) {
                    case 0: dy = -LADO; break;
                    case 1: dy = LADO; break;
                    case 2: dx = -LADO; break;
                    case 3: dx = LADO; break;
                    default: break;
                }
            }

            if (!deveMover(mapa, mobs[i].monstroLoc.x, mobs[i].monstroLoc.y, dx, dy)) {
                move(dx, dy, &mobs[i].monstroLoc.x, &mobs[i].monstroLoc.y);
            }
        }
    }
    return 0;
}

void VerificarInteracoes(personagem *jogador, monstro mobs[], int numMonstros, vetor posicaoInicial) {
    Rectangle recJogador = { (float)jogador->jogadorLoc.x, (float)jogador->jogadorLoc.y, LADO, LADO };
    for (int i = 0; i < numMonstros; i++) {
        if (mobs[i].vida <= 0) continue;
        Rectangle recMonstro = { (float)mobs[i].monstroLoc.x, (float)mobs[i].monstroLoc.y, LADO, LADO };
        if (CheckCollisionRecs(recJogador, recMonstro)) {
            jogador->vida--;
            jogador->jogadorLoc = posicaoInicial;
            WaitTime(0.5);
            break;
        }
    }
    if (IsKeyPressed(KEY_J) && jogador->espada == 1) {
        int dx = 0, dy = 0;
        switch (jogador->orientacao) {
            case 'N': dy = -1; break;
            case 'S': dy = 1; break;
            case 'L': dx = 1; break;
            case 'O': dx = -1; break;
        }
        for (int i = 1; i <= 3; i++) {
            Rectangle recEspada = { (float)(jogador->jogadorLoc.x + i * dx * LADO), (float)(jogador->jogadorLoc.y + i * dy * LADO), LADO, LADO };
            for (int k = 0; k < numMonstros; k++) {
                if (mobs[k].vida <= 0) continue;
                Rectangle recMonstro = { (float)mobs[k].monstroLoc.x, (float)mobs[k].monstroLoc.y, LADO, LADO };
                if (CheckCollisionRecs(recEspada, recMonstro)) {
                    mobs[k].vida = 0;
                    jogador->pts += 10;
                }
            }
        }
    }
}

// ---------- FUNCAO PRINCIPAL ----------
int main(void) {
    InitWindow(LARGURA, ALTURA + ALTURA_BARRA, "ZINF");
    SetTargetFPS(60);
    srand(time(NULL));

    Highscore highscores[MAX_SCORES];
    carregarHighscores(highscores);

    // Carregando texturas
    Texture2D jogadorSul = LoadTexture("sprites/jogador-sul.png");
    Texture2D jogadorNorte = LoadTexture("sprites/jogador-norte.png");
    Texture2D jogadorLeste = LoadTexture("sprites/jogador-leste.png");
    Texture2D jogadorOeste = LoadTexture("sprites/jogador-oeste.png");
    Texture2D texturaEspada = LoadTexture("sprites/espada.png");
    Texture2D texturaVida = LoadTexture("sprites/vida.png");
    Texture2D texturaParede = LoadTexture("sprites/parede.png");
    Texture2D texturaChao = LoadTexture("sprites/chao.png");
    Texture2D texturaMonstroSul = LoadTexture("sprites/monstro-sul.png");
    
    GameScreen currentScreen = MENU;
    char mapa[LINHAS][COLUNAS];
    personagem jogador;
    monstro mobs[MONSTROS] = {0};
    int numMonstrosAtivos = 0;
    vetor posicaoInicial;
    char nomeJogador[20] = {0};
    int contadorLetras = 0;
    Rectangle caixaTexto = { LARGURA / 2 - 120, 280, 240, 40 };

    int faseAtual;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen) {
            case MENU:
                DrawText("MENU PRINCIPAL", 400, 100, 30, BLACK);
                DrawText("1 - Jogar", 450, 160, 20, DARKGRAY);
                DrawText("2 - Ver Highscores", 450, 200, 20, DARKGRAY);
                DrawText("3 - Sair", 450, 240, 20, DARKGRAY);
                if (IsKeyPressed(KEY_ONE)) {
                    faseAtual = 1;
                    jogador.pts = 0;
                    if (carregarFase(faseAtual, mapa, &jogador, mobs, &numMonstrosAtivos, &posicaoInicial) == 0) {
                        currentScreen = GAMEPLAY;
                    } else {
                        // Opcional: Mostrar uma mensagem de que nenhum mapa foi encontrado
                        currentScreen = MENU;
                    }
                } else if (IsKeyPressed(KEY_TWO)) {
                    currentScreen = HIGHSCORES;
                } else if (IsKeyPressed(KEY_THREE)) {
                    currentScreen = EXIT;
                }
                break;

            case GAMEPLAY:
                if (jogador.vida <= 0) {
                    currentScreen = ENTER_HIGHSCORE;
                } else {
                    DrawRectangle(0, 0, LARGURA, ALTURA_BARRA, BLACK);
                    DrawText(TextFormat("Vida: %d | Pontos: %d | Fase: %d", jogador.vida, jogador.pts, faseAtual + 1), 20, 20, 20, WHITE);

                    IniciaMapa(mapa, texturaParede, texturaChao, texturaVida, texturaEspada);
                    AtualizaJogador(&jogador, mapa, jogadorNorte, jogadorSul, jogadorLeste, jogadorOeste);
                    verificaVidaColetada(mapa, &jogador);
                    verificaEspadaColetada(mapa, &jogador);
                    atualizaMonstros(mapa, mobs, numMonstrosAtivos, jogador.jogadorLoc);
                    VerificarInteracoes(&jogador, mobs, numMonstrosAtivos, posicaoInicial);

                    for (int i = 0; i < numMonstrosAtivos; i++) {
                        if (mobs[i].vida > 0) {
                            DrawTextureEx(texturaMonstroSul, (Vector2){(float)mobs[i].monstroLoc.x, (float)mobs[i].monstroLoc.y}, 0.0f, (float)LADO / texturaMonstroSul.width, WHITE);
                        }
                    }

                    int todosMortos = 1;
                    for (int i = 0; i < numMonstrosAtivos; i++) {
                        if (mobs[i].vida > 0) { todosMortos = 0; break; }
                    }

                    if (todosMortos) {
                        faseAtual++;
                        // Verifica se o próximo mapa existe. Se não, o jogador venceu.
                        if (carregarFase(faseAtual, mapa, &jogador, mobs, &numMonstrosAtivos, &posicaoInicial) != 0) {
                            currentScreen = VICTORY;
                        }
                    }
                    if (IsKeyPressed(KEY_P)) currentScreen = PAUSE;
                }
                break;

            case PAUSE:
                DrawRectangle(0, 0, LARGURA, ALTURA + ALTURA_BARRA, Fade(BLACK, 0.7f));
                DrawText("PAUSA", LARGURA / 2 - MeasureText("PAUSA", 50) / 2, 200, 50, WHITE);
                DrawText("Pressione ENTER para continuar", LARGURA / 2 - MeasureText("Pressione ENTER para continuar", 20) / 2, 300, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) currentScreen = GAMEPLAY;
                break;

            case HIGHSCORES:
                DrawText("HIGHSCORES", 400, 80, 30, BLACK);
                for (int i = 0; i < MAX_SCORES; i++) {
                    DrawText(TextFormat("%d. %s - %d", i + 1, highscores[i].nome, highscores[i].pontos), 400, 130 + i * 30, 20, BLACK);
                }
                DrawText("Pressione ENTER para voltar", 400, 350, 20, RED);
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;

            case VICTORY:
                ClearBackground(RAYWHITE);
                DrawText("PARABÉNS!", LARGURA / 2 - MeasureText("PARABÉNS!", 60) / 2, 200, 60, GREEN);
                DrawText("Você completou todas as fases!", LARGURA / 2 - MeasureText("Você completou todas as fases!", 30) / 2, 280, 30, DARKGREEN);
                DrawText("Pressione ENTER para salvar seu score", LARGURA / 2 - MeasureText("Pressione ENTER para salvar seu score", 20) / 2, 350, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = ENTER_HIGHSCORE;
                }
                break;

            case ENTER_HIGHSCORE:
                ClearBackground(BLACK);
                DrawText("FIM DE JOGO", LARGURA / 2 - MeasureText("FIM DE JOGO", 60) / 2, 150, 60, RED);
                DrawText("Digite seu nome e pressione ENTER", LARGURA / 2 - MeasureText("Digite seu nome e pressione ENTER", 20) / 2, 230, 20, WHITE);
                DrawRectangleRec(caixaTexto, LIGHTGRAY);
                DrawText(nomeJogador, (int)caixaTexto.x + 5, (int)caixaTexto.y + 10, 20, MAROON);
                
                int key = GetKeyPressed();
                if ((key >= 32) && (key <= 125) && (contadorLetras < 19)) {
                    nomeJogador[contadorLetras] = (char)key;
                    nomeJogador[contadorLetras + 1] = '\0';
                    contadorLetras++;
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (contadorLetras > 0) contadorLetras--;
                    nomeJogador[contadorLetras] = '\0';
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (contadorLetras == 0) strcpy(nomeJogador, "Jogador");
                    inserirHighscore(nomeJogador, jogador.pts, highscores);
                    salvarHighscores(highscores);
                    contadorLetras = 0;
                    memset(nomeJogador, 0, 20);
                    currentScreen = HIGHSCORES;
                }
                break;

            case EXIT:
                goto end_loop; // Sai do switch e vai para o final do loop
        }
        EndDrawing();
    }

end_loop:
    UnloadTexture(texturaParede);
    UnloadTexture(texturaChao);
    UnloadTexture(jogadorNorte);
    UnloadTexture(jogadorSul);
    UnloadTexture(jogadorLeste);
    UnloadTexture(jogadorOeste);
    UnloadTexture(texturaVida);
    UnloadTexture(texturaEspada);
    UnloadTexture(texturaMonstroSul);
    
    CloseWindow();
    return 0;
}