#ifndef COMUNICACION_MEMSWAP_KERNEL_H_
#define COMUNICACION_MEMSWAP_KERNEL_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "sockets.h"
#include "estructuras.h"
#include <math.h>
#include "sem_init.h"

void comunicacion_MEMSWAP_Kernel(void*);
int asignarMCB(t_list*, int, int, int);
void asignarTablasSegundoNivel(int**, int);
void generarSwapDelPCB(int, int, int);
void estructurarSWAP(char*, int);
void recuperarPagina(int, int, int, void*);
void escribirPagina(int, int, int, void*);
void suspenderProcesoEnMemoria(int, t_list*,int);
void liberarProcesoEnMemoria(int, t_list*,int);
void swapingPaginasModificadas(MCB*);
void liberarMCB(MCB*,int);
MCB *recuperarMCB(int, t_list*);

#endif
