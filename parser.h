#ifndef __PARSER_H
#define __PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <expat.h>
#include <ctype.h>
#include <math.h>
#include "structs.h"

// Check doc/diagramaEstadosParserDS.dia for states
// parseerror comes from EXPAT syntax error, error means DS syntax error, like invalid tag
typedef enum __State { 
	start, prehost, hostdec, preplace, placement, prelay, layout, preend, end, //estados principais
	//host states
	resources,
	filter, //placement states
	stream, fromto, tostream, //layout states
	error, parseerror, //error states
	matLab, matLabFilter, matLabFunction, 
	matLabFunctionArg, preMatLab // matLab states
} State;


/// reads a xml, fill the layout passed as argument
int readConfig(char *fileName, Layout *layout);
#endif
