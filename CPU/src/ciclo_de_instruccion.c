#include "../include/ciclo_de_instruccion.h"
#define CONFIG_FILE_PATH "CPU.config"

void cicloDeInstruccion(PCB* PCB_en_ejecucion, bool* interrupcionGlobal, respuesta_a_kernel* respuestaGlobal, TLB* tlb, mensaje_MEMSWAP* mensaje_MEMSWAP_global)
{

    t_log *cpu_log = log_create("cpu.log", "CICLO_DE_INSTRUCCION", true, LOG_LEVEL_INFO);
    log_info(cpu_log, ANSI_COLOR_GREEN "Ingreso en ciclo De Instruccion" ANSI_COLOR_RESET);
    bool interrupcion = false;
    bool cambioDeContexto = false;
    t_config *config1 = config_create(CONFIG_FILE_PATH);
    int retardo_NOOP = config_get_int_value(config1, "RETARDO_NOOP");
    int socketDeMemoria;
    int valorDeCopy = NULL;
    config_destroy(config1);
    log_info(cpu_log, ANSI_COLOR_GREEN "Se limpia la cache TLB" ANSI_COLOR_RESET);
    limpiarCacheTLB(tlb);
    log_info(cpu_log, ANSI_COLOR_GREEN "Recibimos el PCB de id %i, y la cantidad de instrucciones que tiene es %i"ANSI_COLOR_RESET, PCB_en_ejecucion->id, list_size(PCB_en_ejecucion->instruccionesPCB));
    while (!interrupcion && !cambioDeContexto)
    {
        log_info(cpu_log, ANSI_COLOR_CYAN "Estamos en la instruccion N°: %i, del PCB de ID: %i"ANSI_COLOR_RESET, PCB_en_ejecucion->PC,PCB_en_ejecucion->id);
        int idMCB = PCB_en_ejecucion->tabla_paginas;
        INSTRUCCION* instruccion_a_ejecutar = fetch(PCB_en_ejecucion);;
        codigo_de_ejecucion COOP_instruccion = decode(instruccion_a_ejecutar->codigo_de_operacion);
        if (COOP_instruccion == COP_COPY)
        {
            valorDeCopy = fetch_operand( instruccion_a_ejecutar->arg2, tlb, mensaje_MEMSWAP_global, idMCB); // hay que agregar los daros de memorua
        }
        if (COOP_instruccion == COP_NO_OP)
        {
            int tiempo_retardo_tot = ((instruccion_a_ejecutar->arg1) * retardo_NOOP);
            log_info(cpu_log, ANSI_COLOR_CYAN "La instruccion a ejecutar es " ANSI_BACKGROUND_RED "%s" ANSI_COLOR_RESET ANSI_COLOR_CYAN " y su tiempo de retardo es " ANSI_BACKGROUND_RED "%i" ANSI_COLOR_RESET, instruccion_a_ejecutar->codigo_de_operacion, tiempo_retardo_tot);
        }
        else
        {
            log_info(cpu_log, ANSI_COLOR_CYAN "La instruccion a ejecutar es " ANSI_BACKGROUND_RED "%s" ANSI_COLOR_RESET, instruccion_a_ejecutar->codigo_de_operacion);
        }

        cambioDeContexto = execute(COOP_instruccion, instruccion_a_ejecutar,PCB_en_ejecucion,respuestaGlobal,tlb,mensaje_MEMSWAP_global, idMCB,valorDeCopy);
        printf("valor despues de salir de execute de Cambio de contexto: %d\n",cambioDeContexto);
        interrupcion = check_interrupt(PCB_en_ejecucion->PC, interrupcionGlobal, respuestaGlobal);
        printf("valor despues de salir de execute de interrupcion: %d\n",interrupcion);
    }
    if (cambioDeContexto)
    {
        log_info(cpu_log, ANSI_COLOR_YELLOW "Salimos del ciclo de instruccion por un cambio de contexto" ANSI_COLOR_RESET);
    }
    if (interrupcion)
    {
        log_info(cpu_log, ANSI_COLOR_YELLOW "Salimos del ciclo de instruccion por una interrupcion" ANSI_COLOR_RESET);
    }
}

INSTRUCCION *fetch(PCB *PCB_en_ejecucion)
{
    INSTRUCCION *instruccion_A_ejecutar;

    instruccion_A_ejecutar = list_get(PCB_en_ejecucion->instruccionesPCB, PCB_en_ejecucion->PC);
    PCB_en_ejecucion->PC += 1;
    return instruccion_A_ejecutar;
}

codigo_de_ejecucion decode(char *codigo_de_operacion)
{
    if (!strcmp(codigo_de_operacion, "NO_OP"))
    {
        return COP_NO_OP;
    }
    if (!strcmp(codigo_de_operacion, "I/O"))
    {
        return COP_I_O;
    }
    if (!strcmp(codigo_de_operacion, "READ"))
    {
        return COP_READ;
    }
    if (!strcmp(codigo_de_operacion, "WRITE"))
    {
        return COP_WRITE;
    }
    if (!strcmp(codigo_de_operacion, "COPY"))
    {
        return COP_COPY;
    }
    if (!strcmp(codigo_de_operacion, "EXIT"))
    {
        return COP_EXIT;
    }
}

bool execute(codigo_de_ejecucion COOP_instruccion, INSTRUCCION* instruccion_a_ejecutar, PCB* unPCB, respuesta_a_kernel* respuestaGlobal,TLB* tlb, mensaje_MEMSWAP* mensaje_MEMSWAP_global,int idMCB,int valorDeCopy) //no es necesario mandarle el pc, ya que respuesta global lo contiene
{
int direccionLogica;
int marcoDeMemoria;
int datoLeido;
int datoAEscribir;
int PC = unPCB->PC;

t_log *cpu_log = log_create("cpu.log", "Execute", true, LOG_LEVEL_INFO);
log_info(cpu_log, ANSI_COLOR_GREEN "Ingreso en Execute " ANSI_COLOR_RESET);

    switch (COOP_instruccion)
    {
    case COP_NO_OP:;
    log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando una NO_OP" ANSI_COLOR_RESET);
        t_config *config1 = config_create(CONFIG_FILE_PATH);
        int retardo_NOOP = config_get_int_value(config1, "RETARDO_NOOP");
        
        usleep((instruccion_a_ejecutar->arg1 * retardo_NOOP) * 1000);
        //config_destroy(config1);
        return false;

    case COP_I_O:;
    log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando una I/O " ANSI_COLOR_RESET);
        respuesta_a_kernel *respuestaIO = malloc(sizeof(codigo_de_ejecucion) + sizeof(int) + sizeof(int));
        respuestaIO->tipo_de_respuesta = I_O;
        respuestaIO->PC = PC;
        respuestaIO->tiempo_espera = instruccion_a_ejecutar->arg1;
        responder_a_Kernel(respuestaIO, respuestaGlobal);
        return true;

    case COP_READ:;
        //(dirección_lógica): Se deberá leer el valor de memoria correspondiente a esa dirección lógica e imprimirlo por pantalla.
      log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando un READ " ANSI_COLOR_RESET);
        direccionLogica = instruccion_a_ejecutar->arg1; 
        marcoDeMemoria = llamada_MMU_TLB( direccionLogica,tlb, mensaje_MEMSWAP_global, idMCB);
        datoLeido = pedidoDeDatoAMemoria(marcoDeMemoria,direccionLogica,mensaje_MEMSWAP_global,mensaje_MEMSWAP_global->datos,  idMCB);
        log_info(cpu_log, ANSI_COLOR_YELLOW "Se Leyo el dato: "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i" ANSI_COLOR_RESET ANSI_COLOR_YELLOW", de la direccion logica "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i" ANSI_COLOR_RESET,datoLeido,direccionLogica);
        return false;

    case COP_WRITE:;
        log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando un WRITE " ANSI_COLOR_RESET);
        //(dirección_lógica, valor): Se deberá escribir en memoria el valor del segundo parámetro en la dirección lógica del primer parámetro.
        direccionLogica = instruccion_a_ejecutar->arg1; 
        datoAEscribir = instruccion_a_ejecutar->arg2; 
        marcoDeMemoria = llamada_MMU_TLB(direccionLogica, tlb, mensaje_MEMSWAP_global, idMCB);
        pedidoEscribirDatoEnMemoria(marcoDeMemoria,direccionLogica,mensaje_MEMSWAP_global,mensaje_MEMSWAP_global->datos,  idMCB, datoAEscribir);
        log_info(cpu_log, ANSI_COLOR_YELLOW "Se escribio el dato "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i"ANSI_COLOR_RESET ANSI_COLOR_YELLOW", en la direccion logica "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i" ANSI_COLOR_RESET ANSI_COLOR_YELLOW", en el marco "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i" ANSI_COLOR_RESET,datoAEscribir,direccionLogica,marcoDeMemoria);
        return false;

    case COP_COPY:;
        // Se deberá escribir en memoria el valor ubicado en la dirección lógica pasada como segundo parámetro, en la dirección lógica pasada como primer parámetro. A efectos de esta etapa, el accionar es similar a la instrucción WRITE ya que el valor a escribir ya se debería haber obtenido en la etapa anterior.
        log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando un COPY " ANSI_COLOR_RESET);
        direccionLogica = instruccion_a_ejecutar->arg1; 
        datoAEscribir = valorDeCopy; 
        marcoDeMemoria = llamada_MMU_TLB(direccionLogica, tlb, mensaje_MEMSWAP_global, idMCB);
        pedidoEscribirDatoEnMemoria(marcoDeMemoria,direccionLogica,mensaje_MEMSWAP_global,mensaje_MEMSWAP_global->datos,  idMCB, datoAEscribir);
        log_info(cpu_log, ANSI_COLOR_YELLOW "Se copio el dato "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i"ANSI_COLOR_RESET ANSI_COLOR_YELLOW", en la direccion logica "ANSI_COLOR_RED ANSI_BACKGROUND_GREEN"%i" ANSI_COLOR_RESET,datoAEscribir,direccionLogica);
        return false;

    case COP_EXIT:;
        // Esta instrucción representa la syscall de finalización del proceso. Se deberá devolver el PCB actualizado al Kernel para su finalización.
        log_info(cpu_log, ANSI_COLOR_BLUE "Estamos ejecutando EXIT " ANSI_COLOR_RESET);
        respuesta_a_kernel *respuestaExit = malloc(sizeof(codigo_de_ejecucion) + sizeof(int) + sizeof(int));
        respuestaExit->tipo_de_respuesta = EXIT_OK;
        respuestaExit->PC = NULL;
        respuestaExit->tiempo_espera = NULL;
        responder_a_Kernel(respuestaExit, respuestaGlobal);
        log_info(cpu_log, ANSI_COLOR_GREEN ANSI_BACKGROUND_BLUE "Se finalizo la ejecucion del PCB de ID: %i"ANSI_COLOR_RESET "\n",unPCB->id );
        return true;

    default:
        log_error(cpu_log,"Hubo un fallo en el ciclo de instruccion!! \n");
        return false;

    }
}
int fetch_operand(int direccion_logica, TLB *tlb, mensaje_MEMSWAP* mensaje_MEMSWAP_global,int idMCB)

{
    int marcoDeMemoria = llamada_MMU_TLB(direccion_logica, tlb, mensaje_MEMSWAP_global, idMCB);
    return pedidoDeDatoAMemoria(marcoDeMemoria,direccion_logica,mensaje_MEMSWAP_global,mensaje_MEMSWAP_global->datos,  idMCB);
}

bool check_interrupt(int PC, bool *interrupcionGlobal, respuesta_a_kernel *respuestaGlobal)
{
    if (*interrupcionGlobal)
    {
        respuesta_a_kernel *respuestaInterrupt = malloc(sizeof(codigo_de_ejecucion) + sizeof(int) + sizeof(int));
        respuestaInterrupt->tipo_de_respuesta = INTERRUPCION;
        respuestaInterrupt->PC = PC;
        respuestaInterrupt->tiempo_espera = NULL;
        responder_a_Kernel(respuestaInterrupt, respuestaGlobal);
        *interrupcionGlobal = false;
        return true;
    }
    return false;
}

void responder_a_Kernel(respuesta_a_kernel *respuesta, respuesta_a_kernel *respuestaGlobal)
{
    respuestaGlobal->tipo_de_respuesta = respuesta->tipo_de_respuesta;
    respuestaGlobal->PC = respuesta->PC;
    respuestaGlobal->tiempo_espera = respuesta->tiempo_espera;
    sem_post(&SEM_RESPUESTA);
}