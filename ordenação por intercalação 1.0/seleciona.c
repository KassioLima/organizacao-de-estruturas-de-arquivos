#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
int main(int argc, char**argv)
{
    char linha[300];
    FILE *f = fopen("cep.dat","r");
    FILE *s = fopen("selecionados.dat", "w");

    printf("\nDigite o numero de linhas que deseja selecionar - ");
    long linhas;
    scanf("%lu", &linhas);
    printf("\n\n");
    srand((unsigned)time(NULL));
    for(long x = 1; x <= linhas; x++)
    {
        fseek(f,(rand()%100000)*300,SEEK_SET);
    	fread(linha,300,1,f);
        printf("%lu - Selecionado\n", x);
        fprintf(s,"%.300s", linha);
    }
    printf("\nCompleto\n");
    fclose(f);
    fclose(s);
    return 0;
}