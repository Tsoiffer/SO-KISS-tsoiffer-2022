#include "../include/comunicacion_KERNEL-MEMSWAP.h"

void comunicacion_MEMSWAP(void *void_args)
{
    memswap_conexion_args *args_memswap = (memswap_conexion_args *)void_args;
    t_log *kernel_log = args_memswap->log;
    char *puerto_escucha_memoria = args_memswap->puerto_escucha_memoria;
    mensaje_MEMSWAP *mensaje_MEMSWAP_Global = args_memswap->mensaje_MEMSWAP;
    char *PUERTO_DISPATCH_MEMORIA = args_memswap->puerto_dispatch_memoria;
    char *IP_MEMORIA = args_memswap->ip_memoria;
    char *IP_KERNEL = args_memswap->ip_kernel;

    int socket_kernel_Memoria = iniciar_servidor(kernel_log, "KERNEL", IP_KERNEL, puerto_escucha_memoria);
    int cliente_socket = esperar_cliente(kernel_log, "PERTO_ESCUCHA_MEMORIA", socket_kernel_Memoria);
    int dispatch_memswap = crear_conexion(kernel_log, "MEMORIA", IP_MEMORIA, PUERTO_DISPATCH_MEMORIA);

    while (1)
    {
        sem_wait(&SEM_SEND_MEMSWAP);
        enviarMensajeAMemswap(mensaje_MEMSWAP_Global, dispatch_memswap);
        log_info(kernel_log, ANSI_COLOR_MAGENTA "Se le envia mensaje a MEMSWAP"ANSI_COLOR_RESET);
        mensaje_MEMSWAP_Global->id_MCB = esperarRespuestadeMemswap(cliente_socket);
        log_info(kernel_log, ANSI_COLOR_MAGENTA "Recibi respuesta de MEMSWAP"ANSI_COLOR_RESET);
        sem_post(&SEM_RECV_MEMSWAP);
    }
}

void enviarMensajeAMemswap(mensaje_MEMSWAP *mensaje_MEMSWAP_Global, int socket)
{
    size_t size;
    void *stream = serializar_Mensaje( mensaje_MEMSWAP_Global);
    size = sizeof(int) + sizeof(int) + sizeof(tipo_mensaje_MEMSWAP);
    send(socket, stream, size, 0);
}

void *serializar_Mensaje( mensaje_MEMSWAP *mensaje_MEMSWAP_Global)
{
    tipo_mensaje_MEMSWAP cop = mensaje_MEMSWAP_Global->tipo_mensaje;
    int id_PCB = mensaje_MEMSWAP_Global->id_PCB;
    int tamanio_PCB = mensaje_MEMSWAP_Global->tamanio_PCB;
    void *stream = malloc(sizeof(int) + sizeof(int) + sizeof(tipo_mensaje_MEMSWAP));
    memcpy(stream, &cop, sizeof(tipo_mensaje_MEMSWAP));
    memcpy(stream + sizeof(op_code), &id_PCB, sizeof(int));
    memcpy(stream + sizeof(op_code)+sizeof(int), &tamanio_PCB, sizeof(int));
    return stream;
}
int esperarRespuestadeMemswap(int cliente_socket)
{
    t_log *kernel_log = log_create("kernel.log", "Respuesta MEMSWAP", true, LOG_LEVEL_INFO);
    tipo_respuesta_MEMSWAP_KERNEL cop;
    int tablaDePagina;
    recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
    switch (cop)
    {
    case COP_ID_MCB:
        recv(cliente_socket, &tablaDePagina, sizeof(int), MSG_WAITALL);
        log_info(kernel_log, ANSI_COLOR_MAGENTA "Ya reservamos el espacio de memoria del proceso y su tabla de paginas es %i"ANSI_COLOR_RESET,tablaDePagina);
        return tablaDePagina;
        break;

    case COP_ERROR:
        log_info(kernel_log, ANSI_COLOR_RED "Mo se pudo  reservamos el espacio de memoria del proceso"ANSI_COLOR_RESET);
        return NULL;
        break;

    case COP_FIN_OK:
        recv(cliente_socket, &tablaDePagina, sizeof(int), MSG_WAITALL);
        log_info(kernel_log, ANSI_COLOR_GREEN "Ya se libero el espacio de memoria del proceso"ANSI_COLOR_RESET);
        return NULL;
        break;
    default:
        // log_error(kernel_log, "Algo anduvo mal en el server de MODULO %s", server_name);

        // log_info(kernel_log, ANSI_COLOR_MAGENTA "C_OP: " ANSI_BACKGROUND_CYAN "%d" ANSI_COLOR_RESET, cop);
        return NULL;
    }
}