#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA 800
#define ALTURA 800

#define LADO 20

int deveMover(int x, int y, int dx, int dy){
  //implementar
}

int move(int dx, int dy, int *x, int *y){
  //implementar
}

int main(void){
  int flagDentroLimites = 1;
  
  struct personagem{
    int x;
    int y;
    int orientacao;
  };

  srand(time(NULL));

  struct personagem jogador;

  jogador.x = (rand() % (LARGURA/LADO)) * LADO;
  jogador.y = (rand() % (ALTURA/LADO)) * LADO;

  InitWindow(LARGURA, ALTURA, "Quadrado"); 
  SetTargetFPS(60);
  
  while (!WindowShouldClose() && flagDentroLimites)
  {
    if(IsKeyPressed(KEY_RIGHT))
      jogador.x+=LADO;
    if(IsKeyPressed(KEY_LEFT))
            jogador.x-=LADO;
    if(IsKeyPressed(KEY_UP))
        jogador.y-=LADO;
    if(IsKeyPressed(KEY_DOWN))
        jogador.y+=LADO;
    BeginDrawing();
    ClearBackground(RAYWHITE); 
    DrawRectangle(jogador.x, jogador.y, LADO, LADO, GREEN);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
