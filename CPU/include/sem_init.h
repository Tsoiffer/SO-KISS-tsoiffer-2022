#ifndef SEM_INIT_H_
#define SEM_INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>
#include<commons/collections/list.h>


void iniciarSemaforos();

sem_t SEM_RESPUESTA;
sem_t SEM_KERNEL_ENCENDIDO;
sem_t SEM_MENSAJE_A_MEMORIA;
sem_t SEM_RESPUESTA_DE_MEMORIA;
//sem_t SEM_RECV_MEMSWAP;
//sem_t SEM_SEND_MEMSWAP;

#endif
