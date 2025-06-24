#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // Para usar o tipo bool

// --- Suas defines existentes ---
#define LARGURA 1200
#define ALTURA 800
#define ALTURA_BARRA 60
#define LINHAS 16
#define COLUNAS 24

// --- Adicionando uma struct para o jogador, como sugerido no enunciado  ---
typedef struct {
    int x; // Coordenada da coluna no grid
    int y; // Coordenada da linha no grid
    int vidas;
    int pontuacao;
} Jogador;


int carrega_mapa(char mapa[LINHAS][COLUNAS], char* nome){
	FILE* arquivo = fopen(nome, "r");
	if(arquivo == NULL){
	return 1;
	}
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS;j++){
			int c;
			do{
				c = fgetc(arquivo);
				if(c == EOF){fclose(arquivo); return 1;}
			}while(c == '\n');
			mapa[i][j] = (char)c;
		}
	}
	fclose(arquivo);
	return 0;
}

// --- Função para encontrar a posição inicial do jogador ('J') no mapa  ---
void encontra_posicao_inicial(char mapa[LINHAS][COLUNAS], int *x, int *y) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (mapa[i][j] == 'J') {
                *y = i;
                *x = j;
                return;
            }
        }
    }
}


void desenha_mapa(char mapa[LINHAS][COLUNAS]){
    // Tamanho de cada "bloco" do mapa na tela
    int tam_bloco_largura = LARGURA / COLUNAS;
    int tam_bloco_altura = ALTURA / LINHAS;

	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++){
			switch(mapa[i][j]){
                // 'P' representa uma pedra (obstáculo) 
				case 'P': 
					DrawRectangle(j * tam_bloco_largura, i * tam_bloco_altura + ALTURA_BARRA, tam_bloco_largura, tam_bloco_altura, BROWN);
					break;
				case 'V':
					DrawRectangle(j * tam_bloco_largura, i * tam_bloco_altura + ALTURA_BARRA, tam_bloco_largura, tam_bloco_altura, MAROON);
					break;
				case 'E':
					DrawRectangle(j * tam_bloco_largura, i * tam_bloco_altura + ALTURA_BARRA, tam_bloco_largura, tam_bloco_altura, GRAY);
					break;
				default:
					break;
			}
		}
	}
}

// --- NOVA FUNÇÃO DE VALIDAÇÃO DE MOVIMENTO ---
// Retorna true se o movimento for válido, false caso contrário
bool pode_mover(char mapa[LINHAS][COLUNAS], int x_atual, int y_atual, int dx, int dy) {
    int novo_x = x_atual + dx;
    int novo_y = y_atual + dy;

    // 1. Validar limites do mapa
    if (novo_x < 0 || novo_x >= COLUNAS || novo_y < 0 || novo_y >= LINHAS) {
        return false;
    }

    // 2. Validar colisão com paredes ('P') 
    if (mapa[novo_y][novo_x] == 'P') {
        return false;
    }

    // Se passou nas validações, o movimento é permitido
    return true;
}


int main(void){
	char mapa[LINHAS][COLUNAS];
    Jogador jogador;
	
	InitWindow(LARGURA, ALTURA + ALTURA_BARRA,"ZINF");
    SetTargetFPS(60);

	if(carrega_mapa(mapa, "zinf/mapa.txt") != 0){
        // Tratar erro de carregamento do mapa se necessário
        return 1; 
    }

    // Inicializa o jogador
    encontra_posicao_inicial(mapa, &jogador.x, &jogador.y);
    jogador.vidas = 3; // O jogador começa com 3 vidas 
    jogador.pontuacao = 0;

    int tam_bloco_largura = LARGURA / COLUNAS;
    int tam_bloco_altura = ALTURA / LINHAS;

	while(!WindowShouldClose()){
        // --- Lógica de Atualização (Update) ---
        // A interação do jogador é feita por teclas 
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            if (pode_mover(mapa, jogador.x, jogador.y, 1, 0)) {
                jogador.x += 1;
            }
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if (pode_mover(mapa, jogador.x, jogador.y, -1, 0)) {
                jogador.x -= 1;
            }
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
             if (pode_mover(mapa, jogador.x, jogador.y, 0, 1)) {
                jogador.y += 1;
            }
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
             if (pode_mover(mapa, jogador.x, jogador.y, 0, -1)) {
                jogador.y -= 1;
            }
        }
        
		// --- Lógica de Desenho (Draw) ---
		BeginDrawing();
		ClearBackground((Color){205, 224, 154, 255}); // Um fundo verde claro
        
        // Barra de Status (exemplo simples)
        DrawRectangle(0, 0, LARGURA, ALTURA_BARRA, BLACK);
        DrawText(TextFormat("VIDAS: %d | NIVEL: 1 | ESCORE: %d", jogador.vidas, jogador.pontuacao), 20, 20, 20, RAYWHITE);
        
        // Cenário e itens
		desenha_mapa(mapa);

        // Desenha o jogador
        DrawRectangle(jogador.x * tam_bloco_largura, jogador.y * tam_bloco_altura + ALTURA_BARRA, tam_bloco_largura, tam_bloco_altura, GREEN);

		EndDrawing();
	}
    CloseWindow();
	return 0;
}