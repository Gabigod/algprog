/*Esse programa le o capital, a taxa de juros e o numero de periodos da aplicacao, e entao calcula (e imprime
com duas casas decimais na tela) o montante ao final da aplicacao.*/
#include <stdio.h>
#include <math.h>

int main(){
    float capital, taxa, montante; //O capital, taxa de juros e montante serao numeros reais. Portanto Float
    int periodos; //O numero de periodos contabilizados será sempre um inteiro.

    //captura de dados pelo usuario    
    printf("Insira o capital inicial: ");
    scanf("%f", &capital);

    printf("\nInsira a taxa de juros em %%: ");
    scanf("%f", &taxa);
    
    printf("Insira o numero de meses da aplicacao: ");
    scanf("%i", &periodos);

    //calculo do montante
    montante = capital * pow( 1 + (taxa/100) , periodos);

    //imprime no terminal o resultado
    printf("O total apos %i meses eh de %.2f reais", periodos, montante);

    return 0;
}