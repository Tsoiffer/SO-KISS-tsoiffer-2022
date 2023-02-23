#ifndef SEM_INIT_H_
#define SEM_INIT_H_


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include<commons/collections/list.h>
#include <commons/config.h>


void iniciarSemaforos();
pthread_mutex_t MUTEX_SWAP;
sem_t SEM_FIN_MEMORIA;

#endif
