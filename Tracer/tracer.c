#include "tracer.h"

TrcData *trcCreateData( char * filename )
{
	TrcData * trc;
	
	trc = (TrcData*) malloc( sizeof(TrcData) );
	if ( ( trc->tracefile = fopen( filename, "a") ) == NULL )
	{
		fprintf(stderr, "%s %d, %s: Cannot open file \"%s\" for writing tracing data.\n", __FILE__, __LINE__, __FUNCTION__, filename);
		return( NULL );
	}

	gettimeofday( &(trc->initTime), NULL );
	fprintf( trc->tracefile, "%lu\n", trc->initTime.tv_usec+(1000000*trc->initTime.tv_sec));
	//current state, state stack and state fifo
	trc->currentState = NULL;
	trc->stackedStates = stCreate();
	trc->savedStates = listCreate();
	trc->numSavedStates = trc->numStackedStates = 0;
	return trc;
}

void trcDestroyData( TrcData ** trcPointer )
{
	if( *trcPointer == NULL ){
		printf(
			"%s %d, %s: Warning! Trying to destroy previously destroyed data.\n",
			__FILE__,
			__LINE__,
			__FUNCTION__
			);
		return;
	}
	if( (*trcPointer)->numStackedStates > 0 )
		printf(
			   "%s %d, %s: Warning, there were still %d stacked states when trcData was destroyed !\n",
			   __FILE__,
			   __LINE__,
			   __FUNCTION__,
			   (*trcPointer)->numStackedStates
			   );

	while( (*trcPointer)->numStackedStates > 0 )
		trcLeaveState( *trcPointer );
	
	trcFlush( (*trcPointer) );
	fclose( (*trcPointer)->tracefile );
	free( (*trcPointer) );
	*trcPointer = NULL;
}

void trcEnterState(TrcData *trc, char * types, ...)
{
	int i, j, *aux;
	char * saux;
	TrcState * state;
	va_list argList;
	
	state = malloc( sizeof( TrcState ));
	state->numAttributes = strlen( types );
	j = 0;
	state->numPosAttributes = 0;
	for( i = 0; i < state->numAttributes; i++ )
	{
		if( types[i] == 'i' || types[i] == 's' || types[i] == 'a' )
			j++;
		else if( types[i] == 'I' || types[i] == 'S' || types[i] == 'A' )
			state->numPosAttributes++;
		else
			fprintf(stderr, "%s [%d], %s: Unknown attibute type.\n",
					__FILE__, __LINE__, __FUNCTION__);
	}

	va_start( argList, j );

	state->attributes=(TrcAttribute*)malloc(sizeof(TrcAttribute)*state->numAttributes);

	state->numPosAttributes = 0;
	for ( i = 0; i < state->numAttributes; i++ )
	{
		state->attributes[i].type = types[i];
		switch( types[i] )
		{
			case 'i':
				state->attributes[i].value = malloc( sizeof(int) );
				aux = state->attributes[i].value;
				(*aux) = va_arg( argList, int );
				break;
			case 'a':
				aux = va_arg( argList, int* );
				state->attributes[i].value = (void *)aux;
				break;
			case 's':
				saux = va_arg( argList, char* );
				state->attributes[i].value = (void *)saux;
				break;
			case 'I':
				state->attributes[i].value = NULL;
				break;
			case 'A':
				state->attributes[i].value = NULL;
				break;
			case 'S':
				state->attributes[i].value = NULL;
				break;
		}
	}

	va_end( argList );

	//compute the timings
	gettimeofday( &(state->initTime), NULL );
	getrusage( RUSAGE_SELF, &(state->initRusage) );
	
	//push current state
	stPush(trc->stackedStates, trc->currentState);
	trc->numStackedStates++;

	//change state
	trc->currentState = state;
	return;
}

void trcLeaveState(TrcData *trc, ...)
{
	va_list argList;
	int i, *aux;
	char * saux;

	//pop the current state
	TrcState *state;
	if (( trc->numStackedStates <= 0 ) || stPop(trc->stackedStates, (void**)&state) == -1)
	{
		fprintf(
				stderr, "%s %d, %s: Leaving unexisting state. Stack empty (stackedStates=%d).\n",
				__FILE__,
				__LINE__,
				__FUNCTION__,
				trc->numStackedStates
				);
		return;
	}
	trc->numStackedStates--;

	gettimeofday( &(trc->currentState->endTime), NULL);
	getrusage( RUSAGE_SELF, &(trc->currentState->endRusage) );
	printf("numPosAttributes = %d\n", trc->currentState->numPosAttributes );
	va_start( argList, trc->currentState->numPosAttributes );
	for( i = 0; i < trc->currentState->numAttributes; i++ )
	{
		switch( trc->currentState->attributes[i].type )
		{
			case 'I':
				trc->currentState->attributes[i].value = malloc( sizeof(int) );
				aux = trc->currentState->attributes[i].value;
				(*aux) = va_arg( argList, int );
				break;
			case 'A':
				aux = va_arg( argList, int* );
				trc->currentState->attributes[i].value = (void *)aux;
				break;
			case 'S':
				saux = va_arg( argList, char* );
				trc->currentState->attributes[i].value = (void *)saux;
				break;
		}
	}
	va_end( argList );

	// save the state info
	listInsertTail(trc->savedStates, (void*)trc->currentState);
	trc->numSavedStates++;
	
	//change state
	trc->currentState = state;

	if( trc->numSavedStates >= MAXSAVEDSTATES )
		trcFlush( trc );
	
	return;
}

static void trcPrintStateData( FILE * file, TrcState * state )
{
	int i, j, *aux, *size;
	char * saux;
	
	for( i = 0; i < state->numAttributes; i++ )
	{
		switch( state->attributes[i].type )
		{
			case 'i':
				aux = state->attributes[i].value;
				fprintf( file, SEPARATOR"%d", *aux );
				break;
			case 'I':
				aux = state->attributes[i].value;
				fprintf( file, SEPARATOR"%d", *aux );
				break;
			case 's':
				saux = state->attributes[i].value;
				fprintf( file, SEPARATOR"%s", saux );
				break;
			case 'S':
				saux = state->attributes[i].value;
				fprintf( file, SEPARATOR"%s", saux );
				break;
			case 'a':
				size = state->attributes[i].value;
				for( j = 1; j <= *size; j++ )
				{
					aux = size + j;
					fprintf( file, SEPARATOR"%d", *aux );
				}
				break;
			case 'A':
				size = state->attributes[i].value;
				for( j = 1; j <= *size; j++ )
				{
					aux = size + j;
					fprintf( file, SEPARATOR"%d", *aux );
				}
				break;

		}
	}
	return;
}
		
void trcFlush(TrcData *trc)
{
	TrcState * state;
	int i;
	for( i = 0; i < trc->numSavedStates; i++ )
	{
		listRemoveHead( trc->savedStates, (void**)&state );
		fprintf( 
			trc->tracefile,
			"%ld"SEPARATOR"%ld"SEPARATOR"%ld"SEPARATOR"%ld",
			(state->initTime.tv_usec + (1000000 * state->initTime.tv_sec)) -
			(trc->initTime.tv_usec + (1000000 * trc->initTime.tv_sec )),
			(state->endTime.tv_usec + (1000000 * state->endTime.tv_sec)) -
			(trc->initTime.tv_usec + (1000000 * trc->initTime.tv_sec )),
			(state->initRusage.ru_utime.tv_usec + (1000000 * state->initRusage.ru_utime.tv_sec)) +
			(state->initRusage.ru_stime.tv_usec + (1000000 * state->initRusage.ru_stime.tv_sec)),
			(state->endRusage.ru_utime.tv_usec + (1000000 * state->endRusage.ru_utime.tv_sec)) +
			(state->endRusage.ru_stime.tv_usec + (1000000 * state->endRusage.ru_stime.tv_sec))
			);
		trcPrintStateData( trc->tracefile, state );
		fprintf( trc->tracefile, "\n" );
		free( state->attributes );
		free( state );
	}
	trc->numSavedStates = 0;
	return;
}

int * trcIArrayToTrc( int size, int * array )
{
	int * aux = malloc( sizeof( int ) * (size + 1) );
	int i;
	aux[0] = size;
	for( i = 1; i <= size; i++ )
		aux[i] = array[i-1];
	return aux;
}

#ifdef TRACER_MAINTEST

int main( int argc, char ** argv )
{
	TrcData * trc;
	int array[4] = { 12, 23, 3 , 6 };
	int * bliu;
	printf("Creating data...\n"); fflush( NULL ); trc = trcCreateData( "./bla" );
	printf("Entering state 0...\n");
	trcEnterState(trc, "si", "Teste_de_instrumenta�o", 0);
	fflush( NULL );
	printf("Entering state 1...\n");
	fflush( NULL );
	bliu = trcIArrayToTrc( 4, array );
	printf("%d, %d, %d, %d, %d\n", bliu[0], bliu[1], bliu[2], bliu[3], bliu[4] );
	trcEnterState(trc, "siI", "Teste_de_instrumenta�o", 1, trcIArrayToTrc( 4, array ));
	printf("Leaving state 1...\n"); fflush( NULL ); trcLeaveState(trc);
	printf("Destroying data...\n"); fflush( NULL ); trcDestroyData(&trc);
	return(0);
}

#endif
