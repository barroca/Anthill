#include "MatLabUtil.h"

int findLib(char *newLib, char **matLabLibs, int numLibs) {
	int i;
	
	for(i = 0; i < numLibs; i++) {
		if(!strcmp(newLib, matLabLibs[i])) {
			return 1;
		}
	}

	return 0;
}

char **getAllMatLabLibs(char *filterType, Layout *layout, int *numLibs) {
	// Maxinum of libs = ((matlab lib * MAXFILTERS) * 
	//                   ((serialize lib + deserialize lib) * MAX_MATLAB_INPUTOUTPUS) +
	//                   (deserialize lib * MAX_MATLAB_INPUTS) +
	//                   (serialize lib * MAX_MATLAB_OUTPUS))
	int maxLibs = MAXFILTERS * (2*MAX_MATLAB_INPUTOUTPUS + MAX_MATLAB_INPUTS + MAX_MATLAB_OUTPUTS);
	char **matLabLibs = (char **) malloc(maxLibs);
	FilterSpec *filterSpec;
	MatLabDesc *matLabDesc;
	int i, j, numLibsCount = 0;

	for(i = 0; i < layout->numFilters; i++) {
		filterSpec = layout->filters[i];
		matLabDesc = &(filterSpec->matLabDesc);
		if(!strcmp(filterType, "-sl")) {
			if(!findLib(matLabDesc->matlablibname, matLabLibs, numLibsCount)) {
				matLabLibs[numLibsCount++] = matLabDesc->matlablibname;	
			}
		}
		for(j = 0; j < matLabDesc->numOutputs; j++) {
			ArgOutput *output = &(matLabDesc->outputs[j]);
			if(!findLib(output->serializeLibName, matLabLibs, numLibsCount)) {
				matLabLibs[numLibsCount++] = output->serializeLibName;
			}
		}
		for(j = 0; j < matLabDesc->numInputs; j++) {
			ArgInput *input = &(matLabDesc->inputs[j]);
			if(!findLib(input->deserializeLibName, matLabLibs, numLibsCount)) {
				matLabLibs[numLibsCount++] = input->deserializeLibName;
			}
		}
		for(j = 0; j < matLabDesc->numInputOutputs; j++) {
			ArgInputOutput *inputOutput = &(matLabDesc->inputOutputs[j]);
			if(!findLib(inputOutput->serializeLibName, matLabLibs, numLibsCount)) {
				matLabLibs[numLibsCount++] = inputOutput->serializeLibName;
			}
			if(!findLib(inputOutput->deserializeLibName, matLabLibs, numLibsCount)) {
				matLabLibs[numLibsCount++] = inputOutput->deserializeLibName;
			}
		}
	}

	*numLibs = numLibsCount;
	
	return matLabLibs;
}

void removeLibAndSo(char *libName, char *newName) {
	int numCaracters = strlen(libName);
	int numUsedCaracters;

	// Warning: assuming that the lib name has lib as prefix and .so as suffix
	if(numCaracters > 6 && !strncmp(libName, "lib", 3) 
		 && !strncmp(&(libName[numCaracters-3]), ".so", 3)) {
		// Remove lib and .so of the lib name
		numUsedCaracters = strlen(libName) - 3 /*lib*/ - 3 /*.so*/;
		strncpy(newName, &(libName[3]), numUsedCaracters);
		newName[numUsedCaracters] = '\0';
	}else {
		printf("FilterMaker ERROR: All libraries names must have the prefix 'lib' and suffix '.so'.\n");
		printf("FilterMaker aborted!!!!\n");
		exit(1);
	}
}
