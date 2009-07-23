#include <iostream.h>
#include "api_cpp.h"
#include "api_cpp_cache.h"

#include "TKHistogramNormalization.h"
#include "serialize.h"
#include "deserialize.h"
#include "TKHistogramNormalization1.h"

class filterB : public DCFilterCache
{

public:
	filterB(){}
	~filterB(){}

	int init();
	int process();
	int fini();
};

int filterB::init() {
	mclInitializeApplication(NULL,0);

	if (!TKHistogramNormalization1Initialize()){
		fprintf(stderr,"could not initialize the library properly\n");
		return -1;
	}

	return 0;
}

int filterB::process() {

	// Input arguments
	mxArray* arg3; // user argument
	mxArray* arg4; // user argument
	mxArray* arg5; // user argument
	mxArray* arg6; // user argument
	mxArray* arg1; // message argument
	mxArray* arg2; // message argument

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
	arg3 = stringToMxArray(userArg1);
	buffer->unpack("s", &userArg2);
	arg4 = stringToMxArray(userArg2);
	buffer->unpack("s", &userArg3);
	arg5 = stringToMxArray(userArg3);
	buffer->unpack("s", &userArg4);
	arg6 = stringToMxArray(userArg4);

	// Choose the correct cache transparent copy to communicate.
	createLinkToCache(dcmpi_get_hostname());


	while (1) {
		string mobiusXML;

		// Receives message from upstream filter
		DCBuffer * inBuffer = read("input", "filterBToCache", "cacheTofilterB", &mobiusXML, false);
		if(inBuffer == NULL) break;
		arg1 = uint8MatrixToMxArray(inBuffer);
		arg2 = uint8MatrixToMxArray(inBuffer);

		// MatLab Function
		mlfTKHistogramNormalization1(arg1, arg2, arg3, arg4, arg5, arg6);
	}

	return 0;
}

int filterB::fini() {
	TKHistogramNormalization1Terminate();

	mclTerminateApplication();

	return 0;
}

provide1(filterB)
