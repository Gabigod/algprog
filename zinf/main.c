#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define LARGURA 1200
#define ALTURA 800
#define ALTURA_BARRA 60
#define LINHAS 16
#define COLUNAS 24

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

void desenha_mapa(char mapa[LINHAS][COLUNAS]){
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++){
			switch(mapa[i][j]){
				case 'P':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS), (LARGURA / COLUNAS), (ALTURA / LINHAS), BROWN);
					break;
				case 'V':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS), (LARGURA / COLUNAS), (ALTURA / LINHAS), MAROON);
					break;
				case 'E':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS), (LARGURA / COLUNAS), (ALTURA / LINHAS), GRAY);
					break;
				default:
					break;
			}
		}
	}
}

void imprime_matriz(char matriz[LINHAS][COLUNAS]){
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++){
			printf("%c",matriz[i][j]);
		}
		printf("\n");
	}
}

int main(void){
	char mapa[LINHAS][COLUNAS];
	
	InitWindow(LARGURA,ALTURA + ALTURA_BARRA,"ZINF");

	carrega_mapa(mapa, "mapa.txt");
	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground((Color){255,255,255});
		desenha_mapa(mapa);
		EndDrawing();
	}
	//printf("%c",mapa[15][23]);
	//imprime_matriz(mapa);
	return 0;
}
