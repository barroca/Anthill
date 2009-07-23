#ifndef MANAGER_H
#define MANAGER_H

#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <pvm3.h>
#include "Layout.h"


// Coutinho: If a c++ app use this file, tell it that these functions are in C
#ifdef __cplusplus
extern "C" {
#endif


#define I_AM_THE_MANAGER (pvm_parent() == PvmNoParent) ///< tells if this process is the manager


/* user functions *************/
/// user function: initialize the manager internal structs and the filters
Layout *initDs(char *confFile, int argc, char **argv);
/// user function:add a new query to filters. Only manager runs this.
///	\param layout System Layout.
///	\param work Buffer with a Unit of Work (UoW)
///	\param workSize Unit of Work Size (UoW)
///	\return Zero.
int appendWork(Layout *layout, void *work, int workSize);
/// user function: Finalize everything. Only manager runs this.
int finalizeDs(Layout *layout);
/* end user functions *********/

//Layout *initManager(char *confFile, int argc, char **argv);
/// A filter calls this function to run
//void runFilter();


#ifdef __cplusplus
}
#endif


#endif
