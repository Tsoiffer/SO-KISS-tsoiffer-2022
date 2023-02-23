#include "../include/comunicacion_MEMSWAP_CPU.h"
#define CONFIG_FILE_PATH "MEMSWAP.config"

t_config *config;
void comunicacion_MEMSWAP_CPU(void *void_args)
{

    memoria_CPU_conexion_args *args = (memoria_CPU_conexion_args *)void_args;
    t_log *memswap_log = args->log;
    int server_socket_cpu = args->server_socket;
    char *server_name = args->server_name;
    int dispatch_cpu = args->dispatch_socket;
    t_list *procesosEnMemoria = args->lista_ProcesosEnMemoria;
    int ENTRADAS_POR_TABLA = args->ENTRADAS_POR_TABLA;
    int TAM_PAGINA = args->TAM_PAGINA;
    void *stream;
    int cliente_socket = esperar_cliente(memswap_log, server_name, server_socket_cpu);

    //---------------MENSAJE INICIALIZACION
    stream = malloc(sizeof(int) * 2);
    memcpy(stream, &TAM_PAGINA, sizeof(int));
    memcpy(stream+sizeof(int), &ENTRADAS_POR_TABLA, sizeof(int));
    send(dispatch_cpu, stream, sizeof(int)*2, 0);
    //---------------FIN MENSAJE INICIALIZACION
    log_info(memswap_log,ANSI_COLOR_MAGENTA  "Comunicacion MEMSWAP CPU INICIALIZADA"ANSI_COLOR_RESET);
    //---------------Variables que uso en el envio y recepcion de mensajes
    int indiceDeMarco;
    int datoAEscribir;
    int idMCB;
    int datoRespuesta;
    int *datoRespuestaPuntero;
    int indiceTablaNivle1;
    int indiceTablaNivle2;
    int offset;
    int *punteroATablaSegundoNivel;
    config = config_create(CONFIG_FILE_PATH);
    int RETARDO_MEMORIA = config_get_int_value(config, "RETARDO_MEMORIA");
    config_destroy(config);
    
    free(args);
    tipo_mensaje_DE_CPU cop;
    while (cliente_socket != -1)
    {
        log_info(memswap_log,  ANSI_COLOR_MAGENTA "Esperando mensaje de CPU"ANSI_COLOR_RESET);
        recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
        log_info(memswap_log,  ANSI_COLOR_GREEN "CPU se comunico con MEMSWAP, esto tiene un retardo de tiempo de %i"ANSI_COLOR_RESET, RETARDO_MEMORIA);

        usleep(RETARDO_MEMORIA * 1000); 
    
        switch (cop)
        {
        case COP_LECTURA:
            log_info(memswap_log, ANSI_COLOR_GREEN "PEDIDO DE LECTURA"ANSI_COLOR_RESET);
            //----------- Recibir instruccion ----------------------
            recv(cliente_socket, &idMCB, sizeof(int), 0);
            recv(cliente_socket, &indiceDeMarco, sizeof(int), 0);
            recv(cliente_socket, &offset, sizeof(int), 0);
            log_info(memswap_log,ANSI_COLOR_YELLOW "voy a leer en el Marco: %i con offset: %i"ANSI_COLOR_RESET, indiceDeMarco,offset);
            datoRespuesta = recuperarDato(list_get(procesosEnMemoria, idMCB), indiceDeMarco, offset);
            log_info(memswap_log,ANSI_COLOR_YELLOW "El dato leido es: %i"ANSI_COLOR_RESET, datoRespuesta);
            stream = malloc(sizeof(int));
            memcpy(stream, &datoRespuesta, sizeof(int));
            send(dispatch_cpu, stream, sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_MAGENTA "Envio respuesta a CPU"ANSI_COLOR_RESET);
            break;

        case COP_ESCRITURA:
            log_info(memswap_log, ANSI_COLOR_GREEN "PEDIDO DE ESCRITURA"ANSI_COLOR_RESET);
            recv(cliente_socket, &idMCB, sizeof(int), 0);
            recv(cliente_socket, &indiceDeMarco, sizeof(int), 0);
            recv(cliente_socket, &offset, sizeof(int), 0);
            recv(cliente_socket, &datoAEscribir, sizeof(int), 0);
            log_info(memswap_log,ANSI_COLOR_YELLOW "El dato a escribir es: %i en el Marco: %i con offset: %i"ANSI_COLOR_RESET, datoAEscribir,indiceDeMarco,offset );
            
            escribirDato(list_get(procesosEnMemoria, idMCB), indiceDeMarco, offset, datoAEscribir);
            datoRespuesta = 0;
            stream = malloc(sizeof(int));
            memcpy(stream, &datoRespuesta, sizeof(int));
            send(dispatch_cpu, stream, sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_MAGENTA "Envio respuesta a CPU"ANSI_COLOR_RESET);
            break;

        case COP_PEDIDO_T_NIVEL2:
            log_info(memswap_log, ANSI_COLOR_GREEN "PEDIDO DE TABLA DE NIVEL 2"ANSI_COLOR_RESET);
            recv(cliente_socket, &idMCB, sizeof(int), 0);
            recv(cliente_socket, &indiceTablaNivle1, sizeof(int), 0);
            log_info(memswap_log, ANSI_COLOR_YELLOW"Indice Tabla Nivle 1: %i"ANSI_COLOR_RESET, indiceTablaNivle1);
            datoRespuestaPuntero = recuperarTablaSegundoNivel(indiceTablaNivle1, idMCB, procesosEnMemoria);
            log_info(memswap_log, ANSI_COLOR_YELLOW"La referencia a tabla de segundo nivel es: %i, que arranca con el id de pagina: %i"ANSI_COLOR_RESET, datoRespuestaPuntero,datoRespuestaPuntero[0]);
            stream = malloc(sizeof(int));
            memcpy(stream, &datoRespuestaPuntero, sizeof(int));
            send(dispatch_cpu, stream, sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_MAGENTA "Envio respuesta a CPU"ANSI_COLOR_RESET);
            break;

        case COP_PEDIDO_D_FISICA:
            log_info(memswap_log, ANSI_COLOR_GREEN "PEDIDO DE DIRECCION FISICA"ANSI_COLOR_RESET);
            recv(cliente_socket, &idMCB, sizeof(int), 0);
            recv(cliente_socket, &punteroATablaSegundoNivel, sizeof(int), 0);
            recv(cliente_socket, &indiceTablaNivle2, sizeof(int), 0);
            log_info(memswap_log,ANSI_COLOR_YELLOW "Puntero A Tabla Segundo Nivel: %i"ANSI_COLOR_RESET, punteroATablaSegundoNivel);
            log_info(memswap_log,ANSI_COLOR_YELLOW "Indice Tabla Nivle 2: %i"ANSI_COLOR_RESET, indiceTablaNivle2);
            datoRespuesta = recuperarIndiceDeMarco(punteroATablaSegundoNivel, indiceTablaNivle2, idMCB, procesosEnMemoria);
            log_info(memswap_log,ANSI_COLOR_YELLOW "El marco es: %i"ANSI_COLOR_RESET, datoRespuesta);
            stream = malloc(sizeof(int));
            memcpy(stream, &datoRespuesta, sizeof(int));
            send(dispatch_cpu, stream, sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_MAGENTA "Envio respuesta a CPU"ANSI_COLOR_RESET);
            break;
        case -1:
            log_error(memswap_log, "Cliente desconectado de MODULO %s... \n", server_name);
            return;
        default:
            log_error(memswap_log, "Algo anduvo mal en el server de MODULO %s \n", server_name);
            return;
        }
    }
    sem_post(&SEM_FIN_MEMORIA);
    log_warning(memswap_log, "El cliente se desconecto de MODULO %s server \n", server_name);
    return;
}

int recuperarDato(MCB *mcb, int indiceDeMarco, int offset)
{
    //printf("ingreso a recuperarDato\n");
    T_marco *marco = list_get(mcb->T_marco, indiceDeMarco);
    uint32_t *direccionDeDato = marco->punteroAMarco + offset;
    marco->bitU = 1;
    return (int)direccionDeDato[0];
    
}
void escribirDato(MCB *mcb, int indiceDeMarco, int offset, int dato)
{
    //printf("ingreso a escribirDato\n");
    T_marco *marco = list_get(mcb->T_marco, indiceDeMarco);
    uint32_t *direccionDeDato = marco->punteroAMarco + offset;
    direccionDeDato[0] = (uint32_t)dato; 
    marco->bitM = 1;
    marco->bitU = 1;

    //printf("voy a escribir en el indiceDeMarco: %i, que tiene el idMarco: %i, y corresponde a la pagina: %i, el dato: %i, dejamos su bitDeModificado en: %i\n",indiceDeMarco,marco->idMarco,marco->idPagina,direccionDeDato[0],marco->bitM);
    
}

int *recuperarTablaSegundoNivel(int indiceTablaNivle1, int idMCB, t_list *procesosEnMemoria)
{
    //printf("ingreso a recuperarTablaSegundoNivel\n");
    MCB *mcbCorrespondiente = list_get(procesosEnMemoria, idMCB);
    int* tablaDeSegundoNivel = mcbCorrespondiente->tabla_paginas_primer_nivel[0];
    return mcbCorrespondiente->tabla_paginas_primer_nivel[indiceTablaNivle1];

}

int recuperarIndiceDeMarco(int *punteroATablaSegundoNivel, int indiceTablaNivle2, int idMCB, t_list *procesosEnMemoria)
{   
    //printf("ingreso a recuperarIndiceDeMarco\n");
    MCB *mcbCorrespondiente = list_get(procesosEnMemoria, idMCB);
    int indiceDeMarco;
    int numeroDePagina = punteroATablaSegundoNivel[indiceTablaNivle2];
    //printf("Estamos viendo si esta presente la pagina %i\n",numeroDePagina);

    if (seEncuantraPresente(&indiceDeMarco, mcbCorrespondiente, numeroDePagina))
    {
        //printf("la pagina estaba presente en memoria en el marco %i\n", indiceDeMarco);
        return indiceDeMarco;
    }
    else
    {

        indiceDeMarco = asignarMarco(mcbCorrespondiente, numeroDePagina);   // Recibo una direccio el indice de tabla y devuelvo la tabla de nivel 2
        //printf("la pagina no estaba presente en memoria y le asignamos el marco %i\n", indiceDeMarco);
        return indiceDeMarco;
    }

    
}

bool seEncuantraPresente(int *marcoDePagina, MCB *mcb, int pagina)
{   t_log *marco_presente_log = log_create("memswap.log", "MARCO_PRESENTE", true, LOG_LEVEL_INFO);

    log_info(marco_presente_log,ANSI_COLOR_BLUE "Voy a buscar pagina en memoria" ANSI_COLOR_RESET);

    bool estaPaginaEnMarco(void *marco)
    {
        T_marco *infoMarco = marco;
        return infoMarco->idPagina == pagina;
    }
    T_marco *unMArco = list_find(mcb->T_marco, estaPaginaEnMarco);
    if (unMArco == NULL)
    {   
        log_info(marco_presente_log,ANSI_COLOR_BLUE "La pagina %i no encuentra presente" ANSI_COLOR_RESET,pagina);
        log_destroy(marco_presente_log);
        return false;    
    }
    else
    {
        *marcoDePagina = unMArco->idMarco;
        log_info(marco_presente_log,ANSI_COLOR_BLUE "La pagina %i se encuentra presente en el marco %i" ANSI_COLOR_RESET, pagina,*marcoDePagina);
        log_destroy(marco_presente_log);
        return true;
    }
}

int asignarMarco(MCB *mcbCorrespondiente, int numeroDePagina)
{
    t_log *marco_log = log_create("memswap.log", "PF_MARCO", true, LOG_LEVEL_INFO);
    int posicionDelMarco = solicitudDeMarcoLibre(mcbCorrespondiente, marco_log);
    T_marco *marcoAsignado = list_get(mcbCorrespondiente->T_marco, posicionDelMarco);
    mcbCorrespondiente->bitMap[posicionDelMarco] = 1;
    marcoAsignado->antiguedad = horaDeEscritura();
    marcoAsignado->idPagina = numeroDePagina;
    marcoAsignado->bitU = 1;
    log_info(marco_log,ANSI_COLOR_RED "PAGE FAULT PAGINA %d" ANSI_COLOR_RESET, numeroDePagina);
    pthread_mutex_lock(&MUTEX_SWAP);
    recuperarPagina(mcbCorrespondiente->idPCB, numeroDePagina, mcbCorrespondiente->tamanioDePagina, (void *)marcoAsignado->punteroAMarco);
    pthread_mutex_unlock(&MUTEX_SWAP);
    log_destroy(marco_log);
    return marcoAsignado->idMarco;
}