#include <stdio.h>
#include <string.h>
 
int main(int argc, char** argv)
{
    char linha[2048];
    int n = 0;
    FILE *f = fopen("indice_ord.dat", "r");
    char *nis;
    fgets(linha, 2048, f);
    while(!feof(f))
    {
        printf("%03d - %s", ++n, linha);
        fgets(linha, 2048, f);
        if(n == 10)
	{
        	break;
	}
    }
    fclose(f);
    return 0;
}
