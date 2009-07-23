#ifndef MESSAGES_H
#define MESSAGES_H

/* this file contains all message types used by the DS plataform */

/** Filter(or pvm) to manager tags 
 * filter->manager messages use the tag as its type
*/

#define MSGT_TEXIT	1 /** used to notify a task exit, from pvm, dont change this */
#define MSGT_HDEL  	2 /** used to notify host delete, crash, dont change this */
#define MSGT_AEXIT	3 /** used to notify manager the application called exit(dsExit) */
#define MSGT_EOW	4 /** used to notify manager this filter ended its work */
#define MSGT_FERROR	5 /** used to notify manager some system error happened, and filter is aborting, similar to AEXIT */
#define MSGT_LOCALTERM  6 /** used to notify manager this filter has nothing to do **/
#define MSGT_CREATETASK 7 /** used to notify filter a new task **/
#define MSGT_ENDTASK	8 /** used to notify filter the end of a task **/


/** Manager to host messages
 * manager->filter message has the type encapsulated, as the tag is already used for stream identifying
*/

#define MSGT_EOF	101 /** if the message type is end of filter, which means, all done */
#define MSGT_WORK	102 /** if the message type is a work */
#define MSGT_FT   103 /** if the message type is fault tolerance, which means, all filters are going to begin in a global state */ 

/** Filter to Filter message(stream) */
#define MSGT_F2F	201 /** all filter to filter messages should have this */
#define MSGT_INITTERM 202 /** filters send to their neighbors when they think there is nothing to be done */
#endif
