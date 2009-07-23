#include <unistd.h>
#include "Layout.h"
#include "FilterSpec.h"
#include "StreamSpec.h"
#include "Hosts.h"

/*
 * 02/07/2004 Coutinho: Implementei getFilterByName
 * 05/07/2004 Coutinho: createFilter2() agora chama initFilterSpec().
 * 14/09/2004 Matheus: changed the struct completly
 */

/* Constructor and destroyer ************************************************/ 
Layout *createLayout() {
	Layout *l = (Layout *) malloc(sizeof(Layout));
	
	//hosts
	l->hostsStruct = hostsCreate();
	//filterspec
	l->numFilters = 0;
	//streamspec
	l->numStreams = 0;

	//xName, cwd and command
	if (getcwd(l->cwd, MAX_CWD_LENGTH) == NULL){
		fprintf(stderr, "Layout.c: directory name is too big\n");
		return NULL;
	}
	sprintf(l->xName, "initScript");
	sprintf(l->command, "%s/%s", l->cwd, l->xName);

	//manager state
	l->managerState.numWorksSent = 0;
	l->managerState.oldestWork = 0;

	return l;
}
void destroyLayout(Layout *l){
	int i;
	
	hostsDestroy(l->hostsStruct);
	for (i=0;i<l->numFilters;i++)
		destroyFilterSpec(l->filters[i]);
	for (i=0;i<l->numStreams;i++)
		destroyStreamSpec(l->streams[i]);
	free(l);

}
/***************************************************************************/

/* Filter stuff ************************************************************/
// adds a filter to the layout
int addFilterSpec(Layout *l, FilterSpec *f) {
	if (l->numFilters == MAXFILTERS)
		return 0;
	else {
		l->filters[l->numFilters++] = f;
		return 1;
	}
}
//returns the pointer to the Filter, NULL if not found
FilterSpec *getFilterSpecByName(Layout *l, char *name){
	int i;
	for (i=0; i<l->numFilters; i++) {
		if (strcmp(name, l->filters[i]->name) == 0) {
			return (l->filters[i]);
		}
	}
	return NULL;
}

void getFilterByTid(Layout *layout, int tid, FilterSpec **pFilterAddress, int *instanceAddress){
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
}

/**************************************************************************/

/**Sanity check for Two Senders and One Receiver
 *
 * The following scenario is not permited by AH currently.
 * <pre>
 * FilterA
 *        \
 *        v
 *      FilterC, inputPort X
 *        ^
 *        /
 * FilterB
 * </pre>
 *
 * OTOH, the scenario with the streams inverted is allowed in this
 * version of AntHill (Itamar's alterations... Not even God knows if
 * they work)
 *
 * @return NO RETURN! This function terminates program execution if an error
 * is found.
 */
void sanityCheckTwoSendersOneReceiver(Layout* l, StreamSpec* s)
{
	int i, j, k;
	_filter_spec_ptr sIF = NULL; /* current stream input filter*/
	_filter_spec_ptr sOF = NULL; /* current stream output filter*/
	_filter_spec_ptr otherIF = NULL;
	_filter_spec_ptr otherOF = NULL;

	char* err_msg = "Invalid layout: filters  '%s' and '%s' are outputting"
		        " to the same input port (%s) of filter '%s'\n";

	sIF = s->fromFilter;
	for (i = 0; i < l->numStreams; ++i ) {
		StreamSpec* other = l->streams[i];
		otherIF = other->fromFilter;
		/* Same input port ... */
		if (strcmp(s->toPortName, other->toPortName) == 0 ) {
			/* ... and same output filter? No can do, buddy.*/
			for(j = 0; j< s->numToSend; ++j) {
				sOF = s->toFilter[j];
				for(k = 0; k < other->numToSend; ++k) {
					otherOF = other->toFilter[k];
					if (sOF == otherOF ){
						fprintf(stderr,
							err_msg,
							sIF->name,
							otherIF->name,
							s->toPortName,
							sOF->name);
						/* No reason to continue. */
						exit(EXIT_FAILURE);
					}
				}
			}
		}
	}

}


/**Sanity check for one sender writing for two receivers in two streams.
 *
 * The following scenario is not permited by AH currently.
 * <pre>
 *                FilterX
 *                 ^
 *                 /
 * FilterA, ouputPort A
 *                 \        << The test is executed from this stream's POV
 *                 v
 *                FilterY 
 * </pre>
 *
 */
void sanityCheckOneSenderTwoReceivers(Layout* l, StreamSpec* s)
{
	int i;
	_filter_spec_ptr sIF = NULL; /* current stream input filter*/
	_filter_spec_ptr otherIF = NULL; /* other stream input filter */

	char* err_msg = "Invalid layout: filter '%s' @ port '%s' is"
			" outputting to different streams at the same time\n";

	sIF = s->fromFilter;
	for (i = 0; i < l->numStreams; ++i ) {
		StreamSpec* other = l->streams[i];
		otherIF = other->fromFilter;
		/* Same output port same (output) filter?  No can do, buddy. */
		if ( (strcmp(s->fromPortName, other->fromPortName) == 0) &&
		     (sIF == otherIF) )
		{
			fprintf(stderr, err_msg, sIF->name, s->fromPortName);
			/* No reason to continue. */
			exit(EXIT_FAILURE);
		}
	}
}

/* StreamSpec *************************************************************/
//adds a stream to the layout, returns 1 on success, 0 otherwise
int addStreamSpec(Layout *l, StreamSpec *s){

	/* Sanity checks
	 *
	 * TODO: move documentation comments of those functions to .h
	 */
	sanityCheckOneSenderTwoReceivers(l, s);
	sanityCheckTwoSendersOneReceiver(l, s);

	/* Register Stream */
	if (l->numStreams == MAXSTREAMS)
		return 0;
	else {
		l->streams[l->numStreams++] = s;
		return 1;
	}
}
//returns the pointer to the stream on success, NULL if not found
StreamSpec *getStreamSpecByName(Layout *l, char *name){
	int i;
	for (i=0; i<l->numStreams; i++) {
		if (strcmp(name, l->streams[i]->name) == 0) {
			return (l->streams[i]);
		}
	}
	return NULL;
}
