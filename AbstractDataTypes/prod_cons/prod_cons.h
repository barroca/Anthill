#ifndef _PROD_CONS_H_
#define _PROD_CONS_H_

#include <pthread.h>
#include <semaphore.h>


typedef struct {
	int prod_pos; // posicao que o produtor vai inserir no buffer
	int cons_pos; // posicao que o consumidor vai retirar do buffer
	sem_t prod_sem; // impede o produtor de escrever em cima de posicoes ocupadas
	sem_t cons_sem; // impede o consumidor de consumir posicoes com lixo
	pthread_mutex_t prod_mutex; // so um produtor insere de cada vez
	pthread_mutex_t cons_mutex; // so um consumidor retira de cada vez
	int buf_size; 
	void **buffer;
} prod_cons_t;	

/*
static inline int __cas(volatile void *p, unsigned long oldval, unsigned long newval)
{
    char ret;
    unsigned long readval;

    __asm__ ("lock; cmpxchgl %3, %1; sete %0"
	     : "=q" (ret), "=m" (*__xg(p)), "=a" (readval)
	     : "r" (newval), "m" (*__xg(p)), "a" (oldval) : "memory");
    return ret;
}

#define compare_and_swap(p, oldval, newval)	__cas(p, (unsigned long) oldval, (unsigned long) newval)
*/


prod_cons_t *create_prod_cons(int sz);
void put(prod_cons_t *prod_cons, void *val);
void *get(prod_cons_t *prod_cons);
void destroy_prod_cons(prod_cons_t *prod_cons);

#endif


