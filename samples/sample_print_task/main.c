#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "Manager.h"

typedef int Work;


int main (int argc, char *argv[]) {
	char confFile[] = "./conf.xml";
	int num_appends = 3;
	int i;

	// work que é enfiado aos filtros
	Work work = 0;

	// inicia o VOID
	Layout *systemLayout = initDs(confFile, argc, argv);

	for(i = 0 ; i < num_appends; i++){
		appendWork(systemLayout, &work, sizeof(Work));
		work++;
	}
	// finaliza o VOID
	finalizeDs(systemLayout);
	return 0;
}

