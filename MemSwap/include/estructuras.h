#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include "printfColores.h"

typedef struct
{
	int bit_precencia;
	int bit_modificado;
	int id_frame;
} frame;

typedef struct
{
	char *proteccion; // READ-ONLY, EXEC-ONLY, READ-WRITE
	int largo;
	int base;
	int id;
} segmento;

typedef struct
{
	segmento *segmentos;  // cada proceso tiene su frame
	int direccionamiento; // ejemplo 18 bits
	int base;
	int limite;
} memory;

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
	t_list *lista_PCB;
} t_procesar_conexion_args;

typedef enum
{
	MENSAJE,
	FIN // instruccion
} op_code;

typedef struct
{
	int tam_memoria;
	int tam_pagina;
	int entradas_por_tabla;
	int marcos_por_proceso;
} comunicacion_memswap_args; // LO QUE NECESITAMOS QUE PASE LA MEMORIA CIANDO SE CONECTA CON EL CPU

typedef struct
{
	t_log *log;
	int server_socket;
	char *server_name;
	int dispatch_socket;
	t_list *lista_ProcesosEnMemoria;
	int ENTRADAS_POR_TABLA;
	int TAM_PAGINA;
	int RETARDO_SWAP;
} memoria_Kernel_conexion_args;

typedef struct
{
	t_log *log;
	int server_socket;
	char *server_name;
	int dispatch_socket;
	t_list *lista_ProcesosEnMemoria;
	int ENTRADAS_POR_TABLA;
	int TAM_PAGINA;
} memoria_CPU_conexion_args;

typedef struct
{
	int bitU;
	int bitM;
	uint32_t *punteroAMarco;
	int idMarco;
	int idPagina;	//
	int antiguedad; // cuanto menor es el numero mas antiguo es
} T_marco;			// representa cada marco de memoria correspondiente al proceso

typedef struct
{
	int id;
	bool libre;
	int idPCB;
	uint32_t *primerMarco; //"base"
	int tamanioDePagina;
	int marcoVictima; // contiene el marco mas antiguo cargado en memoria del proceso
	int *bitMap;
	int **tabla_paginas_primer_nivel; // estaba como **
	t_list *T_marco;
} MCB;

typedef enum
{
	CLOCK,
	CLOCK_M
} algoritmo_reemplazo_MP;

typedef enum
{
	COP_NEW_PCB,
	COP_SUSPEND_READY_PCB,
	COP_SUSPEND_PCB_KERNEL,
	COP_END_PCB
} tipo_mensaje_DE_KERNEL;

typedef enum
{
	COP_ID_MCB, // si esta todo ok devuelve el id
	COP_ERROR,	// si hubo una falla o se perdio algun dato o no da el especio en memoria
	COP_FIN_OK	// cuando confirma que se elimino
} tipo_respuesta_MEMSWAP_KERNEL;
typedef enum
{
	COP_LECTURA,
	COP_ESCRITURA,
	COP_PEDIDO_T_NIVEL2,
	COP_PEDIDO_D_FISICA
} tipo_mensaje_DE_CPU;

#endif
