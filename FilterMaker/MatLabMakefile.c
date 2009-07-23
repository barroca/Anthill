#include "MatLabMakefile.h"

void makefileConfMaker(char *filterType, FILE *makeFile, Layout *layout) {
	// Warning: we are assuming that the user has ANTHILL_ROOT in his 
	//          enviromental variables and inside of this directory there are
	//          the api_c++ and void_3.1 directories
	fprintf(makeFile, "AH_API_CPP = ${ANTHILL_ROOT}/api_c++\n");
	fprintf(makeFile, "AH_SRC = ${ANTHILL_ROOT}\n");

	// Warning: we are assuming that the user has MATLAB_ROOT in his 
	//          enviromental variables and inside of this directory there are
	//          the lib, libinclude and matlabinclude directories
	fprintf(makeFile, "\nMT_LIB = ${TOOLKIT_ROOT}/libs/lib\n");
	fprintf(makeFile, "MT_LIBINLUCDE = ${TOOLKIT_ROOT}/libs/include\n");
	fprintf(makeFile, "MT_INCLUDE = ${TOOLKIT_ROOT}/libs/matlabinclude\n");

	fprintf(makeFile, "\nCACHE_ROOT = ${TOOLKIT_ROOT}/cache\n");

	fprintf(makeFile, "\nCC = g++ -g -Wall\n");
	// Warning: we are assuming that the user has CACHE_ROOT in his enviromental variables
	fprintf(makeFile, "CFLAGS = -Wno-deprecated -I${AH_API_CPP} -I${AH_SRC} -I${AH_SRC}/FilterDev -I${PVM_ROOT}/include -I${MT_LIBINLUCDE} -I${MT_INCLUDE}");
#ifdef USE_CACHE
	fprintf(makeFile, " -I${CACHE_ROOT} -DUSE_CACHE -DBMI_FT\n");
#else
	fprintf(makeFile, "\n");
#endif
	fprintf(makeFile, "CLIBS = -L${AH_API_CPP} -ldscpp -L${AH_SRC} -lds -lexpat -L. -L${MT_LIB}");

	int numLibs, i;
	char ** matLabLibs = getAllMatLabLibs(filterType, layout, &numLibs);
	char libName[MAX_LNAME_LENGTH];

	for(i = 0; i < numLibs; i++) {
		// Warning: assuming that the lib name has lib as prefix and .so as suffix
		removeLibAndSo(matLabLibs[i], libName);
		fprintf(makeFile, " -l%s", libName);
	}
	
	fprintf(makeFile, "\n");
}

char **getFilterNames(Layout *layout, int *numFilters) {
	char **filterNames = (char **) malloc(MAXFILTERS);
	int i;

	for(i = 0; i < layout->numFilters; i++) {
		FilterSpec *filterSpec = layout->filters[i];
		filterNames[i] = filterSpec->name;
	}

	*numFilters = layout->numFilters;

	return filterNames;
}


void makefileBodyMaker(FILE *makeFile, Layout *layout) {
	int numFilters, i;
	char **filterNames = getFilterNames(layout, &numFilters);

	// all
	fprintf(makeFile, "\nall: main");
	for(i = 0; i < numFilters; i++) {
		fprintf(makeFile, " %s.so", filterNames[i]);
	}
	fprintf(makeFile, "\n");

	// filter libs
	for(i = 0; i < numFilters; i++) {
		fprintf(makeFile, "\n%s.so: %s.cpp\n", filterNames[i], filterNames[i]);
		fprintf(makeFile, "\t${CC} ${CFLAGS} ${CLIBS} -fPIC -shared -o %s.so %s.cpp\n", filterNames[i], filterNames[i]);
	}

	// main
	fprintf(makeFile, "\nmain: matlab_filters_main.cpp\n");
	fprintf(makeFile, "\tcp ${AH_API_CPP}/DCBuffer.o .\n");
	fprintf(makeFile, "\tcp ${AH_API_CPP}/api_util.o .\n");
	fprintf(makeFile, "\t${CC} ${CFLAGS} -L${AH_SRC} -lds DCBuffer.o api_util.o matlab_filters_main.cpp -o main\n");

	// clean
	fprintf(makeFile, "\nclean:\n");
	fprintf(makeFile, "\trm -f *.o main");
	for(i = 0; i < numFilters; i++) {
		fprintf(makeFile, " %s.so", filterNames[i]);
	}
}

// Generates the Makefile to compile filters
int generateMatLabMakefile(char *filterType, char *filterDir, Layout *layout) {
	
	// Warning: the Makefile name is Makefile1 not to overwrite the current one
	char *makeFileName = malloc(strlen(filterDir) + strlen("Makefile") + 1);
	strcpy(makeFileName, filterDir);
	strcat(makeFileName, "Makefile");

	FILE *makeFile = fopen(makeFileName, "w");
	if(!makeFile) {
		printf("Error (%s:%d). Could not open file %s.\n", __FILE__, __LINE__, makeFileName);
		exit(1);
	}

	makefileConfMaker(filterType, makeFile, layout);

	makefileBodyMaker(makeFile, layout);

	fclose(makeFile);

	return 1;
}
