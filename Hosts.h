#ifndef _HOSTS_H
#define _HOSTS_H
#include "constants.h"


// $Id: Hosts.h 32 2006-04-18 20:44:52Z ttavares $


/// Host status
#define NOTINIT		-2	///< not initialized
#define NOTAVAIL 	-1	///< not available
#define AVAIL		 0	///< available, but not used
// if used, status is a refcounter

///this struct defines a Host
typedef struct {
	char name[MAX_HNAME_LENGTH+1]; ///< the hostname
	int numResources;
	char resources[MAXRESOURCES][MAX_RNAME_LENGTH+1]; ///< the resources this host uses, array of strings
	int status; ///< the status of this host
	int weight;
	int mem; ///< machine memory in MB
} Host;

///this is the hosts array
typedef struct {
	Host hosts[MAXHOSTS];
	int numHosts;
	int totalWeight;
} HostsStruct;

/// Constructor
HostsStruct *hostsCreate();
/// destroyer
void hostsDestroy(HostsStruct *h);

//add functions
int hostsAdd(HostsStruct *h); //< adds a host to the struct HostsStruct, returns the index of the host
int hostsAddResource(HostsStruct *h, int hostIndex, char *resourceName); //< adds a resource to the given host

//set functions, argument can be released
int hostsSetStatus(HostsStruct *h, int hostIndex, int status); //< sets status of a host
int hostsSetWeight(HostsStruct *h, int hostIndex, int weight); //< sets host weight
int hostsSetName(HostsStruct *h, int hostIndex, char *name); //< sets the host name
int hostsSetMemory(HostsStruct *h, int hostIndex, int mem); //< how much memory this host can use, -1 for auto detect

// get functions
int hostsGetIndex(HostsStruct *h); //< returns a host index, based on weight
int hostsGetIndexByName(HostsStruct *h,char *name); //< gets the host index, based on the name
int hostsGetIndexByResource(HostsStruct *h, char *resourceName); //< gets the host index which has the resource
int hostsGetStatus(HostsStruct *h, int hostIndex); //< returns the status of the given host
int hostsGetWeight(HostsStruct *h, int hostIndex); //< returns the weight of the given host
int hostsGetMemory(HostsStruct *h, int hostIndex); //< returns the declared memory qty this host can use
char *hostsGetName(HostsStruct *h, int hostIndex); //< returns the name of the given host



//resource stuff......
int hostsGetNumResources(HostsStruct *h, int hostIndex); //< returns the resources used by this filter
//char **hostsGetResources(HostsStruct *h, int hostIndex); //< returns the resources array
int hostsHasResource(HostsStruct *h, int hostIndex, char *resourceName); //< does the host have the resource?
//....................


#endif
