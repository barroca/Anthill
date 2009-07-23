#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "structs.h"

/// constructor
Layout *createLayout();
/// destroyer 
void destroyLayout(Layout *l);

/* Filter stuff ************************************************************/
///add filter to the layout, returns 1 on success, 0 otherwise
int addFilterSpec(Layout *l, FilterSpec *f);
///returns a pointer to a filter, given its name, NULL if not found
FilterSpec *getFilterSpecByName(Layout *l, char *name);

/** Given a tid, returns a pointer to the filter which is using it
 * @param layout the layout
 * @param tid the tid we are looking for
 * @param pFilterAddress the address of the filter pointer, so we can return the filter here
 * @param instanceAddress we return the instance holding the tid in the variable pointed by this address
 */
void getFilterByTid(Layout *layout, int tid, FilterSpec **pFilterAddress, int *instanceAddress);

/* Streams stuff ***********************************************************/
///adds a given stream to the layout, returns 1 on success, 0 otherwise
int addStreamSpec(Layout *l, StreamSpec *s);
///returns a pointer to a stream, given its name, NULL if not found
StreamSpec *getStreamSpecByName(Layout *l, char *name);
#endif
