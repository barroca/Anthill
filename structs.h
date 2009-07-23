/**
 *Modificado Itamar 03/10/2006
 */

#ifndef _ESTRUTURAS_H
#define _ESTRUTURAS_H

#include "constants.h"
#include "Hosts.h"

/* Main structures */
typedef struct _filter_spec_ * _filter_spec_ptr;
typedef struct _stream_spec_ * _stream_spec_ptr;

/** This is the Manager state type */
typedef struct _ManagerState {
	int numWorksSent; //< the number of append works sent
	int oldestWork; //< the id of the oldest work appended to the pipe
} ManagerState;

/* FilterSpec ******************************************************************/
typedef struct {
	int numInstances; ///< this filter has this number of instances
	char *hosts[MAXINSTANCES]; ///< the places this filter runs, points to HostsStruct hostname
	int tids[MAXINSTANCES]; ///< the tids of the running instances
	char *resources[MAXINSTANCES];
} FilterPlacement;

#ifdef MATLAB
typedef struct{
	char argType[MAX_ARGTYPE_NAME_LENGTH];
	char deserializeFunctionName[MAX_FUNCNAME_LENGTH];
	char deserializeLibName[MAX_LNAME_LENGTH];
	int userArgIndex;
	char inputType[MAX_INPUTYPE_NAME_LENGTH];
	int parameterOrder;
	int mesgIndexIn;
} ArgInput;

typedef struct{
	char argType[MAX_ARGTYPE_NAME_LENGTH];
	char serializeFunctionName[MAX_FUNCNAME_LENGTH];
	char serializeLibName[MAX_LNAME_LENGTH];
	int parameterOrder;
	int mesgIndexOut;
} ArgOutput;

typedef struct{
	char argType[MAX_ARGTYPE_NAME_LENGTH];
	char deserializeFunctionName[MAX_FUNCNAME_LENGTH];
	char deserializeLibName[MAX_LNAME_LENGTH];
	int userArgIndex;
	char inputType[MAX_INPUTYPE_NAME_LENGTH];
	char serializeFunctionName[MAX_FUNCNAME_LENGTH];
	char serializeLibName[MAX_LNAME_LENGTH];
	int parameterOrder;
	int mesgIndexIn;
	int mesgIndexOut;
} ArgInputOutput;

typedef struct{
	char matlablibname[MAX_LNAME_LENGTH];
	char functionName[MAX_FUNCNAME_LENGTH];
	int numOutputs;
	int numOutputsAdded;
	ArgOutput outputs[MAX_MATLAB_OUTPUTS];
	int numInputs;
	int numInputsAdded;
	ArgInput inputs[MAX_MATLAB_INPUTS];
	int numInputOutputs;
	int numInputOutputsAdded;
	ArgInputOutput inputOutputs[MAX_MATLAB_INPUTOUTPUS];
	int isFirstFilter; // 1, if the filter is the first filter in the pipeline, otherwise, 0
} MatLabDesc;
#endif

/// FilterSpec:
typedef struct _filter_spec_ {
	char name[MAX_FNAME_LENGTH]; ///< filter name, duh !!!!
	char libname[MAX_LNAME_LENGTH]; ///< the library name used by this filter
	int numInputs; ///< number of inputs this filter has
	_stream_spec_ptr inputs[MAXINPSTREAMS]; ///< array of stream pointers
	int numOutputs; ///< number of outputs this filter has
	_stream_spec_ptr outputs[MAXOUTSTREAMS]; ///< array of stream pointers
	int useTasks;	///< 1: filter use tasks, 0: otherwise (set in execution time)
	FilterPlacement filterPlacement; ///< this represents the places this filter runs, see above
#ifdef ATTACH
	int attached; ///< 1: if this is a attached filter, 0: otherwise
	int attach;
	char *command; ///< the command to execute the attached filter or NULL, if the filter isn't attached
#endif
#ifdef MATLAB
	MatLabDesc matLabDesc;
#endif
#ifdef BMI_FT
	int faultStatus; ///< NO_FAULT, FAULT_OTHER_FILTER_INST, or filter instance rank
	int lastFilter; ///< 1 if this is the last filter, 0, otherwise
#endif
} FilterSpec;
/*******************************************************************************/

/// StreamSpec: Define the main stream options
typedef struct _stream_spec_ {
	char name[MAX_SNAME_LENGTH]; ///< the stream name
	_filter_spec_ptr fromFilter; ///< we point to the filter who writes to this stream
	char fromPortName[MAX_PTNAME_LENGTH]; ///< and this is the port
	_filter_spec_ptr toFilter[MAXFILTERS]; ///<array of  filters which reads from this stream //ITAMAR
	char toPortName[MAXFILTERS][MAX_PTNAME_LENGTH]; ///<array containing the output port name for each filter connected to this (multi)stream //HAWKS
	char writePolicyName[MAXFILTERS][MAX_PLNAME_LENGTH]; ///< this is the write policy used by each filter //ITAMAR
	char readPolicyName[MAX_PLNAME_LENGTH]; ///< this is not used
	int tag;///< tag that identify the stream 
	char lsLibName[MAXFILTERS][MAX_LNAME_LENGTH]; ///< if any write policy is labeled stream, this is the library names //HAWKS
	int numToSend;//ITAMAR how much 'to' one stream has
//#ifdef VOID_TERM
	int breakLoop; ///< if this stream is a loop component and the loop need to be broke here
//#endif
} StreamSpec;


typedef struct _layout_ {
	HostsStruct *hostsStruct; ///< hosts structure, contains the hosts and its attributes
	int numFilters; ///< total number of filters
	FilterSpec *filters[MAXFILTERS]; ///< array of filters pointers
	int numStreams; ///< the streams
	StreamSpec *streams[MAXSTREAMS]; ///< array of stream pointers

	ManagerState managerState; //< the state of the manager

	char **argvSpawn; //< this is what we send to the filters being spawned
	char cwd[MAX_CWD_LENGTH+1]; //< the program directory
	char xName[MAX_XNAME_LENGTH+1]; //< the executable name
	char command[MAX_COM_LENGTH+1]; //< the command we execute in the program, cwd + xname
} Layout;


#endif
