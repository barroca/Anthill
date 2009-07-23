#include <iostream.h>
#include "api_cpp.h"
#include "api_cpp_cache.h"

#include "Util.h"

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

	if (!UtilInitialize()){
		fprintf(stderr,"could not initialize the library properly\n");
		return -1;
	}

	return 0;
}

int filterB::process() {

	// Input arguments
	string arg1; // message argument

	// Receives the program arguments
	DCBuffer* buffer = this->get_init_filter_broadcast();

	// System arguments: <first collection> <first stream type> <mobius hosts file>
	string firstCollection, firstStream, mobiusHostFiles;
	buffer->unpack("sss", &firstCollection, &firstStream, &mobiusHostFiles);

	// Choose the correct cache transparent copy to communicate.
	createLinkToCache(dcmpi_get_hostname());


	while (1) {
		string mobiusXML;

		// Receives message from upstream filter
		DCBuffer * inBuffer = read(firstStream, "filterBToCache", "cacheTofilterB", &mobiusXML, true);
		if(inBuffer == NULL) break;

		// Receives message from upstream filter
		DCBuffer * inBuffer = read("input");
		arg1 = DCBufferString(inBuffer);

		// MatLab Function
		matFunction1(arg1);
	}

	return 0;
}

int filterB::fini() {
	UtilTerminate();

	mclTerminateApplication();

	return 0;
}

provide1(filterB)
