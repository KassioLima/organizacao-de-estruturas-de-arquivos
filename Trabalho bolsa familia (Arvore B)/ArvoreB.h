#ifndef __ARVORE_B__
#define __ARVORE_B__

#define TAM_CHAVE 14 //14 bytes cada elemento
#define TAM_PAG 300 //300 elementos

#include <stdio.h>

typedef struct _ArvoreB ArvoreB;
typedef struct _ArvoreB_Cabecalho ArvoreB_Cabecalho;
typedef struct _ArvoreB_Elemento ArvoreB_Elemento;
typedef struct _ArvoreB_Pagina ArvoreB_Pagina;

struct _ArvoreB_Cabecalho
{
    long raiz;
};

struct _ArvoreB
{
    ArvoreB_Cabecalho* cabecalho;
    FILE* f;    
};

struct _ArvoreB_Elemento
{
    char chave[TAM_CHAVE];
    long posicaoRegistro;
    long paginaDireita;
};

struct _ArvoreB_Pagina
{
    int quantidadeElementos;
    long paginaEsquerda;
    ArvoreB_Elemento elementos[TAM_PAG];
};

#endif
