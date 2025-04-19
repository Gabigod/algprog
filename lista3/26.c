/* Esse programa aproxima a serie de leibniz para pi calculando n termos*/
#include <stdio.h>

int main (){
  int n,i;
  float pi = 0;
  
  printf("Digite o numero de termos para calcular pi: ");
  scanf("%d",&n);
  
  for(i = 0; i < n; i++){
    if(i == 0) pi += 1;  
    else if( i % 2 == 0) pi -= 1/(2 * i + 1); //caso a posicao do termo seja par, subtrai 1/ segundo impar mais proximo 
    else pi += 1 / (2 * i + 1); //senao, a posicao eh impar e soma 1 / segundo impar mais proximo
  }
  
  printf("\nO valor de pi calculado foi de: %f\n", pi);

  return 0;
}
