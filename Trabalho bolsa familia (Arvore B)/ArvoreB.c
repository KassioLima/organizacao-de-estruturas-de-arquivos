#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include "ArvoreB.h"

ArvoreB_Pagina* ArvoreB_alocaPagina()
{
    int i;
    ArvoreB_Pagina* resp = (ArvoreB_Pagina*) malloc(sizeof(ArvoreB_Pagina));
    resp->quantidadeElementos = 0;
    resp->paginaEsquerda = 0;
    for(i=0; i<TAM_PAG; i++)
    {
        memset(resp->elementos[i].chave,'\0',TAM_CHAVE);
        resp->elementos[i].paginaDireita = 0;
        resp->elementos[i].posicaoRegistro = 0;
    }
    return resp;
}

void ArvoreB_desalocaPagina(ArvoreB_Pagina* p)
{
    if(p)
    {
        free(p);
    }
}

ArvoreB* ArvoreB_Abre(const char* nomeArquivo)
{
    ArvoreB* resp = (ArvoreB*) 0;
    ArvoreB_Cabecalho* cabecalho = (ArvoreB_Cabecalho*) 0;
    ArvoreB_Pagina* raiz = (ArvoreB_Pagina*) 0;
    FILE* f = fopen(nomeArquivo, "r");
    if(!f)
    {
        f = fopen(nomeArquivo,"w");
        if(!f)
        {
            fprintf(stderr,"Arquivo %s não pode ser criado\n", nomeArquivo);
            return resp;
        }
        cabecalho = (ArvoreB_Cabecalho*) malloc(sizeof(ArvoreB_Cabecalho));
        cabecalho->raiz = sizeof(ArvoreB_Cabecalho);
        fwrite(cabecalho,sizeof(ArvoreB_Cabecalho),1,f);
        raiz = ArvoreB_alocaPagina();
        fwrite(raiz,sizeof(ArvoreB_Pagina),1,f);
        ArvoreB_desalocaPagina(raiz);
        free(cabecalho);
    }
    fclose(f);
    f = fopen(nomeArquivo, "rb+");
    resp = (ArvoreB*) malloc(sizeof(ArvoreB));
    resp->f = f;
    resp->cabecalho = (ArvoreB_Cabecalho*) malloc(sizeof(ArvoreB_Cabecalho));
    fread(resp->cabecalho,sizeof(ArvoreB_Cabecalho),1,f);
    return resp;
}

int ArvoreB_Compara(const void *e1, const void* e2)
{
    return strncmp(((ArvoreB_Elemento*)e1)->chave,((ArvoreB_Elemento*)e2)->chave,TAM_CHAVE); 
}

void ArvoreB_escreveCabecalho(ArvoreB* arvore)
{
    fseek(arvore->f, 0, SEEK_SET);
    fwrite(arvore->cabecalho, sizeof(ArvoreB_Cabecalho), 1, arvore->f);
}

void ArvoreB_Fecha(ArvoreB* arvore)
{
    if(arvore)
    {
        ArvoreB_escreveCabecalho(arvore);
        fclose(arvore->f);
        free(arvore->cabecalho);
        free(arvore);
    }
}

ArvoreB_Elemento* ArvoreB_Split(ArvoreB *arvore, long posicaoPagina, ArvoreB_Pagina* pagina, ArvoreB_Elemento* overflow)
{
    int i;
    ArvoreB_Elemento aux;
    ArvoreB_Elemento* resp;
    ArvoreB_Pagina* paginaSplit;
    // O elemento na área de overflow é menor que o ultimo elemento da página?
    if(ArvoreB_Compara(overflow,&pagina->elementos[pagina->quantidadeElementos-1])<0)
    {
        // Trocar o último com o elemento no overflow e reordenar a página.
        aux = *overflow;
        *overflow = pagina->elementos[pagina->quantidadeElementos-1];
        pagina->elementos[pagina->quantidadeElementos-1] = aux;
        qsort(pagina->elementos,pagina->quantidadeElementos,sizeof(ArvoreB_Elemento),ArvoreB_Compara);
    }
    paginaSplit = ArvoreB_alocaPagina();
    for(i=TAM_PAG/2+1;i<TAM_PAG;i++)
    {
        paginaSplit->elementos[paginaSplit->quantidadeElementos] = pagina->elementos[i];
        paginaSplit->quantidadeElementos++;
    }
    paginaSplit->elementos[paginaSplit->quantidadeElementos] = *overflow;
    paginaSplit->quantidadeElementos++;
    pagina->quantidadeElementos = TAM_PAG/2;
    fseek(arvore->f, posicaoPagina, SEEK_SET);
    fwrite(pagina, sizeof(ArvoreB_Pagina),1,arvore->f);
    fseek(arvore->f,0,SEEK_END);
    resp = (ArvoreB_Elemento*) malloc(sizeof(ArvoreB_Elemento));
    paginaSplit->paginaEsquerda = pagina->elementos[TAM_PAG/2].paginaDireita;
    memcpy(resp->chave,pagina->elementos[TAM_PAG/2].chave,TAM_CHAVE);
    resp->posicaoRegistro = pagina->elementos[TAM_PAG/2].posicaoRegistro;
    resp->paginaDireita = ftell(arvore->f);
    fwrite(paginaSplit, sizeof(ArvoreB_Pagina),1,arvore->f);
    ArvoreB_desalocaPagina(paginaSplit);
    return resp;
}

ArvoreB_Elemento* ArvoreB_Insere_Recursiva(ArvoreB* arvore, long posicaoPagina, char chave[TAM_CHAVE], long posicaoRegistro)
{
    int i;
    long posicaoPaginaFilho;
    ArvoreB_Elemento* resp = (ArvoreB_Elemento*) 0;
    ArvoreB_Elemento *elementoSplit = (ArvoreB_Elemento*) 0;
    ArvoreB_Elemento overflow;
    // printf("Inserindo %.8s na página %ld\n", chave, posicaoPagina);
    ArvoreB_Pagina *pagina = ArvoreB_alocaPagina();
    fseek(arvore->f, posicaoPagina, SEEK_SET);
    fread(pagina, sizeof(ArvoreB_Pagina),1,arvore->f);
    if(pagina->paginaEsquerda == 0) // Folha
    {
        if(pagina->quantidadeElementos < TAM_PAG)
        {
            memcpy(pagina->elementos[pagina->quantidadeElementos].chave,chave,TAM_CHAVE);
            pagina->elementos[pagina->quantidadeElementos].posicaoRegistro = posicaoRegistro;
            pagina->quantidadeElementos++;
            qsort(pagina->elementos,pagina->quantidadeElementos,sizeof(ArvoreB_Elemento),ArvoreB_Compara);
            fseek(arvore->f, posicaoPagina, SEEK_SET);
            fwrite(pagina, sizeof(ArvoreB_Pagina),1,arvore->f);
        }
        else
        {
            memcpy(overflow.chave, chave, TAM_CHAVE);
            overflow.posicaoRegistro = posicaoRegistro;
            overflow.paginaDireita = 0;
            resp = ArvoreB_Split(arvore, posicaoPagina, pagina, &overflow);
        }
    }
    else // Não é folha
    {
        posicaoPaginaFilho = pagina->paginaEsquerda;
        for(i=0; i<pagina->quantidadeElementos; i++)
        {
            if(strncmp(chave,pagina->elementos[i].chave,TAM_CHAVE)<0)
            {
                break;
            }
            posicaoPaginaFilho = pagina->elementos[i].paginaDireita;
        }
        elementoSplit = ArvoreB_Insere_Recursiva(arvore,posicaoPaginaFilho,chave,posicaoRegistro);
        if(elementoSplit)
        {
            if(pagina->quantidadeElementos < TAM_PAG)
            {
                pagina->elementos[pagina->quantidadeElementos] = *elementoSplit;
                pagina->quantidadeElementos++;
                qsort(pagina->elementos,pagina->quantidadeElementos,sizeof(ArvoreB_Elemento),ArvoreB_Compara);
                fseek(arvore->f,posicaoPagina,SEEK_SET);
                fwrite(pagina,sizeof(ArvoreB_Pagina),1,arvore->f);
                free(elementoSplit);
            }
            else
            {
                resp = ArvoreB_Split(arvore, posicaoPagina, pagina, elementoSplit);
                free(elementoSplit);
            }
        }
    }
    ArvoreB_desalocaPagina(pagina);
    return resp;
}

void ArvoreB_Insere(ArvoreB* arvore, char chave[TAM_CHAVE], long posicaoRegistro )
{
    ArvoreB_Pagina *novaRaiz;
    ArvoreB_Elemento *elementoSplit;
    elementoSplit = ArvoreB_Insere_Recursiva(arvore, arvore->cabecalho->raiz, chave, posicaoRegistro);
    if(elementoSplit)
    {
        novaRaiz = ArvoreB_alocaPagina();
        novaRaiz->quantidadeElementos = 1;
        novaRaiz->elementos[0] = *elementoSplit;
        novaRaiz->paginaEsquerda = arvore->cabecalho->raiz;
        fseek(arvore->f, 0, SEEK_END);
        arvore->cabecalho->raiz = ftell(arvore->f);
        fwrite(novaRaiz,sizeof(ArvoreB_Pagina),1,arvore->f);
        ArvoreB_escreveCabecalho(arvore);
        ArvoreB_desalocaPagina(novaRaiz);
        free(elementoSplit);
    }
}

void ArvoreB_PrintDebug(ArvoreB* arvore)
{
    //VARIÁVEIS
    long posicaoPagina;
    int i;
    FILE *saida = fopen("saida.txt", "w");
    ArvoreB_Pagina *pagina = ArvoreB_alocaPagina(); //SEPARA UM ESPAÇO PARA UMA PÁGINA

    //VAI PRA RAIZ
    fseek(arvore->f, sizeof(ArvoreB_Cabecalho), SEEK_SET);

    //PEGA A POSIÇÃO ATUAL
    posicaoPagina = ftell(arvore->f);
    while(fread(pagina,sizeof(ArvoreB_Pagina),1,arvore->f))
    {
        if(posicaoPagina == arvore->cabecalho->raiz)
        {
            //printf("* ");
            fprintf(saida, "* ");
        }
        else
        {
            //printf("  ");
            fprintf(saida, "  ");
        }
        //printf("%5ld => %5ld|",posicaoPagina, pagina->paginaEsquerda);
        fprintf(saida, "%10ld => %10ld|",posicaoPagina, pagina->paginaEsquerda);
        for(i=0; i < pagina->quantidadeElementos; i++)
        {

            //printf("(%.14s)|%5ld|",pagina->elementos[i].chave, pagina->elementos[i].paginaDireita);
            fprintf(saida, "(%.14s - pos %10ld)|%10ld|",pagina->elementos[i].chave, pagina->elementos[i].posicaoRegistro, pagina->elementos[i].paginaDireita);

        }
        //printf("\n");
        fprintf(saida, "\n");
        posicaoPagina = ftell(arvore->f);
    }
    fclose(saida);
}

long buscaPos(ArvoreB* arvore, char nis[14])
{
    //VARIÁVEIS
    int i;
    ArvoreB_Pagina *pagina = ArvoreB_alocaPagina(); //SEPARA UM ESPAÇO PARA UMA PÁGINA

    fseek(arvore->f, arvore->cabecalho->raiz, SEEK_SET); // vai para raiz
 
    while(fread(pagina, sizeof(ArvoreB_Pagina), 1, arvore->f))
    {
        for(i = 0; i < pagina->quantidadeElementos; i++)
        {
            //printf( "|%.14s| ",pagina->elementos[i].chave);


            if(strcmp(nis, pagina->elementos[i].chave) == 0) //compara nis digitado com os elementos da pag
            {
                return pagina->elementos[i].posicaoRegistro; //retorna a posicao
            }
            else if(strcmp(nis, pagina->elementos[i].chave) < 0) // ve se esta na pagina anterior
            {
                if(i == 0) //se esta no primeiro elemento
                {
                    if(pagina->paginaEsquerda != 0) //E tiver elemento a esquerda, entao vai para a pagina da esq
                    {
                        fseek(arvore->f, pagina->paginaEsquerda, SEEK_SET); //vai para a esquerda da página
                    }
                    else //se não tiver filho, elemento, então para ai
                    {
                        return -1;
                    }
                }
                else //nos elementos do meio
                {
                    if(pagina->elementos[i-1].paginaDireita != 0) //tiver filhos a direita no elemento anterior
                    {
                        fseek(arvore->f, pagina->elementos[i-1].paginaDireita, SEEK_SET); //vai para esquerda do elemento, que é a direita do anterior
                    }
                    else
                    {
                        return -1;
                    }
                }
                break;
            }
            else if(strcmp(nis, pagina->elementos[i].chave) > 0) //vai para direita 
            {
                if(i+1 == pagina->quantidadeElementos) //ve se é ultimo elemento
                {
                    if(pagina->elementos[i].paginaDireita != 0) //se tem elemento a direita
                    {
                        fseek(arvore->f, pagina->elementos[i].paginaDireita, SEEK_SET); //passa para prox pag à direita
                    }
                    else
                    {
                        return -1;
                    }
                    break;
                }
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

void buscaNoArquivo(ArvoreB* a, char bolsa[20])
{
    FILE *f = fopen(bolsa, "r");
    printf("\nDigite o NIS: ");
    char nis[14];
    scanf("%s",&nis); //p pegar todos os caracteres

    long pos = buscaPos(a, nis);
    if(pos > -1)
    {
        char linha[2048];
        fseek(f, pos, SEEK_SET);
        fgets(linha, 2048, f);
        printf("\n%s", linha);
    }
    else
    {
        printf("\nNao achou\n");
    }
    fclose(f);
}


typedef struct _Bolsa Bolsa;
struct _Bolsa
{
    char chave[16];
    long posicao;
};

void leBolsa(ArvoreB* a, char bolsa[20])
{
    char linha[2048]; //por ter tamanhos variados chuta um valor grande
    Bolsa b; 
    char* campo;
    FILE *f = fopen(bolsa, "r");
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);

   if(bolsa == "bolsa.csv")
        fgets(linha, 2048, f); //descarta a primeira linha do arquivo [IMPORTANTE NO BOLSA.CSV]
    b.posicao = ftell(f);
    fgets(linha, 2048, f);

    printf("\nInserindo na arvore...\n");
    printf("\n%ld / %ld bytes\t restam %ld bytes", b.posicao, tamanho, (tamanho-b.posicao));
    while(!feof(f))
    {
        campo = strtok(linha,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        strcpy(b.chave, campo);
        //printf("nis => %s esta em %ld\n", b.chave, b.posicao);
        ArvoreB_Insere(a, b.chave, b.posicao);
        b.posicao = ftell(f);
 
        if(b.posicao % 20000 == 0)
            printf("\n%ld / %ld bytes\t restam %ld bytes", b.posicao, tamanho, (tamanho-b.posicao));
        fgets(linha, 2048, f);

    }
    printf("\n%d / %ld bytes\t restam 0 bytes", b.posicao, tamanho);
    printf("\n\nInserido\n");
    fclose(f);
}

void geraListaNis(char bolsa[20])
{
    printf("\nDigite a quantidade de linhas para adicionar a lista: ");
    long limite;
    scanf("%ld", &limite);
    srand((unsigned)time(NULL));
    char linha[2048]; //por ter tamanhos variados chuta um valor grande
    Bolsa b; 
    char* campo;
    FILE *f = fopen(bolsa, "r");
    FILE *f2 = fopen("nis.dat", "w");
    if(bolsa == "bolsa.csv")
        fgets(linha, 2048, f); //descarta a primeira linha do arquivo [IMPORTANTE NO BOLSA.CSV]
    fgets(linha, 2048, f);
    printf("\nGerando lista...\n\n");
    long x = 0;
    while(!feof(f) && (x++ < limite))
    {
        campo = strtok(linha,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        campo = strtok(NULL,"\t");
        strcpy(b.chave, campo);

        fprintf(f2, "%s\n", b.chave);

        fgets(linha, 2048, f);
    }
    printf("\nCompleto.\n");
    fclose(f);
    fclose(f2);
}


void buscaParaGrafico(ArvoreB* a)
{
    clock_t c2, c1; /* variáveis que contam ciclos do processador */
    float tempo;
    char linha[2048];
    FILE *f = fopen("nis.dat", "r");
    fgets(linha, 2048, f);
    char nis[14];
    sprintf(nis, "%.14s", linha);
    printf("\nQual eh o numero maximo de elementos a serem buscados? ");
    int limite;
    scanf("%d", &limite);
    int x = 0;
    c1 = clock();
    while(!feof(f) && (x++ < limite))
    {
        sprintf(nis, "%.14s", linha);
        //printf("%ld\n", buscaPos(a, nis));
        buscaPos(a, nis);
        fgets(linha, 2048, f);
    }
    c2 = clock();
    fclose(f);
    tempo = (c2 - c1)*1000/CLOCKS_PER_SEC;
    printf("\n\n%f milissegundos", tempo);
}

void aleatorios(char bolsa[20])
{
	char linha[2048];
    FILE *f = fopen(bolsa,"r");
    FILE *s = fopen("bolsa2.csv", "w");

    printf("\nDigite o numero de linhas: ");
    long linhas;
    scanf("%ld", &linhas);
    srand((unsigned)time(NULL));
    long x;
    for(x = 1; x <= linhas; x++)
    {
        fseek(f,(rand()%100000),SEEK_SET);
    	fgets(linha, 2048, f);
    	fgets(linha, 2048, f);
    	if(linha == NULL)
            x--;
        else
		  fprintf(s, "%s", linha);
    }
    printf("\nCompleto\n");
    fclose(f);
    fclose(s);
}
void sequencial(char arqbolsa[20])
{
	//
	// ARQUIVOS
	//
	FILE *bolsa = fopen(arqbolsa, "r");
	FILE *saida = fopen("bolsa2.csv", "w");

	//
	// VARIÁVEIS
	//
	
	printf("\nDigite o numero de linhas: ");
	long linhas;
	scanf("%ld", &linhas);
	
	char linha[2048];
	long x = 0;

	//
	// LEITURA
	//
	fgets(linha, 2048, bolsa); //a primeira linha é descartada
	while(x++ < linhas && !feof(bolsa))
	{
		fgets(linha, 2048, bolsa);
		fprintf(saida, "%s", linha);
	}

	//
	// FECHAR ARQUIVOS
	//
	printf("\nCompleto\n");
	fclose(bolsa);
	fclose(saida);
}

void mudarbolsa(char *bolsa)
{
    printf("\ncom qual arquivo de bolsa deseja trabalhar? ");
    scanf("%s", bolsa);
    printf("\nUtilizando '%s'\n", bolsa);
}

void gerarArqMenor(char bolsa[20])
{
	int op = 0;
	do
	{
		printf("\nSELECIONE UMA OPCAO\n");
		printf("\n1 - Sequencial\n2 - Aleatorios\n\n0 - sair\n\n");

		scanf("%d", &op);

		switch(op)
		{
			case 1: sequencial(bolsa); printf("\nGerado arquivo 'bolsa2.csv'\n"); mudarbolsa(bolsa); break;
			case 2: aleatorios(bolsa); printf("\nGerado arquivo 'bolsa2.csv'\n"); mudarbolsa(bolsa); break;
			case 0: break;
		}

		printf("\n");

	}
	while(op != 0);

}

void manual()
{
    FILE *manual = fopen("manual.txt", "r");
    char linha[2048];
    fgets(linha, 2048, manual);
    printf("\n");
    printf("                                            MANUAL\n-----------------------------------------------------------------------------------------------\n\n");
    while(!feof(manual))
    {
        printf("\t%s", linha);
        fgets(linha, 2048, manual);
    }
    printf("\t%s", linha);
    printf("\n\n-----------------------------------------------------------------------------------------------");}



int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Portuguese");
    
    ArvoreB* a = ArvoreB_Abre("arvore.dat");
    
    char bolsa[20] = "bolsa.csv";
    printf("\n\nLEIA COMO FUNCIONAM TODAS AS FUNCOES ANTES DE RODAR O PROGRAMA (FUNCAO 8)\n\n");
    printf("\n[ATENCAO!] Utilizando '%s'\n", bolsa);

    //mudarbolsa(bolsa);
    
    int op;
    do
    {
        printf("\nSELECIONE UMA OPCAO\n\n");
        printf("1 - Marcar tempo para X buscas\n2 - Buscar por NIS\n3 - Gerar arquivo menor\n4 - Gerar lista de NIS\n5 - Gerar debug\n6 - Gerar arvore\n7 - Mudar de arquivo de bolsas\n\n8 - LER MANUAL\n\n0 - sair\n\n");
        scanf("%d", &op);

        switch(op)
        {
            case 1: buscaParaGrafico(a); break;
            case 2: buscaNoArquivo(a, bolsa); break;
            case 3: gerarArqMenor(bolsa); break;
            case 4: geraListaNis(bolsa); break;
            case 5: ArvoreB_PrintDebug(a); printf("\nDebug gerado\n"); break;
            case 6: leBolsa(a, bolsa); break;
            case 7: mudarbolsa(bolsa); break;
            case 8: manual(); break;
        }

        printf("\n");

    }
    while(op != 0);

    ArvoreB_Fecha(a);
    return 0;
}
