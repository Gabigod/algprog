/*Esse programa le uma string de tamanho ate STRMAX, considerando que serao inseridas
apenas letras minusculas e sem acento. Ao alterar o valor de STRMAX o programa deve
continuar funcionando normalmente. O programa deve exibir a string digitada transformada
para o estilo leer (pt.wikipedia.org/wiki/Leet), ou informar ao usuário que não eh possivel
realizar a conversao (caso a string n contenha 'a' 'b' 'e' 'l' 'o' 's' 't')*/
#include <stdio.h>
#include <string.h>
#define STRMAX 30

int main(){
    char str[STRMAX] = {0};
    int i, len, flag = 0;
    int c;      // propositalmente inteiro para detectar o EOF

    //entrada de dados
    printf("Insira a string: ");
    fgets(str, STRMAX, stdin);

    len = strlen(str);

    //limpa buffer de entrada
    if(str[len - 1] != '\n'){
        while((c = getchar()) != '\n' && c != EOF);     //consome da stdin até achar um \n ou EOF
    }

    //Tratativa condicional do \n no final da string deixado pelo fgets
    if(str[len - 1] == '\n') str[len - 1] = '\0';
    len = strlen(str);

    //conversao da string com o for para iterar pelo arranjo e switch para alterar caso
    for(i = 0; i < len; i++){
        switch(str[i]){
            case 'a':
                str[i] = '4';
                flag++;
                break;
            case 'b':           
                str[i] = '8';
                flag++;
                break;
            case 'e':
                str[i] = '3';
                flag++;
                break;
            case 'l':
                str[i] = '1';
                flag++;
                break;
            case 'o':
                str[i] = '0';
                flag++;
                break;
            case 's':
                str[i] = '5';
                flag++;
                break;
            case 't':
                str[i] = '7';
                flag++;
                break;
        }
    }
    

    //saidas do programa
    if(flag != 0){
        printf("%s\n", str);
    }else printf("Não possui leet\n");

    return 0;
}