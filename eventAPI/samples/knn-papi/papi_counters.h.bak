
#ifndef PAPI_COUNTERS
#define PAPI_COUNTERS


/* INCLUSIONS */
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* PROTOTYPES */

/* Initialization of PAPI library and relevant counters
 *
 * @param num_counters Variable in which the number of counters will be written
 *
 * @return Address of an array (of size 'num_counters') of event codes
 *
 * */
int *init_counters(int *num_counters);


/* Print values observed in counters since last invocation of this method. At
 * the 1st time this method is called, it prints the values observed since the
 * call to init_counters.
 *
 * @param num_events Number of events (informed by 'init_counters' method)
 *
 * @return Non-zero on sucess. Zero, otherwise.
 * */
short print_counters(const int num_events, const char *filterName);


/* Start measure of hardware counters specified in file 'papi_counters.h' */
int PAPI_SIMPLE_INIT();

/* Print values in each observed hardware counter */
void PAPI_SIMPLE_PRINT(const int num_counters, const char *filterName);

#endif  // End of header

