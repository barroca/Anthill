#ifndef DESERIALIZE_H
#define DESERIALIZE_H

#include "test.h"
#include "DCBuffer.h"
#include "api_util.h"
#include <string>

using namespace std;


// Char and String
mxArray *stringToMxArray(string str);
mxArray *charArrayToMxArray(DCBuffer *buffer);
mxArray *StringArrayToMxArray(DCBuffer *buffer);

// Double
mxArray *doubleToMxArray(DCBuffer *buffer);
mxArray *doubleArrayToMxArray(DCBuffer *buffer);
mxArray *doubleMatrixToMxArray(DCBuffer *buffer);

// Logical
mxArray *boolToMxArray(DCBuffer *buffer);
mxArray *boolArrayToMxArray(DCBuffer *buffer);
mxArray *boolMatrixToMxArray(DCBuffer *buffer);

// Single
mxArray *singleToMxArray(DCBuffer *buffer);
mxArray *singleArrayToMxArray(DCBuffer *buffer);
mxArray *singleMatrixToMxArray(DCBuffer *buffer);

// Integer 8 bits
mxArray *int8ToMxArray(DCBuffer *buffer);
mxArray *uint8ToMxArray(DCBuffer *buffer);
mxArray *int8ArrayToMxArray(DCBuffer *buffer);
mxArray *uint8ArrayToMxArray(DCBuffer *buffer);
mxArray *int8MatrixToMxArray(DCBuffer *buffer);
mxArray *uint8MatrixToMxArray(DCBuffer *buffer);

// Integer 16 bits
mxArray *int16ToMxArray(DCBuffer *buffer);
mxArray *uint16ToMxArray(DCBuffer *buffer);
mxArray *int16ArrayToMxArray(DCBuffer *buffer);
mxArray *uint16ArrayToMxArray(DCBuffer *buffer);
mxArray *int16MatrixToMxArray(DCBuffer *buffer);
mxArray *uint16MatrixToMxArray(DCBuffer *buffer);

// Integer 32 bits
mxArray *int32ToMxArray(DCBuffer *buffer);
mxArray *uint32ToMxArray(DCBuffer *buffer);
mxArray *int32ArrayToMxArray(DCBuffer *buffer);
mxArray *uint32ArrayToMxArray(DCBuffer *buffer);
mxArray *int32MatrixToMxArray(DCBuffer *buffer);
mxArray *uint32MatrixToMxArray(DCBuffer *buffer);

// Integer 64 bits
mxArray *int64ToMxArray(DCBuffer *buffer);
mxArray *uint64ToMxArray(DCBuffer *buffer);
mxArray *int64ArrayToMxArray(DCBuffer *buffer);
mxArray *uint64ArrayToMxArray(DCBuffer *buffer);
mxArray *int64MatrixToMxArray(DCBuffer *buffer);
mxArray *uint64MatrixToMxArray(DCBuffer *buffer);

// N dimensions array uint 8 bits
mxArray *uint8MatrixMToMxArray(DCBuffer *buffer);


// Generates functions to deserialize integer scalar
#define INT_SCALAR_DESERIALIZE(funcName, classType, DCType, DCUnpackType) \
		mxArray *funcName(DCBuffer *buffer){ \
		int dims = 1; \
		mxArray* returnValue = mxCreateNumericArray(1, &dims, classType, mxREAL); \
		buffer->unpack(DCUnpackType, (DCType *)mxGetPr(returnValue)); \
		if(returnValue == NULL){ \
			printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);	\
			exit(1);	\
		} \
		return returnValue; \
		}

// Generates functions to deserialize integer arrays
#define INT_ARRAY_DESERIALIZE(funcName, classType, DCType) \
	mxArray *funcName(DCBuffer *buffer){ \
		int numberOfElemnts; \
		buffer->unpack("i", &numberOfElemnts); \
		mxArray *returnValue = mxCreateNumericArray(1, &numberOfElemnts, classType, mxREAL); \
		buffer->Extract((DCType *)mxGetData(returnValue), numberOfElemnts); \
		return returnValue; \
	}

// Generates functions to deserialize integer matrix
#define INT_MATRIX_DESERIALIZE(funcName, classType, DCType) \
	mxArray *funcName(DCBuffer *buffer){ \
		int numberOfElemnts, numRows, numColumns; \
		mxArray *returnValue; \
		buffer->unpack("i", &numberOfElemnts); \
		buffer->unpack("i", &numRows); \
		buffer->unpack("i", &numColumns); \
		if(numRows < 2) printf("Erro: %s:%d Its not a Matrix: numRows < 2\n", __FILE__, __LINE__); \
		returnValue = mxCreateNumericMatrix(numRows, numColumns, classType, mxREAL); \
		buffer->Extract((DCType *) mxGetData(returnValue), numberOfElemnts); \
		return returnValue; \
	}

#endif
