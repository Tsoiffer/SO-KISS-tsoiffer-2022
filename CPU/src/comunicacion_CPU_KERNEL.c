#include "../include/comunicacion_CPU_KERNEL.h"


int server_escuchar(t_log* cpu_log, char* server_name, int server_socket, PCB* PCB_en_ejecucion,respuesta_a_kernel* respuestaGlobal,bool* interrupcion_global,TLB* tlb_global, mensaje_MEMSWAP *mensaje_MEMSWAP_global) {
    int cliente_socket = esperar_cliente(cpu_log, server_name, server_socket);

    if (cliente_socket != -1 ) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = cpu_log;
        args->fd = cliente_socket;
        args->server_name = server_name;
        args->PCB = PCB_en_ejecucion;
        args->respuestaGlobal = respuestaGlobal;
        args->interrupcionGlobal = interrupcion_global;
        args->tlb_global = tlb_global;
        args->mensaje_MEMSWAP_global = mensaje_MEMSWAP_global;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}

static void procesar_conexion(void* void_args) {
    sem_post(&SEM_KERNEL_ENCENDIDO);
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* cpu_log = args->log;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    PCB* unPCB = (void *) malloc(sizeof(int)+sizeof(size_t)+sizeof(t_list*)+sizeof(int)+sizeof(int)+sizeof(int));
    t_list* instrucciones = list_create();
    unPCB->instruccionesPCB = instrucciones;
    respuesta_a_kernel* respuestaGlobal;
    respuestaGlobal = args->respuestaGlobal;
    bool* interrupcion_global = args->interrupcionGlobal;
    TLB* tlb_global = args->tlb_global;
    mensaje_MEMSWAP* mensaje_MEMSWAP_global = args->mensaje_MEMSWAP_global;
    //sumar semaforos
    //free(args);
    op_code cop;
    while (cliente_socket != -1) {

    //t_list* instrucciones = list_create();
    //unPCB->instruccionesPCB = instrucciones;

        log_info(cpu_log, ANSI_COLOR_MAGENTA "Esperando mensaje de Kernel "ANSI_COLOR_RESET);
    	recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
        log_info(cpu_log, ANSI_COLOR_MAGENTA "Recibi un mensaje de Kernel "ANSI_COLOR_RESET);
        
        switch (cop) {
            case RECIBIR_PCB:

                    
            	        
                        //--------------- Recibir PCB ------------------
                        recibir_PCB(cliente_socket,unPCB);
                        cicloDeInstruccion(unPCB,interrupcion_global,respuestaGlobal,tlb_global,mensaje_MEMSWAP_global);
                        //liberarMemoriaPCB(unPCB);
                        break;

            // Errores

            case -1:
                log_error(cpu_log, "Cliente desconectado de MODULO %s", server_name);
                return;
            default:
                log_error(cpu_log, "Algo anduvo mal en el server de MODULO %s ", server_name);
                log_info(cpu_log, ANSI_COLOR_MAGENTA"C_OP: "ANSI_BACKGROUND_CYAN"%d"ANSI_COLOR_RESET, cop);
                return;
        }
    }

    log_warning(cpu_log, "El cliente se desconecto de MODULO %s server", server_name);
    return;
}

void recibir_PCB(int  fd,PCB* unPCB){
    int id;
    if (recv(fd, &id, sizeof(int), 0) != sizeof(int))
        return;
    unPCB->id = id;
    size_t size;
    if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t))
        return;
    unPCB->size = size;
    int PC;
    if (recv(fd, &PC, sizeof(int), 0) != sizeof(int))
        return;
    unPCB->PC = PC;
    int tabla_paginas;
    if (recv(fd, &tabla_paginas, sizeof(int), 0) != sizeof(int))
        return;
    unPCB->tabla_paginas = tabla_paginas;
    int estimacion_rafaga;
    if (recv(fd, &estimacion_rafaga, sizeof(int), 0) != sizeof(int))
        return;
    unPCB->estimacion_rafaga = estimacion_rafaga;
    list_clean(unPCB->instruccionesPCB);
    recibir_instrucciones(fd,unPCB->instruccionesPCB);

}

void recibir_instrucciones(int fd, t_list* instrucciones) {
    int cantidadDeInstrucciones;
    if (recv(fd, &cantidadDeInstrucciones, sizeof(int), 0) != sizeof(int))
        return;
    deserializar_instrucciones(fd ,instrucciones,cantidadDeInstrucciones);
    return ;
}

static void deserializar_instrucciones(int fd,t_list* instrucciones, int cantidadDeInstrucciones) {    
    for(int i = 0; i < cantidadDeInstrucciones ; i++ ){
        void* stream = malloc((sizeof(int)+sizeof(int)+sizeof(char)*6));
        if (recv(fd, stream, (sizeof(int)+sizeof(int)+sizeof(char)*6), 0) != (sizeof(int)+sizeof(int)+sizeof(char)*6)) {
        return;
        }
        deserializar_instruccion(instrucciones,stream);
    }

}

static void deserializar_instruccion(t_list* instrucciones,void* stream) {
    INSTRUCCION* unaInstruccion = malloc((sizeof(int)+sizeof(int)+sizeof(char)*6));
    // la Instruccion
    char* r_instruccion = malloc(sizeof(char)*6);
    memcpy(r_instruccion, stream, sizeof(char)*6);
    // arg1
    int r_arg1;
    memcpy(&r_arg1, stream+sizeof(char)*6, sizeof(int));
    // arg2
    int r_arg2;
    memcpy(&r_arg2, stream+sizeof(char)*6+sizeof(int), sizeof(int));
    unaInstruccion->codigo_de_operacion = r_instruccion;
    unaInstruccion->arg1 = r_arg1;
    unaInstruccion->arg2 = r_arg2;
    list_add(instrucciones,unaInstruccion);
}

//------------------------- Interrupcion

void comunicacion_interrupcion(void* void_args){

    interrupcion_args* args_interrupcion = (interrupcion_args*) void_args;
    int cliente_socket = esperar_cliente(args_interrupcion->log, args_interrupcion->server_name, args_interrupcion->fd_interrupt);
    t_log* cpu_log = args_interrupcion->log;
    if (cliente_socket != -1) {
        recibir_interrupcion(cliente_socket,args_interrupcion->interrupcionGlobal,cpu_log);
    }
}

void recibir_interrupcion(int cliente_socket, bool* interrupcionGlobal,t_log* cpu_log){
    char* interrupcion = malloc(sizeof(char)*13);
    void* stream = malloc(sizeof(char)*13);
    while(cliente_socket != -1){
        int bytesRecibidos = recv(cliente_socket, stream, (sizeof(char))*13, MSG_WAITALL);
        memcpy(interrupcion,stream,sizeof(char)*13);
        if(!strcmp(interrupcion,"interrupcion")){
            *interrupcionGlobal = true;
            log_info(cpu_log,ANSI_COLOR_MAGENTA "Se recibio una "ANSI_BACKGROUND_CYAN "INTERRUPCION" ANSI_COLOR_RESET ANSI_COLOR_MAGENTA" desde el puerto de interrupcion"  ANSI_COLOR_RESET);
        }
        //free(stream);
        stream = malloc(sizeof(char)*13);

        
    }

     //free(stream);
}

void liberarMemoriaPCB(PCB* unPCB){
    for(int i = 0; i< list_size(unPCB->instruccionesPCB);i++){
        INSTRUCCION* unaInstruccion = list_get(unPCB->instruccionesPCB,i);
        //free(unaInstruccion->codigo_de_operacion);
        //free(unaInstruccion);
    }
    //list_destroy(unPCB->instruccionesPCB);
    //free(unPCB);
}