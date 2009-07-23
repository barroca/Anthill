#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pvm3.h>
#include "FilterSpec.h"
#include "Messages.h"
#include <unistd.h>
/*
 * Matheus 14,07/2004: changed all this stuff
 */

/* Constructor and destroyer **********************************************/
FilterSpec *createFilterSpec(char *filterName, char *libname) {
	int i;
	FilterSpec *fs = (FilterSpec *)malloc(sizeof(FilterSpec));
	for (i=0;i<MAXINSTANCES;i++){
		fs->filterPlacement.hosts[i] = (char*)malloc(sizeof(char)*MAX_HNAME_LENGTH);
		fs->filterPlacement.numInstances = 0;
	}
	strncpy(fs->name, filterName, MAX_FNAME_LENGTH);
	strncpy(fs->libname, libname, MAX_LNAME_LENGTH);
	return fs;
}

void destroyFilterSpec(FilterSpec *f){
	//we dont need to free stuff, everything is allocated statically
	free(f);
}
/**************************************************************************/

/* add hosts *************************************************************/
/// adds a host to the filter, return 1 on success 0 otherwise
int addHostToFilter(FilterSpec *fs, char *hostName){

	if (fs->filterPlacement.numInstances >= MAXINSTANCES){
		printf("FilterSpec.c: cant add host, maximum number of hosts reached for this filter");
		return -1;
	}
	else{
		strncpy(fs->filterPlacement.hosts[fs->filterPlacement.numInstances++], hostName, MAX_HNAME_LENGTH);
	}

	return (1);
}

int addResourceToFilter(FilterSpec *fs, char *resourceName){

	if (fs->filterPlacement.numInstances >= MAXINSTANCES){
		printf("FilterSpec.c: cant add host, maximum number of hosts reached for this filter");
		return -1;
	}
	else{
		fs->filterPlacement.resources[fs->filterPlacement.numInstances-1] = (char *) malloc(sizeof(char) * MAX_RNAME_LENGTH);
		strncpy(fs->filterPlacement.resources[fs->filterPlacement.numInstances-1], resourceName, MAX_RNAME_LENGTH);
	}

	return (1);
}


/// adds a host qty times
int addHostsToFilter(FilterSpec *fs, char *hostName, int qty){
	int i;
	for (i=0; i<qty; i++){
		if (!addHostToFilter(fs, hostName)) return -1;
	}
	return (1);
}
/****************************************************************************/

/* add streams **************************************************************/
/// Adds a stream to the inputs of a filter
int addInputToFilter(FilterSpec *fs, StreamSpec *ss){
	if (fs->numInputs >= MAXINPSTREAMS)
		return 0;
	
	fs->inputs[fs->numInputs++] = ss;
	return 1;
}
/// Adds a stream to the outputs of a filter
int addOutputToFilter(FilterSpec *fs, StreamSpec *ss){
	if (fs->numInputs >= MAXOUTSTREAMS)
		return 0;
	
	fs->outputs[fs->numOutputs++] = ss;
	return 1;
}
/***************************************************************************/

#ifdef ATTACH
int setAttached(FilterSpec* filter, int attached){
	filter->attached =  attached;	
	return 1;
}
int setAttach(FilterSpec* filter, int attach){
	filter->attach =  attach;	
	return 1;
}

int readAttConf(FilterSpec*fs, FILE *attFile){
	char line[MAX_HNAME_LENGTH + 22];
	char hName[MAX_HNAME_LENGTH + 1];
	int numInstancesCount = 0;
	int trash = 0;	
	fgets (line, 2000, attFile);

	// this is initialized before but in this case we need fill the
	// filter Spec structure using a file, so we back to 0 instaces
	fs->filterPlacement.numInstances = 0;

	fs->command = (char *) malloc(MAX_COM_LENGTH+1);
	sscanf(line, "%s", fs->command);

	fgets (line, 2000, attFile);
	while(!feof(attFile)){	
//		printf("line = %s\n", line);
		sscanf(line, "%d %d %s", &trash,&(fs->filterPlacement.tids[numInstancesCount]), hName);
//		printf("tid = %d host = %s\n", fs->filterPlacement.tids[numInstancesCount], hName);
		addHostToFilter(fs, hName);
		fgets (line, 2000, attFile);
		numInstancesCount++;
	}	
	return 1;
}
#endif

void setLastFilter(FilterSpec *fs, int lastFilter) {
#ifdef BMI_FT
	fs->lastFilter = lastFilter;
#endif
}

void handleSpawnError(int errorCondition) {
		switch (errorCondition){
			case PvmBadParam:
				printf("FilterSpec.c: Invalid argument\n");
				break;
			case PvmNoHost:
				printf("FilterSpec.c: Specified host not on virtual machine\n");
				break;
			case PvmNoFile:
				printf("FilterSpec.c: Cant find executable on remote machine\n");
				break;
			case PvmNoMem:
				printf("FilterSpec.c: out of memory\n");
				break;
			case PvmSysErr:
				printf("FilterSpec.c: system error\n");
				break;
			case PvmOutOfRes:
				printf("FilterSpec.c: out of resources\n");
				break;
			default:
				pvm_perror("pvm_spawn()");
				break;
		}
}


/* Spawns the instances of a filter */
int fsSpawnInstances(FilterSpec *fs, char *command, char **argvSpawn){
	int j, spawnReturn;
#ifdef ATTACH
	FILE *attachConfFile;
	char cwd[MAX_CWD_LENGTH];
	char attachConf[MAX_FNAME_LENGTH + MAX_CWD_LENGTH+5];
	getcwd(cwd, MAX_CWD_LENGTH);
	sprintf(attachConf, "%s/%s.atc", cwd, fs->name);

	// if this filter is attached we dont need spawn it
	if(fs->attached){
		attachConfFile = fopen(attachConf,"r");
		if(attachConfFile == NULL){
			printf("FilterSpec.c: Error opennig attachConfFile. file path = %s\n", attachConf); 
			return -1;	
		}
		readAttConf(fs, attachConfFile);
		fclose(attachConfFile);
		for(j = 0; j < fs->filterPlacement.numInstances; j++) {
			pvm_notify(PvmTaskExit, MSGT_TEXIT, 1, &(fs->filterPlacement.tids[j]));
			pvm_notify(PvmHostDelete, MSGT_HDEL, 1, &(fs->filterPlacement.tids[j]));
		}
	}else{
	// used to create the file "filter.atc" whos content is:
	// a list of the tid and host of each filter instance	
		attachConfFile = fopen(attachConf,"w");
		if(attachConfFile == NULL){
			printf("FilterSpec.c: Error creating attachConfFile. file path = %s\n", attachConf); 
			return -1;	
		}
		fprintf(attachConfFile, "%s\n", command);
#endif	
	//for each instance of this filter...
	for(j = 0; j < fs->filterPlacement.numInstances; j++) {	
		// runs the instance
		char hname[MAX_HNAME_LENGTH+1];
		strcpy(hname, fs->filterPlacement.hosts[j]);
		spawnReturn = pvm_spawn(command, argvSpawn, PvmTaskHost,
				hname, 1, 
				&(fs->filterPlacement.tids[j]));
#ifdef ATTACH
		fprintf(attachConfFile, "%d %d %s\n", j,fs->filterPlacement.tids[j], hname);
#endif		

		//error handling
		if (spawnReturn < 0) {
			printf("FilterSpec.c: Error creating filter %s, host %s\n", 
					fs->name, hname);
			pvm_perror("");
			exit(1);
		}
		else if (spawnReturn == 0){
			printf("FilterSpec.c: Error creating filter %s, host %s\n", 
					fs->name, hname);
			handleSpawnError(fs->filterPlacement.tids[j]);
			return -1;
		}

		pvm_notify(PvmTaskExit, MSGT_TEXIT, 1, &(fs->filterPlacement.tids[j]));
		pvm_notify(PvmHostDelete, MSGT_HDEL, 1, &(fs->filterPlacement.tids[j]));
	}
#ifdef ATTACH
	fclose(attachConfFile);
	}
#endif	
	return 0;

}

/* Spawns one instance of a filter */
int fsSpawnOneInstance(FilterSpec *fs, int instanceAddress, char *command, char **argvSpawn){
	int spawnReturn;
	char hname[MAX_HNAME_LENGTH+1];
	
	strcpy(hname, fs->filterPlacement.hosts[instanceAddress]);
	spawnReturn = pvm_spawn(command, argvSpawn, PvmTaskHost,
			hname, 1,	&(fs->filterPlacement.tids[instanceAddress]));

	//error handling
	if (spawnReturn < 0) {
		printf("FilterSpec.c: Error creating filter %s, host %s\n", 
				fs->name, hname);
		pvm_perror("");
		exit(1);
	}
	else if (spawnReturn == 0){
		printf("FilterSpec.c: Error creating filter %s, host %s\n", 
				fs->name, hname);
		handleSpawnError(fs->filterPlacement.tids[instanceAddress]);
		return -1;
	}

	pvm_notify(PvmTaskExit, MSGT_TEXIT, 1, &(fs->filterPlacement.tids[instanceAddress]));
	pvm_notify(PvmHostDelete, MSGT_HDEL, 1, &(fs->filterPlacement.tids[instanceAddress]));

	return 0;
}

void fsSetFaultStatus(FilterSpec *fs, int faultStatus) {
#ifdef BMI_FT
	fs->faultStatus = faultStatus;
#endif
}
