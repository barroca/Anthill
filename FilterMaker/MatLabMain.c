#include "MatLabMain.h"

int getNumUserArgs(Layout *layout) {
	FilterSpec *filterSpec;
	MatLabDesc *matLatDesc;
	int i, j;
	int bigestUserArgIndex = 0;

	for(i = 0; i < layout->numFilters; i++) {
		filterSpec = layout->filters[i];
		matLatDesc = &(filterSpec->matLabDesc);

		// Check all input arguments
		if(matLatDesc->numInputs > 0) {
			for (j = 0; j < matLatDesc->numInputs; j++) {
				ArgInput argInput = matLatDesc->inputs[j];
				if(!strcmp(argInput.inputType, "userArg")) {
					if(argInput.userArgIndex > bigestUserArgIndex) {
						bigestUserArgIndex = argInput.userArgIndex;
					}
				}
			}
		}

		// Check all arguments that are also input and output
		if(matLatDesc->numInputOutputs > 0) {
			for (j = 0; j < matLatDesc->numInputOutputs; j++) {
				ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
				if(!strcmp(argInputOutput.inputType, "userArg")) {
					if(argInputOutput.userArgIndex > bigestUserArgIndex) {
						bigestUserArgIndex = argInputOutput.userArgIndex;
					}
				}
			}
		}
	}

	return bigestUserArgIndex;
}

void mainIncludeMaker(FILE *mainFile) {
	fprintf(mainFile, "#include <stdio.h>\n");
	fprintf(mainFile, "#include <stdlib.h>\n");
	fprintf(mainFile, "\n#include \"void.h\"\n");
	fprintf(mainFile, "\n#include \"api_util.h\"\n");
	fprintf(mainFile, "#include \"DCBuffer.h\"\n");
}

void mainGetUserParamsMaker(FILE *mainFile, int numUserArgs) {
	char *strFormat = "%s";
	char *intFormat = "%d";
	int i;

	fprintf(mainFile, "\nDCBuffer get_users_params(int argc, char *argv[]) {\n");
	fprintf(mainFile, "\tDCBuffer work_buffer;\n");
#ifdef USE_CACHE
	fprintf(mainFile, "\n\tif(argc != %d) {\n", (1+3+numUserArgs));
	fprintf(mainFile, "\t\tprintf(\"HOW TO USE: %s <first collection> <first stream type> <mobius hosts file> <your %d arguments...>\", argv[0]);", strFormat, numUserArgs);
#else
	fprintf(mainFile, "\n\tif(argc != %d) {\n", (1+numUserArgs));
	fprintf(mainFile, "\t\tprintf(\"You have specified %d arguments. You are using only %s \", argc);\n", numUserArgs, intFormat);
#endif
	fprintf(mainFile, "\t\texit(1);\n");
	fprintf(mainFile, "\t}\n\n");

#ifdef USE_CACHE
	// Declare and initialize the cache arguments
	fprintf(mainFile, "\t// Cache arguments\n");
	fprintf(mainFile, "\tchar *collectionName = argv[1];\n");
	fprintf(mainFile, "\tchar *streamType = argv[2];\n");
	fprintf(mainFile, "\tchar *mobiusHostFileName = argv[3];\n");
	fprintf(mainFile, "\twork_buffer.pack(\"sss\", collectionName, streamType, mobiusHostFileName);\n");
#endif

	// Declare and initialize the user arguments
	if(numUserArgs > 0) {
		fprintf(mainFile, "\n\t// User arguments\n");
		for (i = 0; i < numUserArgs; i++) {
			fprintf(mainFile, "\tchar *userArg%d = argv[%d];\n", (i+4), (i+4));
			fprintf(mainFile, "\twork_buffer.pack(\"s\", userArg%d);\n", (i+4));
		}
	}

	fprintf(mainFile, "\n\treturn work_buffer;\n");
	
	fprintf(mainFile, "}\n");
}

void mainMaker(FILE *mainFile, char *filterDir) {
	fprintf(mainFile, "int main (int argc, char *argv[]) {\n");
	fprintf(mainFile, "\tchar confFile[] = \"%s/conf1.xml\";\n", filterDir);
	fprintf(mainFile, "\tDCBuffer work = get_users_params(argc, argv);\n\n");
	fprintf(mainFile, "\tLayout *systemLayout = initDs(confFile, argc, argv);\n");
	fprintf(mainFile, "\tappendWork(systemLayout, (void *) work.getPtr(), work.getUsedSize());\n\n");
	fprintf(mainFile, "\tfinalizeDs(systemLayout);\n");
	fprintf(mainFile, "\treturn 0;\n");
	fprintf(mainFile, "}\n");
}

// Generates the Anthill Manager (Main) filter
int generateMatLabMain(char *filterDir, Layout *layout, char *confName) {
	
	// Warning: the main file name is always matlab_filters_main.cpp
	char *mainFileName = malloc(strlen(filterDir) + strlen("matlab_filters_main.cpp") + 1);
	strcpy(mainFileName, filterDir);
	strcat(mainFileName, "matlab_filters_main.cpp");

	FILE *mainFile = fopen(mainFileName, "w");
	if(!mainFile) {
		printf("Error (%s:%d). Could not open file %s.\n", __FILE__, __LINE__, mainFileName);
		exit(1);
	}

	int numUserArgs = getNumUserArgs(layout);

	mainIncludeMaker(mainFile);

	mainGetUserParamsMaker(mainFile, numUserArgs);

	mainMaker(mainFile, filterDir);

	fclose(mainFile);

	return 1;
}

