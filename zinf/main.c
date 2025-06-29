// ---------- BIBLIOTECAS ----------
#include <raylib.h>    // Biblioteca grafica para criar janelas, desenhar, etc.
#include <stdio.h>     // Entrada e saida padrao (printf, scanf, FILE, etc.)
#include <stdlib.h>    // Funcoes gerais (como rand, malloc)
#include <string.h>    // Para manipulacao de strings
#include <time.h>

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
    float tempoProximoMovimento;
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
//verifica se o jogador passou em cima de uma vida
void verificaVidaColetada(char mapa[LINHAS][COLUNAS], personagem *jogador) {
    int lin = ((*jogador).jogadorLoc.y - ALTURA_BARRA) / LADO;
    int col = (*jogador).jogadorLoc.x / LADO;

    if (mapa[lin][col] == 'V') {
        (*jogador).vida++;
        mapa[lin][col] = '.'; // Remove a vida do mapa
    }
}

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

// Verifica se o jogador passou em cima da espada
void verificaEspadaColetada(char mapa[LINHAS][COLUNAS], personagem *jogador) {
    int lin = ((*jogador).jogadorLoc.y - ALTURA_BARRA) / LADO;
    int col = (*jogador).jogadorLoc.x / LADO;

    if (mapa[lin][col] == 'E') {
        (*jogador).espada = 1;
        mapa[lin][col] = '.';
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
void IniciaMapa(char mapa[LINHAS][COLUNAS], Texture2D texturaParede, Texture2D texturaChao, Texture2D texturaVida, Texture2D texturaEspada) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            int x = j * LADO;
            int y = i * LADO + ALTURA_BARRA;

            // Define o ponto de destino para DrawTextureEx
            Vector2 posicao = { (float)x, (float)y };

            // Define a escala para que a textura tenha o tamanho de LADO x LADO
            // Assumimos que as texturas de itens e mapa sao quadradas (largura == altura)
            float escalaChao = (float)LADO / texturaChao.width; // Escala baseada na largura da textura de chao
            float escalaParede = (float)LADO / texturaParede.width; // Escala baseada na largura da textura de parede
            float escalaVida = (float)LADO / texturaVida.width; // Escala baseada na largura da textura de vida
            float escalaEspada = (float)LADO / texturaEspada.width; // Escala baseada na largura da textura de espada

            // Ponto de origem para rotacao (0,0 para canto superior esquerdo)
            //Vector2 origem = { 0.0f, 0.0f };
            float rotacao = 0.0f;
            Color tintura = WHITE;

            switch (mapa[i][j]) {
                case 'P':
                    DrawTextureEx(texturaParede, posicao, rotacao, escalaParede, tintura);
                    break;
                case '.':
                    DrawTextureEx(texturaChao, posicao, rotacao, escalaChao, tintura);
                    break;
                case 'V':
                    DrawTextureEx(texturaChao, posicao, rotacao, escalaChao, tintura); // desenha chao atras
                    DrawTextureEx(texturaVida, posicao, rotacao, escalaVida, tintura);
                    break;
                case 'E':
                    DrawTextureEx(texturaChao, posicao, rotacao, escalaChao, tintura); // desenha chao atras
                    DrawTextureEx(texturaEspada, posicao, rotacao, escalaEspada, tintura);
                    break;
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
                matriz[i][j] = '.';
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
int AtualizaJogador(personagem* jogador, char matriz[LINHAS][COLUNAS],
                    Texture2D jogadorNorte, Texture2D jogadorSul,
                    Texture2D jogadorLeste, Texture2D jogadorOeste) {

    if (IsKeyPressed(KEY_RIGHT) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, 0)) {
        move(LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
        jogador->orientacao = 'L';
    }
    if (IsKeyPressed(KEY_LEFT) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, -LADO, 0)) {
        move(-LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
        jogador->orientacao = 'O';
    }
    if (IsKeyPressed(KEY_UP) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, -LADO)) {
        move(0, -LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
        jogador->orientacao = 'N';
    }
    if (IsKeyPressed(KEY_DOWN) && !deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, LADO)) {
        move(0, LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);
        jogador->orientacao = 'S';
    }

    // Seleciona a textura de acordo com a direcao
    Texture2D texturaAtual;
    float escalaJogador; // Declara a variavel de escala para o jogador

    switch (jogador->orientacao) {
        case 'N': 
            texturaAtual = jogadorNorte; 
            escalaJogador = 0.7 * (float)LADO / jogadorNorte.width; // Calcula a escala para a textura Norte
            break;
        case 'S': 
            texturaAtual = jogadorSul; 
            escalaJogador = 0.7 * (float)LADO / jogadorSul.width; // Calcula a escala para a textura Sul
            break;
        case 'L': 
            texturaAtual = jogadorLeste; 
            escalaJogador = 0.6 * (float)LADO / jogadorLeste.width; // Calcula a escala para a textura Leste
            break;
        case 'O': 
            texturaAtual = jogadorOeste; 
            escalaJogador = 0.6 * (float)LADO / jogadorOeste.width; // Calcula a escala para a textura Oeste
            break;
        default: // Caso padrao para evitar usar textura nao inicializada
            texturaAtual = jogadorSul; // ou qualquer outra textura padrao
            escalaJogador = 0.7 * (float)LADO / jogadorSul.width;
            break;
    }
    
    // Desenha a textura do jogador com a escala calculada
    Vector2 posicaoJogador = { (float)jogador->jogadorLoc.x, (float)jogador->jogadorLoc.y };
    //Vector2 origemJogador = { 0.0f, 0.0f }; // Canto superior esquerdo como origem
    float rotacaoJogador = 0.0f;
    Color tinturaJogador = WHITE;

    DrawTextureEx(texturaAtual, posicaoJogador, rotacaoJogador, escalaJogador, tinturaJogador);

    // Hitbox da espada (nao precisa de mudanca, pois e um retangulo desenhado diretamente)
    if (jogador->espada == 1) {
        int dx = 0, dy = 0;
        switch (jogador->orientacao) {
            case 'N': dy = -1; break;
            case 'S': dy = 1; break;
            case 'L': dx = 1; break;
            case 'O': dx = -1; break;
        }
        for (int i = 1; i <= 3; i++) {
            int x = jogador->jogadorLoc.x + i * dx * LADO;
            int y = jogador->jogadorLoc.y + i * dy * LADO;
            DrawRectangle(x, y, LADO, LADO, Fade(RED, 0.4f)); // hitbox com transparencia
        }
    }

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
                mobs[k].orientacao = 'S';
                mobs[k].vida = 1;
                mobs[k].tempoProximoMovimento = 0; //pode mover imediatamente
                k++;
                mapa[i][j] = '.';
            }
        }
    }
    return k;
}

// Atualiza a posicao dos monstros
int atualizaMonstros(char mapa[LINHAS][COLUNAS], monstro mobs[MONSTROS], int len) {
    float deltaTime = GetFrameTime();
    for (int i = 0; i < len; i++) {

        if(mobs[i].vida <= 0) continue;

        mobs[i].tempoProximoMovimento -= deltaTime;

        if (mobs[i].tempoProximoMovimento <= 0.0f) {
            mobs[i].tempoProximoMovimento = 0.3f;
        
            // Gera uma direcao aleatoria
            int direcao = sorteia(0, 5); // 0: Cima, 1: Baixo, 2: Esquerda, 3: Direita, 4: Parado

            int dx = 0; // Deslocamento em X
            int dy = 0; // Deslocamento em Y

            // Calcula o deslocamento em pixels (múltiplos de LADO)
            switch (direcao) {
                case 0: // Cima
                    dy = -LADO;
                    mobs[i].orientacao = 'N';
                    break;
                case 1: // Baixo
                    dy = LADO;
                    mobs[i].orientacao = 'S';
                    break;
                case 2: // Esquerda
                    dx = -LADO;
                    mobs[i].orientacao = 'O';
                    break;
                case 3: // Direita
                    dx = LADO;
                    mobs[i].orientacao = 'L';
                    break;
                case 4: // Parado (dx=0, dy=0)
                    // Não faz nada, já estão em zero
                    break;
            }

            // Verifica se o monstro PODE MOVER para a nova posicao
            if (deveMover(mapa, mobs[i].monstroLoc.x, mobs[i].monstroLoc.y, dx, dy) == 0) {
                move(dx, dy, &mobs[i].monstroLoc.x, &mobs[i].monstroLoc.y);
            }
        }
    }
    return 0;
}

// Verifica interacoes entre os mobs e o jogador
void VerificarInteracoes(personagem *jogador, monstro mobs[], int numMonstros, vetor posicaoInicial) {
    
    // Cria um retangulo de colisao para o jogador
    Rectangle recJogador = { jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, LADO };

    // --- 1. VERIFICA SE MONSTRO ENCOSTA NO JOGADOR ---
    for (int i = 0; i < numMonstros; i++) {
        // Ignora monstros mortos
        if (mobs[i].vida <= 0) continue;

        Rectangle recMonstro = { mobs[i].monstroLoc.x, mobs[i].monstroLoc.y, LADO, LADO };

        // Se o jogador colidir com um monstro...
        if (CheckCollisionRecs(recJogador, recMonstro)) {
            jogador->vida--; // Perde uma vida
            jogador->jogadorLoc = posicaoInicial; // Volta para o inicio do mapa

            // Pausa pequena para o jogador entender o que aconteceu
            WaitTime(0.5); 
            break; // Sai do loop para evitar perder multiplas vidas de uma vez
        }
    }

    // --- 2. VERIFICA SE O JOGADOR ATACA UM MONSTRO ---
    // So acontece se a tecla J for pressionada e o jogador tiver a espada
    if (IsKeyPressed(KEY_J) && jogador->espada == 1) {
        // Calcula a hitbox da espada (3 quadrados a frente do jogador)
        int dx = 0, dy = 0;
        switch (jogador->orientacao) {
            case 'N': dy = -1; break;
            case 'S': dy = 1; break;
            case 'L': dx = 1; break;
            case 'O': dx = -1; break;
        }

        // Verifica cada um dos 3 quadrados da hitbox da espada
        for (int i = 1; i <= 3; i++) {
            Rectangle recEspada = {
                jogador->jogadorLoc.x + i * dx * LADO,
                jogador->jogadorLoc.y + i * dy * LADO,
                LADO, LADO
            };

            // Compara a hitbox com cada monstro vivo
            for (int k = 0; k < numMonstros; k++) {
                if (mobs[k].vida <= 0) continue;

                Rectangle recMonstro = { mobs[k].monstroLoc.x, mobs[k].monstroLoc.y, LADO, LADO };

                if (CheckCollisionRecs(recEspada, recMonstro)) {
                    mobs[k].vida = 0;   // Monstro morre
                    jogador->pts += 10; // Ganha 10 pontos
                }
            }
        }
    }
}


// ---------- FUNCAO PRINCIPAL ----------
int main(void) {
    const int screenWidth = LARGURA;
    const int screenHeight = ALTURA + ALTURA_BARRA;
    monstro mobs[MONSTROS] = {0};
    int numMonstrosAtivos = 0;
    Highscore highscores[MAX_SCORES]; // Vetor que guarda os 5 melhores scores

    carregarHighscores(highscores); // Carrega os highscores no inicio
    srand(time(NULL));  // Define a seed da funcao rand como unix time stamp

    InitWindow(screenWidth, screenHeight, "ZINF");
    SetTargetFPS(60);  // Limita a 60 frames por segundo
    Texture2D jogadorSul = LoadTexture("sprites/jogador-sul.png");
    Texture2D jogadorNorte = LoadTexture("sprites/jogador-norte.png");
    Texture2D jogadorLeste = LoadTexture("sprites/jogador-leste.png");
    Texture2D jogadorOeste = LoadTexture("sprites/jogador-oeste.png");

    Texture2D texturaEspada = LoadTexture("sprites/espada.png");
    Texture2D texturaVida = LoadTexture("sprites/vida.png");
    Texture2D texturaParede = LoadTexture("sprites/parede.png");
    Texture2D texturaChao = LoadTexture("sprites/chao.png");

    //Texture2D texturaMonstroNorte = LoadTexture("sprites/monstro-norte.png");
    Texture2D texturaMonstroSul = LoadTexture("sprites/monstro-sul.png");
    //Texture2D texturaMonstroLeste = LoadTexture("sprites/monstro-leste.png");
    //Texture2D texturaMonstroOeste = LoadTexture("sprites/monstro-oeste.png");

    GameScreen currentScreen = MENU;  // Comeca no menu

    char mapa[LINHAS][COLUNAS];
    personagem jogador;
    vetor posicaoInicial;
    int pontuacaoSalva = 0;

    // ---------- LOOP PRINCIPAL ----------
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Troca entre telas
        switch (currentScreen) {
            case MENU:
                DrawRectangle(0, 0, screenWidth, ALTURA_BARRA, WHITE);
                DrawText("MENU PRINCIPAL", 400, 100, 30, BLACK);
                DrawText("1 - Jogar", 450, 160, 20, DARKGRAY);
                DrawText("2 - Ver Highscores", 450, 200, 20, DARKGRAY);
                DrawText("3 - Sair", 450, 240, 20, DARKGRAY);

                if (IsKeyPressed(KEY_ONE)) {
                carrega_mapa(mapa, "mapa.txt");
                jogador.vida = 3;
                jogador.espada = 0;
                jogador.orientacao = 'S';
                jogador.pts = 0; // Exemplo de pontos
                IniciaPosicaoJogador(mapa, &jogador);
                posicaoInicial = jogador.jogadorLoc;
                numMonstrosAtivos = iniciaMonstros(mapa, mobs); // Inicializa os monstros aqui!
                pontuacaoSalva = 0;
                currentScreen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_TWO)) {
                    currentScreen = HIGHSCORES;
                } else if (IsKeyPressed(KEY_THREE)) {
                    currentScreen = EXIT;
                }
                break;

            case GAMEPLAY:
                if (jogador.vida <= 0) {
                    if (!pontuacaoSalva) {  // Salva a pontuacao se ainda nao foi salva
                        inserirHighscore("Jogador", jogador.pts, highscores);
                        salvarHighscores(highscores);
                        pontuacaoSalva = 1;
                    }
                    // Mostra a tela de Game Over
                    ClearBackground(BLACK);
                    DrawText("GAME OVER", LARGURA / 2 - MeasureText("GAME OVER", 60) / 2, ALTURA / 2 - 40, 60, RED);
                    DrawText("Pressione ENTER para voltar ao Menu", LARGURA / 2 - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2, ALTURA / 2 + 40, 20, WHITE);

                    if (IsKeyPressed(KEY_ENTER)) {
                        currentScreen = MENU;
                    }
                }else{
                    DrawRectangle(0, 0, screenWidth, ALTURA_BARRA, BLACK); // fundo preto do topo

                    char texto[50];
                    sprintf(texto, "Vida: %d   Pontos: %d", jogador.vida, jogador.pts);
                    DrawText(texto, 20, 20, 20, WHITE); // desenha texto branco com vida e pontos

                    IniciaMapa(mapa, texturaParede, texturaChao, texturaVida, texturaEspada);
                    AtualizaJogador(&jogador, mapa, jogadorNorte, jogadorSul, jogadorLeste, jogadorOeste);
                    verificaVidaColetada(mapa, &jogador);
                    verificaEspadaColetada(mapa, &jogador);

                    atualizaMonstros(mapa, mobs, numMonstrosAtivos);

                    // Loop para desenhar cada monstro
                    for(int i = 0; i < numMonstrosAtivos; i++) {
                        if(mobs[i].vida > 0){
                            Texture2D texturaMonstroAtual;
                            // Se tiver lagica de orientacao para monstros, implemente um switch aqui.
                            texturaMonstroAtual = texturaMonstroSul; 
                            float escalaMonstro = (float)LADO / texturaMonstroAtual.width;

                            Vector2 posicaoMonstro = { (float)mobs[i].monstroLoc.x, (float)mobs[i].monstroLoc.y };
                            //Vector2 origemMonstro = { 0.0f, 0.0f };
                            float rotacaoMonstro = 0.0f;
                            Color tinturaMonstro = WHITE;

                            DrawTextureEx(texturaMonstroAtual, posicaoMonstro, rotacaoMonstro, escalaMonstro, tinturaMonstro);
                        }
                    }

                    VerificarInteracoes(&jogador, mobs, numMonstrosAtivos, posicaoInicial);

                    if (IsKeyPressed(KEY_ESCAPE)) {
                        if (!pontuacaoSalva) {
                            inserirHighscore("Jogador", jogador.pts, highscores);
                            salvarHighscores(highscores);
                            pontuacaoSalva = 1;
                        }
                        currentScreen = MENU;
                    }
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
                DrawText("Pressione ENTER para voltar", 400, 350, 20, RED);
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;

            case EXIT:
                CloseWindow(); // Fecha a janela
                return 0;
        }

        EndDrawing(); // Termina o frame
    }
    UnloadTexture(texturaParede);
    UnloadTexture(texturaChao);
    UnloadTexture(jogadorNorte);
    UnloadTexture(jogadorSul);
    UnloadTexture(jogadorLeste);
    UnloadTexture(jogadorOeste);

    CloseWindow(); // Fecha ao sair do loop
    return 0;
}