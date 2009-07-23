#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "util.h"
#include "str.h"
#include "valor.h"

/**
 * \file str.c Operações com strings. 
 */

//-----------------------------------------------------------------------------
/// Compara 2 strings.
//-----------------------------------------------------------------------------
int str_compare(const void *v1, const void *v2) {
	char *c1 = (char*)v1, *c2 = (char*)v2;
	return strcmp(c1, c2);
}

//-----------------------------------------------------------------------------
/// Copia um string.
//-----------------------------------------------------------------------------
char* str_clone(const char* str) {
	return strdup(str);
}

/// Define qual implementação da função str_hash_code() será usada.
///	\sa str_hash_code()
#define STR_HASH_CODE1

//-----------------------------------------------------------------------------
/** Retorna um hashcode para a chave key. Possui 3 implementações:
STR_HASH_CODE1: utiliza um vetor valor[256][32] de números primos. Para cada caracter key[i] da chave soma valor[ (int)key[i] ][i] ao hashcode.
STR_HASH_CODE2: implementação abandonada (igual a STR_HASH_CODE1, mas limitava a fazer hashcode dos 32 primeiros caracteres do string).
STR_HASH_CODE3: implementação original (numero de colisões (43386 para os 110124 termos de wsj88) ligeiramente superior aos 2 de cima (43319)).
	\sa valor.h
*/
//-----------------------------------------------------------------------------
int str_hash_code(char *key) {
	int tam = strlen(key);
	int i;
	int soma = 0;

#ifdef STR_HASH_CODE1
	if (tam > 32) {
		int j=0;
		for (i=0; i<tam; i++) {
			soma += valor[ (int)key[i] ][j];
			j++;
			if (j>=32) j=0;
		}
	} else {
		for (i=0; i<tam; i++) {
			soma += valor[ (int)key[i] ][i];
		}
	}
#endif
#ifdef STR_HASH_CODE2
	if (tam > 32) tam = 32;
	for (i=0; i<tam; i++) {
		soma += valor[ (int)key[i] ][i];
	}
#endif
#ifdef STR_HASH_CODE3
	int off = 0;
	
	if (tam > 0) {
		do {
			soma = 31*soma + (int)key[off++];
		} while (--tam > 0);
	}
#endif
	if(soma < 0) soma = (-1) * soma;
	return soma;
}

//-----------------------------------------------------------------------------
/// Destrói um string.
//-----------------------------------------------------------------------------
void str_destroy(void *value) {
	char *str = (char*)value;
	free(str);
}

//-----------------------------------------------------------------------------
/// Elimina espaços, \r, quebra de linha e \t do início e do final de um string.
//-----------------------------------------------------------------------------
void str_trim(char *str) {
	int i, j, len;

	len = strlen(str);
	i = 0;
	while((i < len) && ((str[i] == ' ') || (str[i] == '\t') || 
			(str[i] == '\n') || (str[i] == '\r'))) {
		i++;
	}
	j = len - 1;
	while((j >= 0) && ((str[j] == ' ') || (str[j] == '\t') ||
			(str[i] == '\n') || (str[i] == '\r'))) {
		j--;
	}

	if (j >= 0) str[j + 1] = '\0';
	
	if (i <= j) {
		strcpy(str, str + i);
	} else {
		strcpy(str, "");
	}
}

//-----------------------------------------------------------------------------
/// Passa um string para minúsculas.
//-----------------------------------------------------------------------------
void str_to_lower(char *str) {
	int i, len = strlen(str);
	for(i=0; i<len; i++) {
		str[i] = (char)tolower((int)str[i]);
	}
}

//-----------------------------------------------------------------------------
/// Passa um string para maiúsculas.
//-----------------------------------------------------------------------------
void str_to_upper(char *str) {
	int i, len = strlen(str);
	for(i=0; i<len; i++) {
		str[i] = (char)toupper((int)str[i]);
	}
}

//-----------------------------------------------------------------------------
/// Elimina tags de um string.
//-----------------------------------------------------------------------------
void str_remove_tags(char *str) {
	int begin = 0, end = 0, i, j;
	int opened = FALSE;
	int len = strlen(str);
	
	for (i=0; i<len; i++) {
		if (str[i] == '<') {
			begin = i;
			opened = TRUE;
		} else if (opened && str[i] == '>') {
			end = i;
			opened = FALSE;
			for (j=begin; j<=end; j++) {
				str[j] = ' ';
			}
		}
	}
	str_trim(str);
}
