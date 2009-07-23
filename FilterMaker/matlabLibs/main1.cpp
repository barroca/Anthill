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

	int8 valores[5] = {-3331, -4444244, 2223, 5555111, 224234324234};
	buffer->pack("i", 5);
	buffer->Append(valores, 5);

	mxArray* teste = int64ArrayToMxArray(&buffer);

	mlfTest(teste);

	mxArrayToDCBuffer(&buffer , teste);

	int num;
	buffer->unpack("i", &num);

	int8 valores2[5];
	buffer->Extract(valores2, num);

	int i;
	printf("Num:%d", num);
	for(i = 0; i < num; i++) {
		printf(" %Li", valores2[i]);
	}
	printf("\n");

	testTerminate();

	mclTerminateApplication();
}
