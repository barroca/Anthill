#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "test.h"
#include "DCBuffer.h"
#include "api_util.h"
#include <string>

using namespace std;
void mxArrayToDCBuffer(DCBuffer *buffer , mxArray *mxStringValue);
void mxArrayArrayToDCBuffer(DCBuffer *buffer , mxArray **mxStringValue);

#define INT_SCALAR_SERIALIZE(DCType, DCUnpackType) \
	DCType *returnedInt = (DCType *)(mxGetData(mxValue)); \
	buffer->pack(DCUnpackType, *returnedInt);

#define INT_ARRAY_SERIALIZE(DCType) \
	int numElements = mxGetNumberOfElements(mxValue); \
	buffer->pack("i", numElements); \
	buffer->Append((DCType *) mxGetData(mxValue), numElements);

#define INT_MATRIX_SERIALIZE(DCType) \
	int numberElements = mxGetNumberOfElements(mxValue); \
	int numRows = mxGetM(mxValue); \
	int numColumns = mxGetN(mxValue); \
	buffer->pack("i", numberElements); \
	buffer->pack("i", numRows); \
	buffer->pack("i", numColumns); \
	buffer->Append((DCType *)mxGetData(mxValue), numberElements);

#ifdef DEBUG
	#define PRINT(args...) printf(args)
#endif

#endif
