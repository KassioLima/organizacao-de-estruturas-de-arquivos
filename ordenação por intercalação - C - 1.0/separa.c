#include <stdio.h>
#include <stdlib.h>
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
    Endereco *e;

    FILE *original = fopen("selecionados.dat", "r");
    // FILE *original = fopen("cep.dat", "r");


    printf("\nDigite o numero de arquivos\nem que deseja dividir - ");
    int arquivos;
    scanf("%d",&arquivos);

    int index, resto, qtd;
    long num_linhas, l, posicao, linhasArq;

    fseek(original,0,SEEK_END); //move o cabeçote para o final do arquivo
    posicao = ftell(original); //pega a posição do cabeçote
    rewind(original); //volta o cabeçote para o começo do arquivo
    num_linhas = (posicao/300);//o arquivo vai de 0 a [num_linhas]  

    //define o numero de linhas por arquivo
    linhasArq = num_linhas / arquivos;
    //Caso sobre linha, [resto] pega quantas linhas sobraram
    resto = num_linhas % arquivos;

    char linha[300];
    e = (Endereco*) malloc(linhasArq*300);
    printf("\nNumero de bytes - %ld\n", posicao);
    printf("\nnumero de arquivos desejado - %d\n", arquivos);
    printf("total de linhas - %ld\n", num_linhas);
    printf("linhas por arquivo - %ld\n", linhasArq);
    printf("linhas restantes - %d\n", resto);

    if(resto > 0)
    {
        printf("\n(Sera gerado pelo menos um arquivo a mais para\ncomportar as %d linhas excedentes)", resto);
    }

    printf("\nDeseja continuar?\n\nSim -> 1\nNao -> 0\n\n");
    int op = 0;
    scanf("%d",&op);

    if(op != 0)
    {
        int gerados = 0;

        printf("\nSeparando...\n");
        //[arquivos] ARQUIVOS
        for(index = 1; index <= arquivos; index++)
        {
            char nome[20];
            sprintf(nome, "blocos/bloco%d.dat", index);
            FILE *saida = fopen(nome, "w");

            fread(e,sizeof(Endereco),linhasArq,original);
            fwrite(e,sizeof(Endereco),linhasArq,saida);
            fclose(saida);

            gerados++;

            printf("\n%d - %s\n", index, nome);

            if((index + 1) > arquivos)
            {
                while(resto > 0)
                {                
                    sprintf(nome, "blocos/bloco%d.dat", ++index);
                    FILE *limpa = fopen(nome, "w");
                    fclose(limpa);

                    // printf("\n");
                    for(int x = 0; x < linhasArq; x++)
                    {
                        if(resto <= 0)
                        {
                            break;
                        }
                        FILE *saida = fopen(nome, "a");
                        fread(linha,300,1,original);
                        fprintf(saida,"%.300s",linha);
                        // printf("Faltam %d linha(s)\n", resto);
                        fclose(saida);
                        resto--;
                    }
                    printf("\n%d - %s\n", index, nome);
                    gerados++;
                }
                printf("\nForam gerados %d arquivos\n",gerados);
                break;
            }
        }
    }
    else
    {
        printf("\nPrograma finalizado\n");
    }
    free(e);
    fclose(original);
    return 0;
}