#ifndef CICLO_DE_INSTRUCCION_H_
#define CICLO_DE_INSTRUCCION_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include <commons/config.h>
#include "estructuras.h"
#include "sem_init.h"




t_log* ciclo_de_instruccion_log;
void cicloDeInstruccion(PCB* ,bool* ,respuesta_a_kernel* , TLB*, mensaje_MEMSWAP*);
INSTRUCCION* fetch(PCB* );
codigo_de_ejecucion decode(char*);
int fetch_operand(int ,TLB* ,mensaje_MEMSWAP* ,int  );
bool execute(codigo_de_ejecucion , INSTRUCCION*,  PCB* , respuesta_a_kernel*,TLB* , mensaje_MEMSWAP* ,int ,int);
void responder_a_Kernel(respuesta_a_kernel*,respuesta_a_kernel*);
bool check_interrupt(int ,bool*,respuesta_a_kernel*);




#endif

