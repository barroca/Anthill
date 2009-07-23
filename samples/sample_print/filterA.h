#ifndef _FILTER_A_
#define _FILTER_A_

#include "void.h"

int initFilter(void *work, int size);
int processFilter(void *work, int size);
int finalizeFilter();

#endif
