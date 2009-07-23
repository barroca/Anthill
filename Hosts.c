#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pvm3.h>
#include "Hosts.h"

// $Id: Hosts.c 32 2006-04-18 20:44:52Z ttavares $


/// Constructor
HostsStruct *hostsCreate(){
	int i;
	HostsStruct *hs = (HostsStruct*)malloc(sizeof(HostsStruct));

	for (i=0;i<MAXHOSTS;i++){
		hs->hosts[i].status = NOTINIT; //not initialized
		hs->hosts[i].numResources = 0;
	}

	hs->totalWeight = 0;
	
	return hs;
}

/// destroyer
void hostsDestroy(HostsStruct *h){
	/*int i, j;
	
	//free host stuff
	for (i=0;i<MAXHOSTS;i++){
		//free resources
		for (j=0;j<h->hosts[i].numResources;j++){
			free(h->hosts[i].resources[j]);
		}
	}*/
	
	//free the struct
	free(h);
}

//add functions
/// add a host to the host struct, empty
int hostsAdd(HostsStruct *h){
	//max hosts reached
	if (h->numHosts >= MAXHOSTS){
		printf("Hosts.c: error, cant add new host, maximum(%d) reached\n", MAXHOSTS);
		return -1;
	}
	
	//get the host slot
	Host *host = &h->hosts[h->numHosts];
	
	//init resources
	host->numResources = 0;
	//host status now is available, but not used
	host->status = AVAIL;
	//set weight to 1, default
	host->weight = 1;

	//update total weight
	h->totalWeight++;
	
	//return the slot index
	return h->numHosts++;
}

/// adds a resource to the host, caller cannot free resource after this
int hostsAddResource(HostsStruct *h, int hostIndex, char *resourceName){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant add resource, invalid index(%d)\n", hostIndex);
		return -1;
	}
	
	//get the host
	Host *host = &h->hosts[hostIndex];

	//check resources
	if (host->numResources > MAXRESOURCES){
		printf("Hosts.c: warning, reached maximum number of resources(%d)\n", MAXRESOURCES);
		return -1;
	}
	
	//add the resource
	strncpy(host->resources[host->numResources], resourceName, MAX_RNAME_LENGTH);

	//return the resource index
	return host->numResources++;

}

//set functions, argument can be released
int hostsSetStatus(HostsStruct *h, int hostIndex, int status){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant set status, invalid index(%d)\n", hostIndex);
		return -1;
	}

	Host *host = &h->hosts[hostIndex];
	
	if (host->status != NOTAVAIL) {	
		if (status == NOTAVAIL){
			//we have to reduce totalWeight if we are disabling a host			
			h->totalWeight -= host->weight;
		}
	}

	host->status = status;
	return 1;
}

int hostsSetWeight(HostsStruct *h, int hostIndex, int weight){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant host weight, invalid index(%d)\n", hostIndex);
		return -1;
	}
	
	Host *host = &h->hosts[hostIndex];
	int oldWeight = host->weight;
	if (weight < 1){
		printf("Hosts.c: warning, invalid weight %d, using 1\n", weight);
		host->weight = 1;
	}
	else {
		host->weight = weight;
	}

	//update total weight
	h->totalWeight += host->weight - oldWeight;
	return 1;
}

int hostsSetMemory(HostsStruct *h, int hostIndex, int mem){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant set host memory, invalid index(%d)\n", hostIndex);
		return -1;
	}
	Host *host = &h->hosts[hostIndex];

	host->mem = mem;
	return 1;


}

int hostsSetName(HostsStruct *h, int hostIndex, char *name){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant set host name, invalid index(%d)\n", hostIndex);
		return -1;
	}
	Host *host = &h->hosts[hostIndex];

	//big name...
	if (strlen(name) > MAX_HNAME_LENGTH){
		printf("Hosts.c: warning, hostname length greater than MAX_HNAME_LENGTH(%d), truncating\n", 
			MAX_HNAME_LENGTH);
	}

	//cpy name
	strncpy(host->name, name, MAX_HNAME_LENGTH);
	host->name[MAX_HNAME_LENGTH] = '\0';

	char *arrayHosts[1];
	arrayHosts[0] = host->name;
	
	//adds the host to the virtual machine
	int hostStatus;
	int status_addhost = pvm_addhosts(arrayHosts, 1, &hostStatus);

	//error handling...
	if (status_addhost < 0) {
		printf("Hosts.c: error, pvm_adhost\n");
		pvm_perror("");
		return -1;;
	}
	else  if ((status_addhost == 0 ) && (hostStatus != PvmDupHost)){
		// created less hosts than we asked for, and no dup
		printf("Hosts.c: error adding host %s to the pvm: ", host->name);
		if (hostStatus == PvmBadParam) {
			printf("bad hostname syntax\n");
		}else if(hostStatus == PvmNoHost) {
			printf("no such host\n");
		}else if(hostStatus == PvmCantStart) {
			printf("failed to start pvmd on host\n");
		}else {
			printf("pvm error %d\n", hostStatus);
		}
		return -1;;
	}

	return 1;
}

// get functions
int hostsGetIndex(HostsStruct *h){
	int i=0;
	
	if (h->totalWeight <= 0) {
		// There's no hosts in the config file or all configured hosts died
		fprintf(stderr, "There's no more hosts in configuration file. Aborting ...\n");	
		exit(1);
	}
	
	//get a random number between >= 0 < totalWeight
	int number = (random() % h->totalWeight) + 1;

	//get the available host
	for (i=0;i<MAXHOSTS;i++){
		Host *host = &h->hosts[i];
		switch (host->status){
			case NOTAVAIL:
			case NOTINIT:
				continue;
			case AVAIL:
				number -= host->weight;
				if (number <= 0){
					return i;
				}
			break;
		}
	}
	return -1;
}

int hostsGetIndexByName(HostsStruct *h, char *name){
	int i=0;

	for (i=0;i<h->numHosts;i++){
		if (strncasecmp(h->hosts[i].name, name, MAX_HNAME_LENGTH) == 0){
			return i;
		}
	}
	//error, could not find the name
	return -1;
}

int hostsGetIndexByResource(HostsStruct *hs, char *resourceName){
	int i, hostsWhoHaveRec[MAXHOSTS], numHostsWhoHave=0;
	static int numHostsAlreadyChoseen = 0;

	for (i=0;i<hs->numHosts;i++){
		if (hostsHasResource(hs, i, resourceName) == 1){
			hostsWhoHaveRec[numHostsWhoHave++] = i;
		}
	}
	
	if (numHostsWhoHave == 0)
		return -1;
	
	//get a random number between >= 0 < totalWeight
	int number = (numHostsAlreadyChoseen++ % numHostsWhoHave);

	return hostsWhoHaveRec[number];
}

int hostsGetStatus(HostsStruct *h, int hostIndex){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get status, invalid index(%d)\n", hostIndex);
		return -1;
	}

	Host *host = &h->hosts[hostIndex];

	return host->status;
}

int hostsGetMemory(HostsStruct *h, int hostIndex){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get memory, invalid index(%d)\n", hostIndex);
		return -1;
	}

	Host *host = &h->hosts[hostIndex];

	return host->mem;
}

int hostsGetWeight(HostsStruct *h, int hostIndex){
	// invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get weight, invalid index(%d)\n", hostIndex);
		return -1;
	}

	Host *host = &h->hosts[hostIndex];

	//we return the weight only if the host status is available
	if (hostsGetStatus(h, hostIndex) >= 0){
		return host->weight;
	}
	else {
		return 0;
	}
}


char *hostsGetName(HostsStruct *h, int hostIndex){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get name, invalid index(%d)\n", hostIndex);
		return NULL;
	}

	Host *host = &h->hosts[hostIndex];

	return host->name;
}

//resource stuff......
int hostsGetNumResources(HostsStruct *h, int hostIndex){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get number of resources, invalid index(%d)\n", hostIndex);
		return -1;
	}

	Host *host = &h->hosts[hostIndex];

	return host->numResources;

}

int hostsHasResource(HostsStruct *h, int hostIndex, char *resourceName){
	//invalid index
	if (hostIndex >= h->numHosts ){
		printf("Hosts.c: cant get resource, invalid index(%d)\n", hostIndex);
		return 0;
	}
	Host *host = &h->hosts[hostIndex];
	int i;
	for (i=0;i<host->numResources;i++){
		if (strncmp(resourceName, host->resources[i], MAX_RNAME_LENGTH) == 0){
			return 1;
		}
	}
	return 0;
}
//....................


