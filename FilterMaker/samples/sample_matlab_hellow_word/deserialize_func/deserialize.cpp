#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "mex.h"
#include "deserialize.h"
//#include "matrix.h"

// create mxArray with a charArray
mxArray *stringToMxArray(string str){
	
	mxArray *returnValue = mxCreateString(str.c_str());
	if(returnValue == NULL){
		printf("Error: %s:%d could not allocate memory\n", __FILE__, __LINE__);	
		exit(1);	
	}
	return returnValue;
}


