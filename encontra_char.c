/*Esse programa le uma string de no maximo TAM-1 caracteres e uma letra, apos procura na string a primeira aparicao dessa letra e retorna a posicao*/

#include <stdio.h>
#define TAM 500

int main(){
    char str[TAM], letra;
    int i, achou = 0;

    printf("Digite a string: ");
    fgets(str, TAM, stdin);

    printf("Digite a letra: ");
    letra = getchar();

    i=0;
    while(str[i] != '\0' && achou == 0){
        if(str[i] == letra) achou = 1;
        else i++;
    }

    printf("A primeira aparicao da letra [%c] na string eh na posicao: %d\n", letra, i);

    return 0;
}