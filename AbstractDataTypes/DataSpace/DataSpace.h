#ifndef _DATASPACE_H_
#define _DATASPACE_H_

#define KEY_STRING
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_STRING

typedef struct _DataSpace_ {
	HashStrVoid *dataHash;
} DataSpace;

DataSpace *  createDataSpace();
void  destroyDataSpace(DataSpace * space);
void *  getData (DataSpace * space, char * key, int * valSz);
int  putData (DataSpace * space, char * key, void * val, int valSz);
int  removeData (DataSpace * space, char * key);
int  readDataSpace (FILE * inputFile, DataSpace * space);
int  writeDataSpace (FILE * outputSpace, DataSpace * space);

#endif
