#ifndef VOID_TRACER_DEFINITIONS_H
#define VOID_TRACER_DEFINITIONS_H

/* Files:
 *
 * FilterData/FilterData.c	0
 * FilterDev/FilterDev.c	1
 */

/* Categories:
 *
 * Idle				0
 * Process			1
 * Communication		2
 * Overhead			3
 */

/* Idle operations:
 *
 * Read				0
 * Write			1
 */

/* Process operations:
 *
 * Init				0
 * Process			1
 * Finalize			2
 * Void				3
 */

/* Communication operations:
 *
 * Read				0
 * Write			1
 */

/* Communication types ( write ):
 *
 * Broadcast			0
 * RoundRobin			1
 * MLS				2
 * LS				3
 * Random			4
 */

/* Overhead operations ( void funcion calls ):
 *
 * dsProbe			0
 * dsReadBuffer			1
 * dsWriteBuffer		2
 * dsGetNumWriters		3
 * dsGetNumReaders		4
 * dsInitPack			5
 * dsPackData			6
 * dsWritePackedBuffer		7
 * dsInitReceive		8
 * dsUnpackData			9
 * dsGetMachineMemory		10
 * dsGetLocalInstances		11
 * dsGetMyRank			12
 * dsGetTotalInstances		13
 * dsExit			14
 * dsCreateTask			15
 * dsEndTask			16
 * dsGetCurrentTask		17
 * dsGetOutputPortByName	18
 * dsGetInputPortByName		19
 * dsCloseOutputPort		20
 */

/* State common attribute fields:
 * 
 * int category
 * int operation
 * int file-in
 * int line-in
 * int work
 * int taskId
 * int filter
 * int instance
 */

/*  Only in Overhead/dsCreateTask:
 *  + int numDeps
 *  + int * taskDeps ( if numDeps > 0 )
 */

/*  Only in Communication/Read:
 *  + int instTid ( PVM tid of sender task )
 *  + int incomingMsgId ( id of incoming msg )
 */

/*  Only in Communication/Write:
 *  + int commPolicy
 *  + int pvm_mytid ( PVM task id )
 *  + int msgId ( id of outcoming msg )
 */

/* Idle: */

#define VT_IDLE_READ "iiiiiiii", 0, 0, 0, __LINE__, cacheGetCurrentWork(), \
		     cacheGetCurrentTask(), fd->id, fd->myRank
		     
#define VT_IDLE_WRITE "iiiiiiii", 0, 1, 1, __LINE__, cacheGetCurrentWork(), \
		      cacheGetCurrentTask(), fd->id, fd->myRank

/* Process: */

#define VT_PROC_INIT "iiiiiiii", 1, 0, 0, __LINE__, currentWork, cacheGetCurrentTask(), \
		     fd->id, fd->myRank
		     
#define VT_PROC_PROC "iiiiiiii", 1, 1, 0, __LINE__, currentWork, cacheGetCurrentTask(), \
		     fd->id, fd->myRank
#define VT_PROC_PROCTASK "iiiiiiii", 1, 1, 1, __LINE__, cacheGetCurrentWork(), \
		     cacheGetCurrentTask(), fd->id, fd->myRank
		     
#define VT_PROC_FINALIZE "iiiiiiii", 1, 2, 0, __LINE__, currentWork, cacheGetCurrentTask(), \
		         fd->id, fd->myRank

/* Communication: */

#define VT_COMM_READ "iiiiiiiiII", 2, 0, 1, __LINE__, cacheGetCurrentWork(), \
		     cacheGetCurrentTask(), fd->id, fd->myRank
		     
#define VT_LEAVE_COMM_READ instTid, incomingMsgId
		     
#define VT_COMM_WRITE "iiiiiiiiiiii", 2, 1, 1, __LINE__, cacheGetCurrentWork(), \
		      cacheGetCurrentTask(), fd->id, fd->myRank, \
		      fd->outputPorts[oph]->writePolicy, bufSz, pvm_mytid(), ++msgId

/* Overhead: */

#define VT_OH_PROBE "iiiiiiii", 3, 0, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_RBUFFER "iiiiiiii", 3, 1, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_WBUFFER "iiiiiiii", 3, 2, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETNW "iiiiiiii", 3, 3, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETNR "iiiiiiii", 3, 4, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_INITPACK "iiiiiiii", 3, 5, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_PACK "iiiiiiii", 3, 6, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_WPBUFFER "iiiiiiii", 3, 7, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_IRECEIVE "iiiiiiii", 3, 8, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_UNPACK "iiiiiiii", 3, 9, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETMEM "iiiiiiii", 3, 10, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETLI "iiiiiiii", 3, 11, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETMYRANK "iiiiiiii", 3, 12, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETTI "iiiiiiii", 3, 13, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_EXIT "iiiiiiii", 3, 14, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_CREATETASK "iiiiiiiiia", 3, 15, 1, __LINE__, cacheGetCurrentWork(), taskId, fd->id, fd->myRank, depSize, trcIArrayToTrc( depSize, deps )
#define VT_OH_ENDTASK "iiiiiiii", 3, 16, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETCT "iiiiiiii", 3, 17, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETOPORT "iiiiiiii", 3, 18, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_GETIPORT "iiiiiiii", 3, 19, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank
#define VT_OH_CLOSEOPORT "iiiiiiii", 3, 19, 1, __LINE__, cacheGetCurrentWork(), cacheGetCurrentTask(), fd->id, fd->myRank

#endif
