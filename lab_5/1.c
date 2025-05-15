/*Esse programa le um valo de angulo x em gaus,
um numero de termos maximo e a tolerancia minima
da aproximacao (positivo). Depois converte o angulo
para radianos radianos e aproxima o valor de seno
usando uma serie de taylor.
Ao final, o programa imprime a aproximacao com 15
casas decimais e o numero de iteracoes necessarias.*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

int main(){
  //vars de entrada
  float ang = 0.0, tolerancia = 0.0, ang_rad = 0;
  int i_max = 0;
  //vars de saida
  double seno = 0.0;
  int i = 0;
  //vars auxiliares
  double tcorrente = DBL_MAX;
  int flag = 1;
  long int fatorial = 1;
  

  //  Entrada e validacao de dados pelo usuario
  printf("Digite o valor do angulo (em graus): ");
  scanf("%f", &ang);
  while(i_max <= 0) {
    printf("Digite o numero maximo de iteracoes: ");
    scanf("%d", &i_max);
  }
  while(tolerancia <= 0){
    printf("Digite a tolerancia: ");
    scanf("%f", &tolerancia);
  }
  
  //conversao de graus para rad
  ang_rad = ang * M_PI / 180;

  //calculo do sen por aproximacao
  while(i <= i_max && fabs(tcorrente) >= tolerancia ){
    //calcula o termo atual da iteracao e acumula no seno
    tcorrente = flag * (pow(ang_rad, 2 * i + 1) / fatorial);
    seno += tcorrente;

    //prepara pra proxima iteracao
    flag = -flag;
    i++;
    fatorial = fatorial * (2 * i) * (2 * i + 1);
  }
  
  //saida do programa
  printf("\nAproximacao de sen(%.2f): %.15lf com %d termos\n", ang, seno, i);
  return 0;
}