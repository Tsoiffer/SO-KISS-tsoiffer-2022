#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

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



void planificador_largo_plazo(void* );
void enviarInterrupcion(int,t_log* );

#endif
