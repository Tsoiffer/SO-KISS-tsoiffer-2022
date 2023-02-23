#ifndef COMUNICACION_KERNEL_H_
#define COMUNICACION_KERNEL_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "estructuras.h"
#include "sem_init.h"
#include "sockets.h"

int server_escuchar(t_log* , char* , int ,PCB*, respuesta_a_kernel*, bool*,TLB*,mensaje_MEMSWAP*);
static void procesar_conexion(void* );
void recibir_PCB(int ,PCB*);
void recibir_instrucciones(int ,t_list* );
static void deserializar_instrucciones(int fd,t_list* , int );
static void deserializar_instruccion(t_list* ,void* );
void comunicacion_interrupcion(void* );
void recibir_interrupcion(int , bool*,t_log* );
void liberarMemoriaPCB(PCB*);

#endif
