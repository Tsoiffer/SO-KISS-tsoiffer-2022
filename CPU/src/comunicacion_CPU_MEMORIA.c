#include "../include/comunicacion_CPU_MEMORIA.h"
void comunicacion_MEMSWAP(void *void_args)
{

    comunicacion_MEMSWAP_args *args_memswap = (comunicacion_MEMSWAP_args *)void_args;
    t_log *cpu_log = args_memswap->log;
    mensaje_MEMSWAP *mensaje_MEMSWAP_Global = args_memswap->mensaje_MEMSWAP;
    char *puerto_escucha_memoria = args_memswap->puerto_escucha_memoria;
    char *PUERTO_DISPATCH_MEMORIA = args_memswap->puerto_dispatch_memoria;
    char *IP_MEMORIA = args_memswap->ip_memoria;
    char *IP_ESCUCHA = args_memswap->ip_kernel;
    int respuesta_de_memoria;
    int socket_kernel_Memoria = iniciar_servidor(cpu_log, "IP_ESCUCHA", IP_ESCUCHA, puerto_escucha_memoria);
    int cliente_socket = esperar_cliente(cpu_log, "PERTO_ESCUCHA_MEMORIA", socket_kernel_Memoria);
    int dispatch_memswap = crear_conexion(cpu_log, "MEMORIA", IP_MEMORIA, PUERTO_DISPATCH_MEMORIA);

    //---------------MENSAJE INICIALIZACION
    int tamanio_pagina;
    int cant_entradas_por_tabla;
    recv(cliente_socket, &tamanio_pagina, sizeof(int), MSG_WAITALL);
    recv(cliente_socket, &cant_entradas_por_tabla, sizeof(int), MSG_WAITALL);
    mensaje_MEMSWAP_Global->datos->tamanio_pagina = tamanio_pagina;
    mensaje_MEMSWAP_Global->datos->cant_entradas_por_tabla = cant_entradas_por_tabla;

    //---------------FIN MENSAJE INICIALIZACION
    log_info(cpu_log, ANSI_COLOR_GREEN "Comunicacion MEMSWAP incializada" ANSI_COLOR_RESET);
    log_info(cpu_log, ANSI_COLOR_MAGENTA "Recibi el tamanio de pagina "ANSI_BACKGROUND_MAGENTA ANSI_COLOR_CYAN"( %i )"ANSI_COLOR_RESET ANSI_COLOR_MAGENTA" y la cantidad de entradas por tabla "ANSI_BACKGROUND_MAGENTA ANSI_COLOR_CYAN"( %i )" ANSI_COLOR_RESET,mensaje_MEMSWAP_Global->datos->tamanio_pagina,mensaje_MEMSWAP_Global->datos->cant_entradas_por_tabla );
    while (1)
    {
        sem_wait(&SEM_MENSAJE_A_MEMORIA);
        enviarMensajeAMemswap(mensaje_MEMSWAP_Global, dispatch_memswap);
        log_info(cpu_log, ANSI_COLOR_GREEN"Envie el mensaje a MEMSWAP"ANSI_COLOR_RESET);
        recv(cliente_socket, &respuesta_de_memoria, sizeof(int), MSG_WAITALL);
        mensaje_MEMSWAP_Global->respuestaMemoria = respuesta_de_memoria;
        log_info(cpu_log, ANSI_COLOR_GREEN"Recibi la respuesta de MEMSWAP"ANSI_COLOR_RESET);
        sem_post(&SEM_RESPUESTA_DE_MEMORIA);
    }
}

void enviarMensajeAMemswap(mensaje_MEMSWAP *mensaje_MEMSWAP_Global, int socket)
{
    void* stream;
    size_t size;
    tipo_mensaje_MEMSWAP cop = mensaje_MEMSWAP_Global->tipo_mensaje;
    int IDMCB = mensaje_MEMSWAP_Global->id_MCB;
    int marco = mensaje_MEMSWAP_Global->marco;
    int offset = mensaje_MEMSWAP_Global->offset;
    int datoAEscribir = mensaje_MEMSWAP_Global->datoAEscribir;
    int referenciaATabla1er_nivel = mensaje_MEMSWAP_Global->referenciaATabla1er_nivel;
    int indiceDeATabla2do_nivel = mensaje_MEMSWAP_Global->indiceDeATabla2do_nivel;
    int referenciaATabla2do_nivel = mensaje_MEMSWAP_Global->referenciaATabla2do_nivel;

    switch (mensaje_MEMSWAP_Global->tipo_mensaje)
    {
    case COP_LECTURA:
        size = sizeof(tipo_mensaje_MEMSWAP) /*Tipo de mensaje*/ +
               sizeof(int) /*IDMCB*/ +
               sizeof(int) /*Referencia a Marco*/ +
               sizeof(int) /*offset*/;
        stream = malloc(size);
        memcpy(stream, &cop, sizeof(tipo_mensaje_MEMSWAP));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP), &IDMCB, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int), &marco, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int) + sizeof(int), &offset, sizeof(int));
        send(socket, stream, size, 0);
        break;
    case COP_ESCRITURA:
        size = sizeof(tipo_mensaje_MEMSWAP) /*Tipo de mensaje*/ +
               sizeof(int) /*IDMCB*/ +
               sizeof(int) /*Referencia a Marco*/ +
               sizeof(int) /*offset*/ +
               sizeof(int) /*datoAEscribir*/;
        stream = malloc(size);
        memcpy(stream, &cop, sizeof(tipo_mensaje_MEMSWAP));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP), &IDMCB, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int), &marco, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int) + sizeof(int), &offset, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int) + sizeof(int) + sizeof(int), &datoAEscribir, sizeof(int));
        send(socket, stream, size, 0);
        break;
    case COP_PEDIDO_T_NIVEL2:
        size = sizeof(tipo_mensaje_MEMSWAP) /*Tipo de mensaje*/ +
               sizeof(int) /*IDMCB*/ +
               sizeof(int) /*referenciaATabla1er_nivel*/;
        stream = malloc(size);
        memcpy(stream, &cop, sizeof(tipo_mensaje_MEMSWAP));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP), &IDMCB, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int), &referenciaATabla1er_nivel, sizeof(int));
        send(socket, stream, size, 0);
        break;
    case COP_PEDIDO_D_FISICA:
        size = sizeof(tipo_mensaje_MEMSWAP) /*Tipo de mensaje*/ +
               sizeof(int) /*IDMCB*/ +
               sizeof(int) /*referenciaATabla2do_nivel*/ +
               sizeof(int) /*indiceDeATabla2do_nivel*/;
        stream = malloc(size);
        memcpy(stream, &cop, sizeof(tipo_mensaje_MEMSWAP));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP), &IDMCB, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int), &referenciaATabla2do_nivel, sizeof(int));
        memcpy(stream + sizeof(tipo_mensaje_MEMSWAP) + sizeof(int)+ sizeof(int), &indiceDeATabla2do_nivel, sizeof(int));
        send(socket, stream, size, 0);
        break;
    default:
        break;
    }
    free(stream);
}
