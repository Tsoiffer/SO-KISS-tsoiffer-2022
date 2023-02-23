#include "../include/sem_init.h"
#define CONFIG_FILE_PATH "Kernel.config"

void iniciarSemaforos(){

t_config* config = config_create(CONFIG_FILE_PATH);
int multiprogramacion;
multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");


pthread_mutex_init(&MUTEX_COLA_NEW, NULL);
pthread_mutex_init(&MUTEX_PROBANDO, NULL);
pthread_mutex_init(&MUTEX_COMUNICACION_MEMSWAP, NULL);

sem_init(&SEM_COLA_NEW_O_SUSPENDED_READY, 0, 0);
sem_init(&SEM_COLA_RUNNING, 0, 0);
sem_init(&SEM_COLA_READY, 0, 0);
sem_init(&MemSwap, 0, 0);
sem_init(&SEM_RESPUESTA_CPU, 0, 0);
sem_init(&SEM_RUNNING_LIBRE, 0, 1);
sem_init(&SEM_COLA_MULTIPROGRAMACION, 0, multiprogramacion);
sem_init(&SEM_COLA_BLOCKED ,0, 0);
sem_init(&SEM_IO_LIBRE, 0, 1);
sem_init(&SEM_COLA_SUSPENDED_BLOCKED,0,0);
sem_init(&SEM_COLA_SUSPENDED_READY,0,0);
sem_init(&SEM_SEND_MEMSWAP,0,0);
sem_init(&SEM_RECV_MEMSWAP,0,0);

config_destroy(config);
}
