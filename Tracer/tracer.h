#ifndef _TRACER_H_
#define _TRACER_H_

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stack/stack.h>
#include <dbLkList/dbLkList.h>

#define MAXSAVEDSTATES 1000

#define SEPARATOR " "

/* Possible atrtibute types:
 *
 * i : integer
 * s : string ( char * )
 * a : integer array ( int * ) -> first element must be the size -1 
 *                                Use iArrayToTrc( int size, int * array )
 * I : Post-informed ( on trcLeaveState ) integer.
 * S : Post-informed ( on trcLeaveState ) string.
 * A : Post-informed ( on trcLeaveState ) integer array.
 */


/** This is a state attribute */
typedef struct _TrcAttribute_ {
	char type;
	void * value;
} TrcAttribute;

/** This is a state */
typedef struct _TrcState_ {
	struct timeval initTime;    // initial time for this state
	struct timeval endTime;     // final time for this state
	struct rusage initRusage;   // initial rusage statistics for this state 
	struct rusage endRusage;    // final rusage statistics for this state 
	int numAttributes;
	int numPosAttributes;
	TrcAttribute * attributes;		// other attributes of the state
} TrcState;

/** This holds the tracing data */
typedef struct _TrcData_ {
	
	TrcState *currentState; // the state we are
	FILE * tracefile;       // FILE * to the file where tracing data will be written
	
	struct timeval initTime;
	
	int numSavedStates; 	// number of states in list
	DbLkList *savedStates;	// a list of saved states

	Stack *stackedStates;	// stack of states, so we can have recursive calls	
	int numStackedStates;

} TrcData;

/** Starts the tracing data, zeroing everything
  * @return the tracing data created
  */
TrcData *trcCreateData( char * filename );

/** Enters a specific state, setting the values of the attributes
 *  @param id = a pointer to an TrcData structure returned by a previous trcCreateData call; 
 *  @param types = a null terminated string of length = numAttributes 
 *                 specifying the types of the incoming attributes, as above:
 *    i = fixed integer
 *    s = fixed string
 *	
 */
void trcEnterState(TrcData *trc, char * types, ...);

/** Destroys the Tracing data, NULLing the pointer.
  *
  *@param trcPointer the address of the Tracing data pointer, which will be NULL after this call
  */
void trcDestroyData(TrcData **trcPointer);

/** leaves the current state, computing times for it.
 * The current state is set to the Top of the stack, which is poped
 * @param trc the tracing data
 */
void trcLeaveState(TrcData *trc, ...);

/** Saves all tracing data to a file, appending time there
  *@param filename the name of the file to save
  *@param label a label to write with the timing results
  */
void trcFlush(TrcData *trc);

int * trcIArrayToTrc( int size, int * array );

#endif
