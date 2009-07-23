#ifndef __TERMINATE_H
#define __TERMINATE_H

#include "../structs.h"

#define NOTPARTICIPATING 0 ///< used to TerminationDetection->status
#define PARTICIPATING    1 ///< used to TerminationDetection->status
#define EMPTY 0 ///< used to TerminationDetection->stream
#define FULL  1 ///< used to TerminationDetection->stream
#define NOTTERMINATED 0 ///< used to TerminationDetection->nd->neighborTerm and GlobalTermination->tfid->terminate
#define TERMINATED    1 ///< used to TerminationDetection->nd->neighborTerm and GlobalTermination->tfid->terminate

/**** Filters structs ****/

typedef struct _NeighborsData_ {
	int neighborStreamId; ///< id to address stream. 0 --> numStreams
	int neighborTid; ///< neighbor pvm tid
	int neighborPortTag; ///< port tag
	int neighborTerm; ///< indicates if I have already received a termination mesg of neighbor i
} NeighborData;

typedef struct _TerminationDetection_ {
	int localTag; ///< # of termination detection round
	int status; ///< indicates if I am participating of a termination detection
	int numNeighbors; ///< my number of neighbors 
	NeighborData *nd;
	int numStreams; ///< Number of streams connected to me (input + output)
	int *stream; ///< indicates if the stream i is empty
} TerminationDetection;

/*************************/

/**** Leader structs ****/

typedef struct _TerminationFilterInstancesData_ {
	int filterInstanceTid; ///< filter instance pvm tid
	int terminate; ///< indicates if filter instance i terminated
	int breakLoop; ///< when leader detect termination, send EOW to filter instances witch breakLoop == 1
	int tagBreakLoop;
} TerminationFilterInstancesData;

typedef struct _GlobalTermination_ {
	int currentRoundTag; ///< current termination detection round tag
	int numFilterInstances; ///< number of filter instances participating of termination detection
	TerminationFilterInstancesData *tfid; ///< 0 --> numFilterInstances
} GlobalTermination;

/************************/


#ifdef TERMINATE_C
TerminationDetection *tdd; ///< Global variable that points to data of termination detection
#else
extern TerminationDetection *tdd; ///< Global variable that points to data of termination detection 
#endif

#ifdef TERMINATE_C
GlobalTermination *gt; ///< Global variable that points to data of global termination
#else
extern GlobalTermination *gt; ///< Global variable that points to data of global termination 
#endif

/**** Leader functions ****/
GlobalTermination *initGlobalTermination(Layout *layout, int numFilterInstances);
int verifyGlobalTermination(int filterInstTid, int filterInstTag);

/**** Filter functions ****/
TerminationDetection *initTerminationDetection();
int beginTerminationDetection();
int terminationDetectionRound(int neighborTid, int neighborTag);
int updateTermStreamToEmpty(int neighborTid);

#endif
