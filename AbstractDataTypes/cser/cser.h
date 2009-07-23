/* By Sean Middleditch <elanthis@awemud.net>
   released into public domain, July 8th, 2003 */

#if !defined(C_SERIALIZE)
#define C_SERIALIZE 1

#include <stdio.h>
#include <string.h>

#ifdef TO_NETWORK

#include "pvm3.h"

#define WRITE_BYTES(file, buffer, bufSize) pvm_pkbyte(buffer, bufSize, 1) // buffer: char *
#define WRITE_NUM(file, varname, var)	   pvm_pkint(&var, 1, 1) 	  // var: int
#define WRITE_STR(file, varname, var)	   cser_pkstr(var) 		  // var: char *
#define WRITE_FLOAT(file, varname, var)	   pvm_pkfloat(&var, 1, 1)	  // var: float      

#define READ_BEGIN(file)
#define READ_END
#define READ_BYTES(file, buffer, bufSize) pvm_upkbyte(buffer, bufSize, 1) // buffer: char *
#define READ_NUM(varname, var)		  pvm_upkint(&var, 1, 1)          // var: int
#define READ_STR(varname, var) 		  (var = cser_upkstr())           // var: char *
#define READ_FLOAT(varname, var) 	  pvm_pkfloat(&var, 1, 1)         // var: float    

#else

#define WRITE_BYTES(file, buffer, bufSize) fwrite(buffer, bufSize, 1, file)
#define WRITE_NUM(file, varname, var) cser_write_num(file, varname, var)
#define WRITE_STR(file, varname, var) cser_write_str(file, varname, var)
#define WRITE_FLOAT(file, varname, var) fwrite(&(var), sizeof(float), 1, file)

#define READ_BEGIN(file) CSER_READ_BEGIN(file)
#define READ_END CSER_READ_END
#define READ_BYTES(file, buffer, bufSize) fread(buffer, bufSize, 1, file)
#define READ_NUM(varname, var) CSER_READ_NUM(varname, var)
#define READ_STR(varname, var) CSER_READ_STR(varname, var)
#define READ_FLOAT(varname, var) fread(&(var), sizeof(float), 1, filePtr)



#define CSER_MAX_DATA 4096

/* write values to a file */
int cser_write_str(FILE* file, const char* name, const char* value);
int cser_write_num(FILE* file, const char* name, long num);

// Original version, struct values cam be in any order but depends on variable
// name unicity to work and this restriction is very serious to me

/* read from file */
#define CSER_READ_BEGIN(file) \
	do { \
		char _cser_buffer[CSER_MAX_DATA]; \
		char* _cser_name = NULL; \
		char* _cstr_string = NULL; \
		long _cstr_number = 0; \
		FILE *filePtr = file;
#define CSER_READ_STR(name,var) \
		if ((cser_read((filePtr), _cser_buffer, sizeof(_cser_buffer), \
			&_cser_name, &_cstr_string, &_cstr_number)) == 0) { \
			if (!strcasecmp((name), _cser_name)) { \
				if ((var) != NULL) free(var); \
				var = NULL; \
				if (_cstr_string != NULL) \
					(var) = strdup(_cstr_string); \
			} else \
			{ fprintf(stderr, "cser var name error\n"); } \
		} else { fprintf(stderr, "cser_read() error\n"); } 
#define CSER_READ_NUM(name,var) \
		if ((cser_read((filePtr), _cser_buffer, sizeof(_cser_buffer), \
			&_cser_name, &_cstr_string, &_cstr_number)) == 0) { \
			if (!strcasecmp((name), _cser_name)) { \
				(var) = _cstr_number; \
			} else 	\
			{ fprintf(stderr, "cser var name error\n"); } \
		} else { fprintf(stderr, "cser_read() error\n"); } 
#define CSER_READ_END \
	} while(0);

/* read one value from file */
int cser_read(FILE* file, char* buffer, size_t blen, char** name, char** str_value, long* num_value);


#endif



typedef int packData_t(void *list);
typedef void *voidp;
typedef voidp unpackData_t(void);

int sendStruct(int tid, int tag, void *Struct, packData_t *packer);
int mcastStruct(int *tids, int tidListSize, int tag, void *dataStruct, packData_t *packer);
void *recvStruct(int tid, int tag, unpackData_t *unpacker);





#endif
