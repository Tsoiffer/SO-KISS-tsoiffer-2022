#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <commons/collections/list.h>
#include <stdbool.h>
#include "printfColores.h"
#include <commons/log.h>

typedef enum
{
  INSTRUCCIONES_PCB,
  RECIBIR_PCB,
	FIN  //instruccion
} op_code;

typedef enum {
COP_NO_OP,
COP_I_O,
COP_READ,
COP_WRITE,
COP_COPY,
COP_EXIT

}codigo_de_ejecucion;

typedef struct
{
	//op_code codigo_de_operacion;
  char* codigo_de_operacion;
  int arg1;
  int arg2;
} INSTRUCCION;

typedef struct{
    int id;
    size_t size;
    t_list* instruccionesPCB;
    int PC;
    int tabla_paginas;
    int estimacion_rafaga;
}PCB;

typedef enum{
    I_O,
    EXIT_OK,
    INTERRUPCION
}tipo_de_respuesta;

typedef struct{
    tipo_de_respuesta tipo_de_respuesta;// i/o   - Exit      - Interrupcion  En los tres casos mantenemos el PCB en CPU, En exit se hace un free luego de enviar mensaje
    int PC;                // PC    - PC=NULL   - PC
    int tiempo_espera;     // TIME  - TIME=NULL - TIME=NULL 
}respuesta_a_kernel;

typedef struct {
    t_log* log;
    char* IP_KERNEL;
    char* PUERTO_KERNEL_CPU;
    respuesta_a_kernel* respuesta;
} comunicacion_kernel_args;

typedef struct {
    t_log* log;
    char* server_name;
    int fd_interrupt;
    bool* interrupcionGlobal;
} interrupcion_args;

/*
typedef struct {
int tam_memoria;
int tam_pagina;
int entradas_por_tabla;
int marcos_por_proceso;
} comunicacion_memswap_args;
*/
 //LO QUE NECESITAMOS QUE NOS PASE LA MEMORIA CIANDO SE CONECTA CON EL CPU
typedef enum
{
    COP_LECTURA,
	COP_ESCRITURA,
	COP_PEDIDO_T_NIVEL2,
	COP_PEDIDO_D_FISICA
} tipo_mensaje_MEMSWAP;

typedef struct{
    int tamanio_pagina;
    int cant_entradas_por_tabla;
}datosMemoria;
typedef struct
{
    tipo_mensaje_MEMSWAP tipo_mensaje;
    int marco;
    int id_MCB;
    int offset;
    int datoAEscribir;
    int referenciaATabla1er_nivel;
    int indiceDeATabla2do_nivel;
    int referenciaATabla2do_nivel;
    int respuestaMemoria;
    datosMemoria* datos;

} mensaje_MEMSWAP;
typedef struct
{
    t_log *log;
    mensaje_MEMSWAP *mensaje_MEMSWAP;
    char* puerto_escucha_memoria;
    char* puerto_dispatch_memoria;
    char* ip_memoria;
    char* ip_kernel;

} comunicacion_MEMSWAP_args;

typedef struct {
    int tamanio;
    int** matriz;
    char* algoritmoDeRemplazo;
}TLB;
typedef enum
{
    LRU,
    FIFO
}algoritmo_reemplazo_TLB;

typedef struct {
    t_log* log;
    int fd;
    char* server_name;
    PCB* PCB;
    respuesta_a_kernel* respuestaGlobal;
    bool* interrupcionGlobal;
    TLB* tlb_global;
    mensaje_MEMSWAP* mensaje_MEMSWAP_global;
    
} t_procesar_conexion_args;


#endif
