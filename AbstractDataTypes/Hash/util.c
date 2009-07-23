#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#include "util.h"

/**
 * \file util.c Algumas funções utilizadas no programa.
 */

/// Mensagens de erro
//#ifndef _QUIET  
static const char *errmsgs[] = {   
  /* E_NONE        0 */  "sem erro\n",
  /* E_NOMEM      -1 */  "memoria insuficiente\n",
  /* E_FOPEN      -2 */  "não consegue abrir arquivo '%s'\n",
  /* E_FREAD      -3 */  "erro de leitura no arquivo '%s'\n",
  /* E_FWRITE     -4 */  "erro de escrita no arquivo '%s'\n",
  /* E_OPTION     -5 */  "opcao desconhecida -%c\n",
  /* E_OPTARG     -6 */  "opcao de argumento invalida -'%s'\n",
  /* E_FINEXIST   -7 */  "arquivo de entrada inexistente\n",
  /* E_UNKNOWN    -8 */  "erro desconhecido '%s'\n",
  /* E_NOTFOUND   -9 */  "elemento não encontrado\n",
  /* E_PARSEFAIL -10 */  "parsing problems: '%s'\n"
};
//#endif

/// Nome deste programa. Aponta para argv[0] e é usado na função error() de 
/// util.c
//char *prgname;

//-----------------------------------------------------------------------------
/// Imprime um erro.
//-----------------------------------------------------------------------------
void error (int code, ...) {                          
//#ifndef _QUIET 
	va_list    args;   
	const char *msg;  
//#endif  

	if ((code > 0) || (code < E_UNKNOWN)) code = E_UNKNOWN;

//#ifndef _QUIET                
	msg = errmsgs[-code];     
//	fprintf(stderr, "\n%s: ", prgname);
	va_start(args, code);         
	vfprintf(stderr, msg, args); 
	va_end(args);                
//#endif 
	exit(code);
}

/// Tamanho atual do buffer usado para ler uma linha.
int actual_line_size = 80;

/// Define qual implementação da função rad_line() será usada.
///	\sa read_line()
#define READ_LINE1

//-----------------------------------------------------------------------------
/** Le uma linha. Possui 2 implementações:
STR_HASH_CODE1: Implementação original: utiliza getc(), gastando muito processamento.
STR_HASH_CODE2: Utiliza fgets, gastando muito (~10x) menos processamento.
*/
//-----------------------------------------------------------------------------
char *read_line(FILE *file) {
	char *line;
#ifdef READ_LINE1
	register int index;
	register int ch;

	if (file == NULL) return NULL;

	line = (char*)calloc(actual_line_size,sizeof(char));
	if (line == NULL) error(E_NOMEM);
	index = 0;
	while (((ch=getc(file)) != '\n') && !feof(file)) {
		line[index] = ch;
		index++;
		if (index >= actual_line_size) {
			actual_line_size += (actual_line_size * 2) / 3;
			line = (char*)realloc(line, actual_line_size*sizeof(char));
			if (line == NULL) error(E_NOMEM);
		}
	}
	line[index] = '\0';

	if (feof(file) && strlen(line) == 0) {
		free(line);
		line = NULL;
	}
#endif
#ifdef READ_LINE2
	int lineLen;

	if (file == NULL) return NULL;

	line = (char*)malloc(actual_line_size*sizeof(char));
	if (line == NULL) error(E_NOMEM);

	line[0] = 0;
	line[actual_line_size-2] = 0;
	fgets(line, actual_line_size*sizeof(char), file);
	
	// Se o penúltima for:
	// \0: leu a linha inteira (próximo obrigatoriamente é \0)
	// \n: foi a conta
	// outro: tem que ler mais
	while ( (line[actual_line_size-2]!='\0') && (line[actual_line_size-2]!='\n') ) {
		int read = actual_line_size-1;

		actual_line_size += (actual_line_size * 2) / 3;
		line = (char*)realloc(line, actual_line_size*sizeof(char));
		if (line == NULL) error(E_NOMEM);
		//line[actual_line_size-2] = 0;

		fgets(line+read, (actual_line_size-read)*sizeof(char), file);
	}
	lineLen = strlen(line);
	if (lineLen > 0) {
		line[strlen(line)-1] = '\0';
	} else {
		if (feof(file)) {
			free(line);
			line = NULL;
		}
	}
#endif
	
	return line;
}

//-----------------------------------------------------------------------------
/// Retorna um string com a diferença de tempo entre 2 structs timeval.
//-----------------------------------------------------------------------------
char* elapsed_time(struct timeval tbegin, struct timeval tend) {
	long seg, mseg;
	char *time_str = (char*)malloc(80*sizeof(char));
	if (time_str == NULL) error(E_NOMEM);
	
	seg = tend.tv_sec-tbegin.tv_sec;
	mseg = tend.tv_usec-tbegin.tv_usec;
	if (mseg < 0) { 
		seg--; 
		mseg+=1000000;
	}
	if (mseg >= 1000000) {
		seg += mseg/1000000;
		mseg = mseg%1000000;
	}
	
	sprintf(time_str, "%ld.%.6ld s", seg, mseg);
	return time_str;
}
