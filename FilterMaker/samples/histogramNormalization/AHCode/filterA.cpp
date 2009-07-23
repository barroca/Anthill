#include <iostream.h>
#include "api_cpp.h"
#include "api_cpp_cache.h"

#include "TKHistogramNormalization.h"
#include "serialize.h"
#include "deserialize.h"
#include "TKHistogramNormalization1.h"

class filterA : public DCFilterCache
{

public:
	filterA(){}
	~filterA(){}

	int init();
	int process();
	int fini();
};

int filterA::init() {
	mclInitializeApplication(NULL,0);

	if (!TKHistogramNormalizationInitialize()){
		fprintf(stderr,"could not initialize the library properly\n");
		return -1;
	}

	return 0;
}

int filterA::process() {

	// Input arguments
	mxArray* arg5; // user argument
	mxArray* arg6; // user argument
	mxArray* arg7; // user argument
	mxArray* arg8; // user argument
	mxArray* arg3; // message argument
	mxArray* arg4; // message argument

	// Output arguments
	mxArray** arg1;
	mxArray** arg2;

	// User arguments
	string userArg1;
	string userArg2;
	string userArg3;
	string userArg4;

	// Receives the program arguments
	DCBuffer* buffer = this->get_init_filter_broadcast();

	// System arguments: <first collection> <first stream type> <mobius hosts file>
	string firstCollection, firstStream, mobiusHostFiles;
	buffer->unpack("sss", &firstCollection, &firstStream, &mobiusHostFiles);

	// Unpack user arguments
	buffer->unpack("s", &userArg1);
	arg5 = stringToMxArray(userArg1);
	buffer->unpack("s", &userArg2);
	arg6 = stringToMxArray(userArg2);
	buffer->unpack("s", &userArg3);
	arg7 = stringToMxArray(userArg3);
	buffer->unpack("s", &userArg4);
	arg8 = stringToMxArray(userArg4);

	// Choose the correct cache transparent copy to communicate.
	createLinkToCache(dcmpi_get_hostname());


	while (1) {
		string mobiusXML;

		// Receives message from cache
		DCBuffer * inBuffer = read(firstStream, "filterAToCache", "cacheTofilterA", &mobiusXML, true);
		if(inBuffer == NULL) break;
		arg3 = uint8MatrixToMxArray(inBuffer);
		arg4 = uint8MatrixToMxArray(inBuffer);

		// MatLab Function
		mlfTKHistogramNormalization(2, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

		// Deserializes arguments that will be sent
		DCBuffer outBuffer;
		mxArrayArrayToDCBuffer(&outBuffer, arg1);
		mxArrayArrayToDCBuffer(&outBuffer, arg2);

		// Sends output arguments to next filter
		write(&outBuffer, "output", "filterAToCache", "cacheTofilterA", mobiusXML);
	}

	return 0;
}

int filterA::fini() {
	TKHistogramNormalizationTerminate();

	mclTerminateApplication();

	return 0;
}

provide1(filterA)
