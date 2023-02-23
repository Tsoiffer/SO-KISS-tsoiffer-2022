#ifndef COMUNICACION_KERNEL_H_
#define COMUNICACION_KERNEL_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include <unistd.h>
#include <sys/types.h>
#include<commons/collections/list.h>
#include "sockets.h"
#include "estructuras.h"
#include <commons/config.h>
#include "sem_init.h"


int server_escuchar(t_log* , char* , int , t_list*);
void recibir_instrucciones(int , t_list*);
void deserializar_instrucciones(int, t_list*, int);
void deserializar_instruccion(t_list*, void*);
void procesar_conexion(void* );
#endif
