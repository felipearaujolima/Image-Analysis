#include "imagem_basics.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <png.h> 
#include <math.h>

Imagem* CriaImagem(int nx, int ny, int nbandas) /* Essa fun��o retorna um ponteiro para uma estrutura Imagem */
{
    Imagem* img = (Imagem*)calloc(1, sizeof(Imagem)); /* estou criando um ponteiro para uma estrutura imagem (casting), 
    e estou alocando mem�ria para ela (iniciando em 0) do tamanho da estrutura da Imagem,
    logo estou alocando mem�ria para o nx, ny, nbandas e int*** bandas, mas veja que para eu acessar corretamente essas 
    estruturas, eu preciso castar esse calloc que retornar um ponteiro generico (retorna um ponteiro para void) para uma estrutura imagem*/

    // perceba que a fun��o calloc recebe dois argumentos, o n�mero de elementos a serem alocados e o tamanho de cada elemento.

    img->nx = nx; /* definindo o parametro nx do img como nx */
    img->ny = ny; /* definindo o parametro ny do img como ny */
    img->nbandas = nbandas; /* definindo o parametro nbandas do img como nbandas */



    img->banda = (int***)calloc(nbandas, sizeof(int**)); /* alocando mem�ria para nbandas numero de elementos, onde cada elemento 
    � do tamanho de um ponteiro para uma matriz bidimensional de inteiros int** 
    depois eu converto esse ponteiro do calloc generico para um ponteiro para uma matriz bidimensional de inteiros int***

    nessa linha, n�s alocamos mem�ria para um array de ponteiros int**, cada um dos quais ir� apontar para uma matriz bidimensional de 
    inteiros. Mas note que esses ponteiros int** ainda n�o apontam para nada �til, s�o apenas ponteiros.
    */
    
    for (int b = 0; b < nbandas; b++) { /* para cada banda */
        img->banda[b] = (int**)calloc(ny, sizeof(int*)); /* perceba que a linha de cima aloca mem�ria para os ponteiros int**,
        e essa linha aloca mem�ria para as matrizes bidimensionais de inteiros que esses ponteiros apontam.
        
        nessa linha, n�s alocamos mem�ria para cada um  desses ponteiros int**. estou alocando mem�ria para ny n�mero de ponteiros int*,
        cada um dos quais ira apontar para uma matriz unidimensional de inteiros (uma linha da imagem), logo, depois dessa linha
        img -> banda[b] � agora um ponteiro para uma matriz bidimensional de inteiros.
        */
        for (int y = 0; y < ny; y++)
            img->banda[b][y] = (int*)calloc(nx, sizeof(int)); /* agora aqui, estamos alocando em cada uma daquelas linhas y, 
        e alocando mem�ria para nx n�mero de inteiros, cada inteiro representar� um pixel na linha da banda da imagem */
    }
    return(img);
}

void* DestroiImagem(Imagem** img) 
    /* recebe um ponteiro para um ponteiro para uma imagem
    nos fazemos isso pois qdo eu passo um ponteiro para uma funcao no C, ele recebe uma c�pia desse ponteiro
    qq modificacao q a funcao fa�a nesse ponteiro (como apontar para um novo local), n vai afetar o ponteiro original fora da funcao
    mas a funcao pode modificar os dados para os quais o ponteiro aponta*/
    /* aqui... queremos n apenas liberar a mem�ria alocada para a imagem, mas tbm garantir que o ponteiro original para a imagem
    seja definido como null, logo, passamos um ponteiro para o ponteiro original, assim, a funcao pode modificar o ponteiro original */
{
    Imagem* aux = *img; /* fazer *img me possibilita obter o valor que img esta apontando, ou seja
    aux agora aponta para a mesma imagem que img estava apontando. logo posso trabalhar com a imagem sem modificar o ponteiro img  */

    if (aux != NULL) {
        for (int b = 0; b < aux->nbandas; b++) {
            for (int y = 0; y < aux->ny; y++)
                free(aux->banda[b][y]); /* aqui estou liberando a mem�ria que foi alocada previamente para a linha y */
            free(aux->banda[b]);
        }
        free(aux);
        (*img) = NULL; /* desativando o ponteiro */
        // perceba que n precisamos faze aux = NULL, pois aux � uma variavel local, ela s� existe dentro do escopo da fun��o
    }
}

Imagem* LeImagem(char* nome_img) /* perceba que fazemos char* nome_img pois um char no C � apenas um caracter, eu qro uma string inteira */
{
    FILE* fp = fopen(nome_img, "r"); /* aqui estamos abrindo esse arquivo */
    Imagem* img = NULL; /* criando um ponteiro para uma estrutura do tipo Imagem vazia */
    char tipo[10]; /* criando uma string tipo com tamanho maximo 10 */
    int  nx, ny, nbandas, max; /* criando os inteiros nx, ny, nbandas e max */

    /* L� o cabe��rio ASCII */
    fscanf_s(fp, "%9s\n", tipo, _countof(tipo)); /* lendo o tipo da imagem... se P5 entao cinza.. se P6 entao colorida */
    fscanf_s(fp, "%d %d\n", &nx, &ny); /* lendo as dimensoes */
    fscanf_s(fp, "%d\n", &max); /* e por fim o valor maximo de pixel */

    /* L� os dados bin�rios dos pixels */
    if (strcmp(tipo, "P5") == 0) {
        nbandas = 1;
    }
    else {
        if (strcmp(tipo, "P6") == 0) {
            nbandas = 3;
        }
        else {
            printf("Formato desconhecido de imagem\n");
            exit(-1);
        }
    }

    int size = nx * ny * nbandas; /* totalidade dos pixels */
    unsigned char* dados = (unsigned char*)calloc(size, sizeof(unsigned char)); /* alocando memoria para armazenar os dados
    dos pixels da imagem, estamos alocando size mem�ria com tamanho unsigned char, e entao mudamos o ponteiro para o unsigned char
    por fim alocamos isso no ponteiro do tipo unsigned char dados */
    fread(dados, sizeof(unsigned char), size, fp); /* aqui estamos lendo os dados dos pixels da imagem do arquivo
    a funcao fread � usada para ler dados de um arquivo. Ela recebe 4 argumentos, um ponteiro para um bloco de memoria onde os dados
    lidos serao armazenados, o tamanho de cada elemento a ser lido, o numero de elementos a serem lidos e um ponteiro para o arquivo
    de onde os dados serao lidos. */

    /* Copia para a imagem */
    img = CriaImagem(nx, ny, nbandas);
    if (nbandas == 3) { /* alocando o valor do pixel em cada banda da estrutura imagem */
        for (int p = 0, q = 0; p < size; p += 3, q++) {
            int x = q % nx, y = q / nx;
            img->banda[0][y][x] = (int)dados[p];
            img->banda[1][y][x] = (int)dados[p + 1];
            img->banda[2][y][x] = (int)dados[p + 2];
        }
    } /* fazendo o mesmo, mas qdo a imagem eh cinza */
    else { /* nbandas == 1 */
        for (int p = 0; p < size; p++) {
            int x = p % nx, y = p / nx;
            img->banda[0][y][x] = (int)dados[p];
        }
    }

    fclose(fp); /* fechando o arquivo fp */
    free(dados); /* liberando a memoria do ponteiro dados */
    return(img); /* retornando img, que eh uma estrutura do tipo Imagem */
}

void  GravaImagem(Imagem* img, char* nome_img)
{
    FILE* fp = fopen(nome_img, "w");

    /* Grava o cabe��rio ASCII */
    if (img->nbandas == 3) {
        fprintf(fp, "P6\n");
    }
    else {
        if (img->nbandas == 1) {
            fprintf(fp, "P5\n");
        }
        else {
            printf("N�mero de bandas %d diferente de 1 e 3 \n", img->nbandas);
            exit(-1);
        }
    }

    fprintf(fp, "%d %d\n", img->nx, img->ny);
    fprintf(fp, "255\n");

    /* Copia a imagem para o vetor de dados */
    int size = img->nx * img->ny * img->nbandas;
    unsigned char* dados = (unsigned char*)calloc(size, sizeof(unsigned char));
    if (img->nbandas == 3) {
        for (int p = 0, q = 0; p < size; p += 3, q++) {
            int x = q % img->nx, y = q / img->nx;
            dados[p] = (unsigned char)img->banda[0][y][x];
            dados[p + 1] = (unsigned char)img->banda[1][y][x];
            dados[p + 2] = (unsigned char)img->banda[2][y][x];
        }
    }
    else { /* nbandas � 1 */
        for (int p = 0; p < size; p++) {
            int x = p % img->nx, y = p / img->nx;
            dados[p] = (unsigned char)img->banda[0][y][x];
        }
    }

    /* Grava os dados bin�rios dos pixels */
    fwrite(dados, sizeof(unsigned char), size, fp);
    fclose(fp);
}