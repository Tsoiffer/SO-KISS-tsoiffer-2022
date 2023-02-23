#ifndef COMUNICACION_MEMSWAP_H_
#define COMUNICACION_MEMSWAP_H_

#include <stdint.h>
#include <commons/log.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "sockets.h"
#include "comunicacion_KERNEL-CONSOLA.h"
#include "estructuras.h"
#include "sem_init.h"



void comunicacion_MEMSWAP(void*);
int esperarRespuestadeMemswap(int );
void *serializar_Mensaje(mensaje_MEMSWAP* );
void enviarMensajeAMemswap(mensaje_MEMSWAP *, int );
#endif
