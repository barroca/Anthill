#ifndef MAT_LAB_MAKEFILE_H
#define MAT_LAB_MAKEFILE_H

#include <string.h>
#include <stdio.h>

#include "structs.h"

#include "MatLabUtil.h"

int generateMatLabMakefile(char *filterType, char *filterDir, Layout *layout);

#endif
