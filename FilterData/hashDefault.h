/// \file hashDefault.h This file is the default hash used by the labeled stream, if the user does not implement one */

#ifndef HASH_DEFAULT_H
#define HASH_DEFAULT_H

#include <stdio.h>
#include <string.h>

/// used when we cant read a hash from lib associated with outputport
int hashDefault(char *label, int image);
void mlsHashDefault(char *label, int image, int destArray[]);

#endif
