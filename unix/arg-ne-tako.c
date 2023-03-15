#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
int i, suma = 0;
printf("Parametri komandne linije: ");
for (i = 0; i < argc; i++)
{
printf("%s ", argv[i]);
if (i > 0)
suma += atoi(argv[i]);
}
printf("\n Suma=%d\n", suma);
return 0;
}
