#include "../include/planificador_corto_plazo.h"
#define CONFIG_FILE_PATH "Kernel.config"
#define ANSI_COLOR_MAGENTA "\x1b[35m"


void planificador_corto_plazo(void* void_args){ 

    planificador_args* args_planificador = (planificador_args*) void_args;
    t_log* kernel_log = args_planificador->log;

    log_info(kernel_log,ANSI_COLOR_GREEN"INICIO PLANIFICADOR CORTO PLAZO" ANSI_COLOR_RESET);
    
    t_list* NEW = args_planificador->NEW;     
    t_list* READY = args_planificador->READY;
    PCB* RUNNING = args_planificador->RUNNING;
    PCB* EXIT = args_planificador->EXIT; 
    t_list* BLOCKED = args_planificador->BLOCKED;
    t_list* SUSPENDED_BLOCKED = args_planificador->SUSPENDED_BLOCKED;
    t_list* SUSPENDED_READY = args_planificador->SUSPENDED_READY;
    tipo_planificador ALGORITMO_PLANIFICACION = args_planificador->tipoPlanificador;
    respuesta_a_kernel* respuesta_del_cpu = args_planificador->respuestaGlobal;
    int socket_interrupt = args_planificador->interrupt_fd;
    mensaje_MEMSWAP* mensaje_MEMSWAP_Global = args_planificador->mensaje_MEMSWAP;
    int ultimaRafaga = 0;
    
    switch (ALGORITMO_PLANIFICACION)
    {
    case FIFO: ;
        while(1){ 
            sem_wait(&SEM_COLA_READY);
            sem_wait(&SEM_RUNNING_LIBRE);    
            PCB* nuevoRUNNING = list_remove(READY,0);
            RUNNING->id = nuevoRUNNING->id;
            RUNNING->size = nuevoRUNNING->size;
            RUNNING->instruccionesPCB = nuevoRUNNING->instruccionesPCB;
            RUNNING->PC = nuevoRUNNING->PC;
            RUNNING->tabla_paginas = nuevoRUNNING->tabla_paginas;
            RUNNING->estimacion_rafaga = nuevoRUNNING->estimacion_rafaga;
            RUNNING->consola_socket = nuevoRUNNING->consola_socket;
            free(nuevoRUNNING);
            
            log_info(kernel_log,ANSI_COLOR_GREEN"Tengo el pcb de id: "ANSI_BACKGROUND_RED"%i"ANSI_COLOR_RESET ANSI_COLOR_GREEN" en RUNNING"ANSI_COLOR_RESET,RUNNING->id);
            sem_post(&SEM_COLA_RUNNING);
            sem_wait(&SEM_RESPUESTA_CPU);

            analizar_respuesta_cpu(respuesta_del_cpu,RUNNING,BLOCKED,EXIT,READY,ultimaRafaga,kernel_log,mensaje_MEMSWAP_Global);
        }
    break;
    case SRT: ;
        char* horaInicio = string_new();
        char* horaFin = string_new();
       
       

        while (1)
        {
            sem_wait(&SEM_COLA_READY);
            sem_wait(&SEM_RUNNING_LIBRE);
            PCB* nuevoRUNNING; 
            nuevoRUNNING = list_remove(READY,PCB_menor_estimacion(READY));
            RUNNING->id = nuevoRUNNING->id;
            RUNNING->size = nuevoRUNNING->size;
            RUNNING->instruccionesPCB = nuevoRUNNING->instruccionesPCB;
            RUNNING->PC = nuevoRUNNING->PC;
            RUNNING->tabla_paginas = nuevoRUNNING->tabla_paginas;
            RUNNING->estimacion_rafaga = nuevoRUNNING->estimacion_rafaga;
            RUNNING->consola_socket = nuevoRUNNING->consola_socket;
            horaInicio = temporal_get_string_time("%H:%M:%S:%MS");
            free(nuevoRUNNING);
            log_info(kernel_log,ANSI_COLOR_GREEN"Tengo el pcb de menor estimacion ("ANSI_BACKGROUND_RED"%i"ANSI_COLOR_RESET ANSI_COLOR_GREEN") de id: "ANSI_BACKGROUND_RED"%i"ANSI_COLOR_RESET ANSI_COLOR_GREEN" en RUNNING" ANSI_COLOR_RESET,RUNNING->estimacion_rafaga,RUNNING->id);
            sem_post(&SEM_COLA_RUNNING);  
            sem_wait(&SEM_RESPUESTA_CPU);
            horaFin = temporal_get_string_time("%H:%M:%S:%MS");
            ultimaRafaga = calcular_rafaga_ejecutada(horaInicio,horaFin);
            analizar_respuesta_cpu(respuesta_del_cpu,RUNNING,BLOCKED,EXIT,READY,ultimaRafaga,kernel_log,mensaje_MEMSWAP_Global);
        }
    break;

    default:
    break;
    
    } 
}


void calcular_estimacion(PCB * UN_PCB, double ALFA, int ULTIMA_RAFAGA){        //esta la llamo cada vez que un PCB vuelve de CPU (?)
    int estimacion = UN_PCB->estimacion_rafaga;
    UN_PCB->estimacion_rafaga = (ALFA * ULTIMA_RAFAGA + ((1 - ALFA) * estimacion));  
} 


int PCB_menor_estimacion(t_list* READY){  
    int tamanio = list_size(READY);
    if(tamanio > 1){
        int menor = 0;
        PCB* PCB1;
        PCB* PCB2;  
        for(int i = 1; i <= tamanio-1; i++){
            PCB1 = list_get(READY,i);
            PCB2 = list_get(READY,menor);
            printf("la estmacion del PCBID %i es %i y la del del PCBID %i es %i\n",PCB1->id,PCB1->estimacion_rafaga,PCB2->id,PCB2->estimacion_rafaga);
            if((PCB1->estimacion_rafaga) < (PCB2->estimacion_rafaga)){
                printf("el pcb %i tiene menos estimacion\n",PCB1->id);
                menor = i;
            }
        }
        return menor;
    }
    else{
        return 0;
    }
}
int calcular_rafaga_ejecutada(char* tiempoInicial, char* tiempoFinal){
    char** tiempoInicial_split = string_split( tiempoInicial, ":");
    char** tiempoFinal_split = string_split( tiempoFinal, ":");
    
    int miliSegundosInicio = atoi(tiempoInicial_split[0]) * 60 * 60 * 1000 + atoi(tiempoInicial_split[1]) * 60 * 1000 + atoi(tiempoInicial_split[2]) * 1000 + atoi(tiempoInicial_split[3]);
    int miliSegundoFinal = atoi(tiempoFinal_split[0]) * 60 * 60 * 1000 + atoi(tiempoFinal_split[1]) * 60 * 1000 + atoi(tiempoFinal_split[2]) * 1000 + atoi(tiempoFinal_split[3]);

    return miliSegundoFinal - miliSegundosInicio;
}

void analizar_respuesta_cpu(respuesta_a_kernel* respuesta_del_cpu,PCB* RUNNING,t_list* BLOCKED,PCB* EXIT,t_list* READY, int ultimaRafaga,t_log* kernel_log,mensaje_MEMSWAP* mensaje_MEMSWAP_Global){
    t_config* config =  config_create(CONFIG_FILE_PATH);
    double ALFA;
    ALFA = config_get_double_value(config, "ALFA");
    //config_destroy(config);

    switch (respuesta_del_cpu->tipo_de_respuesta)
    {
        case I_O: ;
        /* planificador_mediano_plazo(void* void_args) */
        //sem_post(SEM_RESPUESTA_I_O);
        log_info(kernel_log, ANSI_COLOR_GREEN  "-------------------- "ANSI_BACKGROUND_MAGENTA"Recibi I/O"ANSI_COLOR_RESET ANSI_COLOR_GREEN" --------------------" ANSI_COLOR_RESET);
        PCB* PCB_a_BLOCKED =  (void*) malloc(sizeof(int)+sizeof(size_t)+sizeof(t_list*)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(int));//free(PCB_a_BLOCK
        PCB_a_BLOCKED->id = RUNNING->id;
        PCB_a_BLOCKED->size = RUNNING->size;
        PCB_a_BLOCKED->instruccionesPCB = RUNNING->instruccionesPCB;
        PCB_a_BLOCKED->PC = respuesta_del_cpu->PC;
        PCB_a_BLOCKED->tabla_paginas = RUNNING->tabla_paginas;
        PCB_a_BLOCKED->estimacion_rafaga = RUNNING->estimacion_rafaga;
        PCB_a_BLOCKED->consola_socket = RUNNING->consola_socket;
        calcular_estimacion(PCB_a_BLOCKED,ALFA,ultimaRafaga);
        INSTRUCCION* instruccion_de_IO = list_get(PCB_a_BLOCKED->instruccionesPCB,PCB_a_BLOCKED->PC -1);
        char * charhoraInicioBlocked = string_new();
        charhoraInicioBlocked = temporal_get_string_time("%H:%M:%S:%MS");
        char** charhoraInicioBlocked_split = string_split( charhoraInicioBlocked, ":");
        int horaInicioBlocked = atoi(charhoraInicioBlocked_split[0]) * 60 * 60 * 1000 + atoi(charhoraInicioBlocked_split[1]) * 60 * 1000 + atoi(charhoraInicioBlocked_split[2]) * 1000 + atoi(charhoraInicioBlocked_split[3]);
        instruccion_de_IO->arg2 = horaInicioBlocked ; 
        list_add(BLOCKED,PCB_a_BLOCKED); 
        RUNNING->id = NULL;
        sem_post(&SEM_COLA_BLOCKED);
        sem_post(&SEM_RUNNING_LIBRE);
        //Da paso a mediano plazo  PCB_EN_IO->instruccionesPCB
        break;

        case EXIT_OK: 
        log_info(kernel_log, ANSI_COLOR_GREEN  "-------------------- "ANSI_BACKGROUND_RED"Recibi EXIT"ANSI_COLOR_RESET ANSI_COLOR_GREEN" --------------------" ANSI_COLOR_RESET);
        // EXIT = RUNNING y RUNNING = null (hay q pasar los VALOREEEEES, no la direccion)
        //sem_post(SEM_RESPUESTA_EXIT);
        /* planificador_largo_plazo(void* void_args) */
        EXIT->id = RUNNING->id;
        EXIT->size = RUNNING->size;
        EXIT->instruccionesPCB = RUNNING->instruccionesPCB;
        EXIT->PC = RUNNING->PC;
        EXIT->tabla_paginas = RUNNING->tabla_paginas;
        EXIT->estimacion_rafaga = RUNNING->estimacion_rafaga;
        EXIT->consola_socket = RUNNING->consola_socket;
        pthread_mutex_lock(&MUTEX_COMUNICACION_MEMSWAP); 
        mensaje_MEMSWAP_Global->tipo_mensaje = COP_END_PCB;
        mensaje_MEMSWAP_Global->id_PCB = EXIT->id;
        mensaje_MEMSWAP_Global->tamanio_PCB = EXIT->size; 
        sem_post(&SEM_SEND_MEMSWAP);
        sem_wait(&SEM_RECV_MEMSWAP); 
        EXIT->tabla_paginas = mensaje_MEMSWAP_Global->id_MCB;

        pthread_mutex_unlock(&MUTEX_COMUNICACION_MEMSWAP);
        RUNNING->id = 0;
        RUNNING->size = 0;
        RUNNING->instruccionesPCB = 0;
        RUNNING->PC = 0;
        RUNNING->tabla_paginas = 0;
        RUNNING->estimacion_rafaga = 0;
        RUNNING->consola_socket = 0;
        sem_post(&SEM_RUNNING_LIBRE);
        // int socket = socketDePCB(EXIT->id); Tener una tupla global de (idPCB - Socket_Conexion)
        // close(socket);
        void *stream = malloc( sizeof(int));
        memcpy(stream, &EXIT->id, sizeof(int));    
        send(EXIT->consola_socket, stream, sizeof(int), 0);
        log_info(kernel_log, ANSI_COLOR_MAGENTA "LE RESPONDI AL CPU " ANSI_COLOR_RESET);

        log_info(kernel_log,ANSI_COLOR_GREEN  "Se finaliza el PCB id: "ANSI_BACKGROUND_RED"%i" ANSI_COLOR_RESET ,EXIT->id);
        sem_post(&SEM_COLA_MULTIPROGRAMACION);
        break;

        case INTERRUPCION:
        /* planificador_corto_plazo(void* void_args) */
        //sem_post(SEM_RESPUESTA_INTERRUPCION;
        log_info(kernel_log,ANSI_COLOR_GREEN  "----- "ANSI_COLOR_YELLOW "Recibi una "ANSI_COLOR_GREEN ANSI_BACKGROUND_YELLOW"INTERRUPCION"ANSI_COLOR_RESET ANSI_COLOR_YELLOW", se procede a Replanificar" ANSI_COLOR_GREEN" -----" ANSI_COLOR_RESET);
        PCB* PCB_a_READY = malloc(sizeof(int)+sizeof(size_t)+sizeof(t_list)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(int)); //free(PCB_a_BLOCKED);
        PCB_a_READY->id = RUNNING->id;
        PCB_a_READY->size = RUNNING->size;
        PCB_a_READY->instruccionesPCB = RUNNING->instruccionesPCB;
        PCB_a_READY->PC = respuesta_del_cpu->PC;
        PCB_a_READY->tabla_paginas = RUNNING->tabla_paginas;
        PCB_a_READY->estimacion_rafaga = RUNNING->estimacion_rafaga -  (ultimaRafaga/100);
        //calcular_estimacion(PCB_a_READY,ALFA,ultimaRafaga);
        PCB_a_READY->consola_socket = RUNNING->consola_socket;
        list_add(READY,PCB_a_READY);
        sem_post(&SEM_COLA_READY);
        RUNNING->id = 0;
        RUNNING->size = 0;
        RUNNING->instruccionesPCB = 0;
        RUNNING->PC = 0;
        RUNNING->tabla_paginas = 0;
        RUNNING->estimacion_rafaga = 0;
        sem_post(&SEM_RUNNING_LIBRE);
        break;
    
        default:
        
        break;
    }
}





