#ifndef RESPUESTA_CPU_KERNEL_H_
#define RESPUESTA_CPU_KERNEL_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "estructuras.h"
#include "sem_init.h"
#include "sockets.h"


void comunicacion_kernel(void*);
void enviar_respuesta_kernel(int ,respuesta_a_kernel*);
void* serializar_respuesta(size_t* ,respuesta_a_kernel*);

#endif
