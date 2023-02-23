#ifndef RECIBIR_RESPUESTA_CPU_H_
#define RECIBIR_RESPUESTA_CPU_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "sockets.h"
#include "estructuras.h"
#include "sem_init.h"


void recibir_respuesta_CPU(void* );
void recibir_respuesta(int ,respuesta_a_kernel*);


#endif
