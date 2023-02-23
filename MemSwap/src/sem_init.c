#include "../include/sem_init.h"

void iniciarSemaforos(){

pthread_mutex_init(&MUTEX_SWAP, NULL);
sem_init(&SEM_FIN_MEMORIA, 0, 0);

}
