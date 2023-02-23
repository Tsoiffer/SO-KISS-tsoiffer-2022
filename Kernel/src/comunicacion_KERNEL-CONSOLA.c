#include "../include/comunicacion_KERNEL-CONSOLA.h"
#define CONFIG_FILE_PATH "Kernel.config"

int idsDeProcesos = 0;

int server_escuchar(t_log *kernel_log, char *server_name, int server_socket, t_list *NEW)
{
    int cliente_socket = esperar_cliente(kernel_log, server_name, server_socket);
    log_info(kernel_log, ANSI_COLOR_MAGENTA "Se ha conectado una nueva consola en el socket " ANSI_BACKGROUND_CYAN "%i" ANSI_COLOR_RESET, cliente_socket);
    if (cliente_socket != -1)
    {
        pthread_t hilo;
        t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
        args->log = kernel_log;
        args->fd = cliente_socket;
        args->server_name = server_name;
        args->lista_PCB = NEW;
        pthread_create(&hilo, NULL, (void *)procesar_conexion, (void *)args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}

void procesar_conexion(void *void_args)
{

    t_config *config = config_create(CONFIG_FILE_PATH); // TODO HACER QUE LAS CONFIGS LEVANTEN DESDE LOS ARGUMENTOS DEL MAIN(?)
    int ESTIMACION_INICIAL;
    ESTIMACION_INICIAL = config_get_int_value(config, "ESTIMACION_INICIAL");


    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    t_log *kernel_log = args->log;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    t_list *NEW = args->lista_PCB;
    PCB *unPCB = (void *)malloc(sizeof(int) + sizeof(size_t) + sizeof(t_list *) + sizeof(int) + sizeof(int) + sizeof(int));
    idsDeProcesos = idsDeProcesos + 1;
    t_list *instrucciones = list_create();
    size_t sizeRecv;
    unPCB->id = idsDeProcesos;
    unPCB->size = 0; // despues ponerlo por argumento del main de consola
    unPCB->instruccionesPCB = instrucciones;
    unPCB->PC = 0;
    unPCB->tabla_paginas = 1;                      // 1er cominucacion con memoria: devuelve el numero de tabla de pag
    unPCB->estimacion_rafaga = ESTIMACION_INICIAL; // Solo para probar
    free(args);
    op_code cop;

    config_destroy(config);
    recv(cliente_socket, &sizeRecv, sizeof(size_t), MSG_WAITALL);
    unPCB->size = sizeRecv;
    unPCB->consola_socket = cliente_socket;
    while (cliente_socket != -1)
    {
        recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
        
        switch (cop)
        {

        case INSTRUCCIONES_PCB:
            list_clean(instrucciones);  //TODO agregado nuevo
            recibir_instrucciones(cliente_socket, instrucciones);
            int cantidadDeInstrucciones = list_size(instrucciones);
            log_info(kernel_log, ANSI_COLOR_MAGENTA "La consola del socket " ANSI_BACKGROUND_CYAN "%i" ANSI_COLOR_RESET ANSI_COLOR_MAGENTA " ha enviado el PCB de id " ANSI_BACKGROUND_CYAN "%i" ANSI_COLOR_RESET ANSI_COLOR_MAGENTA ", que contiene " ANSI_BACKGROUND_CYAN "%i" ANSI_COLOR_RESET ANSI_COLOR_MAGENTA " Instrucciones" ANSI_COLOR_RESET, cliente_socket, unPCB->id, cantidadDeInstrucciones);
            pthread_mutex_lock(&MUTEX_COLA_NEW);
            list_add(NEW, unPCB);
            pthread_mutex_unlock(&MUTEX_COLA_NEW);
            sem_post(&SEM_COLA_NEW_O_SUSPENDED_READY);

            
            
            break;
        // Errores
        case FIN:
            log_info(kernel_log, ANSI_COLOR_MAGENTA "Entro en el case FIN, se finaliza comunicacion con el socket " ANSI_BACKGROUND_CYAN "%i" ANSI_COLOR_RESET, cliente_socket);

            return;

        case -1:
            log_error(kernel_log, ANSI_COLOR_MAGENTA "Cliente desconectado de MODULO " ANSI_BACKGROUND_CYAN "%s" ANSI_COLOR_RESET, server_name);
            return;

        default:
            log_error(kernel_log, "Algo anduvo mal en el server de MODULO %s", server_name);

            log_info(kernel_log, ANSI_COLOR_MAGENTA "C_OP: " ANSI_BACKGROUND_CYAN "%d" ANSI_COLOR_RESET, cop);
            return;
        }
    }

    log_warning(kernel_log, "El cliente se desconecto de MODULO %s server", server_name);
    return;
}

void recibir_instrucciones(int fd, t_list *instrucciones)
{
    size_t cantidadDeInstrucciones;
    if (recv(fd, &cantidadDeInstrucciones, sizeof(int), 0) != sizeof(int))
        return;
    deserializar_instrucciones(fd, instrucciones, cantidadDeInstrucciones);
    return;
}

void deserializar_instrucciones(int fd, t_list *instrucciones, int cantidadDeInstrucciones)
{
    for (int i = 0; i < cantidadDeInstrucciones; i++)
    {
        void *stream = malloc((sizeof(int) + sizeof(int) + sizeof(char) * 6));
        if (recv(fd, stream, (sizeof(int) + sizeof(int) + sizeof(char) * 6), 0) != (sizeof(int) + sizeof(int) + sizeof(char) * 6))
        {
            free(stream);
            return;
        }
        deserializar_instruccion(instrucciones, stream);
    }
}

void deserializar_instruccion(t_list *instrucciones, void *stream)
{
    INSTRUCCION *unaInstruccion = malloc((sizeof(int) + sizeof(int) + sizeof(char) * 6));
    // la Instruccion
    char *r_instruccion = malloc(sizeof(char) * 6);
    memcpy(r_instruccion, stream, sizeof(char) * 6);
    // arg1
    int r_arg1;
    memcpy(&r_arg1, stream + sizeof(char) * 6, sizeof(int));
    // arg2
    int r_arg2;
    memcpy(&r_arg2, stream + sizeof(char) * 6 + sizeof(int), sizeof(int));
    unaInstruccion->codigo_de_operacion = r_instruccion;
    unaInstruccion->arg1 = r_arg1;
    unaInstruccion->arg2 = r_arg2;

    list_add(instrucciones, unaInstruccion);
}
