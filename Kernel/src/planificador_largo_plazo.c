#include "../include/planificador_largo_plazo.h"

void planificador_largo_plazo(void *void_args)
{


    planificador_args *args_planificador = (planificador_args *)void_args;
    t_log *kernel_log = args_planificador->log;

    log_info(kernel_log,ANSI_COLOR_GREEN"INICIO PLANIFICADOR LARGO PLAZO" ANSI_COLOR_RESET);

    t_list *NEW = args_planificador->NEW;
    t_list *READY = args_planificador->READY;
    PCB *RUNNING = args_planificador->RUNNING;
    PCB *EXIT = args_planificador->EXIT;
    t_list *BLOCKED = args_planificador->BLOCKED;
    t_list *SUSPENDED_BLOCKED = args_planificador->SUSPENDED_BLOCKED;
    t_list *SUSPENDED_READY = args_planificador->SUSPENDED_READY;
    respuesta_a_kernel *respuestaGlobal = args_planificador->respuestaGlobal;
    tipo_planificador ALGORITMO_PLANIFICACION = args_planificador->tipoPlanificador;
    int interrupt_fd = args_planificador->interrupt_fd;
    mensaje_MEMSWAP *mensaje_MEMSWAP_Global = args_planificador->mensaje_MEMSWAP;

    while (1)
    {
        sem_wait(&SEM_COLA_NEW_O_SUSPENDED_READY);
        log_info(kernel_log, ANSI_COLOR_GREEN "Cantidad de PCBs en NEW: " ANSI_BACKGROUND_RED "%i" ANSI_COLOR_RESET, list_size(NEW));
        sem_wait(&SEM_COLA_MULTIPROGRAMACION);
        PCB *PCBaREADY;
        if (list_size(SUSPENDED_READY) > 0)
        {
            PCBaREADY = list_remove(SUSPENDED_READY, 0);
            pthread_mutex_lock(&MUTEX_COMUNICACION_MEMSWAP);
            mensaje_MEMSWAP_Global->tipo_mensaje = COP_SUSPEND_READY_PCB;
        }
        else
        {
            PCBaREADY = list_remove(NEW, 0);
            pthread_mutex_lock(&MUTEX_COMUNICACION_MEMSWAP);
            mensaje_MEMSWAP_Global->tipo_mensaje = COP_NEW_PCB;
        }


        mensaje_MEMSWAP_Global->id_PCB = PCBaREADY->id;
        mensaje_MEMSWAP_Global->tamanio_PCB = PCBaREADY->size; 
        sem_post(&SEM_SEND_MEMSWAP);
        sem_wait(&SEM_RECV_MEMSWAP);
        PCBaREADY->tabla_paginas = mensaje_MEMSWAP_Global->id_MCB;
        pthread_mutex_unlock(&MUTEX_COMUNICACION_MEMSWAP);

        if ((ALGORITMO_PLANIFICACION == SRT) && (RUNNING->id > 0))
        {
            enviarInterrupcion(interrupt_fd, kernel_log);
            printf("MANDE UNA INTERRUP DESDE READY\n");
        }

        list_add(READY, PCBaREADY);
        sem_post(&SEM_COLA_READY);
        log_info(kernel_log, ANSI_COLOR_GREEN "Cantidad de PCBs en READY: " ANSI_BACKGROUND_RED "%i" ANSI_COLOR_RESET, list_size(READY));
    }
}

void enviarInterrupcion(int interrupt_fd, t_log *kernel_log)
{
    char *unaInterrupcion = "interrupcion";
    size_t size = (sizeof(char)) * 13;
    void *stream = malloc(size);
    memcpy(stream, unaInterrupcion, size);
    send(interrupt_fd, stream, size, 0);
    free(stream);
}
