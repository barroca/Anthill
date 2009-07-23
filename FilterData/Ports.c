#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pvm3.h>
#include "Ports.h"
#include "../Messages.h"


/**
 * \file Ports.c Input port and output port implementations.
 *
 * \todo Make message buffering.
 */


///constructor
OutputPort *createOutputPort(int numToSend){
	OutputPort *outputPort = (OutputPort *) malloc(sizeof(OutputPort));
	if(outputPort == NULL){
		printf("createOutputPort: Could not alocate memory\n");
		exit(1);
	}

	outputPort->numToSend = numToSend;
	setOPState(outputPort, 1);
	return outputPort;
}

///destroyer: release the libraries and free the outputPort struct
void destroyOutputPort(OutputPort *o){
	int i;
	//if we are labeled stream or labeled multicast, we free the library
	for( i = 0; i < o->numToSend; i++ )
	{
		if ((o->writePolicy[i] == LABELED_STREAM) || (o->writePolicy[i] == MULTICAST_LABELED_STREAM)){
			dlclose(o->lsData[i].lsHandler);
		}
	}

  	if(o == NULL){
		printf("destroyOutputPort: We cant destroy a null outputPort\n");
		exit(1);
	}

	free(o);
}

///resets the port, openin it
void resetOutputPort(OutputPort *op){
	setOPState(op, OP_STATE_OPEN);
}

///close the port, but does not destroy it
void closeOutputPort(OutputPort *op){
	int i;
	if (op->state == OP_STATE_CLOSED)
		return;

	//send our EOW to listeners on the other side
	int msgType = MSGT_EOW;
	int taskId = -1;

	pvm_initsend(PvmDataRaw);
	pvm_pkint(&msgType, 1, 1);
	pvm_pkint(&taskId, 1, 1);

	for( i = 0; i < op->numToSend; i++ ){
		pvm_mcast(op->tidsDestinations[i], op->numDestinations[i], op->tag);
	}

	setOPState(op, OP_STATE_CLOSED);
}

/// set the number of destination filters of this output port
void setOPNumToSend(OutputPort *op, int numToSend)
{
	op->numToSend = numToSend;
}

//set the port state(open, closed)
void setOPState(OutputPort *op, int state){
	op->state = state;
}

// set name of thip output port
void setOPName(OutputPort *op, const char *name){
	op->name = (char *)strdup(name);
}

// set number of destinations connected to this port
void setOPNumDestinations(OutputPort *op, int toFilter, int num){
	op->numDestinations[toFilter] = num;
}

// set the used policy to this port
void setOPWritePolicy(OutputPort *op, int toFilter, writePolicy_t p){
	op->writePolicy[toFilter] = p;
}

// Change the next one filter to receive mensage
void setOPNextToSend(OutputPort *op, int toFilter, int next){
	op->nextToSend[toFilter] = next;
}

// set the tag
void setOPTag(OutputPort *op, const int tag){
	op->tag = tag;
}

void setOPLibName(OutputPort *op, int toFilter, const char *libname){
  	int libnameSize = strlen(libname);

	// case the host name be bigger than our host name limit
	if((libnameSize + 1) >  MAX_HNAME_LENGTH){
		printf("Warning: Lib name out of bounds\n");
		memcpy(op->lsData[toFilter].libname, libname,  MAX_LNAME_LENGTH);
		op->lsData[toFilter].libname[MAX_LNAME_LENGTH -1] = '\0';
	}else{
		memcpy(op->lsData[toFilter].libname, libname, libnameSize +1);
	}
}

// coping the pvm tid of all process connected to this outputPort
void setOPTidsDestinations(OutputPort *op, int toFilter, const int *tids){
  	int i;
  	// the number of tids needs to be seted in op->numDestinations before copy
	op->tidsDestinations[toFilter] = (int *) malloc(sizeof(int) * op->numDestinations[toFilter]);

	if(op->tidsDestinations[toFilter] == NULL){
		printf("Could not allocate memory\n");
	}
	for(i = 0; i < op->numDestinations[toFilter]; i++){
		op->tidsDestinations[toFilter][i] = tids[i];
	}
}


// --------------------------InputFunctions-----------------------------//

///constructor
InputPort *createInputPort(){
	InputPort *inputPort = (InputPort *) malloc(sizeof(InputPort));
	if(inputPort == NULL){
		printf("createInputPort: Could not allocate memory\n");
		exit(1);
	}

	inputPort->numEowRecv = 0;
	inputPort->ls = 0;

	return inputPort;
}

///destroyer: free the InputPort
void destroyInputPort(InputPort *o){
	if(o == NULL){
		printf("destroyInputPort: We cant destroy a null outputPort\n");
		exit(1);
	}

	free(o);
}

void resetInputPort(InputPort *ip){
	//flush everything
	while (pvm_probe(-1, ip->tag) != 0){
		pvm_recv(-1, ip->tag);
	}

	ip->numEowRecv = 0;
}

// set name of thip input port
void setIPName(InputPort *ip, const char *name){
	ip->name = (char *)strdup(name);
}

// set the number of process connecteted whith this input
void setIPNumSources(InputPort *ip, int num){
	ip->numSources = num;
}

// set the number of process connecteted whith this input
void setIPLS(InputPort *ip, int ls){
	ip->ls = ls;
}


// set the stream tag that this input receive
void setIPTag(InputPort *ip, int tag){
	ip->tag = tag;
}

void setIPTidsSources(InputPort *ip, const int *tids){
  	int i;
	// case we have an input whithout source
	if(ip->numSources <= 0 ){
		printf("Warning: the number of tids needs to be set in ip->numSource before copy\n");
		exit(1);
	}
	ip->tidsSources = (int *) malloc(sizeof(int) * ip->numSources);

	// coping tids of process connected whith this source
	for(i = 0; i < ip->numSources; i++){
		ip->tidsSources[i] = tids[i];
	}
}

int loadOPLSData(OutputPort *o, int toFilter){
	char *error = NULL;
	char *libnameLocal;
	// try loading lib in local directory, so we need 2 strings,
	// the first with ./, em the second whithout
	libnameLocal = (char*)malloc(strlen(o->lsData[toFilter].libname) + 3);
	sprintf(libnameLocal, "./%s",o->lsData[toFilter].libname);

	// get library handler
	if 	(((o->lsData[toFilter].lsHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((o->lsData[toFilter].lsHandler = dlopen(o->lsData[toFilter].libname, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "Ports.c: could not load labeled stream %s library: %s\n", o->lsData[toFilter].libname, dlerror());
			return -1;
	}

	free(libnameLocal);

	// load functions (hash and getLabelStream) from labelStream dynamic library
	// warnnig: The function simbols can be in C or C++

	//loads the hash
	o->lsData[toFilter].hash = (Hash*)dlsym(o->lsData[toFilter].lsHandler, "hash");
	if ((error = dlerror()) != NULL)  {
		o->lsData[toFilter].hash = (Hash*)dlsym(o->lsData[toFilter].lsHandler, "_Z4hashPci"); // (mangling, g++ 3.2+) using c++ symbol
		if ((error = dlerror()) != NULL)  {
		  		o->lsData[toFilter].hash = &hashDefault;
				fprintf (stderr, "Ports.c: Loading default hash: %s\n", error);
		}
	}

	//loads getLabel
	o->lsData[toFilter].getLabel = (GetLabel*)dlsym(o->lsData[toFilter].lsHandler, "getLabel");
	if ((error = dlerror()) != NULL)  {
		o->lsData[toFilter].getLabel = (GetLabel*)dlsym(o->lsData[toFilter].lsHandler, "_Z8getLabelPviPc"); // (mangling, g++ 3.2+) using c++ symbol
		if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "Ports.c: error loading getLabel: %s\n", error);
			return -1;
		}
	}
	return 1;
}

//just like the above, but used for loading mls libraries
int loadOPMLSData(OutputPort *o, int toFilter){
	char *error = NULL;
	char *libnameLocal;
	// try loading lib in local directory, so we need 2 strings,
	// the first with ./, em the second whithout
	libnameLocal = (char*)malloc(strlen(o->lsData[toFilter].libname) + 3);
	sprintf(libnameLocal, "./%s",o->lsData[toFilter].libname);

	// get library handler
	if 	(((o->lsData[toFilter].lsHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((o->lsData[toFilter].lsHandler = dlopen(o->lsData[toFilter].libname, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "Ports.c: could not load mls %s library: %s\n", o->lsData[toFilter].libname, dlerror());
			return -1;
	}

	free(libnameLocal);

	// load functions (hash and getLabelStream) from labelStream dynamic library
	// warnnig: The function simbols can be in C or C++

	//loads the mls hash
	o->lsData[toFilter].mlshash = (MLSHash*)dlsym(o->lsData[toFilter].lsHandler, "mlshash");
	if ((error = dlerror()) != NULL)  {
		/// \todo Check the mangled name of this function
//		o->lsData[toFilter].mlshash = (MLSHash*)dlsym(o->lsData[toFilter].lsHandler, "_Z4hashPci"); // (mangling, g++ 3.2+) using c++ symbol
//		if ((error = dlerror()) != NULL)  {
		  		o->lsData[toFilter].mlshash = &mlsHashDefault;
				fprintf (stderr, "Ports.c: Loading default mlshash: %s\n", error);
//		}
	}

	//loads getLabel
	o->lsData[toFilter].getLabel = (GetLabel*)dlsym(o->lsData[toFilter].lsHandler, "getLabel");
	if ((error = dlerror()) != NULL)  {
		o->lsData[toFilter].getLabel = (GetLabel*)dlsym(o->lsData[toFilter].lsHandler, "_Z8getLabelPviPc"); // (mangling, g++ 3.2+) using c++ symbol
		if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "Ports.c: error loading getLabel: %s\n", error);
			return -1;
		}
	}
	return 1;
}


void setIPLibName(InputPort *ip, int toFilter, const char *libname){
  	int libnameSize = strlen(libname);

	// case the host name be bigger than our host name limit
	if((libnameSize + 1) >  MAX_HNAME_LENGTH){
		printf("Warning: Lib name out of bounds\n");
		memcpy(ip->lsData[toFilter].libname, libname,  MAX_LNAME_LENGTH);
		ip->lsData[toFilter].libname[MAX_LNAME_LENGTH -1] = '\0';
	}else{
		memcpy(ip->lsData[toFilter].libname, libname, libnameSize +1);
	}
}



int loadIPLSData(InputPort *i, int toFilter){
	char *error = NULL;
	char *libnameLocal;
	// try loading lib in local directory, so we need 2 strings,
	// the first with ./, em the second whithout
	libnameLocal = (char*)malloc(strlen(i->lsData[toFilter].libname) + 3);
	sprintf(libnameLocal, "./%s",i->lsData[toFilter].libname);

	// get library handler
	if 	(((i->lsData[toFilter].lsHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((i->lsData[toFilter].lsHandler = dlopen(i->lsData[toFilter].libname, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "Ports.c: could not load labeled stream %s library: %s\n", i->lsData[toFilter].libname, dlerror());
			return -1;
	}

	free(libnameLocal);

	//loads getLabel
	i->lsData[toFilter].getLabel = (GetLabel*)dlsym(i->lsData[toFilter].lsHandler, "getLabel");
	if ((error = dlerror()) != NULL)  {
		i->lsData[toFilter].getLabel = (GetLabel*)dlsym(i->lsData[toFilter].lsHandler, "_Z8getLabelPviPc"); // (mangling, g++ 3.2+) using c++ symbol
		if ((error = dlerror()) != NULL)  {
			fprintf (stderr, "Ports.c: error loading getLabel: %s\n", error);
			return -1;
		}
	}
	return 1;
}
