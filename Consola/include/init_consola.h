#ifndef INIT_H_
#define INIT_H_

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "logs.h"
#include "comunicacion_CONSOLA.h"
#include "sockets.h"

void cerrar_programa(t_log*,int* socket_cliente,t_config*);
void finalizarConexion(int);
#endif
