#ifndef COMUNICACION_MEMSWAP_CPU_H_
#define COMUNICACION_MEMSWAP_CPU_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "sockets.h"
#include "estructuras.h"
#include <commons/config.h>
#include "sem_init.h"

void comunicacion_MEMSWAP_CPU(void *);
int recuperarDato(MCB *, int, int);
void escribirDato(MCB *, int, int, int);
int *recuperarTablaSegundoNivel(int, int, t_list *);
int recuperarIndiceDeMarco(int *, int, int, t_list *);
bool seEncuantraPresente(int *, MCB *, int);

#endif