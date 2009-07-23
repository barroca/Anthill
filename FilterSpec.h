#ifndef _FILTER_SPEC_H_
#define _FILTER_SPEC_H_

#include <string.h>
#include "constants.h"
#include "structs.h" 

/// Constructor
FilterSpec *createFilterSpec(char *filterName, char *libname);
/// Destroyer
void destroyFilterSpec(FilterSpec *f);

int addResourceToFilter(FilterSpec *fs, char *resourceName);
/// These functions add hosts to the filter
int addHostToFilter(FilterSpec *, char *hostName);
/// The second adds the given host qty times
int addHostsToFilter(FilterSpec *, char *hostName, int qty);

/// Adds a stream to the inputs of a filter
int addInputToFilter(FilterSpec *, StreamSpec *);
/// Adds a stream to the outputs of a filter
int addOutputToFilter(FilterSpec *, StreamSpec *);

/// Spawn all instances of the filter
int fsSpawnInstances(FilterSpec *, char *, char **);
/// Spawns one instance of a filter
int fsSpawnOneInstance(FilterSpec *fs, int instanceAddress, char *command, char **argvSpawn);

#ifdef ATTACH
int setAttach(FilterSpec* , int);
int setAttached(FilterSpec* , int);
#endif

void setLastFilter(FilterSpec *, int);
void fsSetFaultStatus(FilterSpec *, int);

#endif


