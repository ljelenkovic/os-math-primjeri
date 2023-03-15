#include <stdio.h>

int main()
{
	int *a, b;

	//a = &b; //"zaboravljena" inicijalizacija kazaljke
	printf("prije moguce greske"); //ali bez \n nece ispisati
	*a = 5;
	printf("\n*a=%d\n", *a);

	return 1;
}
// koristiti zastavicu -Wall radi ispisa svih upozorenja
// (i tako mozda otkrijemo greske)
// npr. gcc -Wall segv.c