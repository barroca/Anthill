#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "str.h"
#include "cser.h"

#define __HASH__IMPLEMENTATION__
#include "hash.h"

/**
 * \file hash.c Implementa??o de um hash gen?rico. Este hash usa lista 
 * encadeada e permite edi??o do valor de uma chave. Para lidar com varia??es 
 * nos tipos de dados sem perder efici?ncia, o tipo da chave (pode ser string 
 * ou inteiro) e o tipo do valor (pode ser inteiro ou void *) s?o definidos 
 * atrav?s de ifdefs. A cada tipo de hash mudam os nomes dos tipos abstratos de
 * dados e das fun??es para que diferentes tipos de hash possam ser usados 
 * dentro de um mesmo arquivo fonte, mas os algoritmos s?o os mesmos para todos
 * os tipos(exceto que se chave ? um string deve-se chamar str_hash_code() para
 * calcular o hash code, se ela ? inteiro ela ? seu pr?prio hash code). As 
 * fun??es desse arquivo t?m nomes como HASH_CREATE que o preprocessador do C 
 * substitui por hashKVCreate onde K ? o tipo da chave que pode ser Int(inteiro)
 * ou Str(String) e V ? o tipo do valor que pode ser Int(inteiro) ou Void 
 * (void *). Nessa documenta??o e de hash.h K ? Str e V ? Int.
 *
 * Complexidade das opera??es:
 * 
 * C?lculo do hashcode: A complexidade do c?lculo do hashcode ? O(n) onde n ? o tamanho do da chave 
 * (se ela for string). Mas como as chaves t?m tamanho m?dio de uns 10 
 * caracteres e poucas devem ter 20 caracteres, a complexidade do c?lculo do 
 * hashcode pode ser considerada O(1).
 * 
 * Inser??o na lista encadeada de uma posi??o do hash: O(n) onde n ? o tamanho da lista.
 *
 * Inser??o no hash: Para inserir uma chave no hash ? necess?rio calcular o
 * hashcode dessa chave e inser?-la na lista encadeada de uma posi??o do hash.
 * O c?lculo do hashcode ? O(1), enquanto a inser??o na lista ? O(n) onde n ?
 * o tamanho da lista. Como o tamanho da lista ? 1 se o n?mero de chaves n?o for 
 * muito maior que o n?mero de posi??es da tabela hash a complexidade esperada 
 * de inser??o no hash ? O(1).
 * 
 * 	\sa hash.h
 */ 


/*----------------------------- hash operations -----------------------------*/

#ifdef KEY_INT
#define hashCode(x) (x)
#define keyEqual(a,b) (a) == (b)
#define keyCopy(d,s) (d) = (s)
#define destroyKey(k)
#elif defined(KEY_STRING)
#define hashCode(x) str_hash_code(x) ///< Calcula o hash code da chave x
#define keyEqual(a,b) (!strcmp(a,b)) ///< Verify if two keys are equal
#define keyCopy(d,s) (d) = strdup(s) ///< Faz d ser uma copia da chave s
#define destroyKey(k) free(k) ///< Destroy a key
#elif defined(KEY_DSI)
#define hashCode(x) (abs(x.work*1000000+x.task))
#define keyEqual(a,b) ( ((a).work==(b).work) && ((a).task==(b).task) )
#define keyCopy(d,s) (d) = (s)
#define destroyKey(k)
#else
#error "Not implemented"
#endif

/// Define o que colocar no campo valor caso n?o haja valor definido.
#ifdef VAL_INT
#define NO_VALUE -1
#endif
#ifdef VAL_VOID
#define NO_VALUE NULL
#endif
#ifdef VAL_FLOAT
#define NO_VALUE 0
#endif

#ifdef KEY_INT
#define KEY_NULL -1
#elif defined(KEY_STRING)
#define KEY_NULL NULL
#elif defined(KEY_DSI)
#define KEY_NULL {-1, -1}
#endif



//-----------------------------------------------------------------------------
/** Cria um hash.
	\param sz Tamanho do hash.
	\return Ponteiro para o hash.
*/
//-----------------------------------------------------------------------------
HASH_TYPE *HASH_CREATE(int sz) {
	HASH_TYPE *hash  = (HASH_TYPE *)malloc(sizeof(HASH_TYPE));
	hash->tam      = sz;
	hash->colisoes = 0;
	hash->chaves   = 0;
	if (sz > 0) {
		hash->entradas = (LISTA_TYPE **)calloc(sz, sizeof(LISTA_TYPE *)); //Create and zeroes all entries
	} else {
		hash->entradas = NULL;
	}
#ifdef VAL_VOID
	hash->serValVoid = NULL;
	hash->deserValVoid = NULL;
#endif
	
	return hash;
}

//-----------------------------------------------------------------------------
/** Insere uma chave no hash (caso ela n?o exista).
	\param h Hash onde a chave ser? inserida.
	\param key Chave a ser inserida (se a chave for um string, ele ser? copiado).
	\return Handler para edi??o da posi??o.
*/
//-----------------------------------------------------------------------------
POS_HANDLER_TYPE HASH_ADD(HASH_TYPE *h, KEY_TYPE key) {
	if (h->tam < 3) {
		HASH_REHASH(h, 3); // 101 is a prime number
	} else {
		float collisions = (float)h->colisoes;
		float size = (float)h->tam;
		
		if ( (collisions/size) > MAX_COLISIONS_FACTOR) {
			float newSize = size * REHASH_MULTIPLIER;			
			HASH_REHASH(h, (int)newSize);
		}
	}
		
	int pos = hashCode(key) % (h->tam);
	LISTA_TYPE **ptr_lista = &(h->entradas[pos]);
	LISTA_TYPE *ptr_item = NULL;

	for (; (*ptr_lista)!=NULL; ptr_lista = &((*ptr_lista)->next) ) {
		if (keyEqual( (*ptr_lista)->key, key )) {
			// ja tinha essa chave no hash
			return *ptr_lista;
		}	
	}

	// Nova entrada no hash
	h->chaves++;

	// Se a chave n?o existia no hash e tem outra chave nessa posi??o,
	// houve uma colis?o.
	if (h->entradas[pos] != NULL) {
		h->colisoes++;
	}

	ptr_item = (LISTA_TYPE *) malloc(sizeof(LISTA_TYPE));
	keyCopy(ptr_item->key, key);
	ptr_item->val  = NO_VALUE;
	ptr_item->next = NULL;

	*ptr_lista = ptr_item;
	return ptr_item;
}

//-----------------------------------------------------------------------------
/// Retorna um Handler da posi??o da chave key no hash h. 
//-----------------------------------------------------------------------------
POS_HANDLER_TYPE HASH_GET(HASH_TYPE *h, KEY_TYPE key) {
	if (h->tam > 0) {
		int pos = hashCode(key) % (h->tam);
		LISTA_TYPE *ptr_lista = h->entradas[pos];

		for (; ptr_lista!=NULL; ptr_lista = ptr_lista->next) {
			if (keyEqual( ptr_lista->key, key )) {
				// essa existe chave no hash
				return ptr_lista;
			}
		}
	}

	return NULL;	
}

//-----------------------------------------------------------------------------
/// Removes a key from the hash. 
///		\param h Hash where the key will be removed.
///		\param removedKey Key to be removed.
///		\return 0 on success, -1 on error.
//-----------------------------------------------------------------------------
int HASH_REMOVE(HASH_TYPE *h, KEY_TYPE removedKey) {
#ifndef VAL_INT
	if (h == NULL) return -1;
#endif
	if (h->tam > 0) {
		int pos = hashCode(removedKey) % (h->tam);
		LISTA_TYPE *ptr_lista = h->entradas[pos];
		LISTA_TYPE **ptr_ant = &(h->entradas[pos]);

		/// \todo Test removal on the begining, middle and end of a >3 node linked list
		while (ptr_lista != NULL) {
			if (keyEqual( ptr_lista->key, removedKey )) {
				*ptr_ant = ptr_lista->next;
				free(ptr_lista);
				h->chaves--;
				return 0;
			}
			
			ptr_ant   = &(ptr_lista->next);
			ptr_lista = ptr_lista->next;
		}	
		return -1;
	}
	return -1;
}

//-----------------------------------------------------------------------------
/// Destroy a hash.
//-----------------------------------------------------------------------------
void HASH_DESTROY(HASH_TYPE *h) {
	int i;

	for (i=0; i<h->tam; i++) {
		if (h->entradas[i] != NULL) {
			LISTA_DESTROY(h->entradas[i]);
		}
	}

	free(h->entradas);
	free(h);
}

//-----------------------------------------------------------------------------
/// Limpa um hash, apagando todos os pares chave->valor.
//-----------------------------------------------------------------------------
void HASH_CLEAN(HASH_TYPE *h) {
	int i;

	for (i=0; i<h->tam; i++) {
		if (h->entradas[i] != NULL) {
			LISTA_DESTROY(h->entradas[i]);
			h->entradas[i] = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
/// Grow the hash to newSize
//-----------------------------------------------------------------------------
void HASH_REHASH(HASH_TYPE *h, int newSize) {
	LISTA_TYPE **newHash = (LISTA_TYPE **)calloc(newSize, sizeof(LISTA_TYPE *));
	ITERATOR_TYPE *oldHashIt = CREATE_ITERATOR(h, 1);
	POS_HANDLER_TYPE oldPos = ITERATOR_NEXT(oldHashIt, h);
	int collisions = 0;
	int newPos = -1;

	while (oldPos != NULL) {
		newPos = hashCode(posGetKey(oldPos)) % (newSize);
		LISTA_TYPE **ptr_lista = &(newHash[newPos]);
		LISTA_TYPE *ptr_item = NULL;

		while ((*ptr_lista)!=NULL) {
			ptr_lista = &((*ptr_lista)->next);
		}

		// Se a chave n?o existia no hash e tem outra chave nessa posi??o,
		// houve uma colis?o.
		if (newHash[newPos] != NULL) {
			collisions++;
		}

		ptr_item = (LISTA_TYPE *) malloc(sizeof(LISTA_TYPE));
		keyCopy(ptr_item->key, posGetKey(oldPos));
		ptr_item->val  = posGetValue(oldPos);
		ptr_item->next = NULL;

		*ptr_lista = ptr_item;
		
		
		oldPos = ITERATOR_NEXT(oldHashIt, h);
	}

	if (h->entradas != NULL) {
		free(h->entradas);
	}
	h->tam      = newSize;
	h->colisoes = collisions;
	h->entradas = newHash;
}

int HASH_SERIALIZE(FILE *outputFile, HASH_TYPE *hash){
	WRITE_NUM(outputFile, "numKeys", hash->chaves);
	
#ifdef DEBUG	
	char header[] = "-#Hash-";
	WRITE_BYTES(outputFile, header, strlen(header));
#endif

	ITERATOR_TYPE *it = CREATE_ITERATOR(hash, 0);
	POS_HANDLER_TYPE pos = NULL;
	for (pos = ITERATOR_NEXT(it, hash); pos != NULL; pos = ITERATOR_NEXT(it, hash)) {
		WRITE_KEY(outputFile, hash, pos);
		WRITE_VALUE(outputFile, hash, pos);
	}
	ITERATOR_DESTROY(it, hash);

#ifdef DEBUG	
	char footer[] = "-#HashEnd-";
	WRITE_BYTES(outputFile, footer, strlen(footer));
#endif

	return 0;
}

int HASH_DESERIALIZE(FILE *inputFile, HASH_TYPE *hash){
	int i = 0, chaves = -1;
	READ_BEGIN(inputFile);
	
	READ_NUM("numKeys", chaves);
	assert(chaves != -1);

	
#ifdef DEBUG
	char header[] = "-#Hash-";
	char *buf = malloc(strlen(header)+1); // +1 to the \0
	READ_BYTES(inputFile, buf, strlen(header));
	buf[strlen(header)] = '\0';
	
	if (strcmp(header, buf)) {
		// If header != buf, use assert to indicate the error
		assert(0 == strcmp(header, buf));
	}
	free(buf);
#endif

	POS_HANDLER_TYPE tmpPos = malloc(sizeof(LISTA_TYPE));
	POS_HANDLER_TYPE hashPos = NULL;
	for (i=0; i<chaves; i++) {
		READ_KEY(inputFile, hash, tmpPos);
		READ_VALUE(inputFile, hash, tmpPos);

		hashPos = HASH_ADD(hash, posGetKey(tmpPos));
		posSetValue(hashPos, posGetValue(tmpPos));
	}
	free(tmpPos);
	READ_END

#ifdef DEBUG	
	char footer[] = "-#HashEnd-";
	// Needed to redeclare buf here because READ_BEGIN()...READ_END have a
	// block ( {} ).
	char *buf = (char *)malloc(strlen(footer)+1); // +1 to the \0
	READ_BYTES(inputFile, buf, strlen(footer));
	buf[strlen(footer)] = '\0';

	if (strcmp(footer, buf)) {
		// If footer != buf, use assert to indicate the error
		assert(0 == strcmp(footer, buf));
	}
	free(buf);
#endif
	
	return 0;
}


int WRITE_KEY(FILE *outputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos) {

#ifdef DEBUG	
	char header[] = "-#Tuple-";
	WRITE_BYTES(outputFile, header, strlen(header));
#endif

#ifdef KEY_INT
	WRITE_NUM(outputFile, "key", posGetKey(pos));
#elif defined(KEY_STRING)
	WRITE_STR(outputFile, "key", posGetKey(pos));
#elif defined(KEY_DSI)
	WRITE_NUM(outputFile, "work", posGetKey(pos).work);
	WRITE_NUM(outputFile, "task", posGetKey(pos).task);	
#else 
#error "Not implemented"
#endif
	
	return 0;
}


int READ_KEY(FILE *inputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos){
	KEY_TYPE key = KEY_NULL;
		
#ifdef DEBUG
	char header[] = "-#Tuple-";
	char *buf = malloc(strlen(header)+1); // +1 to the \0
	READ_BYTES(inputFile, buf, strlen(header));
	buf[strlen(header)] = '\0';
	
	if (strcmp(header, buf)) {
		// If header != buf, use assert to indicate the error
		assert(0 == strcmp(header, buf));
	}
	free(buf);
#endif

	READ_BEGIN(inputFile);
#ifdef KEY_INT
	READ_NUM("key", key);
#elif defined(KEY_STRING)
	READ_STR("key", key);
#elif defined(KEY_DSI)
	READ_NUM("work", key.work);
	READ_NUM("task", key.task);
#else
#error "Not implemented"
#endif
	READ_END
	pos->key=key;

	return 0;
}

int WRITE_VALUE(FILE *outputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos) {
#ifdef DEBUG	
	char header[] = "-#Value-";
	WRITE_BYTES(outputFile, header, strlen(header));
#endif

	
#ifdef VAL_INT
	WRITE_NUM(outputFile, "value", posGetValue(pos));
#endif
#ifdef VAL_VOID
	/// \todo Test values with \0 inside
	hash->serValVoid(outputFile, posGetValue(pos));
#endif
#ifdef VAL_FLOAT
	WRITE_FLOAT(outputFile, "value", posGetValue(pos));
#endif

#ifdef DEBUG	
	char footer[] = "-#TupleEnd-";
	WRITE_BYTES(outputFile, footer, strlen(footer));
#endif

	return 0;
}

int READ_VALUE(FILE *inputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos) {
#ifdef DEBUG	
	char header[] = "-#Value-";
	char *buf = malloc(strlen(header)+1); // +1 to the \0
	READ_BYTES(inputFile, buf, strlen(header));
	buf[strlen(header)] = '\0';
	
	if (strcmp(header, buf)) {
		// If header != buf, use assert to indicate the error
		assert(0 == strcmp(header, buf));
	}
	free(buf);
#endif

	// Set value to NULL, to catch cases where cser read nothing
	posSetValue(pos, NO_VALUE);

#ifdef VAL_VOID
	/// \todo Test values with \0 inside
	posSetValue(pos, hash->deserValVoid(inputFile));
#else

	READ_BEGIN(inputFile);	
#ifdef VAL_INT
	READ_NUM("value", posGetValue(pos));
#endif
#ifdef VAL_FLOAT
	READ_FLOAT("value", posGetValue(pos));
#endif
	READ_END

#endif // ifdef VAL_VOID		
	

#ifdef DEBUG	
	char footer[] = "-#TupleEnd-";
	buf = malloc(strlen(footer)+1); // +1 to the \0
	READ_BYTES(inputFile, buf, strlen(footer));
	buf[strlen(footer)] = '\0';
	
	if (strcmp(footer, buf)) {
		// If footer != buf, use assert to indicate the error
		assert(0 == strcmp(footer, buf));
	}
	free(buf);
#endif

	return 0;
}

#ifdef VAL_VOID
void SET_SERIALIZER_VAL_VOID(HASH_TYPE *hash, hashSerializeValVoid *serializer, hashDeserializeValVoid *deserializer) {
	hash->serValVoid = serializer;
	hash->deserValVoid = deserializer;
}
#endif


/*---------------- Opera??es sobre a lista encadeada do hash ----------------*/

//-----------------------------------------------------------------------------
/// Destr?i a lista encadeada de um hash.
//-----------------------------------------------------------------------------
void LISTA_DESTROY(LISTA_TYPE *l) {
	if (l->next != NULL) {
		LISTA_DESTROY(l->next);
	}
	destroyKey(l->key);

	free(l);
}


/*------------------- Opera??es sobre um iterador do hash -------------------*/

//-----------------------------------------------------------------------------
/** Cria um iterador de hash.
	\param hash Hash sobre o qual o iterador vai percorrer.
	\param is_destructor se for =1 o iterador vai limpando o hash a medida que passa por ele.
	\return Ponteiro para o iterador.
	\sa hashStrIntIteratorNext()
*/
//-----------------------------------------------------------------------------
ITERATOR_TYPE *CREATE_ITERATOR(HASH_TYPE *hash, int is_destructor) {
	ITERATOR_TYPE *hdi = (ITERATOR_TYPE *) malloc(sizeof(ITERATOR_TYPE));

	hdi->is_destructor = is_destructor;
	hdi->entr_atual    = -1;
	hdi->elem_atual    = NULL;
	
	return hdi;
}


//-----------------------------------------------------------------------------
/** Retorna a posi??o do pr?ximo elemento no hash. Se o iterador foi criado com 
 * o argumento is_destructor =1 destr?i toda lista encadeada do hash que 
    terminar de percorrer.
	\param hdi Iterador.
	\param hash Hash sobre o qual o iterador vai percorrer.
	\return Ponteiro para a posi??o do pr?ximo elemento do hash.
*/
//-----------------------------------------------------------------------------
POS_HANDLER_TYPE ITERATOR_NEXT(ITERATOR_TYPE *hdi, HASH_TYPE *hash) {
	if (hdi->elem_atual != NULL) {
		hdi->elem_atual = hdi->elem_atual->next;
	}
	if (hdi->is_destructor && hdi->elem_atual==NULL && hdi->entr_atual >=0) {
		LISTA_DESTROY(hash->entradas[hdi->entr_atual]);
		hash->entradas[hdi->entr_atual] = NULL;
	}
	while (hdi->elem_atual == NULL) {
		hdi->entr_atual++;
		// Se o hash acabou retorna NULL
		if (hdi->entr_atual >= hash->tam) return NULL;
		hdi->elem_atual = hash->entradas[hdi->entr_atual];			
	}

#ifdef DEBUG
	if (hdi->elem_atual == NULL) {
		if (hdi->entr_atual < hash->tam) {
			printf("%s(%d): Deu pau!!! Vai retornar NULL sem o hash ter acabado\n", __FILE__, __LINE__);
			exit(1);
		}
	}
#endif
	return hdi->elem_atual;
}

//-----------------------------------------------------------------------------
/** Destroi um iterador de hash. Se o iterador foi criado com o argumento 
 * is_destructor =1 e tiver acabado de percorrer uma lista encadeada do hash, 
 * acaba com ela.
 */ 
//-----------------------------------------------------------------------------
void ITERATOR_DESTROY(ITERATOR_TYPE *hdi, HASH_TYPE *hash) {
	if (hdi->is_destructor && hdi->entr_atual<hash->tam && hdi->entr_atual >=0 && hdi->elem_atual==NULL) {
		LISTA_DESTROY(hash->entradas[hdi->entr_atual]);
		hash->entradas[hdi->entr_atual] = NULL;
	}
	free(hdi);
}

