#ifndef _COMUNICACION_CONSOLA_H
#define _COMUNICACION_CONSOLA_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include "estructuras.h"


void parsear_codigo_ejecucion(char*, t_list*);
void* serializar_instruccion(void*  ,INSTRUCCION*,int);
void inicializar_codigo_de_operacion(char*);
void enviarInstrucciones(int ,t_list*);
void* serializar_instrucciones(size_t* ,t_list*);

#endif // _COMUNICACION_CONSOLA_H


