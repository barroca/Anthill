#include <iostream.h>
#include "api_cpp.h"
#include "api_cpp_cache.h"

#include "Util.h"

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

	if (!UtilInitialize()){
		fprintf(stderr,"could not initialize the library properly\n");
		return -1;
	}

	return 0;
}

int filterA::process() {

	// Input arguments
	string arg2; // user argument
	string arg3; // user argument

	// Arguments that are input and output
	string arg4;

	// Output arguments
	string arg1;

	// User arguments
	string userArg1;
	string userArg2;
	string userArg3;

	// Receives the program arguments
	DCBuffer* buffer = this->get_init_filter_broadcast();

	// System arguments: <first collection> <first stream type> <mobius hosts file>
	string firstCollection, firstStream, mobiusHostFiles;
	buffer->unpack("sss", &firstCollection, &firstStream, &mobiusHostFiles);

	// Unpack user arguments
	buffer->unpack("s", &userArg1);
	arg2 = stringToString(userArg1);
	buffer->unpack("s", &userArg2);
	arg3 = stringToString(userArg2);
	buffer->unpack("s", &userArg3);
	arg4 = stringToString(userArg3);

	// Choose the correct cache transparent copy to communicate.
	createLinkToCache(dcmpi_get_hostname());


	while (1) {
		string mobiusXML;

		// Receives message from upstream filter
		DCBuffer * inBuffer = read(firstStream, "filterAToCache", "cacheTofilterA", &mobiusXML, true);
		if(inBuffer == NULL) break;

		// MatLab Function
		matFunction4(arg1, arg2, arg3, arg4);

		// Deserializes arguments that will be sent
		DCBuffer outBuffer;
		stringDCBuffer(arg1, &outBuffer);
		stringDCBuffer(arg4, &outBuffer);

		// Sends output arguments to next filter
		write(&outBuffer, "output");
	}

	return 0;
}

int filterA::fini() {
	UtilTerminate();

	mclTerminateApplication();

	return 0;
}

provide1(filterA)
