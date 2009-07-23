#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "serialize.h"

void mxArrayToDCBuffer(DCBuffer *buffer , mxArray *mxValue){
	int status;

	// check if there is any elemente in mxValue
	if(mxIsEmpty(mxValue)){
		printf("Error: %s:%d mxArrayToDCBuffer. mxValue Empty\n", __FILE__, __LINE__);
		exit(1);
	}

	if(mxIsChar(mxValue) && (mxGetM(mxValue) == 1)){
		// if is vector char
		printf("IsChar and numElements = %d\n", mxGetN(mxValue));
		int buflen = (mxGetM(mxValue) * mxGetN(mxValue) * sizeof(mxChar)) + 1;
		printf("sizeof(char) = %d\n", sizeof(char));
		printf("bufLen = %d\n", buflen);
		char *buf = (char *) malloc(buflen);
		status = mxGetString(mxValue, buf, buflen);
		if(status != 0) {
			printf("Not enough space. String is truncated.");
		}
		printf("buf = %s\n", buf);
		buffer->pack("s", buf);
	} else if(mxIsChar(mxValue) && mxGetM(mxValue) > 1 && mxGetN(mxValue) > 1 && mxGetNumberOfDimensions(mxValue) == 2){
		// Matrix of char
		char **auxCopy;
		int i, j;
		char *auxCharPtr;
		printf("String Matrix\n");
		int numRows = mxGetM(mxValue);
		int numColumns = mxGetN(mxValue);

		// malloc space for the pointers
		auxCopy = (char **) malloc(sizeof(numRows) * sizeof(char *));
		// create space for each string
		for(i = 0; i < numRows; i++){
			auxCopy[i] = (char *) malloc(sizeof(char) * numColumns + 1);		
		}

		// copy the strings
		for(i = 0; i < numRows; i++){
			auxCharPtr =(char *) mxGetPr(mxValue);
			auxCharPtr+=i*2;
			for(j = 0; j < numColumns; j++){
				auxCopy[i][j] = *auxCharPtr;
				auxCharPtr += numRows *2; 
			}
			auxCopy[i][j] = '\0';
		}
		buffer->pack("i", numRows);
	
		// pack strings and free memory
		for(i = 0; i < numRows; i++){
			buffer->pack("s", auxCopy[i]);
			printf("auxCopy[%d] = %s\n", i, auxCopy[i]);
			free(auxCopy[i]);	
		}
		free(auxCopy);
	
	}else	if(mxIsDouble(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// we are working with double - scalar - and not logical TODO: i dont no what is logical
		printf("IsDouble\n");
		double returnedDouble = (double)(*mxGetPr(mxValue));
		printf ("Double ToDCBuffer = %f\n", returnedDouble);
		buffer->pack("d", returnedDouble);
	} else	if(mxIsDouble(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == 1)){
		//  double array
		printf("DoubleArray\n");
		int numberElements = mxGetNumberOfElements(mxValue);
		buffer->pack("i", numberElements);
		buffer->Append(mxGetPr(mxValue), numberElements);
	} else if(mxIsDouble(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) > 1)  && (mxGetN(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// double matrix
		printf("Double Matrix\n");
		int numberElements = mxGetNumberOfElements(mxValue);
		int numRows = mxGetM(mxValue);
		int numColumns = mxGetN(mxValue);
		buffer->pack("i", numberElements);
		buffer->pack("i", numRows);
		buffer->pack("i", numColumns);
		buffer->Append(mxGetPr(mxValue), numberElements);
	} else if((mxIsSingle(mxValue)) && (mxGetNumberOfElements(mxValue) == 1) && (mxGetM(mxValue) == 1) && (!mxIsLogical(mxValue))){
		// single
		printf("Single Scalar\n");
		float *auxCopy;
		auxCopy = (float *)mxGetPr(mxValue);
		buffer->Append(auxCopy, 1);

	}else if((mxIsSingle(mxValue)) && (mxGetNumberOfElements(mxValue) >= 1) && (mxGetM(mxValue) == 1) && (!mxIsLogical(mxValue))){
		// single array
		printf("Single Array\n");
		float *auxCopy;
		int numberElements = mxGetNumberOfElements(mxValue);
		if(numberElements == 1){
			printf("Scalar\n");	
		}
		buffer->pack("i", numberElements);
		auxCopy = (float *)mxGetPr(mxValue);
		buffer->Append(auxCopy, numberElements);
	} else if((mxIsSingle(mxValue)) && (mxGetNumberOfElements(mxValue) > 1) && (mxGetM(mxValue) > 1) && (mxGetN(mxValue) > 1) && (mxGetNumberOfDimensions(mxValue) == 2)){
		// single matrix
		printf("Single Matrix\n");
		float *auxCopy;
		int numberElements = mxGetNumberOfElements(mxValue);
		int numRows = mxGetM(mxValue);
		int numColumns = mxGetN(mxValue);
		buffer->pack("i", numberElements);
		buffer->pack("i", numRows);
		buffer->pack("i", numColumns);
		auxCopy = (float *)mxGetPr(mxValue);
		buffer->Append(auxCopy, numberElements);
	} else if(mxIsLogical(mxValue) && mxGetNumberOfElements(mxValue) == 1) {
		// logical scalar
		PRINT("LogicalScalar\n");
		bool returnedLogical = (bool) (*mxGetPr(mxValue));
		buffer->pack("b", returnedLogical);
	} else if(mxIsLogical(mxValue) && mxGetNumberOfElements(mxValue) > 1 && (mxGetNumberOfElements(mxValue) == mxGetM(mxValue))) {
		// logical array
		PRINT("LogicalArray\n");
		int numElements = mxGetNumberOfElements(mxValue);
		buffer->pack("i", numElements);
		buffer->Append((int1 *) mxGetLogicals(mxValue), numElements);
	} else if((mxIsLogical(mxValue)) && (mxGetNumberOfElements(mxValue) > 1) && (mxGetM(mxValue) > 1) && (mxGetN(mxValue) > 1)  && (mxGetNumberOfDimensions(mxValue) == 2)){
		// single	matrix
		printf("Logical Matrix\n");
		int numberElements = mxGetNumberOfElements(mxValue);
		int numRows = mxGetM(mxValue);
		int numColumns = mxGetN(mxValue);
		buffer->pack("i", numberElements);
		buffer->pack("i", numRows);
		buffer->pack("i", numColumns);
		buffer->Append((int1 *) mxGetLogicals(mxValue), numberElements);
	}	else if(mxIsInt8(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// signed 8-bit integers
		PRINT("Integer8-bit\n");
		INT_SCALAR_SERIALIZE(int1, "b")
	} else if(mxIsUint8(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// unsigned 8-bit integers
		PRINT("Uinteger8-bit\n");
		INT_SCALAR_SERIALIZE(uint1, "B")
	} else if(mxIsInt16(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// signed 8-bit integers
		PRINT("Integer16-bit\n");
		INT_SCALAR_SERIALIZE(int2, "h")
	} else if(mxIsUint16(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// unsigned 16-bit integers
		PRINT("Uinteger16-bit\n");
		INT_SCALAR_SERIALIZE(uint2, "H")
	} else if(mxIsInt32(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// signed 32-bit integers
		PRINT("Integer32-bit\n");
		INT_SCALAR_SERIALIZE(int4, "i")
	} else if(mxIsUint32(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// unsigned 32-bit integers
		PRINT("Uinteger32-bit\n");
		INT_SCALAR_SERIALIZE(uint4, "I")
	} else if(mxIsInt64(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// signed 64-bit integers
		PRINT("Integer64-bit\n");
		INT_SCALAR_SERIALIZE(int8, "l")
	} else if(mxIsUint64(mxValue) && (mxGetNumberOfElements(mxValue) == 1) && (!mxIsLogical(mxValue)) ){
		// unsigned 64-bit integers
		PRINT("Uinteger64-bit\n");
		INT_SCALAR_SERIALIZE(uint8, "L")
	} else if(mxIsInt8(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// signed 8-bit integers array
		PRINT("Integer8-bit Array\n");
		INT_ARRAY_SERIALIZE(int1)
	}  else if(mxIsUint8(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// unsigned 8-bit integers array
		PRINT("Uinteger8-bit Array\n");
		INT_ARRAY_SERIALIZE(uint1)
	} else if(mxIsInt16(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// signed 16-bit integers array
		PRINT("Integer16-bit Array\n");
		INT_ARRAY_SERIALIZE(int2)
	}  else if(mxIsUint16(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// unsigned 16-bit integers array
		PRINT("Uinteger16-bit Array\n");
		INT_ARRAY_SERIALIZE(uint2)
	} else if(mxIsInt32(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// signed 32-bit integers array
		PRINT("Integer32-bit Array\n");
		INT_ARRAY_SERIALIZE(int4)
	}  else if(mxIsUint32(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// unsigned 32-bit integers array
		PRINT("Uinteger32-bit Array\n");
		INT_ARRAY_SERIALIZE(uint4)
	} else if(mxIsInt64(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// signed 64-bit integers array
		PRINT("Integer64-bit Array\n");
		INT_ARRAY_SERIALIZE(int8)
	} else if(mxIsUint64(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetM(mxValue) == mxGetNumberOfElements(mxValue))){
		// unsigned 64-bit integers array
		PRINT("Uinteger64-bit Array\n");
		INT_ARRAY_SERIALIZE(uint8)
	} else if(mxIsInt8(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// signed 8-bit integer matrix
		PRINT("Integer8-bit Matrix\n");
		INT_MATRIX_SERIALIZE(int1)
	} else if(mxIsUint8(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// unsigned 8-bit integer matrix
		PRINT("UInteger8-bit Matrix\n");
		INT_MATRIX_SERIALIZE(uint1)
	} else if(mxIsInt16(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// signed 16-bit integer matrix
		PRINT("Integer16-bit Matrix\n");
		INT_MATRIX_SERIALIZE(int2)
	} else if(mxIsUint16(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// unsigned 16-bit integer matrix
		PRINT("UInteger16-bit Matrix\n");
		INT_MATRIX_SERIALIZE(uint2)
	} else if(mxIsInt32(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// signed 32-bit integer matrix
		PRINT("Integer32-bit Matrix\n");
		INT_MATRIX_SERIALIZE(int4)
	} else if(mxIsUint32(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// unsigned 32-bit integer matrix
		PRINT("UInteger32-bit Matrix\n");
		INT_MATRIX_SERIALIZE(uint4)
	} else if(mxIsInt64(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// signed 64-bit integer matrix
		PRINT("Integer64-bit Matrix\n");
		INT_MATRIX_SERIALIZE(int8)
	} else if(mxIsUint64(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) &&  (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) == 2){
		// unsigned 64-bit integer matrix
		PRINT("UInteger64-bit Matrix\n");
		INT_MATRIX_SERIALIZE(uint8)
	} else if(mxIsUint8(mxValue) && (mxGetNumberOfElements(mxValue) > 1) && (!mxIsLogical(mxValue)) && (mxGetN(mxValue) > 1) && (mxGetM(mxValue) > 1) && mxGetNumberOfDimensions(mxValue) > 2){
		int ndim = mxGetNumberOfDimensions(mxValue);		
		int *dims = (int *) mxGetDimensions(mxValue);
		int numberElements = 1;
		
		buffer->pack("i", ndim);
		for(int i = 0; i < ndim; i++){
			buffer->pack("i", dims[i]);			
			numberElements *= dims[i];
		}
		buffer->Append((uint1 *) mxGetPr(mxValue), numberElements);
	}
}

void mxArrayArrayToDCBuffer(DCBuffer *buffer , mxArray **mxValue) {
	mxArrayToDCBuffer(buffer, *(mxValue));
}
