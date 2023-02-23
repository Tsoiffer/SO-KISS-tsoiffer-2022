#ifndef COMUNICACION_CPU_H_
#define COMUNICACION_CPU_H_

#include <stdint.h>
#include <commons/log.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include <stdbool.h>
#include<commons/collections/list.h>
#include "sockets.h"
#include "comunicacion_KERNEL-CONSOLA.h"
#include "estructuras.h"
#include "sem_init.h"




void comunicacion_CPU(void*);
void enviarPCB(int,PCB*);
void* serializar_PCB(size_t*,PCB*);
void* serializar_instruccion_pcb(void* ,INSTRUCCION* , int );
PCB* pcbDeprueba();


#endif
