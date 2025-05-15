/*Faça um programa que preenche um vetor de N posições (assuma #define N 10).
Ao fim, o programa deve preencher um segundo vetor de N posições, de tal modo
que os primeiros números sejam os números pares do primeiro vetor e os últimos
números sejam os números ímpares do primeiro vetor.*/

#include <stdio.h>
#define N 10
int main(){
  int vet[N] = {1,2,3,4,5,6,7,8,9,10};
  int arr[N];
  int i, j;

  i = 0;
  for(j = 0; j < N; j++){
    if(vet[j] % 2 == 0){
      arr[i] = vet[j];
      i++;
    }
  }
  for(j = 0; j < N; j++){
    if(vet[j] % 2 != 0){
      arr[i] = vet[j];
      i++;
    }
  }
  printf("O array principal era: ");
  for(i = 0; i < N; i++){
    printf("%d ", vet[i]);
  }
  printf("\nO novo array eh: ");
  for(i = 0; i < N; i++){
    printf("%d ", arr[i]);
  }
  printf("\n");
  return 0;
}
