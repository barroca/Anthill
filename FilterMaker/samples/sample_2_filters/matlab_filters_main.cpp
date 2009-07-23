#include <stdio.h>
#include <stdlib.h>

#include "void.h"

#include "api_util.h"
#include "DCBuffer.h"

DCBuffer get_users_params(int argc, char *argv[]) {
	DCBuffer work_buffer;

	if(argc != 7) {
		printf("HOW TO USE: %s <first collection> <first stream type> <mobius hosts file> <your 3 arguments...>", argv[0]);		exit(1);
	}

	// Cache arguments
	char *collectionName = argv[1];
	char *streamType = argv[2];
	char *mobiusHostFileName = argv[3];
	work_buffer.pack("sss", collectionName, streamType, mobiusHostFileName);

	// User arguments
	char *userArg4 = argv[4];
	work_buffer.pack("s", userArg4);
	char *userArg5 = argv[5];
	work_buffer.pack("s", userArg5);
	char *userArg6 = argv[6];
	work_buffer.pack("s", userArg6);

	return work_buffer;
}
int main (int argc, char *argv[]) {
	char confFile[] = "conf.xml";
	DCBuffer work = get_users_params(argc, argv);

	Layout *systemLayout = initDs(confFile, argc, argv);
	appendWork(systemLayout, (void *) work.getPtr(), work.getUsedSize());

	finalizeDs(systemLayout);
	return 0;
}
