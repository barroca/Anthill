
#ifndef MANAGER_C
#define MANAGER_C

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <assert.h>

#include "Messages.h"
#include "Manager.h"
#include "Layout.h"
#include "parser.h"
#include "FilterSpec.h"
#include "FilterData/Policies.h"
#include "FilterData/Ports.h"
#include "FilterData/FilterData.h"
#include "FilterData/Termination.h"
#include "constants.h"
#include <TaskIdList/TaskIdList.h>
#include "StreamSpec.h"

// $Id: Manager.c 32 2006-04-18 20:44:52Z ttavares $
/*
 * 05/10/2006 ITAMAR Changes in sendFiltersData to permit one "from" and various "to"
 * 03/10/2006 FELIPE
 *
 * 12/07/2004 Coutinho: void doesn't call filterFactory anymore. Now we
 * 			use dynamic filter library loading.
 * 14/07/2004 Coutinho: initDatacutter was split in initManager (if is the
 * 			manager) and runFilter (if is a filter).
 * 21/07/2004 Matheus: adding the shell as executble, to avoid problems with
 * 			LD_LIBRARY_PATH
 * 23/07/2004 Matheus: console no more, now is Manager.c and Manager.h
 * 03/09/2004 Matheus: fazer os filtors nao lerem o conf.xml
 *
 */

/* Static functions, not exported, should be declared here */


/** Spawn all filters of the layout. Layout structure must be set.
 * \param layout Pointer to the system layout
 */
static void spawnAllFilter(Layout *layout){
	int i;

	for(i = 0; i < layout->numFilters; i++) {
		FilterSpec *pfilter = layout->filters[i];

#ifdef ATTACH
		if(!(pfilter->attached)) {
#endif
			//spawn all instances of this filter
			printf("Manager.c: spawning filter %s instances\n", pfilter->name);
			if (fsSpawnInstances(pfilter, layout->command, layout->argvSpawn) == -1){
				printf("manager.c: error spawning filter %s, aborting\n", pfilter->name);
				exit(-1);
			}
#ifdef ATTACH
		}
#endif
	}
	printf("Manager.c: all filters spawned successfully\n");
}

static void spawnOneAttachedInstance(Layout *layout, FilterSpec *pFilter, int instanceAddress) {
#ifdef ATTACH
		//spawn specified instance of this filter
		printf("Manager.c: spawning filter %s instances\n", pFilter->name);
		if (fsSpawnOneInstance(pFilter, instanceAddress, pFilter->command, NULL) == -1){
			printf("manager.c: error spawning filter %s, aborting\n", pFilter->name);
			exit(-1);
		}
#endif
}

void updateAllFiltersFaultStatus(Layout *layout, int faultStatus) {
#ifdef BMI_FT
	int i;

	for(i = 0; i < layout->numFilters; i++) {
		FilterSpec *pfilter = layout->filters[i];
		fsSetFaultStatus(pfilter, faultStatus);
	}
#endif
}

void sendEof(FilterPlacement *pFilterP){
	pvm_initsend(PvmDataRaw);
	int tipo_msg = MSGT_EOF;
	pvm_pkint(&tipo_msg, 1, 1);
	//sends the EOF message for all instances of the filter
	pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
}
#ifdef ATTACH
void sendEowToAllInputs(FilterSpec * filterSpec){
	int i, j;
	// for each input Port of this filter
	for(i = 0; i < filterSpec->numInputs; i++){
		// i must send 1 Eow for each instace that writes to this filter
		for(j = 0; j < filterSpec->inputs[i]->fromFilter->filterPlacement.numInstances; j++){
			//send our EOW to listeners on the other side
			int msgType = MSGT_EOW;
			int taskId = -1;

			pvm_initsend(PvmDataRaw);
			pvm_pkint(&msgType, 1, 1);
			pvm_pkint(&taskId, 1, 1);

			pvm_mcast(filterSpec->filterPlacement.tids, filterSpec->filterPlacement.numInstances, filterSpec->inputs[i]->tag);
		}
	}
}
#endif

void resetStreams(Layout *layout){
	int i =0;
	for (i = 0; i < layout->numStreams; i ++){
		resetStream((layout->streams[i]));
	}
}

/** Kills all filters in the void pipeline.
 * \param layout the system layout
 */
static void killAllFilters(Layout *layout){
	int x, y;
	for(x = 0; x < layout->numFilters; x++) {
#ifdef ATTACH
			// if this is an attached filter i dont want to kill it
			// TODO: i just need to notify this filter with EOW
			if(layout->filters[x]->attached){
				sendEowToAllInputs(layout->filters[x]);
				sendEof(&(layout->filters[x]->filterPlacement));
				continue;
			}
#endif
		for(y = 0; y < layout->filters[x]->filterPlacement.numInstances; y++) {
			if (pvm_kill(layout->filters[x]->filterPlacement.tids[y]) < 0) {
				fprintf(stderr, "%s(%d): Error killing filter %s instance: %d (pvm tid:t%x)\n",
						__FILE__, __LINE__, layout->filters[x]->name, y,
						layout->filters[x]->filterPlacement.tids[y]);
			}
		}
	}
}

/** when we receive a signal, we call this function
 * \param signal the signal we captured
*/
/*static void captureSignal(int signal){
	// The manager needs receive all signals...
	if (I_AM_THE_MANAGER ){
		//exit signaling
		pvm_halt();
		printf("Manager.c: received signal %d, bailing...\n", signal);
		exit(128 + signal);
	}
	else {
		//send the signal to the manager
		pvm_sendsig(pvm_parent(),signal);

		pvm_exit();
		exit(128+signal);
	}
}
*/
/** Given a tid, returns a pointer to the filter which is using it
 * \param layout the layout
 * \param tid the tid we are looking for
 * \param pFilterAddress the address of the filter pointer, so we can return the filter here
 * \param instanceAddress we return the instance holding the tid in the variable pointed by this address
 */
/*static void getFilterByTid(Layout *layout, int tid, FilterSpec **pFilterAddress, int *instanceAddress){
	int z, w;
	FilterSpec *pFilter = NULL;
	instanceAddress[0] = -1;
	//find who sent the EOW
	for (z=0; z < layout->numFilters; z++){
		pFilter = layout->filters[z];
		for (w=0; w < pFilter->filterPlacement.numInstances; w++){
			if (pFilter->filterPlacement.tids[w] == tid){
				instanceAddress[0] = w;
				pFilterAddress[0] = pFilter;
				return;
			}
		}
	}
}*/

/** This sends filter data to all filters we have in the layout
 * \param layout the void layout
 */
static void sendFiltersData(Layout *layout){
	FilterSpec *pFilter;
	int i, j, k, x;
	printf("Manager.c: sending filter data now\n");

#ifdef VOID_INST
	//build instrumentation directory(a string like 1-1-1, which is the number of instances of all filters)
	//this is the same for all filters, so we build here and send inside the loop
	char instDir[MAX_IDIR_LENGTH];
	sprintf(instDir, "%s/", INST_DIR);
	for (i=0; i < layout->numFilters-1; i++){
		if (strlen(instDir) >= (MAX_IDIR_LENGTH - 3)){
			//dont want to overflow this array
			fprintf(stderr, "%s %d: warning, instrumentation directory name too big, truncating to %s\n", __FILE__, __LINE__, instDir);
			break;
		}
		sprintf(instDir, "%s%d-", instDir, layout->filters[i]->filterPlacement.numInstances);
	}
	sprintf(instDir, "%s%d", instDir, layout->filters[layout->numFilters-1]->filterPlacement.numInstances);
#endif

	for(i = 0; i < layout->numFilters; i++) {
		pFilter = layout->filters[i];

#ifdef ATTACH
		// if this is a filter of the type attach it wait for another process attach him
		// so we dont need send information about layout now, because the process tha
		// attach this filter go to do it.
		if(pFilter->attach) continue;
#endif
		//for each instance of the filter, we send its data
		for (j=0; j < pFilter->filterPlacement.numInstances; j++){
			int l1, l2, l3, l4, l5, l6, l7, res, pTid;

			//we send only one message with all information inside
			pvm_initsend(PvmDataRaw);
            //Teste
			//current working directory
			l1 = strlen(layout->cwd);
			pvm_pkint(&l1, 1, 1);
			pvm_pkbyte(layout->cwd, l1, 1);

			//send filterID
			pvm_pkint(&i, 1, 1);
			// rank
			pvm_pkint(&j, 1, 1);
			// total number of instances of this filter
			pvm_pkint(&pFilter->filterPlacement.numInstances, 1, 1);
			//send all tids of this filter
			pvm_pkint(pFilter->filterPlacement.tids, pFilter->filterPlacement.numInstances, 1);

#ifdef ATTACH
			pTid = pvm_mytid();
			// if you are using attach your parent in this moment
			// can be other process and not the guy that create you
			// so the parent must send his tid
			pvm_pkint(&pTid, 1, 1);

			// this said if this filter have been attached to this pipeline
			// its usefull because this filter cant finish with a msg of
			// the type EOF from this manager
			pvm_pkint(&pFilter->attached ,1, 1);
#endif
#ifdef BMI_FT
			// indicates the fault status: NO_FAULT, FAULT_OTHER_FILTER_INST,
			// or filter instance rank
			pvm_pkint(&pFilter->faultStatus, 1, 1);

			// if this filter is the last filter in the pipeline
			pvm_pkint(&pFilter->lastFilter, 1, 1);
#endif
			// filtername
			l2 = strlen(pFilter->name);
			pvm_pkint(&l2, 1, 1);
			pvm_pkbyte(pFilter->name, l2, 1);

			//machine memory
			HostsStruct *h = layout->hostsStruct;
			int hostIndex = hostsGetIndexByName(h, pFilter->filterPlacement.hosts[j]);
			int memory = hostsGetMemory(h, hostIndex);
			pvm_pkint(&memory, 1, 1);

			//how many brothers do I have in this machine?(used for memory management)
			int z, numLocalInstances = 0; // me and my brothers
			for (z=0; z < pFilter->filterPlacement.numInstances; z++){
				if (strncmp(pFilter->filterPlacement.hosts[z], pFilter->filterPlacement.hosts[j], MAX_HNAME_LENGTH) == 0){
					numLocalInstances++;
				}
			}
			pvm_pkint(&numLocalInstances, 1, 1);

#ifdef VOID_INST
			int lInst;

			//send instrumentation directory
			lInst = strlen(instDir);
			pvm_pkint(&lInst, 1, 1);
			pvm_pkbyte(instDir, lInst, 1);
#endif
			// shared lib name
			l3 = strlen(pFilter->libname);
			pvm_pkint(&l3, 1, 1);
			pvm_pkbyte(pFilter->libname, l3, 1);

			//port data
			//numOutputs
			pvm_pkint(&pFilter->numOutputs, 1, 1);
			//numInputs
			pvm_pkint(&pFilter->numInputs, 1, 1);

			// OutputPorts
			// for each OutputPort
			// Alterado por FELIPE
			// Because now every filter output can have
			// many destinations, we are implementing a
			// loop to send the information of each destination filter
			for(k = 0; k < pFilter->numOutputs; k++) {
				//port data
				//ITAMAR numToSend = numbers of 'to' one from has
				pvm_pkint(&(pFilter->outputs[k]->numToSend), 1, 1);

				l4 = strlen(pFilter->outputs[k]->fromPortName);
				pvm_pkint(&l4, 1, 1);
				pvm_pkbyte(pFilter->outputs[k]->fromPortName, l4, 1);  //portname

				//stream tag
				pvm_pkint(&pFilter->outputs[k]->tag, 1, 1);

				for( x = 0; x < pFilter->outputs[k]->numToSend; x++ )
				{
					//number of tids & tids
					pvm_pkint(&pFilter->outputs[k]->toFilter[x]->filterPlacement.numInstances, 1, 1);

					pvm_pkint(pFilter->outputs[k]->toFilter[x]->filterPlacement.tids,
							pFilter->outputs[k]->toFilter[x]->filterPlacement.numInstances, 1); //tids

					//write policy name
					l5 = strlen(pFilter->outputs[k]->writePolicyName[x]);
					pvm_pkint(&l5, 1, 1);
					pvm_pkbyte(pFilter->outputs[k]->writePolicyName[x], l5, 1);

					writePolicy_t wp = getWritePolicyByName(pFilter->outputs[k]->writePolicyName[x]);

					// send labeled stream libname if policy is LS
					if ( (wp == LABELED_STREAM) || (wp == MULTICAST_LABELED_STREAM) ){
						l6 = strlen(pFilter->outputs[k]->lsLibName[x]);
						pvm_pkint(&l6, 1, 1);
						pvm_pkbyte(pFilter->outputs[k]->lsLibName[x], l6, 1);
					}
					else {
						//if not LS, we need know who will be the first instance to receive msgs
						//else we can create hotspots
						//we use the rank % number of receiving instances
						res = j % pFilter->outputs[k]->toFilter[x]->filterPlacement.numInstances;
						pvm_pkint(&res, 1, 1);
					}
				}
			}

			// InputPorts
			// foreach InputPort
			for(k = 0; k < pFilter->numInputs; k++) {

				int ind_toPort;

				// Search for the index corresponding to current filter in the
				// destinations of current stream 
				for(x = 0; x < pFilter->inputs[k]->numToSend; x++){  

					if( strcmp(pFilter->name, pFilter->inputs[k]->toFilter[x]->name) == 0 ){ // Found it!

						ind_toPort = x;
						break;
					}

				}

				//portName
				l7 = strlen(pFilter->inputs[k]->toPortName[ind_toPort]);
				pvm_pkint(&l7, 1, 1);
				pvm_pkbyte(pFilter->inputs[k]->toPortName[ind_toPort], l7, 1);

				//number of tids we listen to
				pvm_pkint(&pFilter->inputs[k]->fromFilter->filterPlacement.numInstances, 1, 1); // number of instances
				pvm_pkint(pFilter->inputs[k]->fromFilter->filterPlacement.tids,
						pFilter->inputs[k]->fromFilter->filterPlacement.numInstances, 1); //the tids of the instances
				pvm_pkint(&pFilter->inputs[k]->tag, 1, 1); //the stream tag

				writePolicy_t wp = getWritePolicyByName(pFilter->inputs[k]->writePolicyName[x]);
				int aux = 0;
				// send labeled stream libname if policy is LS
				if ( (wp == LABELED_STREAM) || (wp == MULTICAST_LABELED_STREAM) ){
					// yes it has a label
					aux = 1;
					pvm_pkint(&aux, 1, 1);
					l6 = strlen(pFilter->inputs[k]->lsLibName[x]);
					pvm_pkint(&l6, 1, 1);
					pvm_pkbyte(pFilter->inputs[k]->lsLibName[x], l6, 1);
				}else{
					pvm_pkint(&aux, 1, 1);
				}


			}
			pvm_send(pFilter->filterPlacement.tids[j], 0);
		}
	}
}


/// Init a pipeline manager. Void function.
///	\param confFile XML file configuration, NULL if we want to make things by hand
///	\param argc argc received by main function that will be forwarded to filter processes
///	\param argv argv received by main function that will be forwarded to filter processes
///	\return Layout of the pipeline
static Layout *initManager(char *confFile, int argc, char **argv) {
	char hostname[50];
	int i,j;
	int numFilterInstances = 0;

	//the manager pointer
	Layout *layout;
	layout = createLayout();
	layout->argvSpawn = (char **)malloc(sizeof(char *) * (argc));

	// my hostname
	gethostname(hostname, 50);
	fprintf(stderr, "manager: pvm tid:t%x hostname:%s\n", pvm_mytid(), hostname);
	printf("manager: pvm tid:t%x hostname:%s\n", pvm_mytid(), hostname);
	// read XML config file
	printf("\n====================================\n");
	printf("Manager.c: start parsing the file...\n");
	if (readConfig(confFile, layout) == -1){
		printf("Manager.c: parse error, aborting\n");
		exit(1);
	}
	printf("Manager.c: parser ended successfully\n");
	printf("====================================\n\n");

	// for each filter, we spawn the children process, but dont send any data yet
	for(i = 0; i < layout->numFilters; i++) {
		FilterSpec *pFilter = layout->filters[i];

		// Copies argv to layout->argvSpawn, except argv[0]
		for (j=0; j<argc-1; j++) {
			layout->argvSpawn[j] = argv[j+1];
		}
		layout->argvSpawn[argc-1] = NULL;

		//spawn all instances of this filter
		printf("Manager.c: spawning filter %s instances\n",
			pFilter->name);

		if (fsSpawnInstances(pFilter, layout->command, layout->argvSpawn) == -1){
			printf("Manager.c: error spawning filter %s, aborting\n", pFilter->name);
			exit(1);
		}

		numFilterInstances += pFilter->filterPlacement.numInstances;
#ifdef BMI_FT
		// Initialize the fault status
		pFilter->faultStatus = NO_FAULT;
#endif
	}

	printf("All process started, sending data now....\n\n");
	sendFiltersData(layout);

#ifdef VOID_TERM
	gt = initGlobalTermination(layout, numFilterInstances);
#endif

	return layout;
}

/* End static funcions ***********************/

/* Here begins the user functions */

/// Init a ds pipeline.
///	\param confFile XML file configuration, NULL if we want to make things by hand
///	\param argc argc received by main function that will be forwarded to filter processes
///	\param argv argv received by main function that will be forwarded to filter processes
///	\return Layout of the pipeline
Layout *initDs(char *confFile, int argc, char **argv) {
	int err;

	//configura pvm pra enviar dado diretamente(IMPORTANTE!!!!)
	pvm_setopt(PvmRoute, PvmRouteDirect);

	//err == 0, OK
 	err = pvm_start_pvmd(0, NULL, 0);
	switch (err){
		case PvmSysErr:
			printf("Manager.c: error starting PVM, aborting\n");
			exit(1);
		break;
	}

	//start random number generator
	srandom(getpid());

	// config DS to use signals
//	signal(SIGSEGV, &captureSignal);
//	signal(SIGINT, &captureSignal);
//	signal(SIGILL, &captureSignal);
//	signal(SIGHUP, &captureSignal);
//	signal(SIGTERM, &captureSignal);

	if (I_AM_THE_MANAGER) {
		return initManager(confFile, argc, argv);
	} else {
		runFilter(argc, argv);
		exit(0);
	}
}

/* Compare 2 task ids (integers)*/
int compareTaskId(const void *a, const void *b)
{
	if (( *(int *)a) < ( *(int *)b)) return -1;
	if (( *(int *)a) > ( *(int *)b)) return  1;
	return 0;
}

///close the port, but does not destroy it
/*void closeOutputPort(OutputPort *op){
	if (op->state == OP_STATE_CLOSED)
		return;

	//send our EOW to listeners on the other side
	int msgType = MSGT_EOW;
	int taskId = -1;

	pvm_initsend(PvmDataRaw);
	pvm_pkint(&msgType, 1, 1);
	pvm_pkint(&taskId, 1, 1);

	pvm_mcast(op->tidsDestinations, op->numDestinations, op->tag);

	setOPState(op, OP_STATE_CLOSED);
}*/


int replaceCrashedHost(Layout *layout, FilterSpec *pCrashedFilter, int crashedInstance) {
	HostsStruct *pHostStruct = layout->hostsStruct;

	char *crashedHostName = pCrashedFilter->filterPlacement.hosts[crashedInstance];
	int hostDeletedIdx = hostsGetIndexByName(pHostStruct, crashedHostName);
	if(strcmp(crashedHostName, hostsGetName(pHostStruct, hostDeletedIdx))) {

		//should never get here
		assert(0);

		printf("Manager.c: Problem detecting dead host. PVM detected: %s. Void detected: %s.\n",
				crashedHostName, hostsGetName(pHostStruct, hostDeletedIdx));
		exit(-1);
	}

	// Change the dead host status
	hostsSetStatus(pHostStruct, hostDeletedIdx, NOTAVAIL);

	// we only need to replace the host in the current instance because for
	// each instance in the dead host, we will recieve a notification
	/// \todo Choose host respecting filter demsnds
	/// \todo change hostsGetIndex()
	int newHostIdx = hostsGetIndex(pHostStruct);
	pCrashedFilter->filterPlacement.hosts[crashedInstance] =  hostsGetName(pHostStruct, newHostIdx);

	//find all filters using the dead host and replace the host
/*	for(i = 0; i < layout->numFilters; i++) {
		FilterPlacement *pfilterPlac = &(layout->filters[i]->filterPlacement);
		for(j = 0; j < pfilterPlac->numInstances; j++) {
			if(strcmp(pfilterPlac->hosts[j], crashedHostName) == 0) {
				int newHostIdx = hostsGetIndex(pHostStruct);
				pfilterPlac->hosts[j] =  hostsGetName(pHostStruct, newHostIdx);
			}
		}
	}*/

	return 0;
}


/// user function:add a new query to a pipeline of filters. Called by user manager runs this.
///	\param layout System Layout.
///	\param work Buffer with a Unit of Work (UoW)
///	\param workSize Unit of Work Size (UoW)
///	\return Zero on success, -1 on error.
int appendWork(Layout *layout, void *work, int workSize){
#ifdef NO_BARRIER

	// sends work for each filter
	pvm_initsend(PvmDataRaw);
	// First tell that is a mensage of WORK
	int msgType = MSGT_WORK;
	pvm_pkint(&msgType, 1, 1);
	//then attach the work to it
	pvm_pkbyte((char *)work, workSize, 1);

	// for each filter, send his work
	for(i = 0; i < layout->numFilters; i++) {
		FilterPlacement *pFilterP = &(layout->filters[i]->filterPlacement);
		// sends work to all filters of this set
		pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
	}

#else
	int i;
	int totalEows = 0, numEowsReceived;
#ifdef ATTACH
	int totalAttachedFilters = 0;
#endif
	int reconf = 0 /** should we reconfigure? */, remainingReconfs = 6; //how many times should we try?

#ifdef VOID_INST
#ifdef BMI_FT
	char instDir[MAX_IDIR_LENGTH];
	sprintf(instDir, "%s/", INST_DIR);
	for (i=0; i < layout->numFilters-1; i++){
		if (strlen(instDir) >= (MAX_IDIR_LENGTH - 3)){
			//dont want to overflow this array
			fprintf(stderr, "%s %d: warning, instrumentation directory name too big, truncating to %s\n", __FILE__, __LINE__, instDir);
			break;
		}
		sprintf(instDir, "%s%d-", instDir, layout->filters[i]->filterPlacement.numInstances);
	}
	sprintf(instDir, "%s%d", instDir, layout->filters[layout->numFilters-1]->filterPlacement.numInstances);
	char managerLogFile[MAX_IDIR_LENGTH+20];
	sprintf(managerLogFile, "%s/manager.time", instDir);
	FILE *fp = fopen(managerLogFile, "w");

	struct timeval tv;
	struct timezone tz; //not used
	//get the time
	gettimeofday(&tv, &tz);

	if(fp != NULL)
	fprintf(fp, "1 %ld %ld\n", tv.tv_sec, tv.tv_usec);
#endif
#endif

	//before sending, we check if we received any filter error
	int bufid = pvm_probe(-1, MSGT_FERROR);
	if (bufid != 0){
		int bytes, tag, tid;
		char *msg;
		pvm_bufinfo(bufid, &bytes, &tag, &tid);
		msg = (char*)malloc(bytes+1);
		pvm_recv(tid, MSGT_FERROR);
		pvm_upkbyte(msg, bytes, 1);
		msg[bytes] = '\0';

		fprintf(stderr, "Manager.c: Error, received death notification\n");
		fprintf(stderr, "Manager.c: %s\n", msg);
		free(msg);
		killAllFilters(layout);
		exit(-1);
	}

	printf("Manager.c: starting work...\n");

	// number of EOWs we expect to receive
	for(i = 0; i < layout->numFilters; i++) {
		totalEows += layout->filters[i]->filterPlacement.numInstances;
#ifdef ATTACH
		if(layout->filters[i]->attached){
			totalAttachedFilters += layout->filters[i]->filterPlacement.numInstances;
		}
		// TODO:TOSCO:::
		if(layout->filters[i]->attach) return 0;
#endif
	}

	//we stay in this loop while we have to reconfigure
	//usually, this will be only one time, unless a we get some reconf message
	do{

		// sends work for each filter
		pvm_initsend(PvmDataRaw);
		int msgType;
#ifdef VOID_FT
		if(!reconf){
#endif
			// First tell that is a mensage of WORK
			msgType = MSGT_WORK;
#ifdef VOID_FT
		} else {
			// one fault has occurred
			msgType = MSGT_FT;
		}
#endif
		pvm_pkint(&msgType, 1, 1);
		//then attach the work to it
		pvm_pkbyte((char *)work, workSize, 1);

		reconf = 0; //we are optimistic, always expect to not reconfigure


		// for each filter, send his work
		for(i = 0; i < layout->numFilters; i++) {
			FilterPlacement *pFilterP = &(layout->filters[i]->filterPlacement);
			// sends work to all filters of this set
			pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
		}
/*

		TaskIdList *finalTaskIdList = NULL, *currentTaskIdList;
		int filtersThatUseTasks = 0;
		// Manager receives filter's terminated tasks list
		for(i = 0; i < layout->numFilters; i++) {
			FilterPlacement *pFilterP = &(layout->filters[i]->filterPlacement);
#ifdef ATTACH
			// if this filter is of the attached
			// type i dont need worry about use Task
//			if(layout->filters[i]->attached)continue;
#endif
			for(j = 0; j < pFilterP->numInstances; j++) {
				int instanceUseTasks = -1;

				// Get is this filter use tasks
				pvm_recv(pFilterP->tids[j], 0);
				pvm_upkint(&instanceUseTasks, 1, 1);
				layout->filters[i]->useTasks = instanceUseTasks;

#ifdef VOID_FT
				if (instanceUseTasks) {
					currentTaskIdList = (TaskIdList *)unpackTaskIdList();

					//	Para fazer intersecao, gerente ordenar? as listas de tarefas recebidas e utilizar? a fun??o meet() do CrazyMiner/ID3.
					qsort(currentTaskIdList->vetor, currentTaskIdList->size, sizeof(int), compareTaskId);
					if(finalTaskIdList == NULL) {
						finalTaskIdList = currentTaskIdList;
					} else {
						// 	Manager makes the intersection of all finished tasks lists
						finalTaskIdList = taskIdListIntersection(finalTaskIdList, currentTaskIdList);
						taskIdListDestroy(currentTaskIdList);
					}
				}
#endif

			} // for

			if (layout->filters[i]->useTasks) filtersThatUseTasks++;
		}

		//	Gerente devolve resultado das intersecoes para todas as instancias de todos os filtros.
		for(i = 0; i < layout->numFilters; i++) {
 			FilterPlacement *pFilterP = &(layout->filters[i]->filterPlacement);
#ifdef ATTACH
			// if this filter is of the attached
			// type i dont need worry about use Task
//TODO			if(layout->filters[i]->attached)continue;
#endif

			int needForwardTaskMessages = 1;
			if (filtersThatUseTasks < 2) needForwardTaskMessages = 0;

#ifdef VOID_FT
			if (layout->filters[i]->useTasks) {
				// Send if they should forward task messages
				// and pigback :-) the final task id list
 				pvm_initsend(PvmDataDefault);
 				pvm_pkint(&needForwardTaskMessages, 1, 1);

				packTaskIdList(finalTaskIdList);
				pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
			} else {
#endif
				// Only send if they should forward task messages
 				pvm_initsend(PvmDataDefault);
 				pvm_pkint(&needForwardTaskMessages, 1, 1);
				pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
#ifdef VOID_FT
			}
#endif
		}
		taskIdListDestroy(finalTaskIdList);

*/

		//now we receive the EOWs
		numEowsReceived = 0;

		//now we expect to receive EOW or errors
		while(numEowsReceived < totalEows){
			//we are open to receive anything from anyone here
			//all messages to the manager should be tagged, so we now their type
			int szMsg = -1;
			int inst_tid = -1;
			int msgTag = -1;
			int bufid = pvm_recv(-1, -1);
			pvm_bufinfo(bufid, &szMsg, &msgTag, &inst_tid);

			switch (msgTag){
				case (MSGT_EOW): {
					//received EOW, expect this usually
					int instance = -1;
					FilterSpec *pFilter = NULL;

					getFilterByTid(layout, inst_tid, &pFilter, &instance);

					if ((pFilter != NULL) && (instance != -1)){
						printf("Manager.c: EOW received from %s, instance %d\n",
								pFilter->name, instance);
					} else {
						fprintf(stderr, "Manager.c: unknown EOW received! Shouldnt get here!\n");
					}
					numEowsReceived++;
					break;
				}
				case (MSGT_AEXIT):
				case (MSGT_FERROR): {
					//someone called dsExit or system error at the filter side
					//common cause for this are library not found, wrong initscritpt etc
					char *message = (char*)malloc(sizeof(char)*szMsg+1);
					pvm_upkbyte(message, szMsg, 1);
					message[szMsg] = '\0';

					//the filter and the instance
					FilterSpec *fp = NULL;
					int instance = -1;
					getFilterByTid(layout, inst_tid, &fp, &instance);

					if (msgTag == MSGT_AEXIT){
						printf("Manager.c: Filter %s, instance %d(tid %x) called dsExit: %s\n",
								fp->name, instance, inst_tid, message);
					} else {
						printf("Manager.c: Filter %s error, instance %d(tid %x) called exit: %s\n",
								fp->name, instance, inst_tid, message);
					}
					free(message);
					// kill all instances
					killAllFilters(layout);
					exit(-1);
					break;
				}
				//task exited or host crashed
				case (MSGT_TEXIT): case (MSGT_HDEL): {
					//we only reconfigure a fixed number of times
					if (remainingReconfs <= 0){
						//max number of reconfigurations reached... aborting
						fprintf(stderr, "Manager.c: max reconfigurations reached, aborting...\n");
						reconf = 0;

						// kill all instances which might be alive
						killAllFilters(layout);
						exit(-1);;

					}
#ifdef BMI_FT
					gettimeofday(&tv, &tz);
					fprintf(fp, "2 %ld %ld\n", tv.tv_sec, tv.tv_usec);
#endif

					remainingReconfs--;
					reconf = 1;
					// In case of pvm notification, inst_tid will be t80000000
					int notifiesRecv = 1; // We are receiving the first death notification
					int deadFilterTid = -1;
					FilterSpec *pFilter = NULL;
					int instanceDead = -1;

					// Get the tid and name of the dead filter
					int info = pvm_upkint(&deadFilterTid, 1, 1);
					if (info < 0) pvm_perror("Manager.c: error calling pvm_upkint");

					//discover which filter died
					getFilterByTid(layout, deadFilterTid, &pFilter, &instanceDead);

					if((pFilter != NULL) && (instanceDead != -1)){
						if (msgTag == MSGT_TEXIT) {
							fprintf(stderr, "Manager.c: filter %s: instance %d (tid t%x) of %d is dead!!!\n",
									pFilter->name, instanceDead, deadFilterTid, pFilter->filterPlacement.numInstances);
						} else {
							fprintf(stderr, "Manager.c: filter %s: instance %d (tid t%x) of %d machine's crashed!!!\n",
									pFilter->name, instanceDead, deadFilterTid, pFilter->filterPlacement.numInstances);
						}
					}
					printf("Manager.c: starting reconfiguration\n");

					// kill all filters in the pipeline
					killAllFilters(layout);

					if (msgTag == MSGT_HDEL) {
						//int his case, host died, so we must change layout
						replaceCrashedHost(layout, pFilter, instanceDead);
					}

#ifdef ATTACH
					if (pFilter->attached) {
						// In this case, all filters that were killed have to notify
						// their dead.
						notifiesRecv = 0;
					}
#endif

					//Flush the streams
					//receive all messages which are about to arrive till we get the death notification
					//pvm order should garantee this
#ifdef ATTACH
					while (notifiesRecv < (totalEows - totalAttachedFilters)){
#else
					while (notifiesRecv < totalEows) {
#endif
						int newMsgTag = -1;
						bufid = pvm_recv(-1, MSGT_TEXIT);
						info = pvm_bufinfo(bufid, NULL, &newMsgTag, &inst_tid);
						info = pvm_upkint(&deadFilterTid, 1, 1);
						if (info < 0) pvm_perror("Manager.c: error calling pvm_upkint");

						fprintf(stderr, "Manager.c: WARNING: received notification (tag %d) about pvm tid t%x death\n", newMsgTag, deadFilterTid);
						notifiesRecv++;
					}

#ifdef ATTACH
					if(pFilter->attached) {
						notifiesRecv = 1;
					}else {
						notifiesRecv = 0;
					}
					// Receive all EOW messages from the attached filters.
					while(notifiesRecv < totalAttachedFilters) {
						int newMsgTag = -1;
						bufid = pvm_recv(-1, MSGT_EOW);
						info = pvm_bufinfo(bufid, NULL, &newMsgTag, &inst_tid);
						if (info < 0) pvm_perror("Manager.c: error calling pvm_upkint");

						fprintf(stderr, "Manager.c: WARNING: received EOW (tag %d) from pvm tid t%x\n", newMsgTag, inst_tid);
						notifiesRecv++;
					}
#endif
					// probes for remaining machine crash notifications
					while (pvm_probe(-1, MSGT_HDEL) > 0) {
						int newMsgTag = -1;
						bufid = pvm_recv(-1, MSGT_HDEL);
						info = pvm_bufinfo(bufid, NULL, &newMsgTag, &inst_tid);
						info = pvm_upkint(&deadFilterTid, 1, 1);
						if (info < 0) pvm_perror("Manager.c: error calling pvm_upkint");

						fprintf(stderr, "Manager.c: WARNING: received notification (tag %d) about pvm tid t%x machine's crash\n", newMsgTag, deadFilterTid);

						// Replace the died host
						FilterSpec *pCrashedFilter = NULL;
						int crashedInstance = -1;
						getFilterByTid(layout, deadFilterTid, &pCrashedFilter, &crashedInstance);
						replaceCrashedHost(layout, pCrashedFilter, crashedInstance);
					}
#ifdef BMI_FT
					updateAllFiltersFaultStatus(layout, FAULT_OTHER_FILTER_INST);
					pFilter->faultStatus = instanceDead;
#endif

					//spawn all filters again
					spawnAllFilter(layout);
#ifdef ATTACH
					// Verifies if the dead filter is an attached. If yes, spawn it.
					if(pFilter->attached == 1) {
						spawnOneAttachedInstance(layout, pFilter, instanceDead);
					}
#endif
					resetStreams(layout);
					//resend the data
					sendFiltersData(layout);
					//start all over again
					numEowsReceived = 0;

#ifdef BMI_FT
					gettimeofday(&tv, &tz);
					fprintf(fp, "3 %ld %ld\n", tv.tv_sec, tv.tv_usec);
#endif
					break;
				}
#ifdef VOID_TERM
				// One filter instance detected local termination
				case (MSGT_LOCALTERM): {
					int localTermTag; // filter instance local termination tag
					pvm_upkint(&localTermTag, 1, 1);
					verifyGlobalTermination(inst_tid, localTermTag);
					break;
				}
#endif
				default: {
					fprintf(stderr, "Manager.c: error receiving EOW, unknown tag!!!\n");
				}
			} //end switch message tag
			if((msgTag == MSGT_TEXIT) || (msgTag == MSGT_HDEL)) {
				// work should be sent again
				break;
			}
		} //end receiving eows
	} while(reconf == 1); //leave this loop if we will not reconfigure

#ifdef BMI_FT
	gettimeofday(&tv, &tz);

	fprintf(fp, "4 %ld %ld\n", tv.tv_sec, tv.tv_usec);
#endif

	printf("Manager.c: Work ended\n\n");
	return 0;
#endif
}


/// Finalize a Void pipeline. Only manager runs this.
int finalizeDs(Layout *layout) {
#ifdef NO_BARRIER

#else
	int i;

	// Envia eof para todos os filtros
	// Primeiro envia se eh work (WORK) ou EOF (END_OF_FILTER)
	pvm_initsend(PvmDataRaw);
	int tipo_msg = MSGT_EOF;
	pvm_pkint(&tipo_msg, 1, 1);
	//sends the EOF message for all instances of the filter
	for(i = 0; i < layout->numFilters; i++) {
		FilterPlacement *pFilterP = &(layout->filters[i]->filterPlacement);
#ifdef ATTACH
		if(layout->filters[i]->attach) continue;// this filter cant not receive a EOF because
						// it needs still runnig
#endif
		pvm_mcast(pFilterP->tids, pFilterP->numInstances, 0);
	}
	destroyLayout(layout);
	pvm_exit();
	return 0;
#endif
}

#endif
