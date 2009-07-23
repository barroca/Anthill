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

/* StreamSpec *************************************************************/
//adds a stream to the layout, returns 1 on success, 0 otherwise
int addStreamSpec(Layout *l, StreamSpec *s){
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
