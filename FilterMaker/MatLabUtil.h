#ifndef MATLAB_UTIL_H
#define MATLAB_UTIL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "structs.h"
#include "constants.h"

char **getAllMatLabLibs(char *filterType, Layout *layout, int *numLibs);
void removeLibAndSo(char *libName, char *newName);

#endif
