#include "../include/planificador_mediano_plazo.h"
#define CONFIG_FILE_PATH "Kernel.config"

void planificador_mediano_plazo_BLOCKED(void* void_args){

    t_config* config = config_create(CONFIG_FILE_PATH);
    planificador_args* args_planificador = (planificador_args*) void_args;
    t_log* kernel_log = args_planificador->log;

    log_info(kernel_log,ANSI_COLOR_GREEN"INICIO PLANIFICADOR MEDIANO PLAZO" ANSI_COLOR_RESET);

    int TIEMPO_MAXIMO_BLOQUEADO;
    TIEMPO_MAXIMO_BLOQUEADO = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
    t_list* NEW = args_planificador->NEW;     
    t_list* READY = args_planificador->READY;
    PCB* RUNNING = args_planificador->RUNNING;
    PCB* EXIT = args_planificador->EXIT;
    t_list* BLOCKED = args_planificador->BLOCKED;
    t_list* SUSPENDED_BLOCKED = args_planificador->SUSPENDED_BLOCKED;
    t_list* SUSPENDED_READY = args_planificador->SUSPENDED_READY;
    respuesta_a_kernel* respuestaGlobal = args_planificador->respuestaGlobal;
    int socket_interrupt = args_planificador->interrupt_fd;
    tipo_planificador ALGORITMO_PLANIFICACION = args_planificador->tipoPlanificador;
    mensaje_MEMSWAP *mensaje_MEMSWAP_Global = args_planificador->mensaje_MEMSWAP;

    pthread_t hilo_SUSPENDED_BLOCKED;
    pthread_create(&hilo_SUSPENDED_BLOCKED, NULL, (void*) planificador_mediano_plazo_SUSPENDED_BLOCKED, (void*) void_args);
    pthread_detach(hilo_SUSPENDED_BLOCKED);
    
    
    PCB* PCB_EN_IO;
    
    while (true)
    {
        
        sem_wait(&SEM_COLA_BLOCKED);
        sem_wait(&SEM_IO_LIBRE);     //Estoy haciendo I/O
        PCB_EN_IO = list_remove(BLOCKED,0);
        INSTRUCCION* instruccion_de_IO = list_get(PCB_EN_IO->instruccionesPCB,PCB_EN_IO->PC -1);
        log_info(kernel_log,ANSI_COLOR_GREEN"El PCB de id: "ANSI_BACKGROUND_RED"%i"ANSI_COLOR_RESET ANSI_COLOR_GREEN" esta en estado BLOCKED, tiempo de I/O de la instruccion es: "ANSI_BACKGROUND_RED"%i" ANSI_COLOR_RESET,PCB_EN_IO->id ,instruccion_de_IO->arg1);
        char * charhoraInicioBlocked = string_new();
        charhoraInicioBlocked = temporal_get_string_time("%H:%M:%S:%MS");
        char** charhoraInicioBlocked_split = string_split( charhoraInicioBlocked, ":");
        int horaInicioBlocked = atoi(charhoraInicioBlocked_split[0]) * 60 * 60 * 1000 + atoi(charhoraInicioBlocked_split[1]) * 60 * 1000 + atoi(charhoraInicioBlocked_split[2]) * 1000 + atoi(charhoraInicioBlocked_split[3]);
        int timepoDeEspera = horaInicioBlocked + instruccion_de_IO->arg2;
        instruccion_de_IO->arg2 = 0;
        printf("tuve un tiempo de espera de %i para ingresar a IO\n",timepoDeEspera);
        if (instruccion_de_IO->arg1 + timepoDeEspera > TIEMPO_MAXIMO_BLOQUEADO){
            
            pthread_mutex_lock(&MUTEX_COMUNICACION_MEMSWAP);
            mensaje_MEMSWAP_Global->tipo_mensaje = COP_SUSPEND_PCB;
            mensaje_MEMSWAP_Global->id_PCB = PCB_EN_IO->id;
            mensaje_MEMSWAP_Global->tamanio_PCB = PCB_EN_IO->size; 
            sem_post(&SEM_SEND_MEMSWAP);
            sem_wait(&SEM_RECV_MEMSWAP);
            PCB_EN_IO->tabla_paginas = mensaje_MEMSWAP_Global->id_MCB;
            pthread_mutex_unlock(&MUTEX_COMUNICACION_MEMSWAP);
            if (instruccion_de_IO->arg1 > TIEMPO_MAXIMO_BLOQUEADO){
                usleep((TIEMPO_MAXIMO_BLOQUEADO)*1000);
            }
            
            
            list_add(SUSPENDED_BLOCKED,PCB_EN_IO);
            sem_post(&SEM_COLA_SUSPENDED_BLOCKED);
            sem_post(&SEM_COLA_MULTIPROGRAMACION);

        }
        else{

            usleep((instruccion_de_IO->arg1)*1000);
            sem_post(&SEM_IO_LIBRE);    //Termino I/O
            list_add(READY, PCB_EN_IO);

            sem_post(&SEM_COLA_READY);

            if(ALGORITMO_PLANIFICACION == SRT){ 
                enviarInterrupcion(socket_interrupt, kernel_log); 
                printf("MANDE UNA INTERRUP DESDE IO\n");   
                //enviarInterrupcion(true);
            }
        }
        
    }
    
    
    
    //config_destroy(config);
}    


void planificador_mediano_plazo_SUSPENDED_BLOCKED(void* void_args){

    t_config* config = config_create(CONFIG_FILE_PATH);
    int TIEMPO_MAXIMO_BLOQUEADO;
    TIEMPO_MAXIMO_BLOQUEADO = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");

    planificador_args* args_planificador = (planificador_args*) void_args;
    t_log* kernel_log = args_planificador->log;
    t_list* NEW = args_planificador->NEW;     
    t_list* READY = args_planificador->READY;
    PCB* RUNNING = args_planificador->RUNNING;
    PCB* EXIT = args_planificador->EXIT;
    t_list* BLOCKED = args_planificador->BLOCKED;
    t_list* SUSPENDED_BLOCKED = args_planificador->SUSPENDED_BLOCKED;
    t_list* SUSPENDED_READY = args_planificador->SUSPENDED_READY;
    respuesta_a_kernel* respuestaGlobal = args_planificador->respuestaGlobal;
    

    PCB* PCB_EN_SUSPENDED_BLOQUED;
    
    while(true){
    sem_wait(&SEM_COLA_SUSPENDED_BLOCKED);
    PCB_EN_SUSPENDED_BLOQUED = list_remove(SUSPENDED_BLOCKED,0);
    INSTRUCCION* instruccion_de_suspend_blocked = list_get(PCB_EN_SUSPENDED_BLOQUED->instruccionesPCB,PCB_EN_SUSPENDED_BLOQUED->PC -1);
    log_info(kernel_log,ANSI_COLOR_GREEN"El PCB de id: "ANSI_BACKGROUND_RED"%i"ANSI_COLOR_RESET ANSI_COLOR_GREEN" esta en estado SUSPENDED_BLOCKED, el tiempo restante de I/O es: "ANSI_BACKGROUND_RED"%i" ANSI_COLOR_RESET,PCB_EN_SUSPENDED_BLOQUED->id,instruccion_de_suspend_blocked->arg1 - TIEMPO_MAXIMO_BLOQUEADO );
    if (instruccion_de_suspend_blocked->arg1 > TIEMPO_MAXIMO_BLOQUEADO){
        usleep((instruccion_de_suspend_blocked->arg1 - TIEMPO_MAXIMO_BLOQUEADO )*1000);
    }else{
        usleep((instruccion_de_suspend_blocked->arg1 )*1000);
    }
    
    sem_post(&SEM_IO_LIBRE);
    list_add(SUSPENDED_READY,PCB_EN_SUSPENDED_BLOQUED);
    sem_post(&SEM_COLA_NEW_O_SUSPENDED_READY);
    }
     config_destroy(config);
}

