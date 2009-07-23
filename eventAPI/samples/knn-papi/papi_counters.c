

#include "papi_counters.h"


int *init_counters(int *num_counters){


    const int num_events = 1;
    char *event_names[num_events];
//    event_names[0] = "PAPI_L1_TCM";  // together with: PAPI_TOT_CYC, PAPI_TOT_IIS, PAPI_TOT_INS
//    event_names[1] = "PAPI_L2_TCM";
// event_names[0] = "PAPI_L1_DCH";
// event_names[1] = "PAPI_L1_DCM";
// event_names[0] = "PAPI_L2_DCH";
 event_names[0] = "PAPI_L2_DCM";

    int i;

    *num_counters = num_events;


    /* Initialize library */
    int retval = PAPI_library_init(PAPI_VER_CURRENT);

    switch(retval){

        case PAPI_NOT_INITED:
            fprintf(stderr, "PAPI library has not been initialized!\n");
            break;

        case PAPI_LOW_LEVEL_INITED:
            fprintf(stderr, "PAPI is already initialized!\n");
            break;

        case PAPI_HIGH_LEVEL_INITED:
            fprintf(stderr, "Another high level function was called before init!\n");
            break;

    }



    /* Check the presence of required counters */
    int *event_codes = (int *) calloc(num_events, sizeof(int));

    for(i=0; i < num_events; i++){

        retval = PAPI_event_name_to_code(event_names[i], event_codes + i);

        switch(retval){

            case PAPI_OK:
                printf("Event %s is present (iteration %d)\n", event_names[i],
                        i);
                break;

            case PAPI_EINVAL:
                fprintf(stderr, "Invalid argument to PAPI_query_event at \
                        iteration %d (code %d)\n", i, event_codes[i]);
                return NULL;
                break;

            case PAPI_ENOTPRESET:
                fprintf(stderr, "Not a valid PAPI preset at iteration %d\n",
                        i);
                return NULL;
                break;

            case PAPI_ENOEVNT:
                fprintf(stderr, "Preset is not available at iteration %d\n",
                        i);
                return NULL;
                break;

            default:
                printf("Unknown return value from PAPI_event_name_to_code!\n");
                return NULL;

        }

    }

    return event_codes;
}



short print_counters(const int num_events, const char *filterName){


    int i=0;
    char *event_names[num_events];
//    event_names[0] = "PAPI_L1_TCM";
//    event_names[1] = "PAPI_L2_TCM";
// event_names[0] = "PAPI_L1_DCH";
// event_names[1] = "PAPI_L1_DCM";
// event_names[0] = "PAPI_L2_DCH";
 event_names[0] = "PAPI_L2_DCM";

    /** Read and print value of each counter **/
    long_long events_count[num_events];
    memset(events_count, 0, num_events*sizeof(long_long));

    printf("[%s] Reading counters ...\n", filterName);
    //if(
    PAPI_read_counters(events_count, num_events);
    //< PAPI_OK ){

    printf("[%s] Printing counters ...\n", filterName);


  // Open log file
   // char logfilename[100];
//    sprintf(logfilename, "%s.execution_log", filterName);
    FILE *log = stdout;//fopen(stdout, "w");
    if( log == NULL ){

        perror("Can't open log file");
    }


    // Print counters
    for(i=0; i < num_events; i++){


        fprintf(log, "[%s] Event %s occured %lld times\n", filterName,
                event_names[i], events_count[i]);
    }

    //}

    //else {

    //    perror("Failed to read counters");
    //}


    return 1;
}


/* Start measure of hardware counters specified in file 'papi_counters.h' */
int PAPI_SIMPLE_INIT(){

    int num_counters = 0;
    int *event_codes = init_counters(&num_counters);

    char *errMsg = "";

    if( event_codes == NULL ){

        errMsg = "Unable to initialize PAPI counters!\n";
        fprintf(stderr, errMsg);
    }

    int retval = PAPI_start_counters(event_codes, num_counters);

    switch(retval){

        case PAPI_OK:
            break;

        case PAPI_EINVAL:
            errMsg = "Invalid arguments to start counting!\n";
            break;

        //case PAPI_ESYS:
        //    errMsg = "Unable to start counting";
        //    perror(errMsg);
        //    dsExit(errMsg);
        //    break;

        case PAPI_ENOMEM:
            errMsg = "Insufficient memory to start counting";
            break;

        case PAPI_ECNFLCT:
            errMsg = "Conflicting events to count in this hardware";
            break;

        case PAPI_ENOEVNT:
            errMsg = "Unavailable event. Can't start counting";
            break;

    }

    return num_counters;
}



/* Print values in each observed hardware counter */
void PAPI_SIMPLE_PRINT(const int num_counters, const char *filterName){

    print_counters(num_counters, filterName);
}





