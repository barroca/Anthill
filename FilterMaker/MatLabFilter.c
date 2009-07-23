#include "MatLabFilter.h"

void includeMaker(char *filterType, FILE *filterFile, Layout *layout) {
	fprintf(filterFile, "#include <iostream.h>\n");
	fprintf(filterFile, "#include \"api_cpp.h\"\n");
#ifdef USE_CACHE
	fprintf(filterFile, "#include \"api_cpp_cache.h\"\n\n");
#endif

	int numLibs, i;
	char **matlabLibNames = getAllMatLabLibs(filterType, layout, &numLibs);
	char libName[MAX_LNAME_LENGTH];

	for(i = 0; i < numLibs; i++) {
		// Warning: assuming that the lib name has lib as prefix and .so as suffix
		removeLibAndSo(matlabLibNames[i], libName);
		fprintf(filterFile, "#include \"%s.h\"\n", libName);
	}

	if(!strcmp(filterType, "-ex")){
		//fprintf(filterFile, "#include \"converter.h\"\n\n");
		fprintf(filterFile, "\n#include <unistd.h>\n");
		fprintf(filterFile, "#include <stdlib.h>\n");
		fprintf(filterFile, "#include <stdio.h>\n");
		fprintf(filterFile, "#include <unistd.h>\n");
		fprintf(filterFile, "#include <sys/types.h>\n");
		fprintf(filterFile, "#include <sys/wait.h>\n");
		fprintf(filterFile, "#include <iostream>\n");
		fprintf(filterFile, "#include <stdio.h>\n");
	}

	fprintf(filterFile, "\n");
}

void classMaker(FILE *filterFile, char *filterName) {
	// Constructs the filter class. It's name is the same of the filter
#ifdef USE_CACHE
	fprintf(filterFile, "class %s : public DCFilterCache\n", filterName);
#else
	fprintf(filterFile, "class %s : public DCFilter\n", filterName);
#endif
	fprintf(filterFile, "{\n\npublic:\n");
	fprintf(filterFile, "\t%s(){}\n", filterName);
	fprintf(filterFile, "\t~%s(){}\n\n", filterName);
	fprintf(filterFile, "\tint init();\n");
	fprintf(filterFile, "\tint process();\n");
	fprintf(filterFile, "\tint fini();\n");
	fprintf(filterFile, "};\n");
}

void initMaker(char * filterType, FILE *filterFile, char *filterName, FilterSpec *filterSpec) {
	// Init Header
	fprintf(filterFile, "\nint %s::init() {\n", filterName);

	if(!strcmp(filterType, "-sl")) {
		// Calls MatLab initialize function
		fprintf(filterFile, "\tmclInitializeApplication(NULL,0);\n");

		// Calls library initialize function
		char *matLabLibName = filterSpec->matLabDesc.matlablibname;
		char newLibName[MAX_LNAME_LENGTH];
		removeLibAndSo(matLabLibName, newLibName);
		fprintf(filterFile, "\n\tif (!%sInitialize()){\n", newLibName);
		fprintf(filterFile, "\t\tfprintf(stderr,\"could not initialize the library properly\\n\");\n");
		fprintf(filterFile, "\t\treturn -1;\n");
		fprintf(filterFile, "\t}\n");
	}
		
	fprintf(filterFile, "\n\treturn 0;\n");
	
	fprintf(filterFile, "}\n"); 
}

int verifyOutputArg(MatLabDesc *matLatDesc) {
	int numOuts = matLatDesc->numOutputs + matLatDesc->numInputOutputs;
	int changeArg = 0, i, j;
	
	for (i = 1; i <= numOuts; i++) {
		// First look for the user argument in the output arguments...
		for(j = 0; j < matLatDesc->numOutputs; j++) {
			ArgOutput argOutput = matLatDesc->outputs[j];
			if(argOutput.mesgIndexOut == i && (!strcmp(argOutput.argType, "mxArray**") || !strcmp(argOutput.argType, "mxArray **"))) {
				changeArg = 1;
				break;
			}
		}
		// ... and then look for the user argument in the inputOutputs arguments
		for(j = 0; j < matLatDesc->numInputOutputs; j++) {
			ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
			if(argInputOutput.mesgIndexOut == i && (!strcmp(argInputOutput.argType, "mxArray**") || !strcmp(argInputOutput.argType, "mxArray **"))) {
				changeArg = 1;
				break;
			}
		}
	}

	return changeArg;
}

char * takeTheSlashesOut(char *fullName) {
	char * shortName = (char *) malloc(strlen(fullName)+1);
	int i, j;

	for(i = 0, j = 0; i < strlen(fullName); i++, j++) {
		if(fullName[i] == '/') {
			j = 0;
		}
		shortName[j] = fullName[i];
	}
	shortName[j] = '\0';

	return shortName;
}

void processMaker(char *filterType, FILE *filterFile, char *filterName, FilterSpec *filterSpec) {
	int i, j;
	int numUserArgs = 0, numMsgArgs = 0;
	int bigestUserArgIndex = 0;
	MatLabDesc *matLatDesc = &(filterSpec->matLabDesc);

	// Process Header
	fprintf(filterFile, "\nint %s::process() {\n", filterName);

	// Declare input arguments
	if(matLatDesc->numInputs > 0) {
		fprintf(filterFile, "\n\t// Input arguments\n");
		for (i = 0; i < matLatDesc->numInputs; i++) {
			ArgInput argInput = matLatDesc->inputs[i];
			if(!strcmp(filterType, "-ex")) {
				if(!strcmp(argInput.argType, "char*") || !strcmp(argInput.argType, "char *")
						|| !strcmp(argInput.argType, "char  *") || !strcmp(argInput.argType, "char\t*")) {
					strcpy(argInput.argType, "string");
				} else if(strcmp(argInput.argType, "string")){
					printf("ERROR: if you are using [-ex], your arguments must be \"char *\" or \"string\"\n");
					exit(1);
				}
			}
			if(!strcmp(argInput.inputType, "userArg")) {
				if(argInput.userArgIndex > bigestUserArgIndex) {
					bigestUserArgIndex = argInput.userArgIndex;
				}
				// User arguments declaration
				fprintf(filterFile, "\t%s arg%d; // user argument\n", argInput.argType, argInput.parameterOrder);
				numUserArgs++;
			}else if(!strcmp(argInput.inputType, "msg")){
				// Message arguments declaration
				fprintf(filterFile, "\t%s arg%d; // message argument\n", argInput.argType, argInput.parameterOrder);
				numMsgArgs++;
			}else {
				printf("Error (%s:%d). There is no input type %s.\n", __FILE__, __LINE__, argInput.inputType);
			}
		}
	}

	// Declare arguments that are also input and output
	if(matLatDesc->numInputOutputs > 0) {
		fprintf(filterFile, "\n\t// Arguments that are input and output\n");
		for (i = 0; i < matLatDesc->numInputOutputs; i++) {
			ArgInputOutput argInputOutput = matLatDesc->inputOutputs[i];
			if(!strcmp(filterType, "-ex")) {
				if(!strcmp(argInputOutput.argType, "char*") || !strcmp(argInputOutput.argType, "char *")
						|| !strcmp(argInputOutput.argType, "char  *") || !strcmp(argInputOutput.argType, "char\t*")) {
					strcpy(argInputOutput.argType, "string");
				} else if(strcmp(argInputOutput.argType, "string")){
					printf("ERROR: if you are using [-ex], your arguments must be \"char *\" or \"string\"\n");
					exit(1);
				}
			}
			if(!strcmp(argInputOutput.inputType, "userArg")) {
				if(argInputOutput.userArgIndex > bigestUserArgIndex) {
					bigestUserArgIndex = argInputOutput.userArgIndex;
				}
				// User arguments declaration
				fprintf(filterFile, "\t%s arg%d;\n", argInputOutput.argType, argInputOutput.parameterOrder);
				numUserArgs++;
			}else if(!strcmp(argInputOutput.inputType, "msg")){
				// Message arguments declaration
				fprintf(filterFile, "\t%s arg%d; // Message argument\n", argInputOutput.argType, argInputOutput.parameterOrder);
				numMsgArgs++;
			}else {
				printf("Error (%s:%d). There is no input type %s.\n", __FILE__, __LINE__, argInputOutput.inputType);
			}
		}
	}

	// Declare output arguments
	if(matLatDesc->numOutputs > 0) {
		fprintf(filterFile, "\n\t// Output arguments\n");
		for (i = 0; i < matLatDesc->numOutputs; i++) {
			ArgOutput argOutput = matLatDesc->outputs[i];
			if(!strcmp(filterType, "-ex")) {
				if(!strcmp(argOutput.argType, "char*") || !strcmp(argOutput.argType, "char *")
						|| !strcmp(argOutput.argType, "char  *") || !strcmp(argOutput.argType, "char\t*")) {
					strcpy(argOutput.argType, "string");
				} else if(strcmp(argOutput.argType, "string")){
					printf("ERROR: if you are using [-ex], your arguments must be \"char *\" or \"string\"\n");
					exit(1);
				}
			}
			if(!strcmp(filterType, "-sl")) {
				// Warning: we are changing the type that the user has specified
				if(!strcmp(argOutput.argType, "mxArray**") || !strcmp(argOutput.argType, "mxArray **")) {
					fprintf(filterFile, "\tmxArray *arg%d = NULL;\n", argOutput.parameterOrder);
				} else {
					fprintf(filterFile, "\t%s arg%d = NULL;\n", argOutput.argType, argOutput.parameterOrder);
				}
			}else {
				fprintf(filterFile, "\tchar *aux%d = (char *) malloc(50);\n", argOutput.parameterOrder);
				fprintf(filterFile, "\tsprintf(aux%d, \"/tmp/doc.out.%%d.%d.png\", getpid());\n", argOutput.parameterOrder, argOutput.parameterOrder);
				fprintf(filterFile, "\t%s arg%d = aux%d;\n", argOutput.argType, argOutput.parameterOrder, argOutput.parameterOrder);
			}
		}
	}

	// Declare work variables
	if(bigestUserArgIndex > 0) {
		fprintf(filterFile, "\n\t// User arguments\n");
		for(i = 1; i <= bigestUserArgIndex; i++) {
			// User arguments declaration.
			fprintf(filterFile, "\tstring userArg%d;\n", i);
		}
	}

	// Receives the program arguments
	fprintf(filterFile, "\n\t// Receives the program arguments\n");
	fprintf(filterFile, "\tDCBuffer* buffer = this->get_init_filter_broadcast();\n");

#ifdef USE_CACHE
	// Unpack the 3 arguments of the system: <first collection> <first stream type> <mobius hosts file>
	fprintf(filterFile, "\n\t// System arguments: <first collection> <first stream type> <mobius hosts file>\n");
	fprintf(filterFile, "\tstring firstCollection, firstStream, mobiusHostFiles;\n");
	fprintf(filterFile, "\tbuffer->unpack(\"sss\", &firstCollection, &firstStream, &mobiusHostFiles);\n");
#endif

	// If there is at least one user argument, gets it through the Anthill work,
	// and then unpack and deserealize them.
	if(numUserArgs > 0) {
		fprintf(filterFile, "\n\t// Unpack user arguments\n");	
		// Warning: we are assuming that the index of the user arguments goes from 1 
		//          to numUserArgs. Here, they are represented by i.
		for (i = 1; i <= bigestUserArgIndex; i++) {
			fprintf(filterFile, "\tbuffer->unpack(\"s\", &userArg%d);\n", i);
			// First look for the user argument in the input arguments...
			for(j = 0; j < matLatDesc->numInputs; j++) {
				ArgInput argInput = matLatDesc->inputs[j];
				if(argInput.userArgIndex == i) {
					fprintf(filterFile, "\targ%d = %s(userArg%d);\n", argInput.parameterOrder, argInput.deserializeFunctionName, argInput.userArgIndex);
					break;
				}
			}
			// ... and then look for the user argument in the inputOutputs arguments
			for(j = 0; j < matLatDesc->numInputOutputs; j++) {
				ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
				if(argInputOutput.userArgIndex == i) {
					fprintf(filterFile, "\targ%d = %s(userArg%d);\n", argInputOutput.parameterOrder, argInputOutput.deserializeFunctionName, argInputOutput.userArgIndex);
					break;
				}
			}
		}
	}

#ifdef USE_CACHE
	fprintf(filterFile, "\n\t// Choose the correct cache transparent copy to communicate.\n");
	fprintf(filterFile, "\tcreateLinkToCache(dcmpi_get_hostname());\n");
#endif

	fprintf(filterFile, "\n");

	// Receive message, if necessary, and then deserialize the arguments
	// TODO: we are assuming that there will be only one input port per filter
#ifdef USE_CACHE
	if(numMsgArgs > 0 || filterSpec->matLabDesc.isFirstFilter) {
		fprintf(filterFile, "\n\twhile (1) {\n");
		fprintf(filterFile, "\t\tstring mobiusXML;\n");
	
		if(filterSpec->matLabDesc.isFirstFilter) {
			fprintf(filterFile, "\n\t\t// Receives message from cache\n");
			fprintf(filterFile, "\t\tDCBuffer * inBuffer = read(firstStream, \"%sToCache\", \"cacheTo%s\", &mobiusXML, true);\n", filterName, filterName);
		}else {
			if(filterSpec->inputs[0]->toPortName != NULL) {
				fprintf(filterFile, "\n\t\t// Receives message from upstream filter\n");
				fprintf(filterFile, "\t\tDCBuffer * inBuffer = read(\"%s\", \"%sToCache\", \"cacheTo%s\", &mobiusXML, false);\n", filterSpec->inputs[0]->toPortName, filterName, filterName);
			} else {
				printf("ERROR: Trying to access an input port of filter %s. This filter has no input port.", filterName);
				exit(1);
			}
		}
		fprintf(filterFile, "\t\tif(inBuffer == NULL) break;\n");
#else
	if(numMsgArgs > 0) {
		fprintf(filterFile, "\n\t\t// Receives message from upstream filter\n");
		fprintf(filterFile, "\t\tDCBuffer * inBuffer = read(\"%s\");\n", filterSpec->inputs[0]->toPortName);
#endif

		// Call all deserialize functions in the rigth order
		// Warning: we are assuming that the index of the message arguments goes from 1 
		//          to numMsgArgs. Here, they are represented by i.
		for (i = 1; i <= numMsgArgs; i++) {
			// First look for the user argument in the input arguments...
			for(j = 0; j < matLatDesc->numInputs; j++) {
				ArgInput argInput = matLatDesc->inputs[j];
				if(argInput.mesgIndexIn == i) {
					fprintf(filterFile, "\t\targ%d = %s(inBuffer);\n", argInput.parameterOrder, argInput.deserializeFunctionName);
					break;
				}
			}
			// ... and then look for the user argument in the inputOutputs arguments
			for(j = 0; j < matLatDesc->numInputOutputs; j++) {
				ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
				if(argInputOutput.mesgIndexIn == i) {
					fprintf(filterFile, "\t\targ%d = %s(inBuffer);\n", argInputOutput.parameterOrder, argInputOutput.deserializeFunctionName);
					break;
				}
			}
		}
#ifdef USE_CACHE
	} else {
		printf("ERROR: You are using cache, but you haven't specified an input port for filter %s\n", filterName);
		exit(1);
	}
#else
	}
#endif

	// Call MatLab function
	// Warning: it is assumed that the argumets have arg as prefix and 
	//          then it's order as sufix
	fprintf(filterFile, "\n\t\t// MatLab Function\n");
	
	int numArgs = matLatDesc->numOutputs + matLatDesc->numInputs + matLatDesc->numInputOutputs;
	int numOuts = matLatDesc->numOutputs + matLatDesc->numInputOutputs;

	if(!strcmp(filterType, "-sl")) {
		// INSTRUMENTATION
		fprintf(filterFile, "#ifdef VOID_INST\n");
		fprintf(filterFile, "\t\tdsInstEnterState(PROCESSING_FUNCTION);\n");
		fprintf(filterFile, "#endif\n");
		
		fprintf(filterFile, "\t\t%s(", matLatDesc->functionName);
		// If there is at least one output argument (return value), we have to pass
		// it's quantity
		if(numOuts > 0) {
			fprintf(filterFile, "%d, ", numOuts);
		}
		for(i = 1; i <= numArgs; i++) {
			int changeArg = 0;
			if(i != 1) {
				fprintf(filterFile, ", ");
			}
			if(numOuts > 0 && i <= numOuts) {
				// Find out if there is any need to put '&' before the argument
				changeArg = verifyOutputArg(matLatDesc);
			}

			if(changeArg == 1) {
				fprintf(filterFile, "&arg%d", i);
			}else {
				fprintf(filterFile, "arg%d", i);
			}
		}
		fprintf(filterFile, ");\n");
	}else {
		fprintf(filterFile, "\t\tint ret, pid, status;\n");
		fprintf(filterFile, "\t\tif ((pid = fork()) < 0) {\n");
		fprintf(filterFile, "\t\t\tperror(\"fork\");\n");
		fprintf(filterFile, "\t\t\texit(1);\n");
		fprintf(filterFile, "\t\t}\n");

		char *executableName = takeTheSlashesOut(matLatDesc->functionName);
		
		fprintf(filterFile, "\t\tif (pid == 0) {\n");
		fprintf(filterFile, "\t\tchar *cmd[] = { \"./%s\"", executableName);

		for(i = 1; i <= numArgs; i++) {
			fprintf(filterFile, ", (char *) arg%d.c_str()", i);
		}

		fprintf(filterFile, "};\n");
		fprintf(filterFile, "\t\t\tret = execv (\"%s\", cmd);\n", matLatDesc->functionName);
		fprintf(filterFile, "\t\t\tperror(\"Not done\\n\");\n");
		fprintf(filterFile, "\t\t\texit(1);\n");
		fprintf(filterFile, "\t\t}\n");

		// INSTRUMENTATION
		fprintf(filterFile, "#ifdef VOID_INST\n");
		fprintf(filterFile, "\t\tdsInstEnterState(PROCESSING_FUNCTION);\n");
		fprintf(filterFile, "#endif\n");
		
		fprintf(filterFile, "\n\t\tif (waitpid(pid, &status, 0) == -1) {\n");
		fprintf(filterFile, "\t\t\tperror(\"wait interrupted\");\n");
		fprintf(filterFile, "\t\t\texit(1);\n");
		fprintf(filterFile, "\t\t}\n");
		fprintf(filterFile, "\t\tif (WIFEXITED(status)) {\n");
		fprintf(filterFile, "\t\t\tcout << \"Child process completed. Status is \" <<  WEXITSTATUS(status) << endl;\n");
		fprintf(filterFile, "\t\t}else {\n");
		fprintf(filterFile, "\t\t\tperror(\"wait failed\");\n");
		fprintf(filterFile, "\t\t\texit(1);\n");
		fprintf(filterFile, "\t\t}\n");
	}

	//INSTRUMENTATION
	fprintf(filterFile, "#ifdef VOID_INST\n");
	fprintf(filterFile, "\t\tdsInstLeaveState();\n");
	fprintf(filterFile, "#endif\n");


	// Call all serialize functions
	int numOutArgs = matLatDesc->numOutputs + matLatDesc->numInputOutputs;
	if(numOutArgs > 0) {
		fprintf(filterFile, "\n\t\t// Deserializes arguments that will be sent\n");
		fprintf(filterFile, "\t\tDCBuffer *outBuffer = new DCBuffer();\n");

		// Warning: we are assuming that the index of the message arguments goes from 1 
		//          to numOutArgs. Here, they are represented by i.
		for (i = 1; i <= numOutArgs; i++) {
			// First look for the user argument in the output arguments...
			for(j = 0; j < matLatDesc->numOutputs; j++) {
				ArgOutput argOutput = matLatDesc->outputs[j];
				if(argOutput.mesgIndexOut == i) {
					fprintf(filterFile, "\t\t%s(outBuffer, arg%d);\n", argOutput.serializeFunctionName, argOutput.parameterOrder);
					break;
				}
			}
			// ... and then look for the user argument in the inputOutputs arguments
			for(j = 0; j < matLatDesc->numInputOutputs; j++) {
				ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
				if(argInputOutput.mesgIndexOut == i) {
					fprintf(filterFile, "\t\t%s(outBuffer, arg%d);\n", argInputOutput.serializeFunctionName, argInputOutput.parameterOrder);
					break;
				}
			}
		}
		if(matLatDesc->numOutputs > 0){	
			// TODO: we are assuming that there will be only one output port per filter
			fprintf(filterFile, "\n\t\t// Sends output arguments to next filter\n");
#ifdef USE_CACHE
			fprintf(filterFile, "\t\twrite(outBuffer, \"%s\", \"%sToCache\", \"cacheTo%s\", mobiusXML);\n", filterSpec->outputs[0]->fromPortName, filterName, filterName);
#else
			fprintf(filterFile, "\t\twrite(outBuffer, \"%s\");\n", filterSpec->outputs[0]->fromPortName);
#endif
		}		
	}

#ifdef USE_CACHE
	if(numMsgArgs > 0 || filterSpec->matLabDesc.isFirstFilter) {
#else
	if(numMsgArgs > 0) {
#endif
		fprintf(filterFile, "\n\t\t// Consume input buffer.\n");
		fprintf(filterFile, "\t\tinBuffer->consume();\n");
	}

	if(numOutArgs > 0) {
		fprintf(filterFile, "\n\t\toutBuffer->consume();\n");
	}

	// Destroy all mxArrays that were created using the deserialise functions
	// Warning: we are assuming that the index of the message arguments goes from 1 
	//          to numMsgArgs. Here, they are represented by i.
	if(!strcmp(filterType, "-sl")) {
		if(numMsgArgs > 0) {
			fprintf(filterFile, "\n\t\t// Destroy the input arguments received by message.\n");
			for (i = 1; i <= numMsgArgs; i++) {
				// First look for the user argument in the input arguments...
				for(j = 0; j < matLatDesc->numInputs; j++) {
					ArgInput argInput = matLatDesc->inputs[j];
					if(argInput.mesgIndexIn == i) {
						fprintf(filterFile, "\t\tmxDestroyArray(arg%d); arg%d = NULL;\n", argInput.parameterOrder, argInput.parameterOrder);
						break;
					}
				}
				// ... and then look for the user argument in the inputOutputs arguments
				for(j = 0; j < matLatDesc->numInputOutputs; j++) {
					ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
					if(argInputOutput.mesgIndexIn == i) {
						fprintf(filterFile, "\t\tmxDestroyArray(arg%d); arg%d = NULL;\n", argInputOutput.parameterOrder, argInputOutput.parameterOrder);
						break;
					}
				}
			}
		}
	}
#ifdef USE_CACHE
	// End of while(1)
	fprintf(filterFile, "\t}\n");

	// Destroy all output arguments
	if(!strcmp(filterType, "-sl")) {
		if(numOutArgs > 0) {
			fprintf(filterFile, "\n\t// Destroy all output arguments\n");

			// Warning: we are assuming that the index of the message arguments goes from 1 
			//          to numOutArgs. Here, they are represented by i.
			for (i = 1; i <= numOutArgs; i++) {
				// First look for the user argument in the output arguments...
				for(j = 0; j < matLatDesc->numOutputs; j++) {
					ArgOutput argOutput = matLatDesc->outputs[j];
					if(argOutput.mesgIndexOut == i) {
						fprintf(filterFile, "\tmxDestroyArray(arg%d); arg%d = NULL;\n", argOutput.parameterOrder, argOutput.parameterOrder);
						break;
					}
				}
				// ... and then look for the user argument in the inputOutputs arguments
				for(j = 0; j < matLatDesc->numInputOutputs; j++) {
					ArgInputOutput argInputOutput = matLatDesc->inputOutputs[j];
					if(argInputOutput.mesgIndexOut == i) {
						fprintf(filterFile, "\tmxDestroyArray(*arg%d); arg%d = NULL;\n", argInputOutput.parameterOrder, argInputOutput.parameterOrder);
						break;
					}
				}
			}
	}
	}
#endif

	fprintf(filterFile, "\n\treturn 0;\n");
	fprintf(filterFile, "}\n"); 
}

void processWritterName(char *filterType, FILE *filterFile, char *filterName, FilterSpec *filterSpec) {

	// Process Header
	fprintf(filterFile, "\nint %s::process() {\n", filterName);
	
	fprintf(filterFile, "\t// Receives the program arguments\n");
	fprintf(filterFile, "\tDCBuffer* buffer = this->get_init_filter_broadcast();\n");
	
	fprintf(filterFile, "\n\t// System arguments: <first collection> <first stream type> <mobius hosts file>\n");
	fprintf(filterFile, "\tstring firstCollection, firstStream, mobiusHostFiles;\n");
	fprintf(filterFile, "\tbuffer->unpack(\"sss\", &firstCollection, &firstStream, &mobiusHostFiles);\n");
	
	fprintf(filterFile, "\n\t// Choose the correct cache transparent copy to communicate.\n");
	fprintf(filterFile, "\tcreateLinkToCache(dcmpi_get_hostname());\n");
	
	fprintf(filterFile, "\n\twhile (1) {\n");
	fprintf(filterFile, "\t\tstring mobiusXML;\n");
	if(filterSpec->inputs[0]->toPortName != NULL) {
		fprintf(filterFile, "\n\t\t// Receives message from upstream filter\n");
		fprintf(filterFile, "\t\tDCBuffer * inBuffer = read(\"%s\", \"%sToCache\", \"cacheTo%s\", &mobiusXML, false);\n", filterSpec->inputs[0]->toPortName, filterName, filterName);
	} else {
		printf("ERROR1: Trying to access an input port of filter %s. This filter has no input port.", filterName);
		exit(1);
	}
	fprintf(filterFile, "\t\tif(inBuffer == NULL) break;\n");
	
	fprintf(filterFile, "\n\t\tinBuffer->consume();\n");
	fprintf(filterFile, "\t\t}\n");
	fprintf(filterFile, "\n\treturn 0;\n");
	fprintf(filterFile, "\n}\n");
}

void finalizeMaker(char *filterType, FILE *filterFile, char *filterName, FilterSpec *filterSpec) {
	// Init Header
	fprintf(filterFile, "\nint %s::fini() {\n", filterName);

	if(!strcmp(filterType, "-sl")) {
		// Calls library termination function
		char *matLabLibName = filterSpec->matLabDesc.matlablibname;
		char newLibName[MAX_LNAME_LENGTH];
		removeLibAndSo(matLabLibName, newLibName);
		fprintf(filterFile, "\t%sTerminate();\n", newLibName);

		// Calls MatLab initialize function
		fprintf(filterFile, "\n\tmclTerminateApplication();\n");
	}

	fprintf(filterFile, "\n\treturn 0;\n");

	fprintf(filterFile, "}\n"); 
}

void provideMaker(FILE *filterFile, char *filterName) {
	// Init Header
	fprintf(filterFile, "\nprovide1(%s)\n", filterName);
}

// Generates one MatLab filter for every MatLab function described by the user
int generateMatLabFilters(char *filterType, char *filterDir, Layout *layout) {
	int i;
	
	for(i = 0; i < layout->numFilters; i++) {
		FilterSpec *filterSpec = layout->filters[i];
		char *filterName = filterSpec->name;

		// Warning: filter file name is the same as filterName.cpp
		char *filterFileName = (char *) malloc(strlen(filterDir) + strlen(filterName) + 5);
		strcpy(filterFileName, filterDir);
		strcat(filterFileName, filterName);
		strcat(filterFileName, ".cpp");

		FILE *filterFile = fopen(filterFileName, "w");
		if(!filterFile) {
			printf("Error (%s:%d). Could not open file %s.\n", __FILE__, __LINE__, filterFileName);
			exit(1);
		}

		includeMaker(filterType, filterFile, layout);
		
		classMaker(filterFile, filterName);

		initMaker(filterType, filterFile, filterName, filterSpec);

		if(strcmp(filterName, "writter")) {
			processMaker(filterType, filterFile, filterName, filterSpec);
		}else {
			processWritterName(filterType, filterFile, filterName, filterSpec);
		}

		finalizeMaker(filterType, filterFile, filterName, filterSpec);

		provideMaker(filterFile, filterName);
		
		fclose(filterFile);
		free(filterFileName);
	}

	return 1;
}
