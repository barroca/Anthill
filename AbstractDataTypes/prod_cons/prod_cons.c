#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prod_cons.h"


prod_cons_t *create_prod_cons(int sz) {
	prod_cons_t *prod_cons = malloc(sizeof(prod_cons_t));


	prod_cons->buffer = calloc(sz, sizeof(int));
	prod_cons->buf_size = sz;
	pthread_mutex_init( &(prod_cons->prod_mutex), NULL);
	pthread_mutex_init( &(prod_cons->cons_mutex), NULL);
	// The buffer starts with all positions free
	sem_init( &(prod_cons->prod_sem), 0, sz);
	// The buffer starts without any filled positions
	sem_init( &(prod_cons->cons_sem), 0, 0);
	prod_cons->prod_pos = 0;
	prod_cons->cons_pos = 0;
	

	return prod_cons;
}


void put(prod_cons_t *prod_cons, void *val) {

	// Wait for a free position
	sem_wait( &(prod_cons->prod_sem) );
	
	pthread_mutex_lock( &(prod_cons->prod_mutex) );	
	prod_cons->buffer[prod_cons->prod_pos] = val;
	prod_cons->prod_pos = (prod_cons->prod_pos+1) % prod_cons->buf_size;
	pthread_mutex_unlock( &(prod_cons->prod_mutex) );
	
	// Let the consumer get another position
	sem_post( &(prod_cons->cons_sem) );
	
}


void *get(prod_cons_t *prod_cons) {
	void *val = NULL;

	// Wait for someting to be consumed
	sem_wait( &(prod_cons->cons_sem) );
	
	pthread_mutex_lock( &(prod_cons->cons_mutex) );
	val = prod_cons->buffer[prod_cons->cons_pos];
	
	// Coutinho: XXX: Due to some signal mess (perhaps libthread_db bug?), in debugging
	// the consomer gets some unfilled positions in next or step gdb commands.
	// To ease debugging we will put NULL in these positions, so this case 
	// could be easily identified
	prod_cons->buffer[prod_cons->cons_pos] = NULL;

	prod_cons->cons_pos = (prod_cons->cons_pos+1) % prod_cons->buf_size;
	pthread_mutex_unlock( &(prod_cons->cons_mutex) );
	
	// Free one buffer's position
	sem_post( &(prod_cons->prod_sem) );
	return val;
}


void destroy_prod_cons(prod_cons_t *prod_cons) {

	// Coutinho: XXX: For now we are destroying the buffer even if there's someting inside
	sem_destroy( &(prod_cons->prod_sem) );
	sem_destroy( &(prod_cons->cons_sem) );
	free(prod_cons->buffer);
	free(prod_cons);
	
}

