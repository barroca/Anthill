#ifndef _UTIL_H_

#define _UTIL_H_

#include <stdio.h> 
#include <sys/time.h>

/**
 * \file util.h Algumas funções, macros e constantes utilizadas no programa.
 */

/// Verdadeiro
#define TRUE 1
/// Falso
#define FALSE 0

/// Numero de Euler
#define E 2.718281828459045
/// Pi
#define PI 3.141592653589793

/// Não é utilizado
#define REALLOC_SIZE 10

/* Codigos de erro */
#define OK            0         /* Sem erro                       */
#define E_NONE        0         /* Sem erro                       */
#define E_NOMEM     (-1)        /* Memória insuficiente           */
#define E_FOPEN     (-2)        /* Não consegue abrir arquivo     */
#define E_FREAD     (-3)        /* Erro na leitura do arquivo     */
#define E_FWRITE    (-4)        /* Erro na escrita do arquivo     */
#define E_OPTION    (-5)        /* Opcao desconhecida             */
#define E_OPTARG    (-6)        /* Opcao de argumento invalida    */
#define E_FRINEXIST (-7)        /* Arquivo de entrada inexistente */
#define E_UNKNOWN   (-8)        /* Erro desconhecido              */
#define E_NOTFOUND  (-9)        /* Elemento nao encontrado        */
#define E_PARSEFAIL (-10)       /* Problems parsing input file    */

/// Retorna a diferença de tempo entre 2 structs timeval.
#define ELAPSE(e,n,s) { e.tv_sec+=n.tv_sec-s.tv_sec; e.tv_usec+=n.tv_usec-s.tv_usec; if(e.tv_usec<0){ e.tv_sec--; e.tv_usec+=1000000; } if(e.tv_usec>1000000){ e.tv_sec+=e.tv_usec/1000000; e.tv_usec=e.tv_usec%1000000; }}

/// Retorna o maior entre 2 valores
#define max(a, b) ((a) > (b) ? (a) : (b))
/// Retorna o menor entre 2 valores
#define min(a, b) ((a) < (b) ? (a) : (b))

/// Converte uma struct timeval (retornada por getrusage() e gettimeofday() ) 
/// em um double.
#define time2double(x) ( (double)(x).tv_sec+0.000001*(double)(x).tv_usec )

/*typedef void _DESTROY(void *value);
typedef int _HASH_CODE(void *value);
typedef int _COMPARE(const void *value1, const void *value2);
typedef void* _CLONE(const void *value);*/

void error (int, ...);

char *read_line(FILE*);

char *elapsed_time(struct timeval, struct timeval);

#endif
