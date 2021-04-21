#include <stdio.h>
#include <string.h>
 
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
 
int main(int argc, char**argv)
{
    FILE *f;
    Endereco e;
    int qt;
 
    if(argc != 2)
    {
        fprintf(stderr, "USO: %s [CEP]", argv[0]);
        return 1;
    }
 
    long posicao, primeiro, ultimo, meio;
    f = fopen("cep_ordenado.dat","r"); //abre arquivo
    fseek(f,0,SEEK_END); //move o cabeçote para o final do arquivo
    posicao = ftell(f); //pega a posição do cabeçote
    rewind(f); //volta o cabeçote para o começo do arquivo
    
    primeiro = 0;  //primeira linha
    ultimo = (posicao/sizeof(Endereco))-1; //penúltima linha

    int achou = 0, lidos = 0;
    
    while(primeiro <= ultimo)
    {
    	lidos++;
    	meio = (primeiro+ultimo)/2;
        fseek(f,meio*sizeof(Endereco),SEEK_SET); //move o cabeçote para a linha do meio
    	fread(&e,sizeof(Endereco),1,f);
        if(strncmp(argv[1],e.cep,8) == 0)
        {
            printf("%.72s\n%.72s\n%.72s\n%.72s\n%.2s\n%.8s\n",e.logradouro,e.bairro,e.cidade,e.uf,e.sigla,e.cep);
            achou = 1;
            break;
        }
        else if(strncmp(argv[1],e.cep,8) > 0) //ir para a direita
        {
            primeiro = meio+1;
        }
        else //ir para a esquerda
        {
            ultimo = meio-1;
        }
    }   
    fclose(f);

    if(achou == 0)
    {
        printf("\nNenhum resultado para %s encontrado\nLidos: %d", argv[1], lidos);
    }
    return 0;
}