#include <string.h>
#include <stdio.h>

#include "structs.h"
#include "parser.h"
#include "Layout.h"

#include "MatLabFilter.h"
#include "MatLabMain.h"
#include "MatLabMakefile.h"
#include "ConfMaker.h"

int main(int argc, char *argv[]) {
	Layout *layout = createLayout();
	char *filterDir, *confName, *filterType;

	if(argc != 4) {
		printf("Usage: %s [-ex|-sl] <conf name> <filters dir>\n", argv[0]);
		exit(1);
	}

	filterType = argv[1];

	if(strcmp(filterType, "-ex") && strcmp(filterType, "-sb")) {
		printf("Usage:: %s [-ex|-sl] <conf name> <filters dir>\n", argv[0]);
		exit(1);
	}
	
	confName = argv[2];
	filterDir = malloc(strlen(argv[3]) + 2);
	strcpy(filterDir, argv[3]);
	strcat(filterDir, "/\0");

	int readConfigStatus = readConfig(confName, layout);
	if(readConfigStatus == -1) {
		printf("Some errors occurred reading the configuration file %s.\n", confName);
		exit(1);
	}

	generateMatLabFilters(filterType, filterDir, layout);
	generateMatLabMain(filterDir, layout, confName);
	generateMatLabMakefile(filterType, filterDir, layout);
	generateConfXML(filterDir, layout);

	return 0;
}
