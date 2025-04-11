#include <pthread.h>
#include <stdio.h>

void *nova_dretva(void *p)
{
	int *n = p;
	int num = *n;
	printf("U dretvi %d\n", num);

	return p; //ili pthread_exit(p);
}

int main()
{
	pthread_t t1, t2;
	int n1 = 1, n2 = 5, *status1, *status2;

	pthread_create(&t1, NULL, nova_dretva, (void *) &n1);
	pthread_create(&t2, NULL, nova_dretva, (void *) &n2);

	pthread_join(t1, (void *) &status1); //cekaj kraj dretve t1
	pthread_join(t2, (void *) &status2);

	printf("Dohvaceni statusi: %d %d\n", *status1, *status2);

	return 0;
}
