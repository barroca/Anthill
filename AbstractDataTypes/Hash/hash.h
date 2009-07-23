#include <stdio.h>


// Magic to make this header able to be included several times
#if (defined(KEY_INT) && defined(VAL_VOID) && (!defined(HASH_KEY_INT_VAL_VOID))) || \
(defined(KEY_STRING) && defined(VAL_VOID) && (!defined(HASH_KEY_STRING_VAL_VOID))) || \
(defined(KEY_STRING) && defined(VAL_INT) && (!defined(HASH_KEY_STRING_VAL_INT))) || \
(defined(KEY_INT) && defined(VAL_FLOAT) && (!defined(HASH_KEY_INT_VAL_FLOAT))) || \
(defined(KEY_INT) && defined(VAL_INT)  && (!defined(HASH_KEY_INT_VAL_INT))) || \
(defined(KEY_DSI) && defined(VAL_VOID)  && (!defined(HASH_KEY_DSI_VAL_VOID)))
	
/**
 * \file hash.h Definição das estruturas de dados e prototipos das 
 * funçoes do hash. Para lidar com variações nos tipos de dados sem perder 
 * eficiência, o tipo da chave (pode ser string ou inteiro) e o tipo do valor 
 * (pode ser inteiro ou void *) são definidos através de ifdefs. O nomes dos 
 * tipos de dados e das funções tem no meio o tipo da chave e o tipo do valor
 * para não haver conflitos entre diferentes tipos de hash. Um exemplo é 
 * hashKVCreate onde K é o tipo da chave que pode ser Int(inteiro) ou 
 * Str(String) e V é o tipo do valor que pode ser Int(inteiro) ou Void (void *). 
 * Nessa documentação e de hash.c K é Str e V é Int.
 * 	\sa hash.c
 */


#define MAX_COLISIONS_FACTOR 0.2 ///< max number of collisions (as factor of hash size) that will trigger a rehash
#define REHASH_MULTIPLIER 1.9150399 ///< constant that the hash size will be multiplyed in case of a rehash


/// Definição do tipo da chave que é inteiro se KEY_INT estiver definido,
/// string se KEY_STRING estiver definido ou DataSpaceId se KEY_DSI estiver
/// definido
#ifdef KEY_INT
#define KEY_TYPE int
#elif defined(KEY_STRING)
#define KEY_TYPE char *
#elif defined(KEY_DSI)
#define KEY_TYPE DataSpaceId
#endif

/// Definição do tipo do valor que é inteiro se VAL_INT estiver definido ou 
/// (void *) se VAL_VOID estiver definido.
#ifdef VAL_INT
#define VAL_TYPE int
#endif
#ifdef VAL_VOID
#define VAL_TYPE void *
#endif
#ifdef VAL_FLOAT
#define VAL_TYPE float
#endif




#if defined(KEY_INT) && defined(VAL_VOID) 
#define HASH_KEY_INT_VAL_VOID
	
#define STRUCT_LISTA listaIntVoid 

#define HASH_TYPE HashIntVoid 
#define LISTA_TYPE ListaIntVoid 
#define POS_HANDLER_TYPE PosHandlerIntVoid
#define ITERATOR_TYPE HashIntVoidIterator

#define HASH_CREATE hashIntVoidCreate
#define HASH_ADD hashIntVoidAdd
#define HASH_GET hashIntVoidGet
#define HASH_REMOVE hashIntVoidRemove
#define HASH_CLEAN hashIntVoidClean
#define HASH_DESTROY hashIntVoidDestroy
#define HASH_REHASH hashIntVoidRehash
#define HASH_SERIALIZE hashIntVoidSerialize
#define HASH_DESERIALIZE hashIntVoidDeserialize

#define WRITE_KEY writeKeyIntVoid
#define READ_KEY readKeyIntVoid
#define WRITE_VALUE writeValueIntVoid
#define READ_VALUE readValueIntVoid
#define SET_SERIALIZER_VAL_VOID setIntVoidSerializer

#define LISTA_DESTROY listaIntVoidDestroy

#define CREATE_ITERATOR createHashIntVoidIterator
#define ITERATOR_NEXT hashIntVoidIteratorNext
#define ITERATOR_DESTROY hashIntVoidIteratorDestroy

#endif

#if defined(KEY_STRING) && defined(VAL_INT) 
#define HASH_KEY_STRING_VAL_INT	
	
#define STRUCT_LISTA listaStrInt ///< Tipo da estrutura da lista encadeada do hash (se for usar essa estrutura, use ListaStrInt.) \sa ListaStrInt 

#define HASH_TYPE HashStrInt ///< Tipo do hash
#define LISTA_TYPE ListaStrInt ///< Tipo da lista encadeada do hash
#define POS_HANDLER_TYPE PosHandlerStrInt ///< Tipo do handler de uma posição
#define ITERATOR_TYPE HashStrIntIterator ///< Tipo de um iterador

#define HASH_CREATE hashStrIntCreate ///< Nome da função que cria um hash
#define HASH_ADD hashStrIntAdd ///< Nome da função que adiciona uma chave a um hash
#define HASH_GET hashStrIntGet ///< Nome da função que obtem uma posição do hash
#define HASH_REMOVE hashStrIntRemove ///< Name of the function that removes a key from the hash
#define HASH_CLEAN hashStrIntClean ///< Nome da função que apaga todas as chaves do hash
#define HASH_DESTROY hashStrIntDestroy ///< Nome da função que destrói uma hash
#define HASH_REHASH hashStrIntRehash ///< Name of the function that grows the hash
#define HASH_SERIALIZE hashStrIntSerialize
#define HASH_DESERIALIZE hashStrIntDeserialize

#define WRITE_KEY writeKeyStrInt
#define READ_KEY readKeyStrInt
#define WRITE_VALUE writeValueStrInt
#define READ_VALUE readValueStrInt

#define LISTA_DESTROY listaStrIntDestroy ///< Nome da função que destrói a lista encadeada de uma posição do hash

#define CREATE_ITERATOR createHashStrIntIterator  ///< Nome da função que cria um iterador
#define ITERATOR_NEXT hashStrIntIteratorNext ///< Nome da função que faz o iterador retornar o próximo documento do hash
#define ITERATOR_DESTROY hashStrIntIteratorDestroy ///< Nome da função que destroi um iterador

#endif

#if defined(KEY_STRING) && defined(VAL_VOID) 
#define HASH_KEY_STRING_VAL_VOID
	
#define STRUCT_LISTA listaStrVoid

#define HASH_TYPE HashStrVoid
#define LISTA_TYPE ListaStrVoid
#define POS_HANDLER_TYPE PosHandlerStrVoid
#define ITERATOR_TYPE HashStrVoidIterator

#define HASH_CREATE hashStrVoidCreate
#define HASH_ADD hashStrVoidAdd
#define HASH_GET hashStrVoidGet
#define HASH_REMOVE hashStrVoidRemove
#define HASH_CLEAN hashStrVoidClean
#define HASH_DESTROY hashStrVoidDestroy
#define HASH_REHASH hashStrVoidRehash
#define HASH_SERIALIZE hashStrVoidSerialize
#define HASH_DESERIALIZE hashStrVoidDeserialize

#define WRITE_KEY writeKeyStrVoid
#define READ_KEY readKeyStrVoid
#define WRITE_VALUE writeValueStrVoid
#define READ_VALUE readValueStrVoid
#define SET_SERIALIZER_VAL_VOID setStrVoidSerializer

#define LISTA_DESTROY listaStrVoidDestroy

#define CREATE_ITERATOR createHashStrVoidIterator
#define ITERATOR_NEXT hashStrVoidIteratorNext
#define ITERATOR_DESTROY hashStrVoidIteratorDestroy

#endif

#if defined(KEY_INT) && defined(VAL_FLOAT) 
#define HASH_KEY_INT_VAL_FLOAT
	
#define STRUCT_LISTA listaIntFloat 

#define HASH_TYPE HashIntFloat 
#define LISTA_TYPE ListaIntFloat 
#define POS_HANDLER_TYPE PosHandlerIntFloat
#define ITERATOR_TYPE HashIntFloatIterator

#define HASH_CREATE hashIntFloatCreate
#define HASH_ADD hashIntFloatAdd
#define HASH_GET hashIntFloatGet
#define HASH_REMOVE hashIntFloatRemove
#define HASH_CLEAN hashIntFloatClean
#define HASH_DESTROY hashIntFloatDestroy
#define HASH_REHASH hashIntFloatRehash
#define HASH_SERIALIZE hashIntFloatSerialize
#define HASH_DESERIALIZE hashIntFloatDeserialize

#define WRITE_KEY writeKeyIntFloat
#define READ_KEY readKeyIntFloat
#define WRITE_VALUE writeValueIntFloat
#define READ_VALUE readValueIntFloat

#define LISTA_DESTROY listaIntFloatDestroy

#define CREATE_ITERATOR createHashIntFloatIterator
#define ITERATOR_NEXT hashIntFloatIteratorNext
#define ITERATOR_DESTROY hashIntFloatIteratorDestroy

#endif

#if defined(KEY_INT) && defined(VAL_INT) 
#define HASH_KEY_INT_VAL_INT
	
#define STRUCT_LISTA listaIntInt 

#define HASH_TYPE HashIntInt 
#define LISTA_TYPE ListaIntInt 
#define POS_HANDLER_TYPE PosHandlerIntInt
#define ITERATOR_TYPE HashIntIntIterator

#define HASH_CREATE hashIntIntCreate
#define HASH_ADD hashIntIntAdd
#define HASH_GET hashIntIntGet
#define HASH_REMOVE hashIntIntRemove
#define HASH_CLEAN hashIntIntClean
#define HASH_DESTROY hashIntIntDestroy
#define HASH_REHASH hashIntIntRehash
#define HASH_SERIALIZE hashIntIntSerialize
#define HASH_DESERIALIZE hashIntIntDeserialize

#define WRITE_KEY writeKeyIntInt
#define READ_KEY readKeyIntInt
#define WRITE_VALUE writeValueIntInt
#define READ_VALUE readValueIntInt

#define LISTA_DESTROY listaIntIntDestroy

#define CREATE_ITERATOR createHashIntIntIterator
#define ITERATOR_NEXT hashIntIntIteratorNext
#define ITERATOR_DESTROY hashIntIntIteratorDestroy

#endif


#if defined(KEY_DSI) && defined(VAL_VOID) 
#define HASH_KEY_DSI_VAL_VOID

#include "DataSpaceId.h"
	
#define STRUCT_LISTA listaDSIVoid 

#define HASH_TYPE HashDSIVoid 
#define LISTA_TYPE ListaDSIVoid 
#define POS_HANDLER_TYPE PosHandlerDSIVoid
#define ITERATOR_TYPE HashDSIVoidIterator

#define HASH_CREATE hashDSIVoidCreate
#define HASH_ADD hashDSIVoidAdd
#define HASH_GET hashDSIVoidGet
#define HASH_REMOVE hashDSIVoidRemove
#define HASH_CLEAN hashDSIVoidClean
#define HASH_DESTROY hashDSIVoidDestroy
#define HASH_REHASH hashDSIVoidRehash
#define HASH_SERIALIZE hashDSIVoidSerialize
#define HASH_DESERIALIZE hashDSIVoidDeserialize

#define WRITE_KEY writeKeyDSIVoid
#define READ_KEY readKeyDSIVoid
#define WRITE_VALUE writeValueDSIVoid
#define READ_VALUE readValueDSIVoid
#define SET_SERIALIZER_VAL_VOID setDSIVoidSerializer

#define LISTA_DESTROY listaDSIVoidDestroy

#define CREATE_ITERATOR createHashDSIVoidIterator
#define ITERATOR_NEXT hashDSIVoidIteratorNext
#define ITERATOR_DESTROY hashDSIVoidIteratorDestroy

#endif








/// Lista encadeada de uma posição do hash.
typedef struct STRUCT_LISTA {
	KEY_TYPE key;		///< Chave de um elemento da lista
	VAL_TYPE val;		///< Valor de um elemento da lista
	struct STRUCT_LISTA *next;	///< Próximo elemento da lista
} LISTA_TYPE;

/// Handler para editar uma posição do hash (na verdade um elemento de uma 
/// lista de uma posição do hash)
typedef LISTA_TYPE *POS_HANDLER_TYPE;

#ifdef VAL_VOID

#ifndef VOID_SERIALIZER_FUNCS
#define VOID_SERIALIZER_FUNCS
typedef int hashSerializeValVoid(FILE *outputFile, void *arg);
typedef void *VoidPointer;
typedef VoidPointer hashDeserializeValVoid(FILE *inputFile);
#endif

#endif

/// Estrutura de dados do hash
typedef struct {
	int tam; ///< Tamanho do hash
	int chaves; ///< Número de chaves dentro do hash
	int colisoes; ///< Numero de colisoes que ocorreram no hash
	LISTA_TYPE **entradas; ///< Entradas do hash (cada uma tem um ponteiro para uma lista encadeada)
#ifdef VAL_VOID	
	hashSerializeValVoid *serValVoid;
	hashDeserializeValVoid *deserValVoid;
#endif
} HASH_TYPE;

/// Iterador do hash
typedef struct {
	int is_destructor; ///< Se for =1 sai destruindo hash a medida que passa por ele
	int entr_atual; ///< Entrada atual
	POS_HANDLER_TYPE elem_atual; ///< Elemento atual da lista da entrada atual
} ITERATOR_TYPE;

HASH_TYPE *HASH_CREATE(int sz);
POS_HANDLER_TYPE HASH_ADD(HASH_TYPE *h, KEY_TYPE key);
POS_HANDLER_TYPE HASH_GET(HASH_TYPE *h, KEY_TYPE key);
int HASH_REMOVE(HASH_TYPE *h, KEY_TYPE removedKey);
void HASH_CLEAN(HASH_TYPE *h);
void HASH_DESTROY(HASH_TYPE *h);
void HASH_REHASH(HASH_TYPE *h, int newSize);
int HASH_SERIALIZE(FILE *outputFile, HASH_TYPE *hash);
int HASH_DESERIALIZE(FILE *inputFile, HASH_TYPE *hash);

int WRITE_KEY(FILE *outputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos);
int READ_KEY(FILE *inputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos);
int WRITE_VALUE(FILE *outputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos);
int READ_VALUE(FILE *inputFile, HASH_TYPE *hash, POS_HANDLER_TYPE pos);
#ifdef VAL_VOID
void SET_SERIALIZER_VAL_VOID(HASH_TYPE *hash, hashSerializeValVoid *serializer, hashDeserializeValVoid *deserializer);
#endif

/// Retorna o numero de colisões que houve no hash.
#define hashGetColisoes(hash) (hash->colisoes)

/// Retorna o numero de chaves no hash.
#define hashGetChaves(hash) (hash->chaves)

void LISTA_DESTROY(LISTA_TYPE *l);

/// Retorna a chave da posição pos.
#define posGetKey(pos) (pos->key)

/// Retorna o valor da posição pos.
#define posGetValue(pos) (pos->val)

/// Altera o valor da posição pos.
#define posSetValue(pos, _val) (pos->val)=(_val)

ITERATOR_TYPE *CREATE_ITERATOR(HASH_TYPE *hash, int is_destructor);
POS_HANDLER_TYPE ITERATOR_NEXT(ITERATOR_TYPE *hdi, HASH_TYPE *hash);
void ITERATOR_DESTROY(ITERATOR_TYPE *hdi, HASH_TYPE *hash);


#ifndef __HASH__IMPLEMENTATION__

#ifdef VAL_VOID
#undef SERIALIZER_FUNC
#undef DESERIALIZER_FUNC
#undef SET_SERIALIZER_VAL_VOID
#endif

#undef VAL_TYPE 
#undef KEY_TYPE

#undef STRUCT_LISTA 

#undef HASH_TYPE 
#undef LISTA_TYPE 
#undef POS_HANDLER_TYPE
#undef ITERATOR_TYPE 

#undef HASH_CREATE 
#undef HASH_ADD 
#undef HASH_GET 
#undef HASH_REMOVE 
#undef HASH_CLEAN   
#undef HASH_DESTROY 
#undef HASH_REHASH
#undef HASH_SERIALIZE
#undef HASH_DESERIALIZE

#undef WRITE_KEY
#undef READ_KEY
#undef WRITE_VALUE
#undef READ_VALUE

#undef LISTA_DESTROY 

#undef CREATE_ITERATOR 
#undef ITERATOR_NEXT 
#undef ITERATOR_DESTROY
#endif




#endif // Matches with the first giant #ifdef
