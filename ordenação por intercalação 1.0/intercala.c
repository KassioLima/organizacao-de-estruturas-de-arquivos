#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Endereco Endereco;
 
struct _Endereco
{
    //Muito bem, vou dar uma bizoiada aqui depois vc vai me explicando, blz?
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
};

void intercala(char n1[40], char n2[40], char n3[40])
{
    //Na linha acima, são recebidos como parâmetros três nomes 
    //de arquivos, onde os dois primeiros são os arquivos que serão intercalados
    // e o terceiro nome se referirá ao arquivo que receberá a intercalação.
   

   //Aqui, vamos declarar dois ponteiros para a estrutura endereço
    Endereco a; 
    Endereco b;

    FILE *f1 = fopen(n1, "r"); //aqui, vamos abrir o arquivo de nome n1, passando uma operação de leitura.
    FILE *f2 = fopen(n2, "r"); //aqui, abrimos em f2 po arquivo de nome n2, passando uma operação de leitura.
    FILE *f3 = fopen(n3, "w"); //finalmente o f3, será criado, e aberto, com nome n3, e será passada uma operação de write.
    // fclose(f3);
    // f3 = fopen(n3, "a");

    //Esta linha expressa uma operação de leitura:
    //podemos ler assim:
    //Estamos lendo "1" registro de cada vez, de tamanho "300 bytes"
    //do arquivo de ponteiro "f1", e jogando o endereço do registro lido em "&a"
    fread(&a, 300, 1, f1);

    //Aqui ocorre o análogo ao raciocínio acima.
    fread(&b, 300, 1, f2);

    //Em cada fread acima, estamos apenas lendo o primeiro registro de cada arquivo.
    //para podermos entrar no while. Similar a inicializar um contador.

    while(!feof(f1)&&!feof(f2))
    {
        if(strncmp(a.cep, b.cep, 8) < 0)
        {
            /*O comando strcmp funciona segundo
              uma operação de subtração, em que:
              a.cep - b.cep : se a.cep > b.cep, então strcmp retorna "+".
              a.cep - b.cep : se a.cep < b.cep, então strcmp retorna "-"  
            */
            //Então, se a.cep < b.cep, "a.cep" será escrito em f3.
            
            //O comando escrever já foi expresso no topo do código. O fprintf
            //está redirecionando a saída para f3, mas essa saída "fica registrada lá"
            
            //esse fprintf poderia ser trocado por um fwrite, o que é mais adequado.
            //seria:
            //fwrite(&a,sizeof(Endereco),1,f3);
            fprintf(f3, "%.72s%.72s%.72s%.72s%.2s%.8s \n",a.logradouro,a.bairro,a.cidade,a.uf,a.sigla,a.cep);
            fread(&a, 300, 1, f1);
            //Passo a passo do que ocorre na linha acima:
            //pega o arquivo cujo ponteiro é f1
            //lê 1 unidade
            //de 300 bytes
            //e armazena em "&a" o endereço do registro que está sendo lido.

        }
        //senao, se b.cep for o menor, ele é que será armazenado:
        else
        {
            fprintf(f3, "%.72s%.72s%.72s%.72s%.2s%.8s \n",b.logradouro,b.bairro,b.cidade,b.uf,b.sigla,b.cep);
            fread(&b, 300, 1, f2);
        }

    }
    //Caso a leitura de um dos arquivos chegar ao final, então o "while" acima
    //para, e então lemos eles novamente, porém, um de cada vez.
    while(!feof(f1))
    {
        fprintf(f3, "%.72s%.72s%.72s%.72s%.2s%.8s \n",a.logradouro,a.bairro,a.cidade,a.uf,a.sigla,a.cep);
        fread(&a, 300, 1, f1);
    }
    while(!feof(f2))
    {
        fprintf(f3, "%.72s%.72s%.72s%.72s%.2s%.8s \n",b.logradouro,b.bairro,b.cidade,b.uf,b.sigla,b.cep);
        fread(&b, 300, 1, f2);
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);
}

void copia(char n1[40], char n2[40])
{
    //aqui ele vai pegar um arquivo e gerar outro identico
    //só que com outro nome
    //vai pegar o arquivo de nome n1 e gerar um de nome n2 igual

    FILE *f1 = fopen(n1, "r");
    FILE *f2 = fopen(n2, "w");
    // fclose(f2);
    // f2 = fopen(n2, "a");

    char linha[300];//Parametriz
    //isso nao funciona, nao pode alocar assim
    //aí eu ja nao sei, mas mesmo assim da ruim
    //hmm entao deixa como estava
    fread(linha, 300, 1, f1);
    while(!feof(f1))
    {
        fprintf(f2, "%.300s", linha);//fwrite kkkkkkk
        fread(linha, 300, 1, f1);
    }
    fclose(f1);
    fclose(f2);
}

int main(int argc, char**argv)
{
    printf("\nDigite o numero de blocos ordenados - ");
    int arquivos;
    scanf("%d", &arquivos);

    if(arquivos > 1)
    {
        int intercalar = arquivos;

        int gerados = 0; //numero de arquivos da pasta inter_blocos

        if(arquivos%2 != 0)
        {
            intercalar--;
        }

        //gera os arquivos da pasta inter_blocos
        for(int x = 0, y = 0; x < intercalar; x+=2)
        {
            char n1[27];
            char n2[27];
            char n3[31];

            sprintf(n1, "ord_blocos/ord_bloco%d.dat", x+1);
            sprintf(n2, "ord_blocos/ord_bloco%d.dat", x+2);
            sprintf(n3, "inter_blocos/intercalado%d.dat", ++y);

            intercala(n1, n2, n3);
            gerados++;

            if(x+2 >= intercalar)
            {
                if(intercalar != arquivos) //sobrou um arquivo
                {
                    char n1[27];
                    char n2[31];

                    sprintf(n1, "ord_blocos/ord_bloco%d.dat", arquivos);
                    sprintf(n2, "inter_blocos/intercalado%d.dat", ++y);
                    copia(n1, n2);
                    gerados++;
                }
                break;
            }
        }
        //essa parte é complicada
        //tu é um gênio kk daqui eu só tô passando o olho
        //faz um chines cara
        //fica bem claro com o chines
        //td bem
        //espera só mais 5h KKKKKK ta
        //se vc quiser a gente chega mais cedo e faz
        //td bem
        //vou dar meu jeito
        //que horas ta bom pra vc? pode ser agr
        //eita nois
        //ta deixa eu falar com minha mãe cao cara
        //ainda nem comi hauahsusah
        //mas umas 16:30 eu ja posso estar la de boas
        //16:30 estarei ocupada. só estarei livre 17:30 
        //17:30 então
        //showzaço
        int numero = gerados;

        while(gerados > 1)
        {
            intercalar = gerados;
            int aux = intercalar;
            int ultimo = numero;

            if(gerados%2 != 0)
            {
                intercalar--;
            }

            gerados = 0;

            for(int x = (numero - aux), y = 0; y < intercalar; x+=2, y+=2)
            {
                char n1[31];
                char n2[31];
                char n3[31];

                sprintf(n1, "inter_blocos/intercalado%d.dat", x+1);
                sprintf(n2, "inter_blocos/intercalado%d.dat", x+2);
                sprintf(n3, "inter_blocos/intercalado%d.dat", ++numero);

                intercala(n1, n2, n3);
                gerados++;

                if(y+2 >= intercalar)
                {
                    if(intercalar != aux) //sobrou um arquivo
                    {
                        char n1[31];
                        char n2[31];

                        sprintf(n1, "inter_blocos/intercalado%d.dat", ultimo);
                        sprintf(n2, "inter_blocos/intercalado%d.dat", ++numero);
                        copia(n1, n2);
                        gerados++;
                    }
                    break;
                }
            }
        }
    }
    return 0;
}