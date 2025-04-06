/*Esse programa lê dois numeros complexos e mostra no terminal o produto destes.*/

#include <stdio.h>

int main(){
    float real1, real2, im1, im2, resultado_real, resultado_im; //Declaração das variaveis reais

    //Le as entradas do primeiro numero complexo, ignorando os espacamentos
    printf("Entre as partes real e imaginaria do primeiro numero: ");
    scanf(" %f %f", &real1, &im1);

    //Le as entradas do segundo numero complexo, ignorando os espacamentos
    printf("\nEntre as partes real e imaginaria do segundo numero: ");
    scanf(" %f %f", &real2, &im2);    

    //calculo dos resultados a partir da expansao binomial
    resultado_real = real1 * real2 - im1 * im2;
    resultado_im = real1 * im2 + im1 * real2;

    printf("\nO produto de %.4f %.4fi por %.4f %.4fi eh %.4f %.4fi", real1, im1, real2, im2, resultado_real, resultado_im);

    return 0;
}