/*Esse programa le 6 vars de entrada que representam as cordenadas de 3 pontos no plano cartesiano.
 Depois ele calcula a distancia entre os pontos, e por fim a area do triangulo formado*/ 

#include <stdio.h>
#include <math.h>

float dist(float a, float b, float c, float d){
	return sqrt(pow((a - c),2) + pow((b - d),2));
}

int main(){
	float x[3], y[3];
	float dist12, dist13, dist23;
	float area, s;
	int i;

	for(i = 0; i < 3; i++){
	  printf("\nInsira as coordenadas dos pontos: ");
	  scanf("%f%f", &x[i], &y[i]);
	}
	
	dist12 = dist(x[0], y[0], x[1], y[1]);
	dist13 = dist(x[0], y[0], x[2], y[2]);
	dist23 = dist(x[1], y[1], x[2], y[2]);
	s = (dist12 + dist13 + dist23) / 2;

	area = sqrt((s)*(s - dist12) * (s - dist13) * (s - dist23));

	printf("\nA distancia do ponto 1 para o 2 eh: %6.2f", dist12);
	printf("\nA distancia do ponto 1 para o 3 eh: %6.2f", dist13);
	printf("\nA distancia do ponto 2 para o 3 eh: %6.2f", dist23);
	printf("\nA area do triangulo eh: %6.2f\n", area);

	return 0;
}
