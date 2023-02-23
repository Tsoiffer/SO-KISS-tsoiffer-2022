#include "../include/planificador.h"



void planificador(void* void_args){
    
    planificador_args* args_planificador = (planificador_args*) void_args;
    t_log* kernel_log = args_planificador->log;
    t_list* NEW = args_planificador->NEW;     
    t_list* READY = args_planificador->READY;
    PCB* RUNNING = args_planificador->RUNNING;
    PCB* EXIT = args_planificador->EXIT;
    respuesta_a_kernel* respuestaGlobal = args_planificador->respuestaGlobal;
    tipo_planificador ALGORITMO_PLANIFICACION = args_planificador->tipoPlanificador;
    int interrupt_fd = args_planificador->interrupt_fd;
    
    

    

    pthread_t hilo_planificador_corto_plazo;
    pthread_create(&hilo_planificador_corto_plazo, NULL, (void*) planificador_corto_plazo, (void*) void_args);
    pthread_detach(hilo_planificador_corto_plazo);

    pthread_t hilo_planificador_mediano_plazo;
    pthread_create(&hilo_planificador_mediano_plazo, NULL, (void*) planificador_mediano_plazo_BLOCKED, (void*) void_args);
    pthread_detach(hilo_planificador_mediano_plazo);
    
    pthread_t hilo_planificador_largo_plazo;
    pthread_create(&hilo_planificador_largo_plazo, NULL, (void*) planificador_largo_plazo, (void*) void_args);
    pthread_detach(hilo_planificador_largo_plazo);

    char* tipoDePlanificador;
    switch (ALGORITMO_PLANIFICACION)
    {
    case FIFO:
        tipoDePlanificador = "FIFO";
         break;
    case SRT:
        tipoDePlanificador = "SRT";
        break;
    default:
        tipoDePlanificador = "ERROR";
        break;
    }
    log_info(kernel_log, ANSI_COLOR_GREEN"Se inicializa el planificador, estamos Planificando en "ANSI_BACKGROUND_RED"%s"ANSI_COLOR_RESET,tipoDePlanificador);

}


tipo_planificador decode(char* ALGORITMO_PLANI){
    if(!strcmp(ALGORITMO_PLANI,"FIFO")){
        return FIFO;
    }
    if(!strcmp(ALGORITMO_PLANI,"SRT")){
        return SRT;
    }
}
