/*Esse programa le uma string do tamanho ate STRMAX e, entao, gera outra string de mesmo tamanho
cujo conteudo seja o mesmo da string original, mas com a ordem reversa. Por fim, programa deve
calcular e imprimir o numero de caracteres iguais que aparecem nas mesmas posicoes das duas strings
(deve-se considerar letras maiusculas e minusculas iguais)*/

#include <stdio.h>
#include <string.h> //strlen
#include <ctype.h> //toupper

#define STRMAX 20

int main(){
    int i, len, cont = 0;
    int c;      // propositalmente inteiro para detectar o EOF
    char str[STRMAX] = {0};
    char reverso[STRMAX] = {0};

    //Entrada de dados
    printf("Entre uma string: ");
    fgets(str, STRMAX, stdin);

    len = strlen(str);
    //Limpa buffer de entrada
    if(str[len - 1] != '\n'){
        while((c = getchar()) != '\n' && c != EOF);    
    }

    //Tratativa condicional do \n no final da string deixado pelo fgets
    if(str[len - 1] == '\n') str[len - 1] = '\0';
    len = strlen(str);

    //copia invertida de str para reverso
    for(i = 0; i < len; i++){
        reverso[i] = str[len -1 - i];              //evitar o \0
        if(toupper(reverso[i]) == toupper(str[i])) cont++;      //conta letra na msm posicao
    }

    //coloca o \0 no final da string copiada
    reverso[len] = '\0';

    //imprime o resultado no terminal
    printf("A string modificada eh: %s", reverso);
    printf("\nAs 2 strings apresentam %d caracteres iguais nas mesmas posicoes.", cont);

    return 0;
}