#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define KEY_INT
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_INT

#define KEY_STRING
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_STRING

#define KEY_STRING
#define VAL_INT
#include "hash.h"
#undef VAL_INT
#undef KEY_STRING

#define KEY_INT
#define VAL_FLOAT
#include "hash.h"
#undef VAL_FLOAT
#undef KEY_INT

#define KEY_INT
#define VAL_INT
#include "hash.h"
#undef VAL_INT
#undef KEY_INT

#define KEY_DSI
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_DSI



// Use TaskIdList as a (void *) value 
#include "TaskIdList.h"

#define INITIAL_HASH_SIZE 10
#define NUM_KEYS 35000 // 35000000 uses around 1GB of RAM
#define NUM_STR_VOID_KEYS 10000

// with 55000000 the hash grows to 83886080 positions


int main(int argc, char *argv[]) {
	int i=0;
	FILE *serializeFile = NULL;

	srandom(107);
	srand48(107);

/*------------------------------ rehash test -----------------------------*/
	
	// Tests rehash, adding NUM_KEYS positions in a 10 positions hash,
	// this will force rehash
	HashIntInt *hash = hashIntIntCreate(0);
	int initialSz = hash->tam;
	for (i=0; i<NUM_KEYS; i++) {
		PosHandlerIntInt pos = hashIntIntAdd(hash, i);
		posSetValue(pos, i);
	}

	// Verify if rehash has been applied
	if (hash->tam == INITIAL_HASH_SIZE) {
		printf("%s (%d): ERROR, rehash hasn't been applied\n", __FILE__, __LINE__);
	}
	
	// Check the NUM_KEYS positions
	for (i=0; i<NUM_KEYS; i++) {
		PosHandlerIntInt pos = hashIntIntGet(hash, i);
		int val = posGetValue(pos);
		if (i != val) {
			printf("%s (%d): ERROR: key=%d val=%d shoud be:%d\n", __FILE__, __LINE__, i, val, i);
		}
	}
	printf("Rehash ok, initial size:%d, grown to:%d, collisions:%d\n", initialSz, hash->tam, hash->colisoes);


/*--------------------------- non-existent key test ---------------------------*/
	
/*	// Try to get a non-existent key 
	PosHandlerIntInt pos = hashIntIntGet(hash, NUM_KEYS+10);
	if (pos != NULL) {
		printf("%s (%d): ERROR: non-existent key returned non NULL position\n", __FILE__, __LINE__);
	}
*/

/*------------------- writeKeyIntInt()/readKeyIntInt() test -----------------*/

/*	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	
	// write the number of keys
	fwrite(&(hash->chaves), sizeof(hash->chaves), 1, serializeFile);
	
	HashIntIntIterator *it = createHashIntIntIterator(hash, 0);
	for (pos = hashIntIntIteratorNext(it,hash); pos!=NULL; pos = hashIntIntIteratorNext(it,hash)) {
		writeKeyIntInt(serializeFile, hash, pos);
		writeValueIntInt(serializeFile, hash, pos);
	}
	hashIntIntIteratorDestroy(it, hash);
	fclose(serializeFile);

	
	printf("Writing complete, now reading back\n");
	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	PosHandlerIntInt readPos = (PosHandlerIntInt)calloc(1, sizeof(ListaIntInt));
	int numKeys = 0;

	fread(&(numKeys), sizeof(hash->chaves), 1, serializeFile);
	assert(numKeys == hash->chaves);

	int accessedKey1000 = 0;
	for (i=0; i<numKeys; i++) {
		readKeyIntInt(serializeFile, hash, readPos);
		readValueIntInt(serializeFile, hash, readPos);
		
		if(posGetKey(readPos) == 1000) {
			accessedKey1000=1;
		}
		
		pos = hashIntIntGet(hash, posGetKey(readPos));		
		assert(posGetKey(readPos) == posGetKey(pos));
		assert(posGetValue(readPos) == posGetValue(pos));
	}
	assert(accessedKey1000);
	fclose(serializeFile);
	hashIntIntDestroy(hash);
*/
/*------------------- writeKeyStrVoid()/readKeyStrVoid() test -----------------*/
/*	
	HashStrVoid *hashsv = hashStrVoidCreate(0);
	setStrVoidSerializer(hashsv, &writeTaskIdList, &readTaskIdList);
	
	PosHandlerStrVoid possv = NULL;
	for (i=0; i<NUM_STR_VOID_KEYS; i++) {
		// Create a random key
		int j=0;
		int keySz = random()%1000;
		char *key = malloc(keySz+1);
		for (j=0; j<keySz; j++) {
			key[j]=(char)random();
		}
		key[keySz]='\0';
		
		// Create a random value
		int valSz = random()%1000;
		TaskIdList *val = taskIdListCreate(0);
		for (j=0; j<valSz; j++) {
			taskIdListAdd(val, random());
		}
		
		possv = hashStrVoidAdd(hashsv, key);
		posSetValue(possv, val);
	}

	// Verify if rehash has been applied
	if (hashsv->tam == 0) {
		printf("%s (%d): ERROR, rehash hasn't been applied\n", __FILE__, __LINE__);
	}
	
	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}

	// write the number of keys
	fwrite(&(hashsv->chaves), sizeof(hashsv->chaves), 1, serializeFile);
	
	HashStrVoidIterator *itsv = createHashStrVoidIterator(hashsv, 0);
	for (possv = hashStrVoidIteratorNext(itsv,hashsv); possv!=NULL; possv = hashStrVoidIteratorNext(itsv,hashsv)) {
		writeKeyStrVoid(serializeFile, hashsv, possv);
		writeValueStrVoid(serializeFile, hashsv, possv);
	}
	hashStrVoidIteratorDestroy(itsv, hashsv);
	fclose(serializeFile);

	
	printf("Writing complete, now reading back\n");
	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	PosHandlerStrVoid readPossv = (PosHandlerStrVoid)calloc(1, sizeof(ListaStrVoid));
	numKeys = -1;

	fread(&(numKeys), sizeof(hashsv->chaves), 1, serializeFile);
	assert(numKeys == hashsv->chaves);

	for (i=0; i<numKeys; i++) {
		readKeyStrVoid(serializeFile, hashsv, readPossv);
		readValueStrVoid(serializeFile, hashsv, readPossv);
		
		possv = hashStrVoidGet(hashsv, posGetKey(readPossv));		

		TaskIdList *hashVal = (TaskIdList *)posGetValue(possv);
		TaskIdList *readVal = (TaskIdList *)posGetValue(readPossv);
	
		// Check read values
		assert(!strcmp(posGetKey(readPossv), posGetKey(possv)) );
		assert(!taskIdListCompare(hashVal, readVal));

		taskIdListDestroy(readVal);
	}
	fclose(serializeFile);

*/
/*----------- hashStrVoidSerialize()/hashStrVoidDeserialize() test ----------*/

/*	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashStrVoidSerialize(serializeFile, hashsv);
	fclose(serializeFile);

	
	// Read a entire copy of hashsv and compare both
	// ATENTION: don't let the hash do rehash, 
	// because it can chage the key positions on a entry list
	HashStrVoid *hashsvRead = hashStrVoidCreate(hashsv->tam);
	setStrVoidSerializer(hashsvRead, &writeTaskIdList, &readTaskIdList);

	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashStrVoidDeserialize(serializeFile, hashsvRead);
	fclose(serializeFile);

	// Compare both
	HashStrVoidIterator *hashIt = createHashStrVoidIterator(hashsv, 0);
	HashStrVoidIterator *readHashIt = createHashStrVoidIterator(hashsvRead, 0);
	PosHandlerStrVoid posHash = hashStrVoidIteratorNext(hashIt, hashsv);
	PosHandlerStrVoid posRead = hashStrVoidIteratorNext(readHashIt, hashsvRead);

	while (posHash != NULL) {
		// Check keys
		if (strcmp( posGetKey(posRead), posGetKey(posHash) )) {
			assert(0);
		}

		// Check values
		TaskIdList *hashVal = (TaskIdList *)posGetValue(posHash);
		TaskIdList *readVal = (TaskIdList *)posGetValue(posRead);
		
		if (taskIdListCompare(hashVal, readVal)) {
			assert(0);
		}

		// Go to next position
		posHash = hashStrVoidIteratorNext(hashIt, hashsv);
		posRead = hashStrVoidIteratorNext(readHashIt, hashsvRead);
	}
	assert(posHash == NULL);
	assert(posRead == NULL);

	// Destroy everything (obs: this doesn't destroy the hashs' values)
	hashStrVoidIteratorDestroy(hashIt, hashsv);
	hashStrVoidIteratorDestroy(readHashIt, hashsvRead);
	hashStrVoidDestroy(hashsv);
	hashStrVoidDestroy(hashsvRead);
*/


/*---------------------- Empty hash serialization test ----------------------*/

	HashIntFloat *hashif = hashIntFloatCreate(0);

	// Serialize the hash
	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashIntFloatSerialize(serializeFile, hashif);
	fclose(serializeFile);

	// Read a entire copy of hashif and compare both
	// ATENTION: don't let the hash do rehash, 
	// because it can chage the key positions on a entry list
	HashIntFloat *hashifRead = hashIntFloatCreate(hashif->tam);
	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashIntFloatDeserialize(serializeFile, hashifRead);
	fclose(serializeFile);

	// Compare both
	HashIntFloatIterator *hashifIt = createHashIntFloatIterator(hashif, 0);
	HashIntFloatIterator *hashifReadIt = createHashIntFloatIterator(hashifRead, 0);
	PosHandlerIntFloat posifHash = hashIntFloatIteratorNext(hashifIt, hashif);
	PosHandlerIntFloat posifRead = hashIntFloatIteratorNext(hashifReadIt, hashifRead);

	while (posifHash != NULL) {
		// Check keys
		if (posGetKey(posifRead) != posGetKey(posifHash)) {
			assert(0);
		}

		// Check values		
		if (posGetValue(posifRead) != posGetValue(posifHash)) {
			assert(0);
		}

		// Go to next position
		posifHash = hashIntFloatIteratorNext(hashifIt, hashif);
		posifRead = hashIntFloatIteratorNext(hashifReadIt, hashifRead);
	}
	assert(posifHash == NULL);
	assert(posifRead == NULL);

	// Destroy everything (obs: this doesn't destroy the hashs' values)
	hashIntFloatIteratorDestroy(hashifIt, hashif);
	hashIntFloatIteratorDestroy(hashifReadIt, hashifRead);
	hashIntFloatDestroy(hashif);
	hashIntFloatDestroy(hashifRead);





	
/*--------------------------- Test Hash int->float --------------------------*/
		
	hashif = hashIntFloatCreate(0);
	for (i=0; i<NUM_KEYS; i++) {
		PosHandlerIntFloat pos = hashIntFloatAdd(hashif, i);
		posSetValue(pos, drand48()*1000000);
	}

	// Verify if rehash has been applied
	if (hashif->tam == 0) {
		printf("%s (%d): ERROR, rehash hasn't been applied\n", __FILE__, __LINE__);
	}

	// Serialize the hash
	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashIntFloatSerialize(serializeFile, hashif);
	fclose(serializeFile);

	// Read a entire copy of hashif and compare both
	// ATENTION: don't let the hash do rehash, 
	// because it can chage the key positions on a entry list
	hashifRead = hashIntFloatCreate(hashif->tam);
	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashIntFloatDeserialize(serializeFile, hashifRead);
	fclose(serializeFile);

	// Compare both
	hashifIt = createHashIntFloatIterator(hashif, 0);
	hashifReadIt = createHashIntFloatIterator(hashifRead, 0);
	posifHash = hashIntFloatIteratorNext(hashifIt, hashif);
	posifRead = hashIntFloatIteratorNext(hashifReadIt, hashifRead);

	while (posifHash != NULL) {
		// Check keys
		if (posGetKey(posifRead) != posGetKey(posifHash)) {
			assert(0);
		}

		// Check values		
		if (posGetValue(posifRead) != posGetValue(posifHash)) {
			assert(0);
		}

		// Go to next position
		posifHash = hashIntFloatIteratorNext(hashifIt, hashif);
		posifRead = hashIntFloatIteratorNext(hashifReadIt, hashifRead);
	}
	assert(posifHash == NULL);
	assert(posifRead == NULL);

	// Destroy everything (obs: this doesn't destroy the hashs' values)
	hashIntFloatIteratorDestroy(hashifIt, hashif);
	hashIntFloatIteratorDestroy(hashifReadIt, hashifRead);
	hashIntFloatDestroy(hashif);
	hashIntFloatDestroy(hashifRead);


/*--------------------------- Test Hash DSI->void --------------------------*/
	
	// Test DSI struct copy
	DataSpaceId dsi1 = {10, 20};
	DataSpaceId dsi2 = {-1, -1};
	TaskIdList *tidl = NULL;

	dsi2 = dsi1;
	assert(dsi2.work == dsi1.work);
	assert(dsi2.task == dsi1.task);


	// Create a random hash
	HashDSIVoid *hashdv = hashDSIVoidCreate(0);
	setDSIVoidSerializer(hashdv, &writeTaskIdList, &readTaskIdList);
	
	PosHandlerDSIVoid posdv = NULL;
	for (i=0; i<NUM_STR_VOID_KEYS; i++) {
		// Create a random key
		dsi1.work = random();
		dsi1.task = random();
		
		// Create a random value
		int j=0;
		int valSz = random()%1000;
		TaskIdList *val = taskIdListCreate(0);
		for (j=0; j<valSz; j++) {
			taskIdListAdd(val, random());
		}

		if (i==20) {
			dsi2 = dsi1;
			tidl = val;
		}
		
		posdv = hashDSIVoidAdd(hashdv, dsi1);
		posSetValue(posdv, val);
	}

	// Verify if rehash has been applied
	if (hashdv->tam == 0) {
		printf("%s (%d): ERROR, rehash hasn't been applied\n", __FILE__, __LINE__);
	}

	// Access keys that exist in hash and keys thah doesn't 
	// to see if it's working
	posdv = hashDSIVoidGet(hashdv, dsi2);
	assert( dsi2.work == (posGetKey(posdv)).work );
	assert( dsi2.task == (posGetKey(posdv)).task );
	assert( tidl == posGetValue(posdv) );

	for (i=0; i<1000; i++) {
		// Create a random key
		dsi1.work = random();
		dsi1.task = random();

		posdv = hashDSIVoidGet(hashdv, dsi1);
		if (posdv != NULL) {
			printf("Key in hash\n");
		}
	}
	
	// Test hash serialization
	serializeFile = fopen("/tmp/hash_serialize.dat", "w");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}	
	hashDSIVoidSerialize(serializeFile, hashdv);
	fclose(serializeFile);

	
	// Read a entire copy of hashdv and compare both
	// ATENTION: don't let the hash do rehash, 
	// because it can chage the key positions on a entry list
	HashDSIVoid *hashdvRead = hashDSIVoidCreate(hashdv->tam);
	setDSIVoidSerializer(hashdvRead, &writeTaskIdList, &readTaskIdList);

	serializeFile = fopen("/tmp/hash_serialize.dat", "r");
	if (serializeFile == NULL) {
		perror("Can't open serialize file /tmp/hash_serialize.dat:");
		exit(1);
	}
	hashDSIVoidDeserialize(serializeFile, hashdvRead);
	fclose(serializeFile);

	// Compare both
	HashDSIVoidIterator *hashdvIt = createHashDSIVoidIterator(hashdv, 0);
	HashDSIVoidIterator *readHashdvIt = createHashDSIVoidIterator(hashdvRead, 0);
	PosHandlerDSIVoid posdvHash = hashDSIVoidIteratorNext(hashdvIt, hashdv);
	PosHandlerDSIVoid posdvRead = hashDSIVoidIteratorNext(readHashdvIt, hashdvRead);

	while (posdvHash != NULL) {
		// Check keys
		dsi1 = posGetKey(posdvRead);
		dsi2 = posGetKey(posdvHash);
		assert(dsi2.work == dsi1.work);
		assert(dsi2.task == dsi1.task);

		// Check values
		TaskIdList *hashVal = (TaskIdList *)posGetValue(posdvHash);
		TaskIdList *readVal = (TaskIdList *)posGetValue(posdvRead);
		
		if (taskIdListCompare(hashVal, readVal)) {
			assert(0);
		}

		// Go to next position
		posdvHash = hashDSIVoidIteratorNext(hashdvIt, hashdv);
		posdvRead = hashDSIVoidIteratorNext(readHashdvIt, hashdvRead);
	}
	assert(posdvHash == NULL);
	assert(posdvRead == NULL);

	// Destroy everything (obs: this doesn't destroy the hashs' values)
	hashDSIVoidIteratorDestroy(hashdvIt, hashdv);
	hashDSIVoidIteratorDestroy(readHashdvIt, hashdvRead);
	hashDSIVoidDestroy(hashdv);
	hashDSIVoidDestroy(hashdvRead);







	
	return 0;
}



