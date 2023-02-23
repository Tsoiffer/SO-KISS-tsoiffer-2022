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
    FIN // instruccion
} op_code;

//char *PSEUDOCODIGO; TODO REVISAR ESTO

typedef struct
{
    t_log *log;
    int fd;
    char *server_name;
    t_list *lista_PCB;
} t_procesar_conexion_args;

typedef enum
{
    COP_NO_OP,
    COP_I_O,
    COP_READ,
    COP_WRITE,
    COP_COPY,
    COP_EXIT

} codigo_de_ejecucion;

typedef enum
{
    FIFO,
    SRT
} tipo_planificador;

typedef struct
{
    // op_code codigo_de_operacion;
    char *codigo_de_operacion;
    int arg1;
    int arg2;
} INSTRUCCION;

typedef struct
{
    int id;
    size_t size;
    t_list *instruccionesPCB;
    int PC;
    int tabla_paginas;
    int estimacion_rafaga;
    int consola_socket;
} PCB;

typedef enum
{
    COP_NEW_PCB,
    COP_SUSPEND_READY_PCB,
    COP_SUSPEND_PCB,
    COP_END_PCB
} tipo_mensaje_MEMSWAP;

typedef struct
{
    tipo_mensaje_MEMSWAP tipo_mensaje;
    int id_PCB;
    int id_MCB;
    int tamanio_PCB;

} mensaje_MEMSWAP;
typedef struct
{
    t_log *log;
    mensaje_MEMSWAP *mensaje_MEMSWAP;
    char *puerto_escucha_memoria;
    char *puerto_dispatch_memoria;
    char *ip_memoria;
    char *ip_kernel;

} memswap_conexion_args;

typedef enum
{
    I_O,
    EXIT_OK,
    INTERRUPCION
} tipo_de_respuesta;

typedef struct
{
    tipo_de_respuesta tipo_de_respuesta; // i/o   - Exit      - Interrupcion  En los tres casos mantenemos el PCB en CPU, En exit se hace un free luego de enviar mensaje
    int PC;                              // PC    - PC=NULL   - PC
    int tiempo_espera;                   // TIME  - TIME=NULL - TIME=NULL
} respuesta_a_kernel;

typedef struct
{
    int dispatch;
    int interrupt;
    PCB *unPCB;
    t_log *log;
    respuesta_a_kernel *respuestaGlobal;
} cpu_conexion_args;
typedef struct
{
    t_log *log;
    char *server_name;
    int server_socket;
    respuesta_a_kernel *respuestaGlobal;
} args_respuesta_CPU;

typedef struct
{
    t_list *NEW;
    t_list *READY;
    t_list *BLOCKED;
    t_list *SUSPENDED_BLOCKED;
    t_list *SUSPENDED_READY;
    PCB *RUNNING;
    PCB *EXIT;
    respuesta_a_kernel *respuestaGlobal;
    tipo_planificador tipoPlanificador;
    int interrupt_fd;
    t_log *log;
    mensaje_MEMSWAP *mensaje_MEMSWAP;
} planificador_args;

typedef enum
{
    COP_ID_MCB, // si esta todo ok devuelve el id
    COP_ERROR,  // si hubo una falla o se perdio algun dato o no da el especio en memoria
    COP_FIN_OK  // cuando confirma que se elimino
} tipo_respuesta_MEMSWAP_KERNEL;

#endif
