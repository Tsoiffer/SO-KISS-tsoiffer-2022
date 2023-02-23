#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include <commons/config.h>
#include "comunicacion_KERNEL-CONSOLA.h"
#include "comunicacion_KERNEL-CPU.h"
#include "comunicacion_KERNEL-MEMSWAP.h"
#include "planificador.h"
#include "estructuras.h"
#include "recibir_respuesta_CPU.h"
#include "sem_init.h"

void inicializarFrees();
void cerrarProgramaPiola();

#endif
