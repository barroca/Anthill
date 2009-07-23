#ifndef FILTERDATA_C
#define FILTERDATA_C

#ifdef VOID_INST
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <pvm3.h>

//abstract data types
#include <Task/Task.h>
#include <Cache/Cache.h>

#include "FilterData.h"
#include "Ports.h"
#include "../constants.h"
#include "../Messages.h"
#ifdef VOID_INST
#include "Instrumentation.h"
#endif
#ifdef VOID_TERM
#include "Termination.h"
#endif
#ifdef VOID_TRACER
#include <Tracer/tracer.h>
#include "tracingdefs.h"
int msgId;
int incomingMsgId;
#endif

/** used to notify manager some system error happened, and filter is aborting,
 * similar to AEXIT.
 *
 * @param msg Message to send describing the error
 * */
void sendFilterError( char *msg ){

    pvm_initsend(PvmDataRaw);

    pvm_pkbyte(msg, strlen(msg), 1);
    pvm_send(pvm_parent(), MSGT_FERROR);

    return;
}



/*
* 05/10/2006 ITAMAR changes to permit that a 'from' has more than one 'to'
*/


//these functions just call the init process and finalize, theyre here to make easier to debug
static int callFDInit(FilterData *fd, void *work, int worksize){
    if(fd->libdata.init != NULL){
        return fd->libdata.init(work, worksize);
    }else{
        return 0;
    }
}

static int callFDProcess(FilterData *fd, void* work, int worksize){
    if(fd->libdata.process != NULL){
        return fd->libdata.process(work, worksize);
    }else{
        return 0;
    }
}
static int callFDFinalize(FilterData *fd){
	if(fd->libdata.finalize != NULL){
		return fd->libdata.finalize();
	}else{
		return 0;	
	}	
}

FilterData *createFilterData(){
	FilterData *fd = (FilterData *) malloc(sizeof(FilterData));
	if(!fd){
	  	printf("createFilterData: could not allocate memory\n");
		return NULL;
	}
	// init num ports
	fd->numInputPorts = 0;
	fd->numOutputPorts = 0;

	// Used to now where a new port can be added
	fd->numInportsAdded = 0;
	fd->numOutportsAdded = 0;

#ifdef VOID_INST
	//we beign at void state
	fd->instData = instCreate();
#endif

#ifdef ATTACH
	fd->attached = 0;
#endif

#ifdef VOID_TRACER
	char tracefilename[ MAX_HNAME_LENGTH + 8 + 100 ], hostname[ MAX_HNAME_LENGTH ];
	gethostname( hostname, MAX_HNAME_LENGTH );
	sprintf( tracefilename, "trace.%s.%d", hostname, pvm_mytid() );
	msgId = 0;
	fd->trcData = trcCreateData( tracefilename );
#endif
	return fd;
}

/// \todo Only free FilterData. Needs to free input e output port.
void destroyFilterData(FilterData *fd){

	//free the library
	dlclose(fd->libdata.libHandler);
	//free ports data
	int i;
	for (i=0;i<fd->numInputPorts;i++){
		destroyInputPort(fd->inputPorts[i]);
	}
	for (i=0;i<fd->numOutputPorts;i++){
		destroyOutputPort(fd->outputPorts[i]);
	}

#ifdef VOID_TRACER
	trcDestroyData( &(fd->trcData) );
#endif

#ifdef VOID_INST
	instSwitchState(fd->instData, &fd->instData->voidStates[TIMER_END]);
	//we save the data to a file named filtername.instance
	char filename[MAX_FNAME_LENGTH + 100];
	sprintf(filename, "%s.%d", fd->name, fd->myRank);
	//for now, we use filename for label also
	instSaveTimings(fd->instData, filename, filename);
	instDestroy(&fd->instData);
	//we dont leave state, as data is destroyed...
#endif


	if(!fd){
		printf("destroyFilterData: FilterData is NULL.. I cant free it\n");
	}else{
		free(fd);
	}
}

#ifdef ATTACH
void destroyFilterDataAfterEOF(FilterData *fd){

	//free the library
	//dlclose(fd->libdata.libHandler);
	//free ports data
	int i;
	for (i=0;i<fd->numInputPorts;i++){
		destroyInputPort(fd->inputPorts[i]);
	}
	for (i=0;i<fd->numOutputPorts;i++){
		destroyOutputPort(fd->outputPorts[i]);
	}

#ifdef VOID_TRACER
	trcDestroyData( &(fd->trcData) );
#endif

#ifdef VOID_INST
	instSwitchState(fd->instData, &fd->instData->voidStates[TIMER_END]);
	//we save the data to a file named filtername.instance
	char filename[MAX_FNAME_LENGTH + 100];
	sprintf(filename, "%s.%d", fd->name, fd->myRank);
	//for now, we use filename for label also
	instSaveTimings(fd->instData, filename, filename);
	backupUserStates(fd->instData);
	instDestroy(&fd->instData);
	//we dont leave state, as data is destroyed...
#endif


	if(!fd){
		printf("destroyFilterData: FilterData is NULL.. I cant free it\n");
	}else{
		free(fd);
	}
}
#endif


int setFDNumInputs(FilterData *fd, int numInp){
	if (numInp >= MAXINPSTREAMS)
		return 0;

	fd->numInputPorts = numInp;
	return 1;
}

int setFDNumOutputs(FilterData *fd, int numOut){
	fd->numOutputPorts = numOut;
	return 0;
}

// get a copy of filter name
char *getFDName(FilterData *fd){
	return(strdup(fd->name));
}

int setFDArgc(FilterData *fd, int argc){
	fd->argc = argc;	
	return 0;
}

int setFDArgv(FilterData *fd, char **argv){
	fd->argv = argv;
	return 0;
}

// set filter name and free name
int setFDName(FilterData *fd, char *name){
  	int nameSize = strlen(name);

	// case the name be bigger than our name limit
	if((nameSize) > MAX_FNAME_LENGTH){
		printf("Warning: Filter name out of bounds\n");
		memcpy(fd->name, name, MAX_FNAME_LENGTH);
		fd->name[MAX_FNAME_LENGTH] = '\0';
		return 0;
	}else{
		memcpy(fd->name, name, nameSize +1); // +1 to copy \0
		return 1;
	}
}

// set the host where this filter is and free name
int setFDHostName(FilterData *fd, char *hostName){
  	int hostNameSize = strlen(hostName);

	// case the host name be bigger than our host name limit
	if((hostNameSize) >  MAX_HNAME_LENGTH){
		printf("Warning: Host name out of bounds\n");
		memcpy(fd->hostName, hostName,  MAX_HNAME_LENGTH);
		fd->hostName[MAX_HNAME_LENGTH] = '\0';
		return 0;
	}else{
		memcpy(fd->hostName, hostName, hostNameSize +1);
		return 1;
	}
}

int getFDArgc(FilterData *fd){
	return fd->argc;	
}

char **getFDArgv(FilterData *fd){
	return fd->argv;	
}


int getFDNumInstances(FilterData *fd) {
	return 	fd->numInstances;
}

int getFDParentTid(FilterData *fd) {
	return 	fd->parentTid;
}

int setFDNumInstances(FilterData *fd, int numInstances){
	if (numInstances >= MAXINSTANCES)
			return -1;

	fd->numInstances = numInstances;
	return 0;
}

void setFDRank(FilterData *fd, int rank){
	fd->myRank = rank;
}

// Set the filter id ... Id is unique for all set of filters
void setFDIdFilter(FilterData *fd, int id){
	fd->id = id;
}

//sets the tids
void setFDTids(FilterData *fd, int *tids){
	memcpy(fd->tids, tids, sizeof(int)*fd->numInstances);
}
void setFDParentTid(FilterData *fd, int tid){
	fd->parentTid = tid;
}

#ifdef ATTACH
void setFDAttached(FilterData *fd, int attached){
	fd->attached = attached;
}
#endif

#ifdef VOID_INST
//sets FilterData instdir
void setFDInstDir(FilterData *fd, char *dir){
	instSetDir(fd->instData, dir);
}
#endif

// only add a InputPort in FilterData
int addFDInputPort(FilterData *fd, InputPort *p){
  	if((fd->numInportsAdded +1) < MAXINPSTREAMS){
		fd->inputPorts[fd->numInportsAdded] = p;
		fd->numInportsAdded++;
		return 1;
	}else{
		printf("Warning: inputPorts number out of bounds\n");
		return 0;
	}
}

// only add a OutputPort in FilterData
int addFDOutputPort(FilterData *fd, OutputPort *p){
  	if((fd->numOutportsAdded + 1) < MAXOUTSTREAMS){
		fd->outputPorts[fd->numOutportsAdded] = p;
		fd->numOutportsAdded++;
		return 1;
	}else{
		printf("Warning: outputPorts number out of bounds\n");
		return 0;
	}
}

int setFDLibName(FilterData *fd, char *libName){
  	int libNameSize = strlen(libName);

	// case the host name be bigger than our host name limit
	if((libNameSize) >  MAX_LNAME_LENGTH){
		printf("FilterData.c: warning, lib name(%s) out of bounds, truncating\n", libName);
		memcpy(fd->libdata.name, libName, MAX_LNAME_LENGTH);
		fd->libdata.name[MAX_LNAME_LENGTH] = '\0';
		return 0;
	}else{
		memcpy(fd->libdata.name, libName, libNameSize +1);
		return 1;
	}
}

//this function loads the filter libraries(init process finalize)
int loadFDLibFunctions(FilterData *fd){

	char *error = NULL;
	char *libnameLocal = NULL;

	// try load lib in local directory, so we need 2 strigns,
	// the first with ./, em the second whithout
	libnameLocal = (char*)malloc(strlen(fd->libdata.name) + 3);
	sprintf(libnameLocal, "./%s", fd->libdata.name);

	// get the library handler
	if 	(((fd->libdata.libHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((fd->libdata.libHandler = dlopen(fd->libdata.name, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "FilterData.c: could not load filter %s library, %s\n", fd->libdata.name, dlerror());
			dlclose(fd->libdata.libHandler);
			return -1;
	}
	free(libnameLocal);

	// load filter from dynamic library : initFilter - processFilter - finalizeFilter
	// warnnig: The function simbols can be in C or C++
	fd->libdata.init = (initialize_t *)dlsym(fd->libdata.libHandler, "initFilter");
	if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "Erro no dlsym: %s\n", error);
			dlclose(fd->libdata.libHandler);
			return -1;
	}

	fd->libdata.process = (process_t *)dlsym(fd->libdata.libHandler, "processFilter");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "Erro no dlsym: %s\n", error);
		dlclose(fd->libdata.libHandler);
		return -1;
	}

	fd->libdata.finalize   = (finalize_t *)dlsym(fd->libdata.libHandler, "finalizeFilter");
	if ((error = dlerror()) != NULL)  {
	        fprintf (stderr, "Erro no dlsym: %s\n", error);
		dlclose(fd->libdata.libHandler);
		return -1;
	}
	return 1;
}

//this function loads the C++ API library and tell it to load the filter object
int loadEventAPI(FilterData *fd){
    char *error = NULL;

    char cwd[MAX_CWD_LENGTH];
    getcwd(cwd, MAX_CWD_LENGTH);

    // try load lib in local directory, so we need 2 strigns,
    // the first with ./, em the second whithout
    char cppLibname[] = "libahevent.so";
    char cppLibnameLocal[] = "./libahevent.so";
    char eventLibNameGlobal[1000];
    sprintf(eventLibNameGlobal, "%s/libahevent.so", cwd);

    // get the C++ API library handler
    if 	(((fd->libdata.libHandler = dlopen(eventLibNameGlobal, RTLD_NOW)) == NULL) &&
	    ((fd->libdata.libHandler = dlopen(cppLibnameLocal, RTLD_NOW)) == NULL) &&
            ((fd->libdata.libHandler = dlopen(cppLibname, RTLD_NOW)) == NULL )) {
        	fprintf(stderr, "FilterData.c: could not load filter %s library, %s\n", eventLibNameGlobal, dlerror());
        return -1;
    }

    // load the C++ API initializer and call it to load the filter
    initCppAPI_t* initEventAPI = (initCppAPI_t*)dlsym(fd->libdata.libHandler, "initEventAPI");

    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }

    if (-1 == initEventAPI(fd->libdata.name)) {
        return -1;
    }

    fd->libdata.init = NULL;
    fd->libdata.process = (process_t *)dlsym(fd->libdata.libHandler, "processFilterEvent");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }
    fd->libdata.finalize   = (finalize_t *)dlsym(fd->libdata.libHandler, "finalizeFilterEvent");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }
    return 1;
}

//this function loads the C++ API library and tell it to load the filter object
int loadCppAPI(FilterData *fd){
    char *error = NULL;

    char cwd[MAX_CWD_LENGTH];
    getcwd(cwd, MAX_CWD_LENGTH);
    printf("loadCppAPI cwd = %s\n", cwd);

    // try load lib in local directory, so we need 2 strigns,
    // the first with ./, em the second whithout
    char cppLibname[] = "libdscpp.so";
    char cppLibnameLocal[] = "./libdscpp.so";
    char cppLibDoidona[1000];
    sprintf(cppLibDoidona, "%s/libdscpp.so", cwd);
    printf("LibGlobal = %s", cppLibDoidona);

    // get the C++ API library handler
    if 	(((fd->libdata.libHandler = dlopen(cppLibnameLocal, RTLD_NOW)) == NULL) &&
            ((fd->libdata.libHandler = dlopen(cppLibname, RTLD_NOW)) == NULL )) {
        fprintf(stderr, "FilterData.c: could not load filter %s library, %s\n", cppLibname, dlerror());
	dlclose(fd->libdata.libHandler);

        return -1;
    }

    // load the C++ API initializer and call it to load the filter
    initCppAPI_t* initCppAPI = (initCppAPI_t*)dlsym(fd->libdata.libHandler, "initCppAPI");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }

    if (-1 == initCppAPI(fd->libdata.name)) {
        return -1;
    }

    // load filter from dynamic library : initFilterCpp - processFilterCpp - finalizeFilterCpp
    // warnnig: The function simbols can be in C or C++
    fd->libdata.init = (initialize_t *)dlsym(fd->libdata.libHandler, "initFilterCpp");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }

    fd->libdata.process = (process_t *)dlsym(fd->libdata.libHandler, "processFilterCpp");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }

    fd->libdata.finalize   = (finalize_t *)dlsym(fd->libdata.libHandler, "finalizeFilterCpp");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "Erro no dlsym: %s\n", error);
        return -1;
    }
    return 1;
}

#ifdef BMI_FT
int getFDFaultStatus(FilterData *fd) {
	return fd->faultStatus;
}

void setFDFaultStatus(FilterData *fd, int faultStatus) {
	fd->faultStatus = faultStatus;
}

void setFDLastFilter(FilterData *fd, int lastFilter) {
	fd->lastFilter = lastFilter;
}

int getFDLastFilter(FilterData *fd) {
	return fd->lastFilter;
}
#endif

int setFDMachineMem(FilterData *fd, int mem){
	//if mem = -1, we autodetect
	if (mem == -1){
		/// \todo Read memory. Now its hardcoded to 512MB
		printf("FilterData.c: Warning, no mem declared using 512MB as memory value\n");
		fd->memory = 512;
	}
	else {
		fd->memory = mem;
	}
	return 1;
}

int getFDMachineMem(FilterData *fd){
	return fd->memory;
}

int setFDNumLocalInstances(FilterData *fd, int numLocalInstances){
	fd->numLocalInstances = numLocalInstances;
	return 1;
}

int getFDLocalInstances(FilterData *fd){
	return fd->numLocalInstances;
}

int recvFilterData(FilterData *fData) {
	int bufId;
	int i, l, toFilter;
	char *cwd = (char *)malloc(MAX_CWD_LENGTH+1);
	int num = 0;
	int *tids = (int *)malloc(sizeof(int)*MAXINSTANCES);
	char *filterName         = (char *)malloc(MAX_FNAME_LENGTH);
	char *libName            = (char *)malloc(MAX_LNAME_LENGTH);
	char *hostname           = (char *)malloc(MAX_HNAME_LENGTH);
	char *labelStreamLibname = (char *)malloc(MAX_LNAME_LENGTH);

	int parentTid = pvm_parent();

#ifdef ATTACH
	// if im using attach i dont no if the
	// manager process is realy my father
	bufId = pvm_recv(-1, 0);
	int bytes, msgtag,tid;
	pvm_bufinfo(bufId, &bytes, &msgtag, &tid );
	printf("recvFilterData: bytes = %d msgtag = %d tid = %d\n", bytes, msgtag, tid);
#else
	//We receive one message with all data in it
	bufId = pvm_recv(parentTid, 0);
#endif
	//get the current working directory
	pvm_upkint(&l, 1, 1);
	pvm_upkbyte(cwd, l, 1);
	cwd[l] = '\0';
#ifdef DEBUG	
	printf("cwd = %s\n",cwd);
#endif	

	// filter id, useful for debugging
	pvm_upkint(&num, 1, 1);
	setFDIdFilter(fData, num);
	// get my rank
	pvm_upkint(&num, 1, 1);
	setFDRank(fData, num);
	// total number of instances of this filter
	pvm_upkint(&num, 1, 1);
	setFDNumInstances(fData, num);
	// get my brothers tids
	pvm_upkint(tids, num, 1);
	setFDTids(fData, tids);
#ifdef ATTACH
	// getting my parent Tid because if i'm a attached filter
	// i can be adopted for anyone.. so we do it for all filters..
	// just to make the code easy to understand..
	pvm_upkint(&parentTid, 1, 1);
	int attached;
	pvm_upkint(&attached, 1, 1);
	setFDAttached(fData, attached);
#endif
	// if you are not using Attach the parentTid still the same
	// or its equal to pvm_parent()
	setFDParentTid(fData, parentTid);

#ifdef BMI_FT
	int faultStatus, lastFilter;
	pvm_upkint(&faultStatus, 1, 1);
	setFDFaultStatus(fData, faultStatus);

	pvm_upkint(&lastFilter, 1, 1);
	setFDLastFilter(fData, lastFilter);
#endif

	// filtername
	pvm_upkint(&l, 1, 1);
	pvm_upkbyte(filterName, l, 1);
	filterName[l] = '\0';
	setFDName(fData, filterName);

	//machine declared memory: -1 autodetect, declared on XML
	pvm_upkint(&num, 1, 1);
	setFDMachineMem(fd, num);

	//number of brothers(+ me) I have on this machine, useful for memory management
	pvm_upkint(&num, 1, 1);
	setFDNumLocalInstances(fd, num);

#ifdef VOID_INST
	char instDir[MAX_IDIR_LENGTH];
	pvm_upkint(&l, 1, 1);
	pvm_upkbyte(instDir, l, 1);
	instDir[l] = '\0';
	setFDInstDir(fd, instDir);
#endif

	// receives shared lib name
	pvm_upkint(&l, 1, 1);
	pvm_upkbyte(libName, l, 1);
	libName[l] = '\0';
	setFDLibName(fData, libName);
	/*if (loadFDLibFunctions(fData) == -1){
	  char msg[1000];
	  sprintf(msg, "could not load shared library %s", libName);
	  pvm_initsend(PvmDataRaw);

	  pvm_pkbyte(msg, strlen(msg), 1);
	  pvm_send(pvm_parent(), MSGT_FERROR);
	  return -1;
	  }*/

	// set hostname
	gethostname(hostname, MAX_HNAME_LENGTH);
	setFDHostName(fData, hostname);

	// data received till now
	fprintf(stderr,"filter %s (rank: %d): pvm_tid:%d hostname:%s\n",
			fData->name, fData->myRank, pvm_mytid(), fData->hostName);

	//port data
	//Receive numOutputs
	pvm_upkint(&num, 1, 1);
	setFDNumOutputs(fData, num);
	//receive numInputs
	pvm_upkint(&num, 1, 1);
	setFDNumInputs(fData, num);

	// for each OutputPort
	for(i = 0; i < fData->numOutputPorts; i++) {
		int nOutHosts = 0, tag = 0;
		int numToSend;
		int *outTids  = NULL;
		int firstInstanceToWrite;
		char *portName = (char *)malloc(MAX_PTNAME_LENGTH + 1);
		char *writePolicyName = (char *)malloc(100);
		writePolicy_t wp;

		OutputPort *outputPort;

		pvm_upkint(&numToSend, 1, 1);
		//now we can create the port
		outputPort = createOutputPort(numToSend);

		//port data
		pvm_upkint(&l, 1, 1);
		pvm_upkbyte(portName, l, 1);  //portname
		portName[l] = '\0';
		setOPName(outputPort, portName);

		pvm_upkint(&tag, 1, 1); //get tag
		setOPTag(outputPort, tag);

		for( toFilter = 0; toFilter < numToSend; toFilter++ )
		{
			pvm_upkint(&nOutHosts, 1, 1); //number of tids it is connected

			setOPNumDestinations(outputPort, toFilter, nOutHosts);
			outTids = (int *) malloc(sizeof(int)*nOutHosts);
			pvm_upkint(outTids, nOutHosts, 1); //get tids
			setOPTidsDestinations(outputPort, toFilter, outTids);

			pvm_upkint(&l, 1, 1);
			pvm_upkbyte(writePolicyName, l, 1); // get write policy
			writePolicyName[l] = '\0';
			wp = getWritePolicyByName(writePolicyName);
			setOPWritePolicy(outputPort, toFilter, wp);

			// get LS sharedlib if policy is LS
			if (wp  == LABELED_STREAM ){
				pvm_upkint(&l, 1, 1);
				pvm_upkbyte(labelStreamLibname, l, 1);
				labelStreamLibname[l] = '\0';
				//set output port library name
				setOPLibName(outputPort, toFilter, labelStreamLibname);
				//load output port library for ls
				if (loadOPLSData(outputPort, toFilter) == -1 ){
					char msg[1000];
					sprintf(msg, "could not load LS shared library %s", labelStreamLibname);
					pvm_initsend(PvmDataRaw);

					pvm_pkbyte(msg, strlen(msg), 1);
					pvm_send(pvm_parent(), MSGT_FERROR);
					return -1;
				}
			}
			else if (wp == MULTICAST_LABELED_STREAM) {
				pvm_upkint(&l, 1, 1);
				pvm_upkbyte(labelStreamLibname, l, 1);
				labelStreamLibname[l] = '\0';
				//set output port library name
				setOPLibName(outputPort, toFilter, labelStreamLibname);
				//load output port library for ls
				if (loadOPMLSData(outputPort, toFilter) == -1){
					char msg[1000];
					sprintf(msg,"could not load MLS shared library %s", labelStreamLibname);
					pvm_initsend(PvmDataRaw);

					pvm_pkbyte(msg, strlen(msg), 1);
					pvm_send(pvm_parent(), MSGT_FERROR);
					return -1;
				}
			}
			else {
				//if not LS, we needa know who will be the first instance to receive msgs
				pvm_upkint(&firstInstanceToWrite, 1, 1); //the first instance to write
				setOPNextToSend(outputPort, toFilter,  firstInstanceToWrite);
			}

		}

		// and we finally add the port to our filterData structure
		addFDOutputPort(fData, outputPort);

		//free pointers
		free(outTids);
		free(portName);
		free(writePolicyName);
	}

	// foreach InputPort
	for(i = 0; i < fData->numInputPorts; i++) {
		int nInHosts = 0;
		int *inTids  = NULL;
		int inTag;
		char portName[MAX_PTNAME_LENGTH + 1];
		int l;

		InputPort *inputPort =  createInputPort();

		//get the portName
		pvm_upkint(&l, 1, 1);
		pvm_upkbyte(portName, l, 1);
		portName[l] = '\0';
		setIPName(inputPort, portName);

		// receive the number of tids of this port
		pvm_upkint(&nInHosts, 1, 1); // number of instances connected to this port
		setIPNumSources(inputPort, nInHosts);

		// get the tids
		inTids = (int *) malloc(sizeof(int)*nInHosts);
		pvm_upkint(inTids, nInHosts, 1); //get tids
		setIPTidsSources(inputPort, inTids);
		free(inTids);

		pvm_upkint(&inTag, 1, 1); //the port tag
		setIPTag(inputPort, inTag);
		int hasLabel;
		pvm_upkint(&hasLabel, 1, 1);

		toFilter = 0;
		// if it receives from a LS or MLS we have to have the library with
		// the function that extracts the label
		if (hasLabel){
			setIPLS(inputPort, 1);
			pvm_upkint(&l, 1, 1);
			pvm_upkbyte(labelStreamLibname, l, 1);
			labelStreamLibname[l] = '\0';
			//set output port library name
			setIPLibName(inputPort, toFilter, labelStreamLibname);
			//load output port library for ls
			if (loadIPLSData(inputPort, toFilter) == -1 ){
				char msg[1000];
				sprintf(msg, "could not load LS shared library %s", labelStreamLibname);
				pvm_initsend(PvmDataRaw);

				pvm_pkbyte(msg, strlen(msg), 1);
				pvm_send(pvm_parent(), MSGT_FERROR);
				return -1;
			}
		}

		//finally add the port to our filterData
		addFDInputPort(fData, inputPort);
	}


	free(tids);
	free(hostname);
	free(filterName);
	free(libName);
	free(labelStreamLibname);

	return 1;
}

/// Run the filter
void runFilter(int argc, char **argv) {
	int i;
	char eowMsg[] = "EOW";
	int currentWork = 0;

	fd = createFilterData();
	if (recvFilterData(fd) == -1){
		/// \todo An error ocourred! We should notify manager of this!!
		return;
	}
	//
	setFDArgc(fd, argc);
	setFDArgv(fd, argv);

	// load filter library, first try loading as a C library
	if (loadFDLibFunctions(fd) == -1){
		// if it does not work, try lods the Event API
		if (loadEventAPI(fd) == -1){
			// if it dosn't work, load as C++ libriry through the C++ API
			if (loadCppAPI(fd) == -1) {
				char msg[1000];
				sprintf(msg, "could not load shared library %s", fd->libdata.name);

				sendFilterError(msg);
				return;
			}
		}
	}

	int parentTid = getFDParentTid(fd);
#ifdef VOID_TERM
	tdd = initTerminationDetection();
	if(tdd == NULL) {
		/// \todo An error ocourred! We should notify manager of this!!
		return;
	}
#endif

	//initCache(fd->id, fd->myRank);

	// now, we start working!
	// loop getting work, till we get EOF.
	// for each appendWork received, calls init, process, finalize
	while (1) {
		// before init, we must reset all ports, so user get valid data
		// Coutinho: this must be done before the UOW recieve because after that,
		// the other filters are already running AND sending vaid data.

		/// \bug Resetting the ports even before receiving the UOW ISN'T SAFE.
		/// Before we even recieve the UOW, the other filters can already
		/// started to run and sending valid data to this filter. The only safe
		/// way is: send EOW at the end of finalizeFilter() and only drain
		/// data from the channels that doesn't recieved a EOW until recieve it

		// reset all input ports. They may be used in the next work
		for(i = 0; i < fd->numInputPorts; i++) {
			//resetInputPort(fd->inputPorts[i]);
			(fd->inputPorts[i])->numEowRecv = 0;
		}
		// reset all output ports. They may be used in the next work
		for(i = 0; i < fd->numOutputPorts; i++) {
			resetOutputPort(fd->outputPorts[i]);
		}


		//get message from manager, hopefully a work
		int bufId = pvm_recv(parentTid, 0);
		int msgSize, msgType;
		pvm_bufinfo(bufId, &msgSize, NULL, NULL);
		pvm_upkint(&msgType, 1, 1);

		//msg type can be either WORK, EOF or FT
		//leave if we get EOF
		if(msgType == MSGT_EOF) {
#ifdef ATTACH
			// I'm a attached filter.. and dont realy need die now
			// so i'm going wait for any that would like attach me
			// to another pipeline
			if(fd->attached){
					printf("I'm filter %s and i cant die\n", fd->name);
					destroyFilterDataAfterEOF(fd);
					fd = createFilterData();
					if (recvFilterData(fd) == -1){
						/// \todo An error ocourred! We should notify manager of this!!
						return;
					}
#ifdef VOID_INST
#ifdef BMI_FT
					restoreUserStates(fd->instData);
					destroyUserStateBackup();
#endif
#endif

					// update my parent Tid
					parentTid = getFDParentTid(fd);

					continue;
			}else{
#endif
				break;
#ifdef ATTACH
			}
#endif
		}

		//get and set work
		int workSize = msgSize - sizeof(int);
		void *work = malloc(workSize);
		pvm_upkbyte((char *)work, workSize, 1);


		// For now, restart the cache for each work, but to do fault tolerance
		// with multiple appendWorks(), we will need to make the cache become
		// capable of saving/recovering multiple works. When the cache became
		// capable of that we will init it only once and use cacheSetCurrentWork().

		// Restarting the cache should be done after the main lopp break,
		// otherwise we will finalize this process without cleaning the cache.
//		initCache(fd->id, fd->myRank);
//		cacheSetCurrentWork(currentWork);

		/// \todo put a signal handler to clean the cache on SIGTERM recievement

		// lets run
#ifdef VOID_INST
		InstData *inst = fd->instData;
		//enter init state
		instSwitchState(inst, &inst->voidStates[TIMER_INIT]);
#endif
#ifdef VOID_TRACER
		// Enter init state
		trcEnterState( fd->trcData, VT_PROC_INIT );
#endif


		callFDInit(fd, work, workSize);
//		int weUseTasks = cacheGetUseTasks();
/*
#ifdef VOID_FT
		if (weUseTasks) {
			// one fault has occurred: filters must begin in a consistent global state

			// Tell manager that we use tasks.
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&weUseTasks, 1, 1);
			// Piggback :-) in this message the terminated tasks list
			TaskIdList *finishedTaskIdList = getFinishedTasks();
			packTaskIdList(finishedTaskIdList);
			taskIdListDestroy(finishedTaskIdList);
			pvm_send(parentTid, 0);

			// get if we need to forward task creation and finalize messages
			pvm_recv(parentTid, 0);
			int needToForwardTaskMsgs = -1;
			pvm_upkint(&needToForwardTaskMsgs, 1, 1);
			cacheSetForwardTaskMsgs(needToForwardTaskMsgs);
			// Receive global state (global terminated tasks list)
			TaskIdList *globalTaskIdList = (TaskIdList *)unpackTaskIdList();

			// Recover tasks checkpoints and restart interrupted tasks (recovery callbacks must be registered)
			cacheRecoverTasks(globalTaskIdList);
			taskIdListDestroy(globalTaskIdList);
		} else {
#endif
			// Tell manager that we don't use tasks
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&weUseTasks, 1, 1);
			pvm_send(parentTid, 0);

			// It will respond if we need to forward task creation messages
			pvm_recv(parentTid, 0);
			int needToForwardTaskMsgs = -1;
			pvm_upkint(&needToForwardTaskMsgs, 1, 1);
			cacheSetForwardTaskMsgs(needToForwardTaskMsgs);
#ifdef VOID_FT
		}
#endif
*/

#ifdef VOID_INST
		//leave init and enter process
		instSwitchState(inst, &inst->voidStates[TIMER_PROC]);
#endif
#ifdef VOID_TRACER
		// Leave init, enter process
		trcLeaveState( fd->trcData );
		trcEnterState( fd->trcData, VT_PROC_PROC );
#endif

		callFDProcess(fd, work, workSize);
#ifdef VOID_INST
		//leave process and enter VOID
		instSwitchState(inst, &inst->voidStates[TIMER_VOID]);
#endif

		// here we have run init and process. We gotta close all doors before finalizing
		// send eow to all outputStream
		for(i = 0; i < fd->numOutputPorts; i++) {

        //close each branch of the (multi)stream
    	       	  if (fd->outputPorts[i]->state == OP_STATE_OPEN){
         		   closeOutputPort(fd->outputPorts[i]);
     		   }
		}

		// and for the manager too, but here we use the type as a tag
		pvm_initsend(PvmDataRaw);
		pvm_pkbyte(eowMsg, strlen(eowMsg)+1, 1);
		pvm_send(parentTid, MSGT_EOW);

		// finalize
#ifdef VOID_INST
		instSwitchState(fd->instData, &inst->voidStates[TIMER_FINALIZE]);
#endif
#ifdef VOID_TRACER
		// Leave process, enter finalize
		trcLeaveState( fd->trcData );
		trcEnterState( fd->trcData, VT_PROC_FINALIZE );
#endif

		callFDFinalize(fd);

#ifdef VOID_TRACER
		trcLeaveState( fd->trcData );
#endif
#ifdef VOID_INST
		instSwitchState(fd->instData, &inst->voidStates[TIMER_VOID]);
#endif

		// With multiple works we will modify this
		currentWork++;
		//cacheSetCurrentWork(currentWork);
//		destroyCache();
	}

	//destroyCache();

	destroyFilterData(fd);
	pvm_exit();
}

#endif
