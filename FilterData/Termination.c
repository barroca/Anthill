#ifndef TERMINATE_C
#define TERMINATE_C

#include <stdlib.h>
#include <pvm3.h>
#include "Termination.h"
#include "FilterData.h"
#include "../Messages.h"

/**** Beging leader functions ****/

GlobalTermination *initGlobalTermination(Layout *layout, int numFilterInstances) {
	int i, j;
	int currentFilterInstance = 0;

#ifdef DEBUG
	printf("Begin initGlobalTermination\n");
#endif

	GlobalTermination *gt = (GlobalTermination *) malloc(sizeof(GlobalTermination));
	if(!gt) {
		printf("initGlobalTermination: could not allocate memory\n");
	}

	gt->currentRoundTag = 0;
	
	gt->numFilterInstances = numFilterInstances;
	gt->tfid = (TerminationFilterInstancesData *) malloc(numFilterInstances*sizeof(TerminationFilterInstancesData));
	
	// Init filter instances termination status
	for(i = 0; i < numFilterInstances; i++) {
		gt->tfid[i].terminate = NOTTERMINATED;
	}
	
	// Init filter instances tids and 
	for(i = 0; i < layout->numFilters; i++) {
		int breakLoop = 0;
		int tagBreakLoop = -1;
		FilterSpec *pFilter = layout->filters[i];
		// Verify if the filter has a stream begining a loop
		for(j = 0; j < pFilter->numInputs; j++) {
			if(pFilter->inputs[j]->breakLoop) {
				breakLoop = 1;
				tagBreakLoop = pFilter->inputs[j]->tag;
				break;
			}
		}
		FilterPlacement pfilterPlacement = layout->filters[i]->filterPlacement;
		for(j = 0; j < pfilterPlacement.numInstances; j++, currentFilterInstance++) {
			gt->tfid[currentFilterInstance].filterInstanceTid = pfilterPlacement.tids[j];
			gt->tfid[currentFilterInstance].breakLoop = breakLoop;
			gt->tfid[currentFilterInstance].tagBreakLoop = tagBreakLoop;
		}
	}

#ifdef DEBUG
	printf("End initGlobalTermination\n");
#endif
	
	return gt;
}

int findFilterInstId(filterInstTid) {
	int i;

	for(i = 0; i < gt->numFilterInstances; i++) {
		if(gt->tfid[i].filterInstanceTid == filterInstTid)
			return i;
	}

	return -1;
}

int verifyGlobalTermination(int filterInstTid, int filterInstTag) {
	int filterInstId, i;
	int globalTermination = 1;

#ifdef DEBUG
	printf("Begin verifyGlobalTermination\n");
#endif

	filterInstId = findFilterInstId(filterInstTid);
	if(filterInstId == -1) {
		printf("verifyGlobalTermination: error finding tid: %d", filterInstTid);
		return -1;
	}
	
	if(filterInstTag == gt->currentRoundTag) {
		gt->tfid[filterInstId].terminate = TERMINATED;
		// Verify if all filterInstances terminated
		for(i = 0; i < gt->numFilterInstances; i++) {
			if(gt->tfid[i].terminate != TERMINATED) {
				globalTermination = 0;
				break;
			}
		}
		if(globalTermination) {
			// Send EOW to specified filter instances
#ifdef DEBUG
			printf("verifyGlobalTermination: all filterInstances terminated\n");
#endif
			for(i = 0; i < gt->numFilterInstances; i++) {
				if(gt->tfid[i].breakLoop) {
					int msgType = MSGT_EOW;
					int taskId = -1;
					pvm_initsend(PvmDataRaw);
					pvm_pkint(&msgType, 1, 1);
					pvm_pkint(&taskId, 1, 1);
					pvm_send(gt->tfid[i].filterInstanceTid, gt->tfid[i].tagBreakLoop);
				}
			}
		}
	}else if(filterInstTag > gt->currentRoundTag) {
		gt->currentRoundTag = filterInstTag;
		for(i = 0; i < gt->numFilterInstances; i++) {
			gt->tfid[i].terminate = NOTTERMINATED;
		}
		gt->tfid[filterInstId].terminate = TERMINATED;
#ifdef DEBUG
		printf("verifyGlobalTermination: filter instance %d terminated\n", filterInstId);
#endif
	}

#ifdef DEBUG
	printf("End verifyGlobalTermination\n");
#endif
	
	return -1;
}

/**** End leader functions ****/


/**** Begin filter functions ****/

TerminationDetection *storeNeighborTids(TerminationDetection *tdd, int numNeighbors, int *neighborTids, int portTag) {
	int i, j;
	
	for(i = 0; i < numNeighbors; i++) {
		for(j = 0; j < tdd->numNeighbors; j++) {
			if(tdd->nd[i].neighborTid == neighborTids[i])
				break;
		}
		// Diferent tid, store it.
		if(j == tdd->numNeighbors) {
			tdd->nd[tdd->numNeighbors].neighborTid = neighborTids[i];
			tdd->nd[tdd->numNeighbors].neighborPortTag = 3;
			tdd->nd[tdd->numNeighbors].neighborTerm = NOTTERMINATED;
			tdd->nd[tdd->numNeighbors].neighborStreamId = tdd->numNeighbors;
			tdd->numNeighbors++;
		}
	}

	return tdd;
}
//ITAMAR
TerminationDetection *initTerminationDetection() {
	int i, j;
	int numPorts = fd->numInputPorts + fd->numOutputPorts;

#ifdef DEBUG
	printf("Begin initTerminationDetection\n");
#endif
	
	TerminationDetection *tdd = (TerminationDetection *) malloc(sizeof(TerminationDetection));
	if(!tdd) {
		printf("initTerminationDetection: could not allocate memory\n");
		return NULL;
	}
	
	tdd->localTag = 0;
	tdd->status = NOTPARTICIPATING; 
	tdd->numNeighbors = 0;

	// Find out the number of neighbors
	tdd->nd = (NeighborData *) malloc(numPorts*MAXINSTANCES*sizeof(NeighborData));
	for(i = 0; i < fd->numInputPorts; i++) {
		// Input neighbors
		tdd = storeNeighborTids(tdd, fd->inputPorts[i]->numSources, fd->inputPorts[i]->tidsSources, fd->inputPorts[i]->tag);
	}

	for(i = 0; i <fd->numOutputPorts; i++) {
		// Output neighbors
		for(j = 0; j < fd->outputPorts[i]->numToSend; j++){//ITAMAR
			tdd = storeNeighborTids(tdd, fd->outputPorts[i]->numDestinations[j], 			fd->outputPorts[i]->tidsDestinations[j], fd->outputPorts[i]->tag);
		}
	}

	tdd->numStreams = numPorts;
	tdd->stream = (int *) malloc((tdd->numStreams)*sizeof(int));
	// Init all streams as FULL
	for(i = 0; i < tdd->numStreams; i++) {
		tdd->stream[i] = FULL;
	}

#ifdef DEBUG
	printf("End initTerminationDetection\n");
#endif
	
	return tdd;
}

void sendTerminationToNeighbors() {
	int i;
	int type = MSGT_INITTERM;

	for(i = 0; i < tdd->numNeighbors; i++) {
		// send a new detection round to all neighbors
		pvm_initsend(PvmDataRaw);
		pvm_pkint(&type, 1, 1); // message type
		pvm_pkint(&tdd->localTag, 1, 1); // local tag
		pvm_send(tdd->nd[i].neighborTid, tdd->nd[i].neighborPortTag); //we send with the right tag
	}

}

void sendTerminationToLeader() {

	// send local termination to leader
	pvm_initsend(PvmDataRaw);
	pvm_pkint(&tdd->localTag, 1, 1); // local tag
	pvm_send(pvm_parent(), MSGT_LOCALTERM); //we send with the right tag
}


/// Begin a new termination detection round
int beginTerminationDetection() {

#ifdef DEBUG
	printf("Begin Termination Detection\n");
#endif
	
	tdd->localTag++;

	sendTerminationToNeighbors();

	tdd->status = PARTICIPATING;

	return 1;
}

int findNeighborId(int neighborTid) {
	int i;

	for(i = 0; i < tdd->numNeighbors; i++) {
		if(tdd->nd[i].neighborTid == neighborTid) {
			return tdd->nd[i].neighborStreamId;
		}
	}

	return -1;
}

// There is a termination detection round created.
int terminationDetectionRound(int neighborTid, int neighborTag) {
	int	neighborId, i;
	int ITerminated = 1;

#ifdef DEBUG
	printf("Begin terminationDetectionRound\n");
#endif
	
	// Find wicth neighbor sent a termination detection round
	neighborId = findNeighborId(neighborTid);
	if(neighborId == -1) {
		printf("terminationDetectionRound: error finding tid: %d", neighborTid);
		return -1;
	}
	// Mark stream as empty
	tdd->stream[neighborId] = EMPTY;

	if(neighborTag > tdd->localTag) {
		// I am not participating of the current termination detection round
		tdd->localTag = neighborTag;
		for(i = 0; i < tdd->numNeighbors; i++) {
			tdd->nd[i].neighborTerm = NOTTERMINATED;
		}
		tdd->nd[neighborId].neighborTerm = TERMINATED;
		tdd->status = PARTICIPATING;
		sendTerminationToNeighbors();
#ifdef DEBUG
		printf("terminationDetectionRound: I was not participating\n");
#endif
	} 
	
	if(neighborTag == tdd->localTag) {
		// I was participating. Am I terminated?
		tdd->nd[neighborId].neighborTerm = TERMINATED;

#ifdef DEBUG
		printf("terminationDetectionRound: I was participating\n");	
#endif
		
		for(i = 0; i < tdd->numNeighbors; i++) {
			if(tdd->nd[i].neighborTerm == NOTTERMINATED || tdd->stream[i] == FULL) {
				ITerminated = 0;
				break;
			}
		}
		if(ITerminated) {
#ifdef DEBUG
			printf("terminationDetectionRound: I terminated\n");
#endif
			sendTerminationToLeader();
		}
	}

#ifdef DEBUG
	printf("End terminationDetectionRound\n");
#endif

	return 1;
}

int updateTermStreamToEmpty(int neighborTid) {
	int	neighborId;

#ifdef DEBUG
	printf("updateTermStreamToEmpty: I am not participating anymore\n");
#endif
	
	// Find witch stream is empty
	neighborId = findNeighborId(neighborTid);

	if((tdd->status == PARTICIPATING) /*&& (tdd->nd[neighborId].neighborTerm == NOTTERMINATED)*/) {
		tdd->stream[neighborId] = FULL;
		tdd->status = NOTPARTICIPATING;
	}

	return 1;
}


/**** End filter functions ****/

#endif
