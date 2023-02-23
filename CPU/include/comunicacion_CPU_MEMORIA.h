#ifndef COMUNICACION_MEMORIA_H_
#define COMUNICACION_MEMORIA_H_


#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "estructuras.h"
#include "sem_init.h"
#include "sockets.h"

void comunicacion_MEMSWAP(void*);

#endif