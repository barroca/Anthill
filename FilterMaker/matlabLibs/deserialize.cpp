#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "mex.h"
#include "deserialize.h"
//#include "matrix.h"

// create mxArray with a charArray
mxArray *stringToMxArray(string str){

	mxArray *returnValue = mxCreateString(str.c_str());
	if(returnValue == NULL){
		printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);
		exit(1);
	}
	return returnValue;
}

// creates mxArray with a charArray
mxArray *charArrayToMxArray(DCBuffer *buffer){
	string input;
	buffer->unpack("s", &input);
	
	mxArray *returnValue = mxCreateString(input.c_str());
	printf("String = %s\n", input.c_str());	
	if(returnValue == NULL){
		printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);	
		exit(1);	
	}
	return returnValue;
}

mxArray *StringArrayToMxArray(DCBuffer *buffer){
	mxArray *returnValue;	
	int numRows, i;
	char **auxCopy;
	string auxString;
	buffer->unpack("i", &numRows);
	printf("numRows = %d\n", numRows);

	// malloc space for the pointers
	auxCopy = (char **) malloc(sizeof(char *) * numRows);
	for(i = 0; i < numRows; i++){
		buffer->unpack("s", &auxString);
		auxCopy[i] = strdup(auxString.c_str());
	}
	returnValue = mxCreateCharMatrixFromStrings(numRows, (const char **)auxCopy);

	for(i = 0; i < numRows; i++){
		if(auxCopy[i]== NULL)printf("sujou\n");
		free(auxCopy[i]);
	}
	free(auxCopy);
	return returnValue; 
}

// creates mxArray with a single double scalar
mxArray *doubleToMxArray(DCBuffer *buffer){
	double input;
	buffer->unpack("d", &input);

	mxArray* returnValue = mxCreateScalarDouble(input);

	if(returnValue == NULL){
		printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);	
		exit(1);	
	}
	return returnValue;
}	

mxArray *doubleArrayToMxArray(DCBuffer *buffer){
	int numberOfElemnts;
	buffer->unpack("i", &numberOfElemnts);
	mxArray *returnValue = mxCreateNumericArray( 1, &numberOfElemnts, mxDOUBLE_CLASS, mxREAL);

	buffer->Extract(mxGetPr(returnValue), numberOfElemnts);
	
	return returnValue;
}

mxArray *doubleMatrixToMxArray(DCBuffer *buffer){
	int numberOfElemnts, numRows, numColumns;
	mxArray *returnValue;
	buffer->unpack("i", &numberOfElemnts);
	buffer->unpack("i", &numRows);
	buffer->unpack("i", &numColumns);

	if(numRows < 2)printf("Erro: %s:%d Its not a Matrix: numRows < 2\n", __FILE__, __LINE__); 
	returnValue = mxCreateNumericMatrix(numRows, numColumns, mxDOUBLE_CLASS, mxREAL);
	buffer->Extract(mxGetPr(returnValue), numberOfElemnts);
	
	return returnValue;
}

// creates mxArray with a single boolean
mxArray *boolToMxArray(DCBuffer *buffer){
	bool input;
	buffer->unpack("b", &input);

	mxArray* returnValue = mxCreateLogicalScalar(input);

	if(returnValue == NULL){
		printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);	
		exit(1);	
	}
	return returnValue;
}

mxArray *singleToMxArray(DCBuffer *buffer){
	float *auxCopy;
	const int dim = 1;
	mxArray *returnValue = mxCreateNumericArray( 1, &dim, mxSINGLE_CLASS, mxREAL);
	auxCopy = (float *)mxGetPr(returnValue);
	buffer->Extract(auxCopy, dim);
	
	return returnValue;
}



mxArray *singleArrayToMxArray(DCBuffer *buffer){
	int numberOfElemnts;
	float *auxCopy;
	buffer->unpack("i", &numberOfElemnts);
	mxArray *returnValue = mxCreateNumericArray( 1, &numberOfElemnts, mxSINGLE_CLASS, mxREAL);
	auxCopy = (float *)mxGetPr(returnValue);
	buffer->Extract(auxCopy, numberOfElemnts);
	
	return returnValue;
}

mxArray *singleMatrixToMxArray(DCBuffer *buffer){
	mxArray *returnValue;
	int numberOfElemnts;
	int numRows;
	int numColumns;
	float *auxCopy;
	
	buffer->unpack("i", &numberOfElemnts);
	buffer->unpack("i", &numRows);
	buffer->unpack("i", &numColumns);

	returnValue = mxCreateNumericMatrix(numRows, numColumns, mxSINGLE_CLASS, mxREAL);

	auxCopy = (float *)mxGetPr(returnValue);
	buffer->Extract(auxCopy, numberOfElemnts);
	
	return returnValue;
}
// creates mxArray with a bool array
mxArray *boolArrayToMxArray(DCBuffer *buffer){
	int numberOfElemnts;
	
	buffer->unpack("i", &numberOfElemnts);

	int dims[2];
	dims[0] = numberOfElemnts;
	dims[1] = 1;
	mxArray *returnValue = mxCreateLogicalArray(2, dims);

	buffer->Extract((int1 *)mxGetLogicals(returnValue), numberOfElemnts);
	
	return returnValue;
}

mxArray *boolMatrixToMxArray(DCBuffer *buffer) {
	mxArray *returnValue;
	int numberOfElemnts;
	int numRows;
	int numColumns;
	
	buffer->unpack("i", &numberOfElemnts);
	buffer->unpack("i", &numRows);
	buffer->unpack("i", &numColumns);

	returnValue = mxCreateLogicalMatrix(numRows, numColumns);

	buffer->Extract((int1 *)mxGetLogicals(returnValue), numberOfElemnts);
	
	return returnValue;
}
mxArray *uint8MatrixMToMxArray(DCBuffer *buffer){
	mxArray *returnValue;
	int ndim, numberOfElemnts = 1;
	buffer->unpack("i", &ndim);
	int *dims =(int *) malloc(sizeof(int) * ndim);
	for (int i = 0; i < ndim; i++){
		buffer->unpack("i", &dims[i]);	
		numberOfElemnts *= dims[i];
	}
	returnValue = mxCreateNumericArray(ndim, dims, mxUINT8_CLASS, mxREAL);

	buffer->Extract((uint1 *)mxGetPr(returnValue), numberOfElemnts);

	return returnValue;
}

// creates mxArray with a single signed 8-bit integer
INT_SCALAR_DESERIALIZE(int8ToMxArray, mxINT8_CLASS, int1, "b")

// creates mxArray with a single signed 8-bit integer
INT_SCALAR_DESERIALIZE(uint8ToMxArray, mxUINT8_CLASS, uint1, "B")

// creates mxArray with a single signed 16-bit integer
INT_SCALAR_DESERIALIZE(int16ToMxArray, mxINT16_CLASS, int2, "h")

// creates mxArray with a single unsigned 16-bit integer
INT_SCALAR_DESERIALIZE(uint16ToMxArray, mxUINT16_CLASS, uint2, "H")

// creates mxArray with a single signed 32-bit integer
INT_SCALAR_DESERIALIZE(int32ToMxArray, mxINT32_CLASS, int4, "i")

// creates mxArray with a single unsigned 32-bit integer
INT_SCALAR_DESERIALIZE(uint32ToMxArray, mxUINT32_CLASS, uint4, "I")

// creates mxArray with a single signed 64-bit integer
INT_SCALAR_DESERIALIZE(int64ToMxArray, mxINT64_CLASS, int8, "l")

// creates mxArray with a single unsigned 64-bit integer
INT_SCALAR_DESERIALIZE(uint64ToMxArray, mxUINT64_CLASS, uint8, "L")

// creates mxArray with a 8-bit signed integer array
INT_ARRAY_DESERIALIZE(int8ArrayToMxArray, mxINT8_CLASS, int1)

// creates mxArray with a 8-bit unsigned integer array
INT_ARRAY_DESERIALIZE(uint8ArrayToMxArray, mxUINT8_CLASS, uint1)

// creates mxArray with a 16-bit signed integer array
INT_ARRAY_DESERIALIZE(int16ArrayToMxArray, mxINT16_CLASS, int2)

// creates mxArray with a 16-bit unsigned integer array
INT_ARRAY_DESERIALIZE(uint16ArrayToMxArray, mxUINT16_CLASS, uint2)

// creates mxArray with a 32-bit signed integer array
INT_ARRAY_DESERIALIZE(int32ArrayToMxArray, mxINT32_CLASS, int4)

// creates mxArray with a 32-bit unsigned integer array
INT_ARRAY_DESERIALIZE(uint32ArrayToMxArray, mxUINT32_CLASS, uint4)

// creates mxArray with a 64-bit signed integer array
INT_ARRAY_DESERIALIZE(int64ArrayToMxArray, mxINT64_CLASS, int8)

// creates mxArray with a 64-bit unsigned integer array
INT_ARRAY_DESERIALIZE(uint64ArrayToMxArray, mxUINT64_CLASS, uint8)

// creates mxArray with a 8-bit signed integer matrix
INT_MATRIX_DESERIALIZE(int8MatrixToMxArray, mxINT8_CLASS, int1)

// creates mxArray with a 8-bit unsigned integer matrix
INT_MATRIX_DESERIALIZE(uint8MatrixToMxArray, mxUINT8_CLASS, uint1)

// creates mxArray with a 16-bit signed integer matrix
INT_MATRIX_DESERIALIZE(int16MatrixToMxArray, mxINT16_CLASS, int2)

// creates mxArray with a 16-bit unsigned integer matrix
INT_MATRIX_DESERIALIZE(uint16MatrixToMxArray, mxUINT16_CLASS, uint2)

// creates mxArray with a 32-bit signed integer matrix
INT_MATRIX_DESERIALIZE(int32MatrixToMxArray, mxINT32_CLASS, int4)

// creates mxArray with a 32-bit unsigned integer matrix
INT_MATRIX_DESERIALIZE(uint32MatrixToMxArray, mxUINT32_CLASS, uint4)

// creates mxArray with a 64-bit signed integer matrix
INT_MATRIX_DESERIALIZE(int64MatrixToMxArray, mxINT64_CLASS, int8)

// creates mxArray with a 64-bit unsigned integer matrix
INT_MATRIX_DESERIALIZE(uint64MatrixToMxArray, mxUINT64_CLASS, uint8)


