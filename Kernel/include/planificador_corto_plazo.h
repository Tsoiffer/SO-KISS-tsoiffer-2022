#ifndef PLANIFICADOR_CORTO_PLAZO_H_
#define PLANIFICADOR_CORTO_PLAZO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include <commons/config.h>
#include "estructuras.h"
#include "sem_init.h"
#include <stdarg.h>


void planificador_corto_plazo(void* );
void analizar_respuesta_cpu(respuesta_a_kernel* ,PCB* ,t_list*,PCB*,t_list*,int,t_log*,mensaje_MEMSWAP*);
void calcular_estimacion(PCB *, double,int);
int PCB_menor_estimacion(t_list* );
int calcular_rafaga_ejecutada(char*, char* );

#endif


