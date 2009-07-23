/**
*Alterado: Itamar 03/10/2006
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "StreamSpec.h"
#include "FilterData/Policies.h"

// tag 0 is for internal Void messages
// tag 1 is for PvmTaskExit notifications
// tag 2 is for PvmHostDelete notifications
// tag 3 is for filter termination
static int nextTag = 4;
void resetStream(StreamSpec *s){
	s->tag = nextTag++;
}

/* Constructor and destroyer *****************************************************/
/// Constructor 
StreamSpec *createEmptyStreamSpec() {
	StreamSpec *s = (StreamSpec *) malloc(sizeof(StreamSpec));
	s->tag = nextTag++;
	s->numToSend = 0;
	return s;
}
/// Destroyer
void destroyStreamSpec(StreamSpec *s){
	//we dont need to free stuff, as they are allocated statically
	free(s);
}
/********************************************************************************/

/* Setting source and destinations **********************************************/
/// Set stream origin, ie, the filter which writes to this stream
int setFrom(StreamSpec *s, FilterSpec *filter, char *portName
#ifdef VOID_TERM
		, int breakLoop
#endif		
){
   
	
	s->fromFilter  = filter;
	strncpy(s->fromPortName, portName, MAX_PTNAME_LENGTH); 
	s->fromFilter->outputs[s->fromFilter->numOutputs++] = s;
	
	
	
#ifdef VOID_TERM
	s->breakLoop = breakLoop;
#endif
	return 1;
}
///set stream destination, ie, the filter which reads from this stream
//ITAMAR changes to permit that various 'to' of one 'from'
int setTo(StreamSpec *s, FilterSpec *filter, char *portName,  char *writePolicyName, char *polibName){
	//we are not using read policies...
	int indStream = s->numToSend;
	s->numToSend += 1;
	writePolicy_t p; //The policy change from 'setFrom' to here
	strncpy(s->writePolicyName[indStream], writePolicyName, MAX_PLNAME_LENGTH);
	s->toFilter[indStream] = filter;
	s->toFilter[indStream]->inputs[s->toFilter[indStream]->numInputs++] = s;
	strncpy(s->toPortName[indStream], portName, MAX_PTNAME_LENGTH);
	//if we are using labeled stream, we need the library name of the functions
	p = getWritePolicyByName(writePolicyName);
	if ((p == LABELED_STREAM) || (p == MULTICAST_LABELED_STREAM)){
		//if we are labeled, we need a policy library name
		if ((polibName == NULL) || (strlen(polibName) == 0))
			return 0;
		else
			strncpy(s->lsLibName[indStream], polibName, MAX_PLNAME_LENGTH);
	}
	else if (p == W_POLICY_ERROR){
		return 0;
	}
	return 1;
}

/*******************************************************************************/
