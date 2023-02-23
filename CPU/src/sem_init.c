#include "../include/sem_init.h"

void iniciarSemaforos(){

sem_init(&SEM_RESPUESTA, 0, 0);
sem_init(&SEM_KERNEL_ENCENDIDO, 0, 0);
sem_init(&SEM_MENSAJE_A_MEMORIA,0,0);
sem_init(&SEM_RESPUESTA_DE_MEMORIA,0,0);
//sem_init(&SEM_RECV_MEMSWAP,0,0);
//sem_init(&SEM_SEND_MEMSWAP,0,0);


}