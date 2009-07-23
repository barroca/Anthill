#include "MatLabDesc.h"
///////////////get Functions
char *getMatLabLibName(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getMatLabLibName: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return NULL;
	}
	return desc->matlablibname;

}

char *getMatLabFunctionName(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getMatLabFunctionName: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return NULL;
	}
	return desc->functionName;
}

int getNumArgOutputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getNumArgOutputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return -1;
	}
	return desc->numOutputs;
}

int getNumArgInputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getNumArgInputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return -1;
	}
	return desc->numInputs;
}

int getNumArgInputOutputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getNumArgInputOutputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return -1;
	}
	return desc->numInputOutputs;
}

ArgOutput *getArgOutputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getArgOutputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return NULL;
	}
	return desc->outputs;
}

ArgInput *getArgInputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getArgInputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return NULL;
	}
	return desc->inputs;
}
ArgInputOutput *getArgInputOutputs(MatLabDesc *desc){
	if(desc == NULL){
		printf("Warnning: getArgInputOutputs: %s:%d MatLabDesc == NULL\n", __FILE__, __LINE__);
		return NULL;
	}
	return desc->inputOutputs;
}
////////////// set Functions
int setMatLabLibName(MatLabDesc *desc, char *libName){

	int libNameSize = strlen(libName);

	// case the name be bigger than our name limit
	if((libNameSize) > MAX_LNAME_LENGTH){
		printf("Warning: MatLabLib name out of bounds\n");
		memcpy(desc->matlablibname, libName, MAX_LNAME_LENGTH);
		desc->matlablibname[MAX_LNAME_LENGTH] = '\0';
		return 0;
	}else{
		memcpy(desc->matlablibname, libName, libNameSize +1); // +1 to copy \0
		return 1;
	}
}

int setFirstFilter(MatLabDesc *desc, int firstFilter){
	desc->isFirstFilter = firstFilter;
  return 1;
}

int setMatLabFunctionName(MatLabDesc *desc, char *funcName){

	int funcNameSize = strlen(funcName);

	// case the name be bigger than our name limit
	if((funcNameSize) > MAX_FUNCNAME_LENGTH){
		printf("Warning: MatLabFunction name out of bounds\n");
		memcpy(desc->functionName, funcName, MAX_FUNCNAME_LENGTH);
		desc->functionName[MAX_FUNCNAME_LENGTH] = '\0';
		return 0;
	}else{
		memcpy(desc->functionName, funcName, funcNameSize +1); // +1 to copy \0
		return 1;
	}
}

void setNumArgs(MatLabDesc *desc, int numInputs, int numInputOutputs, int numOutputs){
	if (numInputs < 0 || numInputOutputs < 0 || numOutputs < 0 ){
		printf("Warning: numInputs or numInputOutputs or numOutputs smaler than zero\n");
	}
	desc->numInputs = numInputs;
	desc->numInputOutputs = numInputOutputs;
	desc->numOutputs = numOutputs;
}

int addArgInput(MatLabDesc *desc, char *argType, char *deserializeFunctionName,
                char *deserializeLibName, int userArgIndex, char *inputType,
		int parameterOrder,int mesgIndexIn){
	int argTypeSize = strlen(argType);
	int deserializeFunctionNameSize = strlen(deserializeFunctionName);
	int deserializeLibNameSize = strlen(deserializeLibName);
	int inputTypeSize = strlen(inputType);
	int returnValue = 1;

	if(desc->numInputsAdded + 1 > MAX_MATLAB_INPUTS){
		printf("Error: could not add ArgInput numInputs > MAX_MATLAB_INPUTS\n");
		exit(0);
	}
	if(desc->numInputsAdded + 1 > desc->numInputs){
		printf("Error: trying to add more ArgInputs then defined in conf file\n");
		exit(0);
	}

	// case the name be bigger than our name limit
	if(argTypeSize > MAX_ARGTYPE_NAME_LENGTH){
		printf("Warning: argType name out of bounds\n");
		memcpy(desc->inputs[desc->numInputsAdded].argType, argType, MAX_ARGTYPE_NAME_LENGTH);
		desc->inputs[desc->numInputsAdded].argType[MAX_ARGTYPE_NAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputs[desc->numInputsAdded].argType, argType, argTypeSize + 1); // +1 to copy \0
	}
	if(deserializeFunctionNameSize > MAX_FUNCNAME_LENGTH){
		printf("Warning: deserializeFunction name out of bounds\n");
		memcpy(desc->inputs[desc->numInputsAdded].deserializeFunctionName, deserializeFunctionName, MAX_FUNCNAME_LENGTH );
		desc->inputs[desc->numInputsAdded].deserializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputs[desc->numInputsAdded].deserializeFunctionName, deserializeFunctionName, deserializeFunctionNameSize + 1); // +1 to copy \0
	}
	if(deserializeLibNameSize > MAX_LNAME_LENGTH){
		printf("Warning: deserializeLib name out of bounds\n");
		memcpy(desc->inputs[desc->numInputsAdded].deserializeLibName, deserializeLibName, MAX_LNAME_LENGTH );
		desc->inputs[desc->numInputsAdded].deserializeLibName[MAX_LNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputs[desc->numInputsAdded].deserializeLibName, deserializeLibName, deserializeLibNameSize + 1); // +1 to copy \0
	}
	if(inputTypeSize > MAX_INPUTYPE_NAME_LENGTH){
		printf("Warning: inputType name out of bounds\n");
		memcpy(desc->inputs[desc->numInputsAdded].inputType, inputType, MAX_INPUTYPE_NAME_LENGTH);
		desc->inputs[desc->numInputsAdded].inputType[MAX_INPUTYPE_NAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputs[desc->numInputsAdded].inputType, inputType, inputTypeSize + 1); // +1 to copy \0
	}
	desc->inputs[desc->numInputsAdded].userArgIndex = userArgIndex;
	if(parameterOrder < 1 ){
		printf("Error: %s:%d parameterOrder < 1\n", __FILE__, __LINE__);
		exit(0);
	}
	desc->inputs[desc->numInputsAdded].parameterOrder = parameterOrder;
	desc->inputs[desc->numInputsAdded].mesgIndexIn = mesgIndexIn;
	desc->numInputsAdded++;
	return returnValue;
}

int addArgInputOutput(MatLabDesc *desc, char *argType, char *deserializeFunctionName,
                      char *deserializeLibName, int userArgIndex, char *inputType,
		      char *serializeFunctionName, char *serializeLibName, int parameterOrder,
		      int mesgIndexIn, int mesgIndexOut){

	int argTypeSize = strlen(argType);
	int deserializeFunctionNameSize = strlen(deserializeFunctionName);
	int deserializeLibNameSize = strlen(deserializeLibName);
	int inputTypeSize = strlen(inputType);
	int serializeFunctionNameSize = strlen(serializeFunctionName);
	int serializeLibNameSize = strlen(serializeLibName);

	int returnValue = 1;


	if(desc->numInputOutputsAdded + 1 > MAX_MATLAB_INPUTS){
		printf("Error: could not add ArgInputOutput numInputOutputs > MAX_MATLAB_INPUTOUTPUS\n");
		exit(0);
	}
	if(desc->numInputOutputsAdded + 1 > desc->numInputOutputs){
		printf("Error: trying to add more ArgInputOutputs then defined in conf file\n");
		exit(0);
	}


	// case the name be bigger than our name limit
	if(argTypeSize > MAX_ARGTYPE_NAME_LENGTH){
		printf("Warning: argType name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].argType, argType, MAX_ARGTYPE_NAME_LENGTH);
		desc->inputOutputs[desc->numInputOutputsAdded].argType[MAX_ARGTYPE_NAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].argType, argType, argTypeSize + 1); // +1 to copy \0
	}
	if(deserializeFunctionNameSize > MAX_FUNCNAME_LENGTH){
		printf("Warning: deserializeFunction name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].deserializeFunctionName, deserializeFunctionName, MAX_FUNCNAME_LENGTH );
		desc->inputOutputs[desc->numInputOutputsAdded].deserializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].deserializeFunctionName, deserializeFunctionName, deserializeFunctionNameSize + 1); // +1 to copy \0
	}
	if(deserializeLibNameSize > MAX_LNAME_LENGTH){
		printf("Warning: deserializeLib name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].deserializeLibName, deserializeLibName, MAX_LNAME_LENGTH );
		desc->inputOutputs[desc->numInputOutputsAdded].deserializeLibName[MAX_LNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].deserializeLibName, deserializeLibName, deserializeLibNameSize + 1); // +1 to copy \0
	}
	if(inputTypeSize > MAX_INPUTYPE_NAME_LENGTH){
		printf("Warning: inputType name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].inputType, inputType, MAX_INPUTYPE_NAME_LENGTH);
		desc->inputOutputs[desc->numInputOutputsAdded].inputType[MAX_INPUTYPE_NAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].inputType, inputType, inputTypeSize + 1); // +1 to copy \0
	}

	if(serializeFunctionNameSize > MAX_FUNCNAME_LENGTH){
		printf("Warning: serializeFunction name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].serializeFunctionName, serializeFunctionName, MAX_FUNCNAME_LENGTH);
		desc->inputOutputs[desc->numInputOutputsAdded].serializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].serializeFunctionName, serializeFunctionName, serializeFunctionNameSize + 1); // +1 to copy \0
	}

	if(serializeLibNameSize > MAX_LNAME_LENGTH){
		printf("Warning: serializeLib name out of bounds\n");
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].serializeLibName, serializeLibName, MAX_LNAME_LENGTH);
		desc->inputOutputs[desc->numInputOutputsAdded].serializeLibName[MAX_LNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->inputOutputs[desc->numInputOutputsAdded].serializeLibName, serializeLibName, serializeLibNameSize + 1); // +1 to copy \0
	}

	desc->inputOutputs[desc->numInputOutputsAdded].userArgIndex = userArgIndex;
	if(parameterOrder < 1 ){
		printf("Error: %s:%d parameterOrder < 1\n", __FILE__, __LINE__);
		exit(0);
	}
	desc->inputOutputs[desc->numInputOutputsAdded].parameterOrder = parameterOrder;
	desc->inputOutputs[desc->numInputOutputsAdded].mesgIndexIn = mesgIndexIn;
	desc->inputOutputs[desc->numInputOutputsAdded].mesgIndexOut = mesgIndexOut;
	desc->numInputOutputsAdded++;
	return returnValue;
}


int addArgOutput(MatLabDesc *desc, char *argType, char *serializeFunctionName,
                 char *serializeLibName, int parameterOrder, int mesgIndexOut){

	int argTypeSize = strlen(argType);
	int serializeFunctionNameSize = strlen(serializeFunctionName);
	int serializeLibNameSize = strlen(serializeLibName);

	int returnValue = 1;



	if(desc->numOutputsAdded + 1 > MAX_MATLAB_OUTPUTS){
		printf("Error: could not add ArgOutput numOutputsAdded > MAX_MATLAB_INPUTS\n");
		exit(0);
	}
	if(desc->numOutputsAdded + 1 > desc->numOutputs){
		printf("Error: trying to add more ArgOutput then defined in conf file\n");
		exit(0);
	}

	// case the name be bigger than our name limit
	if(argTypeSize > MAX_ARGTYPE_NAME_LENGTH){
		printf("Warning: argType name out of bounds\n");
		memcpy(desc->outputs[desc->numOutputsAdded].argType, argType, MAX_ARGTYPE_NAME_LENGTH);
		desc->outputs[desc->numOutputsAdded].argType[MAX_ARGTYPE_NAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->outputs[desc->numOutputsAdded].argType, argType, argTypeSize + 1); // +1 to copy \0
	}
	if(serializeFunctionNameSize > MAX_FUNCNAME_LENGTH){
		printf("Warning: serializeFunction name out of bounds\n");
		memcpy(desc->outputs[desc->numOutputsAdded].serializeFunctionName, serializeFunctionName, MAX_FUNCNAME_LENGTH);
		desc->outputs[desc->numOutputsAdded].serializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->outputs[desc->numOutputsAdded].serializeFunctionName, serializeFunctionName, serializeFunctionNameSize + 1); // +1 to copy \0
	}

	if(serializeLibNameSize > MAX_LNAME_LENGTH){
		printf("Warning: serializeLib name out of bounds\n");
		memcpy(desc->outputs[desc->numOutputsAdded].serializeLibName, serializeLibName, MAX_LNAME_LENGTH);
		desc->outputs[desc->numOutputsAdded].serializeLibName[MAX_LNAME_LENGTH] = '\0';
		returnValue = 0;
	}else{
		memcpy(desc->outputs[desc->numOutputsAdded].serializeLibName, serializeLibName, serializeLibNameSize + 1); // +1 to copy \0
	}

	if(parameterOrder < 1 ){
		printf("Error: %s:%d parameterOrder < 1\n", __FILE__, __LINE__);
		exit(0);
	}
	desc->outputs[desc->numOutputsAdded].parameterOrder = parameterOrder;
	desc->outputs[desc->numOutputsAdded].mesgIndexOut = mesgIndexOut;
	desc->numOutputsAdded++;
	return returnValue;
}
