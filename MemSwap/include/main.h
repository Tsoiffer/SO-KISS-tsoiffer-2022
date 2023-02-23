#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "comunicacion_MEMSWAP_CPU.h"
#include "comunicacion_MEMSWAP_KERNEL.h"
#include "estructuras.h"
#include <pthread.h>

MCB *generarEstructuraDeProceso(int, int, int, int, void *);
void *inicializarMemoriaUsuario(int, int, int, int, t_list *);
void generarListaTMarco(MCB *, int);
void limpiarListaTMarco(t_list *);
int marcoMasAntiguo(t_list *, int);
int solicitudDeMarcoLibre(MCB *, t_log *);
int solicitudDeMarcoLectura(MCB *, t_log *);
int horaDeEscritura();
void imprimirInfoDeMarcos(t_list* );
void imprimirTablaSegNivel(MCB* , int );
#endif
