#ifndef MATLABDESC_H
#define MATLABDESC_H

#include <string.h>
#include "constants.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

int setMatLabLibName(MatLabDesc *desc, char *libName);
int setMatLabFunctionName(MatLabDesc *desc, char *funcName);
int setFirstFilter(MatLabDesc *desc, int firstFilter);

void setNumArgs(MatLabDesc *desc, int numInputs, int numInputOutpus, int numOutputs);

int addArgInput(MatLabDesc *desc, char *argType, char *deserializeFunctionName, 
                char *deserializeLibName, int userArgIndex, char *inputType, 
		int parameterOrder,int mesgIndexIn);

int addArgInputOutput(MatLabDesc *desc, char *argType, char *deserializeFunctionName, 
                      char *deserializeLibName, int userArgIndex, char *inputType, 
		      char *serializeFunctionName, char *serializeLibName, int parameterOrder,
		      int mesgIndexIn, int mesgIndexOut);

int addArgOutput(MatLabDesc *desc, char *argType, char *serializeFunctionName, 
                 char *serializeLibName, int parameterOrder, int mesgIndexOut);


/////////////////////gets... you cant free the return

char *getMatLabLibName(MatLabDesc *desc);
char *getMatLabFunctionName(MatLabDesc *desc);
int getNumArgOutputs(MatLabDesc *desc);
int getNumArgInputs(MatLabDesc *desc);
int getNumArgInputOutputs(MatLabDesc *desc);
ArgOutput *getArgOutputs(MatLabDesc *desc);
ArgInput *getArgInputs(MatLabDesc *desc);

#endif
