/*Esse programa le um valor inteiro N e preenche por leitura do teclado o arranjo, validando, antes, se N eh menor ou igual que TAMMAX
Em seguida, o programa calcula e mostra na tela o maior e menor valor informado (com 4 decimais), a média dos valores informados (com 4 decimais)
a quantidade de valores do arranjo tal que o modulo da diferença entre o valor e a média é menor que metade da diferença entre o maior e menor numero
do arranjo.*/
#include <stdio.h>
#include <math.h>

#define TAMMAX 7

int main(){
    int n,i,qtd = 0;
    float v[TAMMAX], maior, menor, media = 0;
    
    do {
        printf("Entre o numero de elementos no arranjo: ");
        scanf("%d", &n);
    } while(n > TAMMAX || n < 0);

    for(i = 0; i < n; i++){
        printf("Entre com um valor: ");
        scanf("%f", &v[i]);
    }
    maior = v[0];
    menor = v[0];
    for(i = 0; i < n; i++){
        if(v[i] > maior) maior = v[i];
        if(v[i] < menor) menor = v[i];
        media += (v[i] - media)/(i+1);
    }
    for(i = 0; i < n; i++){
        if(fabs(v[i] - media) < 0.5*(maior - menor)) qtd++;
    }

    printf("\nMenor valor: %.4f", menor);
    printf("\nMaior valor: %.4f", maior);
    printf("\nMedia: %.4f", media);
    printf("\nQuantidade de valores perto da media: %d\n", qtd);
}