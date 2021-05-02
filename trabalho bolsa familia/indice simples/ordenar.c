#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct _Nis Nis;
 
struct _Nis
{
    char nis[14];
    char posicao[12];
};
 
int compara(const void *e1, const void *e2)
{
    return strncmp(((Nis*)e1)->nis,((Nis*)e2)->nis,14);
}
 
int main(int argc, char**argv)
{
    FILE *f = fopen("indice.dat", "r");
    Nis *e;
    long posicao, qtd;
       
    fseek(f,0,SEEK_END);
    posicao = ftell(f);
    qtd = posicao/sizeof(Nis);
    rewind(f);

    printf("\nAlocando...\n");
    e = (Nis*) malloc(posicao);
    
    printf("\nTrazendo...\n");
    fread(e,sizeof(Nis),qtd,f);
    fclose(f);

    printf("\nOrdenando...\n");
    qsort(e,qtd,sizeof(Nis),compara);

    f = fopen("indice_ord.dat","w");
    printf("\nEscrevendo...\n");
    fwrite(e,sizeof(Nis),qtd,f);
    fclose(f);

    free(e);

    printf("\nO arquivo foi ordenado\n");
    return 0;
}
