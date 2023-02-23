#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"
#include "sem_init.h"


int algoritmo_Sustitucion_Clock(MCB *, int, int);
int algoritmo_Sustitucion_Clock_Modificado(MCB *, int, int);
int aplicarAlgoritmoCorrespondiente(MCB *, char *, int, int);
algoritmo_reemplazo_MP decodificadorAlgoritmo(char *);

#endif