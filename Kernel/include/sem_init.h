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

pthread_mutex_t MUTEX_PROBANDO;
pthread_mutex_t MUTEX_COLA_NEW;
pthread_mutex_t MUTEX_COMUNICACION_MEMSWAP;
sem_t SEM_COLA_NEW_O_SUSPENDED_READY;
sem_t SEM_COLA_RUNNING;
sem_t SEM_COLA_MULTIPROGRAMACION;
sem_t SEM_COLA_READY;
sem_t SEM_RUNNING_LIBRE;
sem_t MemSwap;
sem_t SEM_RESPUESTA_CPU;
sem_t SEM_COLA_BLOCKED;
sem_t SEM_IO_LIBRE;
sem_t SEM_COLA_SUSPENDED_BLOCKED;
sem_t SEM_COLA_SUSPENDED_READY;
sem_t SEM_SEND_MEMSWAP;
sem_t SEM_RECV_MEMSWAP;


#endif
