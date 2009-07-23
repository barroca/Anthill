/**
 *28/09/2006 Changes Itamar - changes in output port struct
 *
 *05/10/2006 Changes Felipe - changing the headers of
 *            the functions that set the output ports parameters
 */

#ifndef _PORTS_H
#define _PORTS_H

#include "../constants.h"
#include "Policies.h"
#include "hashDefault.h"
//TODO: nao esquecer que na hora de dar o set no nome da biliboteca
//temos que passar a mesma no formato lib*.so

#ifndef _PORT_HANDLERS_
#define _PORT_HANDLERS_
// output port handler
typedef int OutputPortHandler; ///< index of the OutputPort int the FilterData, used for every operations the filter make to a stream.
typedef int InputPortHandler; ///< Index of InputPort in the FilterData
#endif

#define OP_STATE_OPEN 1
#define OP_STATE_CLOSED 0

/* Output port stuff *************************************************/
/* Labeled stream data */
typedef void (GetLabel)(void *msg, int tamMsg, char label[]); ///< getLabel prototype
typedef int (Hash)(char *label, int image); ///< Hash function prototype
typedef void (MLSHash)(char *label, int  image, int destArray[]); ///< Multicast LS hash function prototype

typedef struct __LabeledStreamData {
  	void *lsHandler; ///< handler for dynamic library that contains hash and getLabel
	char libname[MAX_LNAME_LENGTH+1];	///< the library name with the functions
	Hash *hash; ///< pointer to the hash function, if used
	MLSHash *mlshash; ///< pointer to the mls hash, if used
	GetLabel *getLabel; ///< getLabel pointer
} LabeledStreamData;


//output port
//The output port will be changed to permit that a filter send multiples
//messages by a unique port to multiples filters
typedef struct {
	char *name; ///< OutputPort Name
	int numDestinations[MAXFILTERS]; ///< Array (filter index -> num instances). In each element, the number of destinations connected to a branch of the (multi)stream //ITAMAR
	int *tidsDestinations[MAXFILTERS];///<each line represents a filter and each columm represents a tid //ITAMAR
    	int numToSend;///<number of filters that receive the same message //ITAMAR
	writePolicy_t writePolicy[MAXFILTERS]; ///<each columm represents the write policy used by this port to send for a specified filter //ITAMAR
	int tag;  ///< each stream has your tag
	int nextToSend[MAXFILTERS]; ///<who is the next instance of the filter to get our messsages? //ITAMAR
	LabeledStreamData lsData[MAXFILTERS]; ///< the labeled stream data //ITAMAR
	int state;

	//TODO: int state[MAXFILTERS]; ///< the port state //ITAMAR
}OutputPort;

///constructor
OutputPort *createOutputPort(int numToSend);

///destroyer: must release the libraries
void destroyOutputPort(OutputPort *o);

///closes the door, must be called before finalizing if port is not closed
void closeOutputPort(OutputPort *o);

///resets the output port, i.e open it
void resetOutputPort(OutputPort *o);


/// set stuff, user can free the second argument after that
void setOPState(OutputPort *op, int state); // FELIPE
void setOPName(OutputPort *op, const char *name);
void setOPNumDestinations(OutputPort *op, int toFilter, int num); // FELIPE
void setOPWritePolicy(OutputPort *op, int toFilter, writePolicy_t p); // FELIPE
void setOPNextToSend(OutputPort *op, int toFilter, int next); // FELIPE
void setOPLibName(OutputPort *op, int toFilter, const char *libname);  // FELIPE
void setOPTidsDestinations(OutputPort *op, int toFilter, const int *tids); // FELIPE
void setOPTag(OutputPort *op, int tag);
void setOPNumToSend(OutputPort *op, int numToSend);

/// This function loads the libraries of the labeled stream, returns 1 on success 0 otherwise
int loadOPLSData(OutputPort *op, int toFilter);
int loadOPMLSData(OutputPort *op, int toFilter);
/* End outputport stuff ***************************************************/


/* Input port stuff *************************************************/

typedef struct {
	char *name; ///< InputPort name
	int numSources; ///< we receive from this number of sources
	int *tidsSources;  ///< the array of tids we receive from
	int tag; //< the stream tag
	int numEowRecv; ///< the number of EOW received so far
	int ls;
	LabeledStreamData lsData[MAXFILTERS]; ///< the labeled stream data //ITAMAR
} InputPort;

///constructor
InputPort *createInputPort();
///destroyer: must release the libraries
void destroyInputPort(InputPort *o);

///reset an input port. Must be used before starting work
void resetInputPort(InputPort *ip);

// set stuff, user can free the second argument after that
void setIPName(InputPort *ip, const char *name);
void setIPNumSources(InputPort *ip, int num);
void setIPTag(InputPort *ip, int tag);
void setIPTidsSources(InputPort *ip, const int *tids);
void setNumEowRecv(InputPort *ip, int numRecv);
/*********************************************************************/
void setIPLibName(InputPort *op, int toFilter, const char *libname);  
int loadIPLSData(InputPort *i, int toFilter);
void setIPLS(InputPort *i, int ls);

#endif
