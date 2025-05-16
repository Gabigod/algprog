#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA 800
#define ALTURA 800

#define LADO 80

int deveMover(int x, int y, int dx, int dy){
  if((x + LADO) + dx > LARGURA || x + dx < 0) return 0;
  if((y + LADO) + dy > ALTURA || y + dy < 0) return 0;
  return 1; //se chegou aqui pode mover.
}

int move(int dx, int dy, int *x, int *y){
  *x = *x + dx;
  *y = *y + dy;
  return 0;
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
      if(deveMover(jogador.x, jogador.y, LADO, 0) == 1)
        move(LADO, 0, &jogador.x, &jogador.y);
    
    if(IsKeyPressed(KEY_LEFT))
      if(deveMover(jogador.x, jogador.y, -LADO, 0) == 1)
        move(-LADO, 0, &jogador.x, &jogador.y);
   
    if(IsKeyPressed(KEY_UP))
      if(deveMover(jogador.x, jogador.y, 0, -LADO) == 1)
        move(0, -LADO, &jogador.x, &jogador.y);
    
    if(IsKeyPressed(KEY_DOWN))
      if(deveMover(jogador.x, jogador.y, 0, LADO) == 1)
        move(0, LADO, &jogador.x, &jogador.y);

    BeginDrawing();
    ClearBackground(RAYWHITE); 
    DrawRectangle(jogador.x, jogador.y, LADO, LADO, GREEN);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
