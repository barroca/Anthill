#ifndef FILTER_H
#define FILTER_H


#include "constants.h"

#define VOID_3

// Coutinho: If a c++ filter use this file, tell it that these functions are in C
#ifdef __cplusplus
extern "C" {
#endif

#define EOW -2 ///< the end of work message code
#define ERROR -3 ///< Reader functions should return this on a internal error

///< Possible fault status
#define NO_FAULT -1 ///< No fault has occurred until now
#define FAULT_OTHER_FILTER_INST -2 ///< One fault has occurred in other filter instance

typedef int RecoverCallback_t( int taskId, int *deps, int depSize, char *metadata, int metaSize );

#ifndef _PORT_HANDLERS_
#define _PORT_HANDLERS_
typedef int InputPortHandler, OutputPortHandler;
#endif

/*! \file FilterDev.h These functions all use the FilterData *fd global variable,
 * which represents the filter on the client side. These functions are all available
 * to the filter developer.
 *
 * Global variable fd is declared on FilterData.h
 */

//the filter library functions datatypes
typedef int initialize_t(void *work, int worksize);
typedef int process_t(void *work, int worksize);
typedef int finalize_t();

initialize_t initFilter;
process_t processFilter;
finalize_t finalizeFilter;

/// get the handlers of ports
InputPortHandler dsGetInputPortByName(char *name);
OutputPortHandler dsGetOutputPortByName(char *name);

OutputPortHandler dsGetOutputPortByNameSameHost(char *name);

int dsCloseOutputPort(OutputPortHandler oph); //< close an output port, sending EOW to receivers

/// funtion to probe an input port for data, returns the size of the buffer, 0 otherwise
int dsProbe(InputPortHandler ip);
/// function that reads from other filter(receive and unpack)
int dsReadBuffer(InputPortHandler ip, void *buf, int szBuf);
/// returns 0 if there is no data to be received
int dsReadNonBlockingBuffer(InputPortHandler iph, void *buf, int szBuf);

/// returns 0 if there is no data to be received
int dsReadNonBlockingBufferMalloc(InputPortHandler iph, void **bufOut, char **labelOut);

/// this function reads a buffer, of size szBuf, of any port
/// The exceptionPorts are the ports that the Anthill doesn't need to wait the EOWs
int dsReadBufferAnyPort(char **portName, char **exceptionPorts, int numExc, void *buf, int szBuf);
/// function that writes to other filter(pack and send)
int dsWriteBuffer(OutputPortHandler op, void *buf, int bufSz);

/** these functions return the number of instances on the other side of the strem
  * say, we have a filter A->B. B knows it will receive one message only from each A. He can use this number
  * to leave a loop like while (numMessages != dsGetNumWriters(iph))
  */
char **dsGetArgv();
int dsGetArgc();

int dsGetNumWriters(InputPortHandler iph);
// Get the count of instances that read from the referred port. This number is
// the sum of instances of each filter that read from the referred port.
int dsGetNumReaders(OutputPortHandler oph);

/// function to start packing data, size in bytes
int dsInitPack(int initSize);

/// function to pack data to the buffer
int dsPackData(void *data, int size);
/// function to send the packed buffer
int dsWritePackedBuffer(OutputPortHandler oph);

/// function to receive a buffer, and unpack later
int dsInitReceive(InputPortHandler iph);
/// function to unpack data
int dsUnpackData(void *buf, int size);

/// get filter name
char *dsGetFilterName();
/// get filter id
int dsGetFilterId();

/// function to get the number of one filter input ports
int dsGetNumInputPorts();
/// function to get all input ports names of one filter
char **dsGetInputPortNames();
/// function to get the number of one filter output ports
int dsGetNumOutputPorts();
/// function to get all output ports names of one filter
char **dsGetOutputPortNames();

/// get the number of writers to me that are still running
int dsGetNumUpStreamsRunning();

/// get the ammount of memory this machine has for this execution
int dsGetMachineMemory();
/// the number of brothers I have in this machine + 1
/// brothers are the same filters as me, so say, you have a filter A
/// running here, and 2 Bs runnning here, if A calls this hell get 1,
/// and B will get 2.
int dsGetLocalInstances();

/// Function that returns which brother am I
int dsGetMyRank();
/// returns the total intances of this filter(me + mybrothers)
int dsGetTotalInstances();
/// kill all filters and finish void, user calls this to exit the system abnormally
int dsExit(char *mesg);

/** Task functions */
void dsUseTasks();
/// creates a new task
int dsCreateTask(int taskId, int *deps, int depSize, char *metadata, int metaSize);
/// ends a task
int dsEndTask(int taskId);
///get the current task we are working on
int dsGetCurrentTask();
int dsSetCurrentTask(int taskId);
int *dsGetTaskDeps(int taskId, int *depsSz);
int dsGetRunningTasks( int * numTasks, int ** taskList );
int *dsGetFinishedTasks(int *numTasks);

int dsRegisterRecoverCallback( RecoverCallback_t * callback );

// data functions
int dsPutData(char *id, void *val, int valSize);
int dsRemoveData(char *id);
void *dsGetData(int taskId, char *id, int *valSz);

// instrumentation stuff
void dsInstSetStates(char **states, int numStates);
void dsInstSwitchState(int stateId);
void dsInstEnterState(int stateId);
void dsInstLeaveState();

#ifdef BMI_FT
void dsInstSaveIntermediaryState();
#endif

/// Returns the status of the fault
int dsAdvertiseFault();

/// Returns 1 if the filter is the last one in the pipeline or 0 otherwise
int dsLastFilter();

#ifdef __cplusplus
}
#endif

#endif
