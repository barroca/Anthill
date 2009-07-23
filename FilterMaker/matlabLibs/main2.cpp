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

	int8 valor = -1441483123;
	buffer->pack("l", valor);

	printf("%d", valor);

	mxArray* teste = int64ToMxArray(&buffer);

	mlfTest(teste);

	mxArrayToDCBuffer(&buffer , teste);

	int8 num;
	buffer->unpack("l", &num);

	printf("%d\n", num);

	testTerminate();

	mclTerminateApplication();
}
