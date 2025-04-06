/* Esse programa em C le os elementos de dois vetores (com 3 elementos cada),
calcula o produto escalar entre os vetores, e imprime o resultado na tela. Assumindo que os vetores
contem somente elementos inteiros*/

#include <stdio.h>

int main(){
    int x1, y1, z1, x2, y2, z2, resultado;

    printf("\nEntre as tres coordenadas do primeiro vetor: ");
    scanf("%d %d %d", &x1, &y1, &z1);

    printf("\nEntre as tres coordenadas do segundo vetor: ");
    scanf("%d %d %d", &x2, &y2, &z2);

    resultado = x1*x2 + y1*y2 + z1*z2;

    printf("\nO produto interno dos vetores eh: %d", resultado);

    return 0;
}