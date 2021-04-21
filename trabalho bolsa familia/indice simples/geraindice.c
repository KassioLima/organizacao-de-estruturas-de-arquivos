#include <stdio.h>
#include <string.h>
 
int main(int argc, char** argv)
{
    char linha[2048];
    int coluna = 0, n = 0;
    long posicao;
    char* campo;
    FILE *f = fopen("../bolsa.csv", "r"), *saida = fopen("indice.dat", "w");
    fgets(linha, 2048, f);
    posicao = ftell(f);
    fgets(linha, 2048, f);
    printf("\nGerando indice...\n");
    while(!feof(f))
    {
        campo = strtok(linha,"\t");
        for (int i = 0; i < 7; ++i)
        	campo = strtok(NULL,"\t");
        
        fprintf(saida, "%s %010ld\n", campo, posicao);
        // printf("%.10s\n", pos);
        posicao = ftell(f);
        fgets(linha, 2048, f);
      
        //if(++n == 10)
        //	break;
    }
    fclose(f);
    fclose(saida);
    return 0;
}
