#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define LARGURA 1200
#define ALTURA 800
#define LADO LARGURA / COLUNAS
#define ALTURA_BARRA 60
#define LINHAS 16
#define COLUNAS 24

typedef struct vetor
{
	int x;
	int y;
}vetor;

typedef struct personagem{
	vetor jogadorLoc;
	char orientacao;
	int pts;
	int vida;
	int espada;
}personagem;


//função que lê o arquivo txt e coloca numa matriz
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

//função que desenha os elementos fixos do mapa com a matriz mapa
void IniciaMapa(char mapa[LINHAS][COLUNAS]){
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++){
			switch(mapa[i][j]){
				case 'P':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS) + ALTURA_BARRA, (LARGURA / COLUNAS), (ALTURA / LINHAS), BROWN);
					break;
				case 'V':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS) + ALTURA_BARRA, (LARGURA / COLUNAS), (ALTURA / LINHAS), MAROON);
					break;
				case 'E':
					DrawRectangle(j * (LARGURA / COLUNAS), i * (ALTURA / LINHAS) + ALTURA_BARRA, (LARGURA / COLUNAS), (ALTURA / LINHAS), GRAY);
					break;
				default:
					break;
			}
		}
	}
}

//função auxiliar para debug
void imprime_matriz(char matriz[LINHAS][COLUNAS]){
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++){
			printf("%c",matriz[i][j]);
		}
		printf("\n");
	}
}

int IniciaPosicaoJogador(char matriz[LINHAS][COLUNAS], personagem* jogador){
	for(int i = 0; i < LINHAS; i++){
		for(int j = 0; j < COLUNAS; j++)
			if(matriz[i][j] == 'J'){
				jogador->jogadorLoc.x = j * LADO;
				jogador->jogadorLoc.y = i * LADO + ALTURA_BARRA;
			}
	}
	return 0;
}

int deveMover(char mapa[LINHAS][COLUNAS], int x, int y, int dx, int dy){
	//testa as bordas da janela
	if((x + LADO) + dx > LARGURA || x + dx < 0) return 1;
  	if((y + LADO) + dy > ALTURA + ALTURA_BARRA || y + dy < 0) return 1;

	//testa colisão com a parede
	if(mapa[(y + dy)/LADO][(x + dx)/LADO] == 'P')  return 1;

  return 0; //se chegou aqui pode mover.
}

int move(int dx, int dy, int *x, int *y){
  *x = *x + dx;
  *y = *y + dy;
  return 0;
}

int AtualizaJogador(personagem* jogador, char matriz[LINHAS][COLUNAS]){
	if(IsKeyPressed(KEY_RIGHT)){
      if(deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, 0) == 0)
        move(LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);}
    
    if(IsKeyPressed(KEY_LEFT)){
      if(deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, -LADO, 0) == 0)
        move(-LADO, 0, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);}
   
    if(IsKeyPressed(KEY_UP)){
      if(deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, -LADO) == 0)
        move(0, -LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);}
    
    if(IsKeyPressed(KEY_DOWN)){
      if(deveMover(matriz, jogador->jogadorLoc.x, jogador->jogadorLoc.y, 0, LADO) == 0)
        move(0, LADO, &jogador->jogadorLoc.x, &jogador->jogadorLoc.y);}
		DrawRectangle(jogador->jogadorLoc.x, jogador->jogadorLoc.y, LADO, LADO, GREEN);
	return 0;	
}

int main(void){
	char mapa[LINHAS][COLUNAS];
	
	//inicia o jogador
	personagem jogador;
	jogador.vida = 3;
	jogador.espada = 0;
	jogador.orientacao = 'S';
	jogador.pts = 0;


	InitWindow(LARGURA,ALTURA + ALTURA_BARRA,"ZINF");

	carrega_mapa(mapa, "mapa.txt");
	IniciaPosicaoJogador(mapa, &jogador);

	while(!WindowShouldClose()){

		BeginDrawing();
		ClearBackground((Color){255,255,255});
		DrawRectangle(0,0,LARGURA,ALTURA_BARRA,BLACK);
		IniciaMapa(mapa);
		AtualizaJogador(&jogador, mapa);
		EndDrawing();
	
	}
	//printf("%c",mapa[15][23]);
	//imprime_matriz(mapa);
	return 0;
}
