#include <stdio.h>
#include "deserialize.h"
#include "serialize.h"
#include "test.h"
#include "matrix.h"

int main(int argc,char **argv){
	DCBuffer *buffer = new DCBuffer();
	
	mclInitializeApplication(NULL,0);

	if (!testInitialize()){
		fprintf(stderr,"could not initialize the library properly\n");
		return -1;
	}

	int1 valores[] = {false, true, true, true, false, false, true, false, false};
	
	mxArray *msg = mxCreateLogicalMatrix(3, 3);
	memcpy(mxGetData(msg), valores, 9*sizeof(int1));

	mlfTest(msg);

	mxArrayToDCBuffer(buffer , msg);

	mxArray* teste = boolMatrixToMxArray(buffer);

	mlfTest(teste);

	testTerminate();

	mclTerminateApplication();
}
