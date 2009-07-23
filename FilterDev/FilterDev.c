#include <pvm3.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "FilterDev.h"

//abstract data types
#include <Task/Task.h>
#include <DataSpace/DataSpace.h>
#include <Cache.h>

#include "../FilterData/FilterData.h"
#include "../FilterData/Termination.h"
#include "../constants.h"
#include "../Messages.h"

#define KEY_INT
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_INT


#ifdef VOID_INST
#include "../FilterData/Instrumentation.h"
#endif
#ifdef VOID_TRACER
#include <Tracer/tracer.h>
#include "tracingdefs.h"
extern int msgId;
extern int incomingMsgId;
#endif

/*
 *ITAMAR 11/10/2006 Changes to permit that one 'from' has various 'to'
 */
/** \file FilterDev.c This file implements all functions called by filters.
 *
 *  \todo Clean this file, It should be like the linux VFS, inly redirecting
 *  calls to the apropriate Anthill module.
 *
 *  \todo Change functions prefix to Anthill prefix: ahXxx().
 *
 *  \todo Purge message packing and create a wrapper to it.
 */


/// This is the send buffer.
typedef struct {
    char *buffer;
    int size;	///< number of bytes unpacked
    int maxSize;   	///< number of bytes received
} BufSt;

static BufSt bufSend = { NULL, 0, 0 };
static BufSt bufRecv = { NULL, 0, 0 };

InputPortHandler dsGetInputPortByName(char *name) {
    int i=0;

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETIPORT );
#endif

    for (i=0; i<fd->numInputPorts; i++) {
        if (0 == strcasecmp(name, fd->inputPorts[i]->name)) {
#ifdef VOID_TRACER
            trcLeaveState( fd->trcData );
#endif
            return i;
        }
    }

    fprintf(stderr, "FilterDev.c: ERROR, could not find Inputport with name %s\n", name);

#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif

    return -1;
}

OutputPortHandler dsGetOutputPortByName(char *name) {
    int i=0;

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETOPORT );
#endif

    for (i=0; i< fd->numOutputPorts; i++) {
        if (0 == strcasecmp(name, fd->outputPorts[i]->name)) {

#ifdef VOID_TRACER
            trcLeaveState( fd->trcData );
#endif
            return i;
        }
    }

    fprintf(stderr, "FilterDev.c: ERROR, could not find Outputport with name %s\n", name);

#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif

    return -1;
}

int dsCloseOutputPort(OutputPortHandler oph){

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_CLOSEOPORT );
#endif

    closeOutputPort(fd->outputPorts[oph]);

#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif

    return 1;
}

int dsGetNumWriters(InputPortHandler iph){

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETNW );
#endif

    if ((iph < 0) || (iph >= fd->numInputPorts)){
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return -1;
    }
    else {
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return fd->inputPorts[iph]->numSources;
    }
}

// Get the count of instances that read from the referred port. This number is
// the sum of instances of each filter that read from the referred port.
int dsGetNumReaders(OutputPortHandler oph){

    int readers_sum = 0, i = 0;
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETNR );
#endif

    if ((oph < 0) || (oph >= fd->numOutputPorts)){
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return -1;
    }
    else {
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif

        // Sum the number of instances of each filter that read from the referred
        // port.
        for(i=0 ; i < fd->outputPorts[oph]->numToSend; i++){

            readers_sum += fd->outputPorts[oph]->numDestinations[i];
        }
        return readers_sum;
    }
}

//ITAMAR changes to permit that one 'from' has varius 'to'
void dsMCast(OutputPort *op, int ind_branch, void *buf, int bufSz) {
    int i, type = MSGT_F2F;
//    int taskId = cacheGetCurrentTask();
	int taskId = 0;
    pvm_initsend(PvmDataInPlace);
    pvm_pkint(&type, 1, 1); //the type of the message
    pvm_pkint(&taskId, 1, 1); //task information
#ifdef VOID_TRACER
    pvm_pkint(&msgId, 1, 1); //msgId
#endif
    pvm_pkbyte((char *) buf, bufSz, 1);
#ifdef VOID_INST
    instEnterState(fd->instData, &fd->instData->voidStates[TIMER_WRITE_BLOCKED]);
#endif
    for ( i = 0; i < op->numDestinations[ind_branch]; i++ )
    {
        pvm_send( op->tidsDestinations[ind_branch][i], op->tag );
    }
    //	pvm_mcast(op->tidsDestinations, op->numDestinations, op->tag);

#ifdef VOID_INST
    instLeaveState(fd->instData);
#endif

}

void dsSend(int tid, int msgtag, void *buf, int bufSz) {

//    int taskId = cacheGetCurrentTask();
    int taskId = 0;

    int type = MSGT_F2F;
    pvm_initsend(PvmDataInPlace);
    pvm_pkint(&type, 1, 1); //the type of the message
    pvm_pkint(&taskId, 1, 1); //task id
#ifdef VOID_TRACER
    pvm_pkint(&msgId, 1, 1); //msgId
#endif

    pvm_pkbyte((char *) buf, bufSz, 1);

#ifdef VOID_INST
    instEnterState(fd->instData, &fd->instData->voidStates[TIMER_WRITE_BLOCKED]);
#endif
    pvm_send(tid, msgtag);

#ifdef VOID_INST
    instLeaveState(fd->instData);
#endif

}

int recvData(InputPort *ip) {
    int bufId = 0;
    struct timeval tmout;
    tmout.tv_sec = 10;
    tmout.tv_usec = 1;

    // wait for data
    while(!bufId){
        bufId = pvm_trecv(-1, ip->tag, &tmout);
#ifdef VOID_TERM
        if(!bufId) {
            if((bufId = pvm_probe(-1, 3))) {
                // There is filter to filter termination msg to recv
                int nBytes, instTid;
                pvm_bufinfo(bufId, &nBytes, NULL, &instTid);
                pvm_recv(instTid, 3);
            } else
                if(tdd->status == NOTPARTICIPATING)
                    beginTerminationDetection();
        }
#endif
    }
    //bufId = pvm_recv(-1, ip->tag);

    return bufId;
}


// -------------------------------------------------------------------------------//
// send related functions --------------------------------------------------------//
int dsWriteBuffer(OutputPortHandler oph, void *buf, int bufSz) {
#ifdef VOID_TRACER
	trcEnterState( fd->trcData, VT_OH_WBUFFER );
#endif
#ifdef VOID_INST
	//enter write state
	instEnterState(fd->instData, &fd->instData->voidStates[TIMER_WRITE]);
#endif

	int instance = -1;
	int curr_branch = 0;  // current branch of the output port (this refers to a multi-stream)

	OutputPort* op = fd->outputPorts[oph];


	// Process the send operation for each branch of the output port
	for(curr_branch = 0; curr_branch < op->numToSend; curr_branch++){

		// BROADCAST policy
		switch (op->writePolicy[curr_branch]){
			/// 2 things can happen here. Either the message destination is a single instance
			/// or message has multiple destinations. we treat the multicast first, then the
			/// others

			/* Multiple destinations --------------------------------*/
			case BROADCAST:
				{
					//send to all instances of current stream branch
					dsMCast(op, curr_branch, buf, bufSz);  
#ifdef VOID_INST
					instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
					msgId++;
					trcLeaveState(fd->trcData);
					// TOSCO.COM
					instance = 1;
#endif

					return bufSz;
				}
			case MULTICAST_LABELED_STREAM:
				{
					//controled by the user, multiple destinations

					// this is the label
					static char label[MAX_LBL_LENGTH];
					static int destArray[MAXINSTANCES], tidsArray[MAXINSTANCES];
					static int clearArray[MAXINSTANCES], clearInit =0;
					if (!clearInit){
						int i;
						for (i=0;i<MAXINSTANCES;i++){
							clearArray[i] = 0;
						}
						clearInit = 1;
					}


					// gets the label
					// this function extracts the label from a message, user must provide it
					// buf is the message buffer
					// bufSz is the size
					// label is a char[]
					op->lsData[curr_branch].getLabel(buf, bufSz, label);

					//zero the destArray
					memcpy(destArray, clearArray, sizeof(int)*MAXINSTANCES);

					// the mlshash function gets a label and fills
					// the destArray with 1 in positions which should receive the message
					op->lsData[curr_branch].mlshash(label, op->numDestinations[curr_branch], destArray);

					int i, tidsArraySize = 0, lastPosition=0;
					for (i=0;i<op->numDestinations[curr_branch];i++){
						if (destArray[i] == 1){
							tidsArraySize++;
							tidsArray[lastPosition++] = op->tidsDestinations[curr_branch][i];
						}
					}

					//we multicast buffer to the tids in tidsArray
					pvm_initsend(PvmDataInPlace);

					int type = MSGT_F2F;
					pvm_pkint(&type, 1, 1); //message type
//					int taskId = cacheGetCurrentTask();
					int taskId = 0;
					pvm_pkint(&taskId, 1, 1); //task
#ifdef VOID_TRACER
					pvm_pkint(&msgId, 1, 1); //msgId
#endif
					pvm_pkbyte((char*)buf, bufSz, 1);

#ifdef VOID_INST
					instEnterState(fd->instData, &fd->instData->voidStates[TIMER_WRITE_BLOCKED]);
#endif
#ifdef VOID_TRACER
					trcEnterState(fd->trcData, VT_COMM_WRITE);
#endif
					int info;
					for ( i = 0; i < tidsArraySize; i++ )
					{
						if ( ( info = pvm_send( tidsArray[i], op->tag )) < 0 )
						{
							printf("FilterDev.c: ERROR, in multicast labeled stream\n");
#ifdef VOID_TRACER
							trcLeaveState(fd->trcData);
#endif
							return -1;
						}
					}
					//			int info = pvm_mcast(tidsArray, tidsArraySize, op->tag);
#ifdef VOID_TRACER
					trcLeaveState(fd->trcData);
#endif

#ifdef VOID_INST
					//leave blocked
					instLeaveState(fd->instData);
#endif
#ifdef VOID_INST
					//leave write
					instLeaveState(fd->instData);
#endif

					/*			if (info < 0){
								printf("FilterDev.c: ERROR, in multicast labeled stream\n");

#ifdef VOID_TRACER
trcLeaveState(fd->trcData);
#endif
return -1;
}
*/

#ifdef VOID_TRACER
					trcLeaveState(fd->trcData);
#endif
					return bufSz;
					}
case RANDOM:
{
	//chose an instance randomly
	instance = lrand48() % op->numDestinations[curr_branch];
	break;
}
case LABELED_STREAM:
{
	//controled by the user, single destination

	// this is the label
	static char label[MAX_LBL_LENGTH];

#ifdef VOID_INST
	instEnterState(fd->instData, &fd->instData->voidStates[TIMER_LS]);
#endif
	// gets the label
	// this function extracts the label from a message, user must provide it
	// buf is the message buffer
	// bufSz is the size
	// label is a char[]
	op->lsData[curr_branch].getLabel(buf, bufSz, label);
	// the hash function gets which instance will receive the message
	// it receives the label, and the number of destinations
	instance = op->lsData[curr_branch].hash(label, op->numDestinations[curr_branch]);

#ifdef VOID_INST
	//leave labeled stream
	instLeaveState(fd->instData);
#endif

	// we dont trust user return, and check if the instance is valid
	while (instance < 0)
		instance += op->numDestinations[curr_branch];
	if (instance >= op->numDestinations[curr_branch])
		instance %= op->numDestinations[curr_branch];

	break;
}
default: {
		 //round robin, default policy
		 instance = op->nextToSend[curr_branch];
		 op->nextToSend[curr_branch] = (op->nextToSend[curr_branch]+1) % op->numDestinations[curr_branch];
		 break;
	 }
/* end single */
}

// sends the buffer
assert(instance >= 0);

/* Put this tracing entrance here, instead of in dsSend body,
 * because os the state attributes ( writePolicy );
 */
#ifdef VOID_TRACER
trcEnterState(fd->trcData, VT_COMM_WRITE);
#endif
dsSend(op->tidsDestinations[curr_branch][instance], op->tag, buf, bufSz);  // <-- The call to 'send' is here
#ifdef VOID_TRACER
trcLeaveState(fd->trcData);
#endif

}  // End of all sends

#ifdef VOID_INST
//leave write
instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
trcLeaveState(fd->trcData);
#endif
return bufSz;
}

/// init packing, size in bytes
/// \warning Multithread apps will crash if they use pack and unpack.
/// \deprecated Packing and unpacking is deprecated, you should use only
/// dsWriteBuffer() and dsReadBuffer().
int dsInitPack(int initSize){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_INITPACK );
#endif

    //we free the buffer if its not null
    if (bufSend.buffer != NULL){
        free(bufSend.buffer);
    }

    int trueSize;

    if (initSize < MINSIZE)
        trueSize = MINSIZE;
    else
        trueSize = initSize;

    if ((bufSend.buffer = (char *)malloc(sizeof(char)*trueSize))==NULL){
        printf("FilterDev/FilterDev.c: error, could not allocate memory for buffer\n");
#ifdef VOID_TRACER
        trcLeaveState(fd->trcData);
#endif
        return -1;
    }


    bufSend.size = 0;
    bufSend.maxSize = trueSize;
    // Coutinho: dsWritePackedBuffer() will put the message type and the taskId
    /*	//the message type
        int type = MSGT_F2F;
        dsPackData(&type, sizeof(int));
    //the message task
    int taskId = cacheGetCurrentTask();
    dsPackData(&taskId, sizeof(int));*/

#ifdef VOID_TRACER
    dsPackData(&msgId, sizeof(int));
    trcLeaveState(fd->trcData);
#endif

    return 1;

}

/// function to pack data to the buffer
/// \warning Multithread apps will crash if they use pack and unpack.
/// \deprecated Packing and unpacking is deprecated, you should use only
/// dsWriteBuffer() and dsReadBuffer().
int dsPackData(void *data, int size){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_PACK );
#endif

    //we only pack void*
    if (bufSend.size + size > bufSend.maxSize){
        //DANGER!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //careful with realloc
        bufSend.maxSize = bufSend.size + size;
        bufSend.buffer = (char*)realloc(bufSend.buffer, bufSend.maxSize*sizeof(char));
    }

    memcpy(&bufSend.buffer[bufSend.size], data, size);
    bufSend.size+= size;
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif
    return 1;
}


/// function to send the packed buffer
/// \warning Multithread apps will crash if they use pack and unpack.
/// \deprecated Packing and unpacking is deprecated, you should use only
/// dsWriteBuffer() and dsReadBuffer().
int dsWritePackedBuffer(OutputPortHandler oph){
    /* Yes: WEIRD !
     * Just for log
     * */
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_WPBUFFER );
    trcLeaveState( fd->trcData );
#endif
    return dsWriteBuffer(oph, bufSend.buffer, bufSend.size);

    //why the hell we were doing all this again(commented)??

    /*	int instance = -1;

        OutputPort* op = fd->outputPorts[oph];

    // BROADCAST policy, we treat it differently
    if (op->writePolicy == BROADCAST) {
    dsMCast(op, bufSend.buffer, bufSend.size);
    return bufSend.size;
    }

    //now the usual policies, we need to get the destination instance first
    if (op->writePolicy == RANDOM) {
    instance = lrand48() % op->numDestinations;
    }
    else if (op->writePolicy == LABELED_STREAM){
    // this is the label
    static char label[MAX_LBL_LENGTH];

    // gets the label
    // this function extracts the label from a message, user must provide it
    // buf is the message buffer
    // bufSz is the size
    // label is a char[]
    op->lsData.getLabel(bufSend.buffer, bufSend.size, label);
    // the hash function gets which instance will receive the message
    // it receives the label, and the number of destinations
    instance = op->lsData.hash(label, op->numDestinations);
    // we dont trust user return, and check if the instance is valid
    while (instance < 0)
    instance += op->numDestinations;
    if (instance >= op->numDestinations)
    instance %= op->numDestinations;
    }
    else {
    //round robin
    instance = op->nextToSend;
    op->nextToSend = (op->nextToSend+1) % op->numDestinations;
    }

    // sends the buffer
    assert(instance >= 0);
    dsSend(op->tidsDestinations[instance], op->tag, bufSend.buffer, bufSend.size);
    //pvm_psend(op->tidsDestinations[instance], op->tag, bufSend.buffer, bufSend.size, PVM_BYTE);
    return bufSend.size;*/
}

//ITAMAR Changes to permit that one 'from' has various 'to'
void taskSend(int taskId, int *deps, int depSize, char *metadata, int metaSize, int creatorTid){
    int i, j, s;
    int type = MSGT_CREATETASK;
    OutputPort* op;

    for(i = 0; i < fd->numOutputPorts; i++){
        op = fd->outputPorts[i];
        for(s = 0; s < op->numToSend; s++){
            int willSendToCreator = 0;
            for ( j = 0; j < op->numDestinations[s]; j++ ) {
                if ( creatorTid == op->tidsDestinations[j][s] ) {
                    willSendToCreator = 1;
                    break;
                }
            }

            // Do not resend task creation message to creator filter instances
            if (!willSendToCreator) {
                pvm_initsend(PvmDataInPlace);
                pvm_pkint(&type, 1, 1); //message type
                pvm_pkint(&taskId, 1, 1);

                // Hack from Coutos to avoid resending
                // task creation message to creator filter instances
                pvm_pkint( &creatorTid, 1, 1);

                pvm_pkint(&depSize, 1, 1);
                if (depSize > 0) pvm_pkint(deps, depSize, 1);

                pvm_pkint(&metaSize, 1, 1);
                if (metaSize > 0) pvm_pkbyte((char *) metadata, metaSize, 1);

                for ( j = 0; j < op->numDestinations[s]; j++ ) {
                    pvm_send( op->tidsDestinations[j][s], op->tag );
                }
                //pvm_mcast(op->tidsDestinations, op->numDestinations, op->tag);

#ifdef DEBUG
                int k;
                fprintf(stderr, "tid %d, tag %d: Multcast de 8+%d bytes p/ instancias de tids:", pvm_mytid(), op->tag, 2*sizeof(int)+metaSize+(depSize*sizeof(int)));
                for (k=0; k<op->numDestinations[s]; k++) {
                    fprintf(stderr, " %d", op->tidsDestinations[k][s]);
                }
                fprintf(stderr,	"\n");
#endif
            }
        }

    }
}

//ITAMAR Changes to permit that one 'from' has various 'to'
int endTaskSend(int taskId, int enderTid){
    int i, j, s, type = MSGT_ENDTASK;
    OutputPort* op = NULL;

    for(i = 0; i < fd->numOutputPorts; i++){
        op = fd->outputPorts[i];
        for(s = 0; s < op->numToSend; s++){
            int willSendToEnder = 0;
            for ( j = 0; j < op->numDestinations[s]; j++ ) {
                if ( enderTid == op->tidsDestinations[j][s] ) {
                    willSendToEnder = 1;
                }
            }

            if (!willSendToEnder) {
                pvm_initsend(PvmDataInPlace);
                pvm_pkint(&type, 1, 1); //message type
                pvm_pkint(&taskId, 1, 1);
                pvm_pkint(&enderTid, 1, 1);

                for ( j = 0; j < op->numDestinations[s]; j++ ) {
                    pvm_send( op->tidsDestinations[j][s], op->tag );
                }
                //pvm_mcast(op->tidsDestinations, op->numDestinations, op->tag);

#ifdef DEBUG
                int k;
                fprintf(stderr, "tid %d, tag %d: Multcast mesgType %d ", pvm_mytid(), op->tag, type);
                for (k=0; k<op->numDestinations[s]; k++) {
                    fprintf(stderr, " %d", op->tidsDestinations[k][s]);
                }
                fprintf(stderr,	"\n");
#endif
            }
        }

    }
    return 0;
}

//----------------------------------------------------------------------------------------//
// recv related functions ----------------------------------------------------------------//

// this function reads a buffer, of size szBuf
int dsReadBuffer(InputPortHandler iph, void *buf, int szBuf) {

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_RBUFFER );
    trcEnterState( fd->trcData, VT_COMM_READ );
    incomingMsgId = -1;
#endif

#ifdef VOID_INST
    //enter read state
    //instEnterState(fd->instData, &fd->instData->voidStates[TIMER_READ]);
#endif

    int ret = ERROR;
    void *recvBuf = NULL;
    int taskId = -1;

    InputPort *ip = fd->inputPorts[iph];

    if ((iph > fd->numInportsAdded) || (iph > fd->numInputPorts)) {
        fprintf(stderr, "%s (%d): iph(%d) > %d or %d\n", __FILE__, __LINE__,
                iph, fd->numInportsAdded, fd->numInputPorts);
    }

    // We try to receive from all instances, and get
    // EOW from all. only then we give our EOW
    while(ip->numEowRecv < ip->numSources) {
        int szMsg  = -1;
        int bufid = recvData(ip);
        int instTid;

        pvm_bufinfo(bufid, &szMsg, NULL, &instTid);

        szMsg -= (2 * sizeof(int)); //we take the msg type and the tasdId out

        int msgType;
        pvm_upkint(&msgType, 1, 1); //get the type of message
#ifdef VOID_TERM
        if(msgType == MSGT_INITTERM) {
            int neighborTermTag; // neighbor's termination detection round tag
            pvm_upkint(&neighborTermTag, 1, 1);
            terminationDetectionRound(instTid, neighborTermTag);
        }else {
            updateTermStreamToEmpty(instTid);
#endif
            pvm_upkint(&taskId, 1, 1); //get the task id

            switch (msgType){

                case (MSGT_CREATETASK):{
                                           int creatorTid, depSize, metaSize, createTaskReturn;
                                           char *metadata = NULL;
                                           int *deps = NULL;

                                           // Get creator pvm_tid
                                           pvm_upkint( &creatorTid, 1, 1 );

                                           // get deps
                                           pvm_upkint(&depSize, 1, 1);
                                           if(depSize > 0){
                                               deps = malloc(sizeof(int) * depSize);
                                               pvm_upkint(deps, depSize,1);
                                           }

                                           // get metadata
                                           pvm_upkint(&metaSize, 1, 1);
                                           if(metaSize > 0){
                                               metadata = malloc(sizeof(char) * metaSize);
                                               pvm_upkbyte(metadata, metaSize,1);
                                           }
                                           createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

                                           // marreta do coutos pra criacao de tarefas
                                           // nao ficar dando volta no loop de filtros.. entao ela
                                           // deve morrer no filtro q criou a mesma
                                           if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
                                               taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
                                           }

                                           break;
                                       }
                case (MSGT_ENDTASK):{
                                        int enderTid;
                                        pvm_upkint( &enderTid, 1, 1);
                                        int endTaskReturn = cacheEndTask(taskId);
                                        // end task and send that to next filter
                                        if((endTaskReturn != E_TASK_NOT_RUNNING) && cacheGetForwardTaskMsgs()){
                                            endTaskSend(taskId, enderTid);
                                        }
                                        break;
                                    }
                case (MSGT_EOW):
                                    {
                                        //we received an EOW
                                        ip->numEowRecv++;
                                        break;
                                    }
                case (MSGT_F2F):
                                    {
#ifdef VOID_TRACER
                                        pvm_upkint( &incomingMsgId, 1, 1 );
#endif
					int status = 0;
//                                        int status = dsSetCurrentTask(taskId);
                                        if ((status < 0) && (!cacheGetForwardTaskMsgs()) && (taskId >= 0)) {
                                            // If this filter don't use tasks, create a dummy task
                                            // and change to it (the filter don't use them anyway).
                                       //     dsCreateTask(taskId, NULL, 0, NULL, 0);
                                         //   dsSetCurrentTask(taskId);
                                        }

                                        //filter to filter message
                                        if (szMsg > szBuf) {
                                            // if message is bigger than buffer size we truncate
                                            printf("FilterDev.c: warning, message size(%d) is bigger than buffer(%d), truncating! bufid=%d ip->tag=%d",
                                                    szMsg, szBuf, bufid, ip->tag);
                                            recvBuf = malloc(szMsg);
                                            status = pvm_upkbyte((char *) recvBuf, szMsg, 1);
                                            ret = szBuf;
                                            memcpy(buf, recvBuf, szBuf);
                                            free(recvBuf);
                                        } else {
                                            //message fits in the buffer
                                            recvBuf = buf;
                                            status = pvm_upkbyte((char *) buf, szMsg, 1);
                                            ret = szMsg;
                                        }

#ifdef VOID_INST
                                        //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
                                        trcLeaveState( fd->trcData, instTid, incomingMsgId );
                                        trcLeaveState( fd->trcData );
#endif

                                        return ret;
                                    }
                default:
                                    {
                                        //shouldn't get here
                                        fprintf(stderr, "FilterDev.c: unknown message type\n");
                                    }
            }
#ifdef VOID_TERM
        }
#endif
    }

#ifdef VOID_INST
    //leave read
    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData, incomingMsgId );
    trcLeaveState( fd->trcData );
#endif
    //if we got here, we received all instances EOW. Return it to the user
    return EOW;
}

int recvDataFromAnyPort() {
    int bufId = 0;
    struct timeval tmout;
    tmout.tv_sec = 10;
    tmout.tv_usec = 1;

    // wait for data
    while(!bufId){
        bufId = pvm_trecv(-1, -1, &tmout);
#ifdef VOID_TERM
        if(!bufId) {
            if((bufId = pvm_probe(-1, 3))) {
                // There is filter to filter termination msg to recv
                int nBytes, instTid;
                pvm_bufinfo(bufId, &nBytes, NULL, &instTid);
                pvm_recv(instTid, 3);
            } else
                if(tdd->status == NOTPARTICIPATING)
                    beginTerminationDetection();
        }
#endif
    }

    //bufId = pvm_recv(-1, -1);

    return bufId;
}

// this function reads a buffer, of size szBuf, of any port
// Warning: you can't use this function to wait EOW
int dsReadBufferAnyPort(char **portName, char **exceptionPorts, int numExc, void *buf, int szBuf) {
    InputPort *ip = NULL;
    int numSources = 0, numEowReceived = 0;

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_RBUFFER );
    trcEnterState( fd->trcData, VT_COMM_READ );
    incomingMsgId = -1;
#endif

#ifdef VOID_INST
    //enter read state
    //instEnterState(fd->instData, &fd->instData->voidStates[TIMER_READ]);
#endif

    int ret = ERROR;
    void *recvBuf = NULL;
    int taskId = -1;

    // We try to receive from all instances, and get EOW from all. only then we give our EOW
    do {
        int szMsg  = -1;
        int bufid = recvDataFromAnyPort();
        int instTid, msgTag, correctTag = 0;
        int i, j, k;
        ip = NULL;

        pvm_bufinfo(bufid, &szMsg, &msgTag, &instTid);

        // The tag must be one that I'm waiting for
        for(i = 0; i < fd->numInputPorts; i++) {
            if(fd->inputPorts[i]->tag == msgTag) {
                correctTag = 1;
                break;
            }
        }
        if(!correctTag) {
            // I have to receive another message.
            numEowReceived = -1;
            continue;
        }

        szMsg -= (2 * sizeof(int)); //we take the msg type and the tasdId out

        // We find out wich filter instance sent this message
        for(i = 0; i < fd->numInputPorts; i++) {
            for(j = 0; j < fd->inputPorts[i]->numSources; j++) {
                if(instTid == fd->inputPorts[i]->tidsSources[j]) {
                    ip = fd->inputPorts[i];
                    *portName = strdup(fd->inputPorts[i]->name);
                    break;
                }
            }
        }

        if(ip == NULL) {
#ifdef ATTACH
            if(instTid == fd->parentTid) {
                // One failure occurred. Get any port to receive EOW.
                for(i = 0; i < fd->numInputPorts; i++) {
                    // Checks if the current port is an exception
                    int isException = 0;
                    for(k = 0; k < numExc; k++) {
                        if(!strcmp(fd->inputPorts[i]->name, exceptionPorts[k]))
                            isException = 1;
                        break;
                    }
                    if(!isException && (fd->inputPorts[i]->numEowRecv < fd->inputPorts[i]->numSources)) {
                        ip = fd->inputPorts[i];
                        break;
                    }
                }
            } else {
#endif
                // No inputPort was found
                fprintf(stderr, "%s (%d): filter instance with tid %d was not found\n",
                        __FILE__, __LINE__, instTid);
#ifdef ATTACH
            }
#endif
        }

        int msgType;
        pvm_upkint(&msgType, 1, 1); //get the type of message
#ifdef VOID_TERM
        if(msgType == MSGT_INITTERM) {
            int neighborTermTag; // neighbor's termination detection round tag
            pvm_upkint(&neighborTermTag, 1, 1);
            terminationDetectionRound(instTid, neighborTermTag);
        }else {
            updateTermStreamToEmpty(instTid);
#endif
            pvm_upkint(&taskId, 1, 1); //get the task id

            switch (msgType){

                case (MSGT_CREATETASK):{
                                           int creatorTid, depSize, metaSize, createTaskReturn;
                                           char *metadata = NULL;
                                           int *deps = NULL;

                                           // Get creator pvm_tid
                                           pvm_upkint( &creatorTid, 1, 1 );

                                           // get deps
                                           pvm_upkint(&depSize, 1, 1);
                                           if(depSize > 0){
                                               deps = malloc(sizeof(int) * depSize);
                                               pvm_upkint(deps, depSize,1);
                                           }

                                           // get metadata
                                           pvm_upkint(&metaSize, 1, 1);
                                           if(metaSize > 0){
                                               metadata = malloc(sizeof(char) * metaSize);
                                               pvm_upkbyte(metadata, metaSize,1);
                                           }
                                           createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

                                           // marreta do coutos pra criacao de tarefas
                                           // nao ficar dando volta no loop de filtros.. entao ela
                                           // deve morrer no filtro q criou a mesma
                                           if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
                                               taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
                                           }

                                           break;
                                       }
                case (MSGT_ENDTASK):{
                                        int enderTid;
                                        pvm_upkint( &enderTid, 1, 1);
                                        int endTaskReturn = cacheEndTask(taskId);
                                        // end task and send that to next filter
                                        if((endTaskReturn != E_TASK_NOT_RUNNING) && cacheGetForwardTaskMsgs()){
                                            endTaskSend(taskId, enderTid);
                                        }
                                        break;
                                    }
                case (MSGT_EOW):
                                    {
                                        //we received an EOW
                                        ip->numEowRecv++;
                                        break;
                                    }
                case (MSGT_F2F):
                                    {
#ifdef VOID_TRACER
                                        pvm_upkint( &incomingMsgId, 1, 1 );
#endif
//                                        int status = dsSetCurrentTask(taskId);
					int status = 0;
                                        if ((status < 0) && (!cacheGetForwardTaskMsgs()) && (taskId >= 0)) {
                                            // If this filter don't use tasks, create a dummy task
                                            // and change to it (the filter don't use them anyway).
  //                                          dsCreateTask(taskId, NULL, 0, NULL, 0);
//                                            dsSetCurrentTask(taskId);
                                        }

                                        //filter to filter message
                                        if (szMsg > szBuf) {
                                            // if message is bigger than buffer size we truncate
                                            printf("FilterDev.c: warning, message size(%d) is bigger than buffer(%d), truncating! bufid=%d ip->tag=%d",
                                                    szMsg, szBuf, bufid, ip->tag);
                                            recvBuf = malloc(szMsg);
                                            status = pvm_upkbyte((char *) recvBuf, szMsg, 1);
                                            ret = szBuf;
                                            memcpy(buf, recvBuf, szBuf);
                                            free(recvBuf);
                                        } else {
                                            //message fits in the buffer
                                            recvBuf = buf;
                                            status = pvm_upkbyte((char *) buf, szMsg, 1);
                                            ret = szMsg;
                                        }

#ifdef VOID_INST
                                        //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
                                        trcLeaveState( fd->trcData, instTid, incomingMsgId );
                                        trcLeaveState( fd->trcData );
#endif

                                        return ret;
                                    }
                default:
                                    {
                                        //shouldn't get here
                                        fprintf(stderr, "FilterDev.c: unknown message type\n");
                                    }
            }
#ifdef VOID_TERM
        }
#endif

        // Counts number of possible sources and number of eow this filter
        // has received
        numSources = 0;	numEowReceived = 0;
        for(i = 0; i < fd->numInputPorts; i++) {
            // Checks if the current port is an exception
            int isException = 0;
            for(k = 0; k < numExc; k++) {
                if(!strcmp(fd->inputPorts[i]->name, exceptionPorts[k])) {
                    isException = 1;
                    break;
                }
            }
            if(!isException) {
                numSources += fd->inputPorts[i]->numSources;
                numEowReceived += fd->inputPorts[i]->numEowRecv;
            }
        }
    } while(numEowReceived < numSources);

#ifdef VOID_INST
    //leave read
    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData, incomingMsgId );
    trcLeaveState( fd->trcData );
#endif
    //if we got here, we received all instances EOW. Return it to the user
    return EOW;
}


int recvNonBlockingData(InputPort *ip) {
    int bufId = 0;
    struct timeval tmout;
    tmout.tv_sec = 0;
    tmout.tv_usec = 60;

    bufId = pvm_trecv(-1, ip->tag, &tmout);

    return bufId;
}

/// returns 0 if there is no data to be received
int dsReadNonBlockingBufferMalloc(InputPortHandler iph, void **buf, char **label) {
	*label = NULL;

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_RBUFFER );
    trcEnterState( fd->trcData, VT_COMM_READ );
    incomingMsgId = -1;
#endif

#ifdef VOID_INST
    //enter read state
    //instEnterState(fd->instData, &fd->instData->voidStates[TIMER_READ]);
#endif

    int ret = ERROR;
//    void *recvBuf = NULL;
    int taskId = -1;

    InputPort *ip = fd->inputPorts[iph];

    if ((iph > fd->numInportsAdded) || (iph > fd->numInputPorts)) {
        fprintf(stderr, "%s (%d): iph(%d) > %d or %d\n", __FILE__, __LINE__,
                iph, fd->numInportsAdded, fd->numInputPorts);
    }

    // We try to receive from all instances, and get EOW from all. only then we give our EOW
    while(ip->numEowRecv < ip->numSources) {
        int szMsg  = -1;
        int bufid = recvNonBlockingData(ip);
        if(bufid == 0) return 0;
        int instTid;

        int status = pvm_bufinfo(bufid, &szMsg, NULL, &instTid);
        szMsg -= (2 * sizeof(int)); //we take the msg type and the tasdId out

        int msgType;
        pvm_upkint(&msgType, 1, 1); //get the type of message
#ifdef VOID_TERM
        if(msgType == MSGT_INITTERM) {
            int neighborTermTag; // neighbor's termination detection round tag
            pvm_upkint(&neighborTermTag, 1, 1);
            terminationDetectionRound(instTid, neighborTermTag);
        }else {
            updateTermStreamToEmpty(instTid);
#endif
            pvm_upkint(&taskId, 1, 1); //get the task id

            switch (msgType){
                case (MSGT_CREATETASK):{
                                           int creatorTid, depSize, metaSize, createTaskReturn;
                                           char *metadata = NULL;
                                           int *deps = NULL;

                                           // Get creator pvm_tid
                                           pvm_upkint( &creatorTid, 1, 1 );

                                           // get deps
                                           pvm_upkint(&depSize, 1, 1);
                                           if(depSize > 0){
                                               deps = malloc(sizeof(int) * depSize);
                                               pvm_upkint(deps, depSize,1);
                                           }

                                           // get metadata
                                           pvm_upkint(&metaSize, 1, 1);
                                           if(metaSize > 0){
                                               metadata = malloc(sizeof(char) * metaSize);
                                               pvm_upkbyte(metadata, metaSize,1);
                                           }
                                           createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

                                           // marreta do coutos pra criacao de tarefas
                                           // nao ficar dando volta no loop de filtros.. entao ela
                                           // deve morrer no filtro q criou a mesma
                                           if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
                                               taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
                                           }

                                           break;
                                       }

                case (MSGT_ENDTASK):{
                                        dsEndTask(taskId);
                                        break;
                                    }
                case (MSGT_EOW):
                                    {
                                        //we received an EOW
                                        ip->numEowRecv++;
                                        break;
                                    }
		case (MSGT_F2F):
				    {
#ifdef VOID_TRACER
					    pvm_upkint( &incomingMsgId, 1, 1 );
#endif
					    taskId = 0;
//					    dsSetCurrentTask(taskId);

					    //filter to filter message

					    *buf = malloc(szMsg);
					    status = pvm_upkbyte((char *) *buf, szMsg, 1);
					    ret = szMsg;
					    if(ip->ls){
						    static char auxLabel[MAX_LBL_LENGTH];
						    ip->lsData[0].getLabel(*buf, szMsg, auxLabel);
						    *label = strdup(auxLabel);
					    }
					    
#ifdef VOID_INST
					    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
					    trcLeaveState( fd->trcData, instTid, incomingMsgId );
					    trcLeaveState( fd->trcData );
#endif

					    return ret;
	    }
                default:
                                    {
                                        //shouldn't get here
                                        fprintf(stderr, "FilterDev.c: unknown message type\n");
                                    }
            }
#ifdef VOID_TERM
        }
#endif
    }

#ifdef VOID_INST
    //leave read
    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData, incomingMsgId );
    trcLeaveState( fd->trcData );
#endif
    //if we got here, we received all instances EOW. Return it to the user
    return EOW;

}


/// returns 0 if there is no data to be received
int dsReadNonBlockingBuffer(InputPortHandler iph, void *buf, int szBuf) {

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_RBUFFER );
    trcEnterState( fd->trcData, VT_COMM_READ );
    incomingMsgId = -1;
#endif

#ifdef VOID_INST
    //enter read state
    //instEnterState(fd->instData, &fd->instData->voidStates[TIMER_READ]);
#endif

    int ret = ERROR;
    void *recvBuf = NULL;
    int taskId = -1;

    InputPort *ip = fd->inputPorts[iph];

    if ((iph > fd->numInportsAdded) || (iph > fd->numInputPorts)) {
        fprintf(stderr, "%s (%d): iph(%d) > %d or %d\n", __FILE__, __LINE__,
                iph, fd->numInportsAdded, fd->numInputPorts);
    }

    // We try to receive from all instances, and get EOW from all. only then we give our EOW
    while(ip->numEowRecv < ip->numSources) {
        int szMsg  = -1;
        int bufid = recvNonBlockingData(ip);
        if(bufid == 0) return 0;
        int instTid;

        int status = pvm_bufinfo(bufid, &szMsg, NULL, &instTid);
        szMsg -= (2 * sizeof(int)); //we take the msg type and the tasdId out

        int msgType;
        pvm_upkint(&msgType, 1, 1); //get the type of message
#ifdef VOID_TERM
        if(msgType == MSGT_INITTERM) {
            int neighborTermTag; // neighbor's termination detection round tag
            pvm_upkint(&neighborTermTag, 1, 1);
            terminationDetectionRound(instTid, neighborTermTag);
        }else {
            updateTermStreamToEmpty(instTid);
#endif
            pvm_upkint(&taskId, 1, 1); //get the task id

            switch (msgType){
                case (MSGT_CREATETASK):{
                                           int creatorTid, depSize, metaSize, createTaskReturn;
                                           char *metadata = NULL;
                                           int *deps = NULL;

                                           // Get creator pvm_tid
                                           pvm_upkint( &creatorTid, 1, 1 );

                                           // get deps
                                           pvm_upkint(&depSize, 1, 1);
                                           if(depSize > 0){
                                               deps = malloc(sizeof(int) * depSize);
                                               pvm_upkint(deps, depSize,1);
                                           }

                                           // get metadata
                                           pvm_upkint(&metaSize, 1, 1);
                                           if(metaSize > 0){
                                               metadata = malloc(sizeof(char) * metaSize);
                                               pvm_upkbyte(metadata, metaSize,1);
                                           }
                                           createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

                                           // marreta do coutos pra criacao de tarefas
                                           // nao ficar dando volta no loop de filtros.. entao ela
                                           // deve morrer no filtro q criou a mesma
                                           if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
                                               taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
                                           }

                                           break;
                                       }

                case (MSGT_ENDTASK):{
                                        dsEndTask(taskId);
                                        break;
                                    }
                case (MSGT_EOW):
                                    {
                                        //we received an EOW
                                        ip->numEowRecv++;
                                        break;
                                    }
                case (MSGT_F2F):
                                    {
#ifdef VOID_TRACER
                                        pvm_upkint( &incomingMsgId, 1, 1 );
#endif
					taskId = 0;
//                                        dsSetCurrentTask(taskId);

                                        //filter to filter message
                                        if (szMsg > szBuf) {
                                            // if message is bigger than buffer size we truncate
                                            printf("FilterDev.c: warning, message size(%d) is bigger than buffer(%d), truncating! bufid=%d ip->tag=%d",
                                                    szMsg, szBuf, bufid, ip->tag);
                                            recvBuf = malloc(szMsg);
                                            status = pvm_upkbyte((char *) recvBuf, szMsg, 1);
                                            ret = szBuf;
                                            memcpy(buf, recvBuf, szBuf);
                                            free(recvBuf);
                                        } else {
                                            //message fits in the buffer
                                            recvBuf = buf;
                                            status = pvm_upkbyte((char *) buf, szMsg, 1);
                                            ret = szMsg;
                                        }

#ifdef VOID_INST
                                        //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
                                        trcLeaveState( fd->trcData, instTid, incomingMsgId );
                                        trcLeaveState( fd->trcData );
#endif

                                        return ret;
                                    }
                default:
                                    {
                                        //shouldn't get here
                                        fprintf(stderr, "FilterDev.c: unknown message type\n");
                                    }
            }
#ifdef VOID_TERM
        }
#endif
    }

#ifdef VOID_INST
    //leave read
    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData, incomingMsgId );
    trcLeaveState( fd->trcData );
#endif
    //if we got here, we received all instances EOW. Return it to the user
    return EOW;

}

/// function to receive a buffer, and unpack later, returning the size
/// \warning Multithread apps will crash if they use pack and unpack.
/// \deprecated Packing and unpacking is deprecated, you should use only
/// dsWriteBuffer() and dsReadBuffer().
int dsInitReceive(InputPortHandler iph){

#ifdef VOID_TRACER
    /* Another weird one... */
    trcEnterState( fd->trcData, VT_OH_IRECEIVE );
    trcLeaveState( fd->trcData );
#endif

#ifdef VOID_INST
    //enter read state
    //instEnterState(fd->instData, &fd->instData->voidStates[TIMER_READ]);
#endif

    //return dsReadBuffer(iph, bufRecv.buffer, bufRecv.maxSize);
    // Coutinho: doing only a dsREadBuffer() doesn't work because buffer size isn't known apriori

    int taskId = -1;
    InputPort *ip = fd->inputPorts[iph];

    if ((iph > fd->numInportsAdded) || (iph > fd->numInputPorts)) {
        fprintf(stderr, "%s (%d): iph(%d) > %d or %d\n", __FILE__, __LINE__,
                iph, fd->numInportsAdded, fd->numInputPorts);
    }

    // We try to receive from all instances, and get EOW from all
    while(ip->numEowRecv < ip->numSources) {
        int szMsg  = -1;
        int bufId = recvData(ip);
        int bufTag, tidSrc;
        char *err;

        switch (bufId){
            case PvmBadParam:
                printf("FilterDev/FilterDev.c: error receiving buffer(bad param)\n");
                return -1;
                break;
            case PvmSysErr:
                printf("FilterDev/FilterDev.c: error receiving buffer(sys error)\n");
                return -1;
                break;
        }

        pvm_bufinfo(bufId, &szMsg, &bufTag, &tidSrc);
        szMsg -= (2 * sizeof(int)); //we take the msg type and the tasdId out

        int msgType;
        pvm_upkint(&msgType, 1, 1); //get the type of message
#ifdef VOID_TERM
        if(msgType == MSGT_INITTERM) {
            int neighborTermTag; // neighbor's termination detection round tag
            pvm_upkint(&neighborTermTag, 1, 1);
            terminationDetectionRound(instTid, neighborTermTag);
        }else {
            updateTermStreamToEmpty(instTid);
#endif
            pvm_upkint(&taskId, 1, 1); //get the task id

            switch (msgType){

                case (MSGT_CREATETASK):{
                                           int creatorTid, depSize, *deps = NULL, metaSize, createTaskReturn;
                                           char *metadata;

                                           // Get creator pvm Tid

                                           pvm_upkint(&creatorTid, 1, 1);

                                           // get deps
                                           pvm_upkint(&depSize, 1, 1);
                                           if(depSize > 0){
                                               deps = malloc(sizeof(int) * depSize);
                                           }
                                           pvm_upkint(deps, depSize,1);

                                           // get metadata
                                           pvm_upkint(&metaSize, 1, 1);
                                           if(metaSize > 0){
                                               metadata = malloc(sizeof(char) * metaSize);
                                           }
                                           pvm_upkbyte(metadata, metaSize,1);
                                           createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

                                           // marreta do coutos pra criacao de tarefas
                                           // nao ficar dando volta no loop de filtros.. entao ela
                                           // deve morrer no filtro q criou a mesma
                                           if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
                                               taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
                                           }

                                           break;
                                       }
                case (MSGT_ENDTASK):{
                                        int enderTid;
                                        pvm_upkint( &enderTid, 1, 1);
                                        int endTaskReturn = cacheEndTask(taskId);
                                        // end task and send that to next filter
                                        if((endTaskReturn != E_TASK_NOT_RUNNING) && cacheGetForwardTaskMsgs()){
                                            endTaskSend(taskId, enderTid);
                                        }
                                        break;
                                    }
                case (MSGT_EOW):
                                    {
                                        //we received an EOW
                                        ip->numEowRecv++;
                                        break;
                                    }
                case (MSGT_F2F):
                                    {
                                        //filter to filter message

					int status = 0;
//                                        int status = dsSetCurrentTask(taskId);
                                        if ((status < 0) && (!cacheGetForwardTaskMsgs()) && (taskId >= 0)) {
                                            // If this filter don't use tasks, create a dummy task
                                            // and change to it (the filter don't use them anyway).
//                                            dsCreateTask(taskId, NULL, 0, NULL, 0);
  //                                          dsSetCurrentTask(taskId);
                                        }

                                        //we free the buffer if its not null
                                        if (bufRecv.buffer != NULL){
                                            free(bufRecv.buffer);
                                        }

                                        bufRecv.buffer = (char *) malloc(szMsg*sizeof(char));
                                        bufRecv.size = 0;
                                        bufRecv.maxSize = szMsg;

                                        status = pvm_upkbyte((char *) bufRecv.buffer, szMsg, 1);
                                        if (status < 0) {
                                            switch (status){
                                                case PvmNoData:
                                                    err = strdup("no data to receive");
                                                    break;
                                                case PvmBadMsg:
                                                    err = strdup("incompatible decoding");
                                                    break;
                                                case PvmNoBuf:
                                                    err = strdup("no active buffer, call dsInitReceive first");
                                                    break;
                                                default:
                                                    err = calloc(1, 1); // Empty string
                                            }

                                            printf("FilterDev/FilterDev.c: error unpacking PVM data: %s\n", err);
                                            free(err);
                                            return -1;
                                        }

#ifdef VOID_INST
                                        //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
                                        trcLeaveState( fd->trcData );
#endif

                                        return szMsg;
                                    }
                default:
                                    {
                                        //shouldn't get here
                                        fprintf(stderr, "FilterDev.c: unknown message type\n");
                                    }
            }
#ifdef VOID_TERM
        }
#endif
    }

#ifdef VOID_INST
    //leave read
    //instLeaveState(fd->instData);
#endif
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif
    //if we got here, we received all instances EOW. Return it to the user
    return EOW;
}

/// function to unpack data
/// \warning Multithread apps will crash if they use pack and unpack.
/// \deprecated Packing and unpacking is deprecated, you should use only
/// dsWriteBuffer() and dsReadBuffer().
int dsUnpackData(void *buf, int size){

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_UNPACK );
#endif

    if(bufRecv.size+size > bufRecv.maxSize) {
        printf("FilterDev/FilterDev.c: Reading beyond the end of the receive buffer.\n");
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return -1;
    }

    memcpy(buf, &(bufRecv.buffer[bufRecv.size]), size);

    bufRecv.size += size;
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif
    /// \todo Modify this to return what it really read
    return size;
}

/// Returns filter name
char *dsGetFilterName() {
    return fd->name;
}

/// Returns filter id
int dsGetFilterId() {
    return fd->id;
}

/// function to get the number of one filter input ports
int dsGetNumInputPorts() {
    return fd->numInputPorts;
}

/// function to get all input ports names of one filter
char **dsGetInputPortNames() {
    char **imputPortNames = (char **) malloc(fd->numInputPorts*sizeof(char*));
    int i, nameSz;

    for(i = 0; i < fd->numInputPorts; i++) {
        nameSz = strlen(fd->inputPorts[i]->name);
        imputPortNames[i] = (char *) malloc(nameSz+1);
        strcpy (imputPortNames[i], fd->inputPorts[i]->name);
    }

    return imputPortNames;
}

/// function to get the number of one filter output ports
int dsGetNumOutputPorts() {
    return fd->numOutputPorts;
}

/// function to get all output ports names of one filter
char **dsGetOutputPortNames() {
    char **outputPortNames = (char **) malloc(fd->numOutputPorts*sizeof(char*));
    int i, nameSz;

    for(i = 0; i < fd->numOutputPorts; i++) {
        nameSz = strlen(fd->outputPorts[i]->name);
        outputPortNames[i] = (char *) malloc(nameSz+1);
        strcpy (outputPortNames[i], fd->outputPorts[i]->name);
    }

    return outputPortNames;
}

/// get the number of writers to me that are still running
int dsGetNumUpStreamsRunning() {
    int i;
    int numUpStreamsRunning = 0;

    for(i = 0; i < fd->numInputPorts; i++) {
        if(fd->inputPorts[i]->numEowRecv < fd->inputPorts[i]->numSources)
            numUpStreamsRunning++;
    }

    return numUpStreamsRunning;
}

/// which instance of the filter am I?
int dsGetMyRank(){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETMYRANK );
#endif
    int a = fd->myRank;
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif
    return a;
}

char **dsGetArgv(){
	return fd->argv;	
}

int dsGetArgc(){
	return fd->argc;
}
/// Returns the amount of this filter's instances
int dsGetTotalInstances(){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETTI );
    trcLeaveState( fd->trcData );
#endif

    return fd->numInstances;
}

/// Exit the system and shutdown the pipeline.
int dsExit(char *userMesg){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_EXIT );
#endif

    int length = 0;
    char voidMesg[1001];

    length = strlen(userMesg);
    if(length > 1000){
        printf("FilterDev.c: Warning: message size bigger than 1000 chars, truncating\n");
        length = 1000;
    }
    strncpy(voidMesg, userMesg, length);
    voidMesg[length] = '\0';

    pvm_initsend(PvmDataRaw);
    pvm_pkbyte(voidMesg, strlen(voidMesg)+1, 1);
    pvm_send(pvm_parent(), MSGT_AEXIT); //we send with the right tag
    pvm_exit();
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
    trcDestroyData( &(fd->trcData) );
#endif
    exit(-1);
}

/// probe the port for message, returns 0 if no message, message size otherwise
int dsProbe(InputPortHandler iph){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_PROBE );
#endif

    int tag, tid = -1, size=0;
    int tag2, tid2;

    //gets the port
    InputPort *ip = fd->inputPorts[iph];

    //get the stream tag
    tag = ip->tag;

    int probRes = pvm_probe(tid, tag);
    if (probRes == 0){
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return 0;
    } else{
        if (pvm_bufinfo(probRes, &size, &tag2, &tid2) < 0){
            //error
            printf("probe error, return 0\n");
#ifdef VOID_TRACER
            trcLeaveState( fd->trcData );
#endif
            return 0;
        }
#ifdef VOID_TRACER
        trcLeaveState( fd->trcData );
#endif
        return size - sizeof(int)*2; //we dont return the msg type nor taskId
    }
}

int dsGetMachineMemory(){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETMEM );
    trcLeaveState( fd->trcData );
#endif
    return getFDMachineMem(fd);
}

int dsGetLocalInstances(){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETLI );
    trcLeaveState( fd->trcData );
#endif
    return getFDLocalInstances(fd);
}


/** Task functions *******************************************/

void dsUseTasks() {
//    cacheSetUseTasks();
}

int dsGetCurrentTask(){
#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_GETCT );
#endif
  //  int ret = cacheGetCurrentTask();
  	int ret = 0;
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif
    return ret;
}

int dsSetCurrentTask(int taskId) {
//    return cacheSetCurrentTask(taskId);
	return 0;
}

int *dsGetTaskDeps(int taskId, int *depsSz) {
    return cacheGetTaskDeps(taskId, depsSz);
}


int dsCreateTask(int taskId, int *deps, int depSize, char *metadata, int metaSize){

#ifdef VOID_TRACER
    trcEnterState( fd->trcData, VT_OH_CREATETASK );
#endif
    int createTaskReturn = cacheCreateTask(taskId, deps, depSize, metadata, metaSize);

    // marreta do coutos pra criacao de tarefas
    // nao ficar dando volta no loop de filtros.. entao ela
    // deve morrer no filtro q criou a mesma
    if ((createTaskReturn == 0) && (cacheGetForwardTaskMsgs())){
        int creatorTid = pvm_mytid();
        taskSend(taskId, deps, depSize, metadata, metaSize, creatorTid);
    }
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
    trcEnterState( fd->trcData, VT_PROC_PROCTASK );
#endif

    return createTaskReturn;
}


int dsEndTask(int taskId){
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
    trcEnterState( fd->trcData, VT_OH_ENDTASK );
#endif
    int endTaskReturn = cacheEndTask(taskId);
    // end task and send that to next filter
    int enderTid = pvm_mytid();
    if((endTaskReturn != E_TASK_NOT_RUNNING) && cacheGetForwardTaskMsgs()){
        endTaskSend(taskId, enderTid);
    }
#ifdef VOID_TRACER
    trcLeaveState( fd->trcData );
#endif

    return 0;
}


/** Data functions *************************************************/
int dsPutData(char *id, void *val, int valSize){
    return cachePutData(id, val, valSize);
}

void *dsGetData(int taskId, char *id, int *valSz){
    return cacheGetData(taskId, id, valSz);
}

int dsRemoveData(char *id){
    return cacheRemoveData(id);
}

// Coutinho: in C its not necessary to declare a global variable as static, the
// static of C isn't equal the object orientation concept of a static class member
//int filterDevVoidInstState;

void dsInstSetStates(char **states, int numStates){
#ifdef VOID_INST
    instSetUserStates(fd->instData, states, numStates);
#endif
}

#ifdef BMI_FT
void ftInstPrintState(int enterOrExit, char* stateName) {
    /*char fileName[MAX_IDIR_LENGTH+1];
      sprintf(fileName, "%s/%s.%d.ft", fd->instData->instDir, fd->name, fd->myRank);
      FILE *fp = fopen(fileName, "a");

      struct timeval tv;
      struct timezone tz; //not used

    //get the time
    gettimeofday(&tv, &tz);

    fprintf(fp, "%d %s %ld %ld", enterOrExit, stateName, tv.tv_sec, tv.tv_usec);

    fclose(fp);*/
}
#endif

void dsInstSwitchState(int stateId){
#ifdef VOID_INST
    instSwitchState(fd->instData, &fd->instData->userStates[stateId]);

#ifdef BMI_FT
    ftInstPrintState(-1, fd->instData->currentState->stateName);
    ftInstPrintState(1, fd->instData->userStates[stateId].stateName);
#endif

#endif
}

void dsInstEnterState(int stateId){
#ifdef VOID_INST
    //we save the void state if its first time calling this
    instEnterState(fd->instData, &fd->instData->userStates[stateId]);

#ifdef BMI_FT
    ftInstPrintState(1, fd->instData->userStates[stateId].stateName);
#endif

#endif
}

void dsInstLeaveState(){
#ifdef VOID_INST
    instLeaveState(fd->instData);

#ifdef BMI_FT
    ftInstPrintState(-1, fd->instData->currentState->stateName);
#endif

#endif
}

#ifdef BMI_FT
void dsInstSaveIntermediaryState() {
    char fileName[MAX_IDIR_LENGTH+1];
    int pid = getpid();
    sprintf(fileName, "%s.%d.%d.ft", fd->name, fd->myRank, pid);
    instSaveIntermediaryTimings(fd->instData, fileName, fileName);
}
#endif

int dsGetRunningTasks( int * numTasks, int ** taskList )
{
    return cacheGetRunningTasks( numTasks, taskList );
}

int *dsGetFinishedTasks(int *numTasks) {
    return cacheGetFinishedTasks(numTasks);
}

int dsRegisterRecoverCallback( RecoverCallback_t *callback ) {
    return cacheRegisterRecoverCallback(callback);
}

int dsAdvertiseFault() {
#ifdef BMI_FT
    return getFDFaultStatus(fd);
#endif
}

int dsLastFilter() {
#ifdef BMI_FT
    return getFDLastFilter(fd);
#endif
}

