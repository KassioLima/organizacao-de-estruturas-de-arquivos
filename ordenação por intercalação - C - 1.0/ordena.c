#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#pragma pack(1)
 
typedef struct _Endereco Endereco;
 
struct _Endereco
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
};
 
int compara(const void *e1, const void *e2)
{
    return strncmp(((Endereco*)e1)->cep,((Endereco*)e2)->cep,8);
}
 
int main(int argc, char**argv)
{
    FILE *f;
    Endereco *e;
    long posicao, qtd;

    printf("\nDigite o numero de arquivos\nque deseja ordenar - ");
    int arquivos;
    scanf("%d", &arquivos);
 	
 	printf("\nOrdenando...\n");
 	for(int index = 1; index <= arquivos; index++)
    {
        char nome[20];
        sprintf(nome, "blocos/bloco%d.dat", index);

	    f = fopen(nome,"r");
	    fseek(f,0,SEEK_END);
	    posicao = ftell(f);
	    qtd = posicao/sizeof(Endereco);
	    e = (Endereco*) malloc(posicao);
	    rewind(f);
	    fread(e,sizeof(Endereco),qtd,f);
	    fclose(f);
	    qsort(e,qtd,sizeof(Endereco),compara);
	    char nomeord[27];
        sprintf(nomeord, "ord_blocos/ord_bloco%d.dat", index);
	    f = fopen(nomeord,"w");
	    fwrite(e,sizeof(Endereco),qtd,f);
	    fclose(f);
	    free(e);
	}

	printf("\nOs %d arquivos foram ordenados\n", arquivos);

	return 0;
}