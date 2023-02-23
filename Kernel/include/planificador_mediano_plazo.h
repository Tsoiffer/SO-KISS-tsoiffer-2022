#ifndef PLANIFICADOR_MEDIANO_PLAZO_H_
#define PLANIFICADOR_MEDIANO_PLAZO_H_

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include<commons/collections/list.h>
#include "estructuras.h"
#include "sem_init.h"

void enviarInterrupcion(int,t_log* );
void planificador_mediano_plazo_BLOCKED(void* );
void planificador_mediano_plazo_SUSPENDED_BLOCKED(void* );
void planificador_mediano_plazo_SUSPENDED_READY(void* );


#endif
