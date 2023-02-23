#ifndef MMU_TLB_H_
#define MMU_TLB_H_

#include <commons/log.h>
#include "estructuras.h"
#include <math.h> 
#include <stdlib.h>
#include "sem_init.h"

int llamada_MMU_TLB(int ,TLB* ,mensaje_MEMSWAP* ,int  );
bool consultaATLB(int* ,int ,TLB*);
int consultaAMMU(int ,mensaje_MEMSWAP*,int ,TLB *);
void limpiarCacheTLB(TLB*);
TLB* incializarTLB(int ,char* );
int devolverVictimaTLB(TLB* );
int pedidoDeDatoAMemoria(int ,int , mensaje_MEMSWAP* ,datosMemoria* , int );
int espacioEnLaTLB(TLB*);
algoritmo_reemplazo_TLB decodificarStringPlanificadorTLB(char*);
void escribirEntradaEnLaTLB(TLB*, int , int);

#endif
