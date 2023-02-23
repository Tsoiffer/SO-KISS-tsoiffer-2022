#include "../include/comunicacion_MEMSWAP_KERNEL.h"
#define CONFIG_FILE_PATH "MEMSWAP.config"

t_config *config;
//t_log *memswap_log;

void comunicacion_MEMSWAP_Kernel(void *void_args)
{
    memoria_Kernel_conexion_args *args = (memoria_Kernel_conexion_args *)void_args;
    t_log *memswap_log = args->log;
    int server_socket_kernel = args->server_socket;
    char *server_name = args->server_name;
    int dispatch_kernel = args->dispatch_socket;
    t_list *procesosEnMemoria = args->lista_ProcesosEnMemoria;
    int entradas_por_tabla = args->ENTRADAS_POR_TABLA;
    int tam_pagina = args->TAM_PAGINA;
    int cliente_socket = esperar_cliente(memswap_log, server_name, server_socket_kernel);
    int retardo_swap = args->RETARDO_SWAP;
    tipo_mensaje_DE_KERNEL cop;

    int idPCB;
    int tamanioDelProceso;
    int idMCBasignado;
    void *stream;
    tipo_respuesta_MEMSWAP_KERNEL copAMemswap;

    while (cliente_socket != -1)
    {
        log_info(memswap_log,  ANSI_COLOR_BLUE "Esperando mensaje de KERNEL"ANSI_COLOR_RESET);
        recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
        log_info(memswap_log,  ANSI_COLOR_GREEN "El codigo de operacion es: %d"ANSI_COLOR_RESET, cop);
        switch (cop)
        {

        case COP_NEW_PCB:
            log_info(memswap_log, ANSI_COLOR_CYAN "NEW PCB" ANSI_COLOR_RESET);
            recv(cliente_socket, &idPCB, sizeof(int), 0);
            recv(cliente_socket, &tamanioDelProceso, sizeof(int), 0);
            idMCBasignado = asignarMCB(procesosEnMemoria, idPCB, tamanioDelProceso, entradas_por_tabla);
            copAMemswap = COP_ID_MCB;
            stream = malloc(sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int));
            memcpy(stream, &copAMemswap, sizeof(tipo_respuesta_MEMSWAP_KERNEL));
            memcpy(stream + sizeof(tipo_respuesta_MEMSWAP_KERNEL), &idMCBasignado, sizeof(int));
            send(dispatch_kernel, stream, sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_BLUE "Envio respuesta a KERNEL"ANSI_COLOR_RESET);
            pthread_mutex_lock(&MUTEX_SWAP);
            generarSwapDelPCB(idPCB, entradas_por_tabla, tamanioDelProceso);
            pthread_mutex_unlock(&MUTEX_SWAP);
            log_info(memswap_log, ANSI_COLOR_GREEN"Genere SWAP del PCB de ID: %i"ANSI_COLOR_RESET,idPCB);
            log_info(memswap_log, ANSI_COLOR_CYAN "se asigno memoria al PCB %i" ANSI_COLOR_RESET,idPCB);
            break;

        case COP_SUSPEND_READY_PCB:
            log_info(memswap_log, ANSI_COLOR_CYAN "SUSPEND READY PCB" ANSI_COLOR_RESET);
            recv(cliente_socket, &idPCB, sizeof(int), 0);
            recv(cliente_socket, &tamanioDelProceso, sizeof(int), 0);
            idMCBasignado = asignarMCB(procesosEnMemoria, idPCB, tamanioDelProceso, entradas_por_tabla);
            copAMemswap = COP_ID_MCB;
            stream = malloc(sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int));
            memcpy(stream, &copAMemswap, sizeof(tipo_respuesta_MEMSWAP_KERNEL));
            memcpy(stream + sizeof(tipo_respuesta_MEMSWAP_KERNEL), &idMCBasignado, sizeof(int));
            send(dispatch_kernel, stream, sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int), 0);
            log_info(memswap_log, ANSI_COLOR_CYAN "Se asigno memoria al PCB %i" ANSI_COLOR_RESET,idPCB);
            log_info(memswap_log,  ANSI_COLOR_BLUE "Envio respuesta a KERNEL"ANSI_COLOR_RESET);
            break;

        case COP_SUSPEND_PCB_KERNEL:

            log_info(memswap_log, ANSI_COLOR_CYAN "SUSPEND PCB" ANSI_COLOR_RESET);
            recv(cliente_socket, &idPCB, sizeof(int), 0);
            recv(cliente_socket, &tamanioDelProceso, sizeof(int), 0);
            log_info(memswap_log, ANSI_COLOR_BLUE "Guardo en SWAP paginas modificadas del PCB %i" ANSI_COLOR_RESET,idPCB);
            suspenderProcesoEnMemoria(idPCB, procesosEnMemoria,entradas_por_tabla);
            //printf("DESPUES DE suspenderProcesoEnMemoria \n");
            idMCBasignado = -80; // se lo enviamos en -1 dando a entender que no esta en memoria
            copAMemswap = COP_ID_MCB;
            stream = malloc(sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int));
            memcpy(stream, &copAMemswap, sizeof(tipo_respuesta_MEMSWAP_KERNEL));
            memcpy(stream + sizeof(tipo_respuesta_MEMSWAP_KERNEL), &idMCBasignado, sizeof(int));
            //printf("ANTES DE ENVIAR MENSAJE A KERNEL \n");
            send(dispatch_kernel, stream, sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int), 0);
            //printf("DESPUES DE ENVIAR MENSAJE A KERNEL \n");
            log_info(memswap_log, ANSI_COLOR_CYAN "Se libero la memoria asignada al PCB %i" ANSI_COLOR_RESET,idPCB);
            log_info(memswap_log,  ANSI_COLOR_BLUE "Envio respuesta a KERNEL"ANSI_COLOR_RESET);
            break;

        case COP_END_PCB:

            log_info(memswap_log, ANSI_COLOR_CYAN "END PCB" ANSI_COLOR_RESET);
            recv(cliente_socket, &idPCB, sizeof(int), 0);
            recv(cliente_socket, &tamanioDelProceso, sizeof(int), 0);
            log_info(memswap_log, ANSI_COLOR_CYAN "Se finaliza el PCB de ID: %i. Libero memoria" ANSI_COLOR_RESET,idPCB);
            liberarProcesoEnMemoria(idPCB, procesosEnMemoria,entradas_por_tabla);
            idMCBasignado = -2; // se lo enviamos en -2 dando a entender que se termino ok
            copAMemswap = COP_FIN_OK;
            stream = malloc(sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int));
            memcpy(stream, &copAMemswap, sizeof(tipo_respuesta_MEMSWAP_KERNEL));
            memcpy(stream + sizeof(tipo_respuesta_MEMSWAP_KERNEL), &idMCBasignado, sizeof(int));
            send(dispatch_kernel, stream, sizeof(tipo_respuesta_MEMSWAP_KERNEL) + sizeof(int), 0);
            log_info(memswap_log,  ANSI_COLOR_BLUE "Envio respuesta a KERNEL"ANSI_COLOR_RESET);

            break;

        case -1:
            log_error(memswap_log, "Cliente desconectado de MODULO %s... \n", server_name);
            return;

        default:
            log_error(memswap_log, "Algo anduvo mal en el server de MODULO %s \n", server_name);
            log_info(memswap_log, "C_OP: %d \n", cop);
            return;
        }
    }
    sem_post(&SEM_FIN_MEMORIA);
    log_warning(memswap_log, "El cliente se desconecto de MODULO %s server \n", server_name);
    return;
}

int asignarMCB(t_list *procesosEnMemoria, int idPCB, int tamanioDelProceso, int entradas_por_tabla) 
{
    bool libre = false;
    int i = 0;
    MCB *mcb;
    while (!libre)
    {
        mcb = list_get(procesosEnMemoria, i);
        libre = mcb->libre;
        i++;
    }

    mcb->libre = false;
    mcb->idPCB = idPCB;

    limpiarListaTMarcos(mcb->T_marco, list_size(mcb->T_marco)); // limpio los t_marcos (no limpio los marcos de memoria)
    for (int i = 0; i < list_size(mcb->T_marco) ; i++)
    {
        mcb->bitMap[i] = 0;
    }

    
    //printf("probando la asignacion de tablas de segundo nivel \n");
    int * tablaDeSegundoNivel = mcb->tabla_paginas_primer_nivel[0] ;
    //printf("primer tabla segundo indice %i\n",tablaDeSegundoNivel[2]);
    tablaDeSegundoNivel = mcb->tabla_paginas_primer_nivel[1] ;
    //printf("segunda tabla tercer indice %i\n",tablaDeSegundoNivel[3]);
    return mcb->id;
}

void asignarTablasSegundoNivel(int **punteroAPaginaPrimerNivel, int entradas_por_tabla)
{
    
    int tablasDeSegundoNivelNecesarias = entradas_por_tabla;
    
    for (int i = 0; i < tablasDeSegundoNivelNecesarias; i++)
    {
        punteroAPaginaPrimerNivel[i] = (int *) malloc(sizeof(int *)*entradas_por_tabla );
        int *tablaDeSegundoNivel = punteroAPaginaPrimerNivel[i];
        for (int j = 0; j < entradas_por_tabla; j++)
        {
            tablaDeSegundoNivel[j] = i * entradas_por_tabla + j; // numeroDeMarcoDElProceso o como diria ezequiel es el numero de pagina
        }
    }
}

void generarSwapDelPCB(int idPCB, int entradasPorTabla, int tamanioDelProceso) 
{
    config = config_create(CONFIG_FILE_PATH);

    char* PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
    //printf("PATH_SWAP: %s\n",PATH_SWAP);
     
    int paginasPorPCB = entradasPorTabla * entradasPorTabla;
    char *nombreidPCB = string_itoa(idPCB);
    char *nombreSwap = string_new();

    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, PATH_SWAP);
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, nombreidPCB);
    string_append(&nombreSwap, ".swap");
    //printf("nombreSwap: %s\n",nombreSwap);
    FILE *swap_nuevo = fopen(nombreSwap, "wb");
    fclose(swap_nuevo);
    estructurarSWAP(nombreSwap, tamanioDelProceso);
    config_destroy(config);
}

void estructurarSWAP(char *nombreSwap, int tamanioDelProceso)
{
    FILE *swap = fopen(nombreSwap, "rb+");
    fseek(swap, 0, SEEK_SET);
    uint32_t a = 0;
    for (int i = 0; i < tamanioDelProceso / 4; i++)
    {
        fwrite(&a, sizeof(uint32_t), 1, swap);
    }
    fclose(swap);
}

void recuperarPagina(int idPCB, int numeroDePagina, int tamanioDePagina, void *marco)
{

    config = config_create(CONFIG_FILE_PATH);
    int RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
    char* PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
     

    //printf("voy a buscar una pagina en swap, esto tiene un retardo de tiempo de %i \n",RETARDO_SWAP);
    usleep(RETARDO_SWAP * 1000); //TODO que llegue RETARDO_SWAP (ya esta cargado en el comunicacion_MEMSWAP_Kernel)
    void *buffer = malloc(tamanioDePagina);
    char *nombreidPCB = string_itoa(idPCB);
    char *nombreSwap = string_new();
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, PATH_SWAP);
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, nombreidPCB);
    string_append(&nombreSwap, ".swap");
    FILE *swap = fopen(nombreSwap, "rb");
    rewind(swap);
    fseek(swap, numeroDePagina * (tamanioDePagina), SEEK_SET); // me psiciono en el principio de la pagina que quiero
    //printf("me despalace al bit %i \n",(numeroDePagina * (tamanioDePagina)));
    fread(buffer, tamanioDePagina, 1, swap);                   // copio toda la pagina
    memcpy(marco, buffer, tamanioDePagina);                    // copio buffer en marco
    fclose(swap);
    config_destroy(config);
}

void escribirPagina(int idPCB, int numeroDePagina, int tamanioDePagina, void *marco)
{

    config = config_create(CONFIG_FILE_PATH);
    int RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
    char* PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
    


    //printf("voy a escribir una pagina en swap, esto tiene un retardo de tiempo de %i\n",RETARDO_SWAP);
    usleep(RETARDO_SWAP*1000); //TODO que llegue RETARDO_SWAP (ya esta cargado en el comunicacion_MEMSWAP_Kernel)
    char *nombreidPCB = string_itoa(idPCB);
    char *nombreSwap = string_new();
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, PATH_SWAP);
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, nombreidPCB);
    string_append(&nombreSwap, ".swap");
    FILE *swap = fopen(nombreSwap, "rb+");
    rewind(swap);
    fseek(swap, numeroDePagina * (tamanioDePagina ), SEEK_SET); 
    fwrite(marco, tamanioDePagina, 1, swap);                       
    fclose(swap);
    config_destroy(config); 
}

void suspenderProcesoEnMemoria(int idPCB, t_list* procesosEnMemoria,int entradasPorTabla)
{
    //printf("entro en suspenderProcesoEnMemoria\n");
    MCB *mcb = recuperarMCB(idPCB, procesosEnMemoria);
    pthread_mutex_lock(&MUTEX_SWAP);
    swapingPaginasModificadas(mcb);
    pthread_mutex_unlock(&MUTEX_SWAP);
    liberarMCB(mcb,entradasPorTabla);
    // TODO Buscar el idPCB dentro de los  MCB y liberarlo, guardar en swapp lo que corresponda swap
}

void liberarProcesoEnMemoria(int idPCB,t_list* procesosEnMemoria, int entradasPorTabla)
{
    config = config_create(CONFIG_FILE_PATH);
    char* PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
    char *nombreidPCB = string_itoa(idPCB);
    char *nombreSwap = string_new();
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, PATH_SWAP);
    string_append(&nombreSwap,"/" );
    string_append(&nombreSwap, nombreidPCB);
    string_append(&nombreSwap, ".swap");
    remove(nombreSwap); //LO COMENTAMOS PARA VER Q SE ESCRIBAN OK LOS ARCHIVOS
    //printf("removemos nombreSwap: %s\n",nombreSwap);
    MCB *unMCB = recuperarMCB(idPCB, procesosEnMemoria);
    liberarMCB(unMCB,entradasPorTabla);
    config_destroy(config);
    // eliminar el swap que corresponda
}

void swapingPaginasModificadas(MCB* mcb)
{
    //printf("Entrando a swaping paginas\n");
    int MARCOS_POR_PROCESO = list_size(mcb->T_marco);
    T_marco *marcoRecorredor;
    for (int i = 0; i < MARCOS_POR_PROCESO; i++)
    {
        if (mcb->bitMap[i] == 1)
        {
            marcoRecorredor = list_get(mcb->T_marco, i);
            if (marcoRecorredor->bitM == 1)
            {
                escribirPagina(mcb->idPCB, marcoRecorredor->idPagina, mcb->tamanioDePagina, marcoRecorredor->punteroAMarco);
            }
        }
    }
}

MCB *recuperarMCB(int idPCB, t_list *procesosEnMemoria)
{
    //printf("entro en recuperarMCB\n");
    bool estaMCB(void *mcbActual)
    {
        MCB *mcbAVerificar = (MCB *)mcbActual;
        return mcbAVerificar->idPCB == idPCB;
    }
    return  list_find(procesosEnMemoria, estaMCB);
}

void liberarMCB(MCB *unMCB,int entradasPorTabla)
{
   //printf("entro en liberarMCB del PCB %i\n",unMCB->idPCB );
    int MARCOS_POR_PROCESO = list_size(unMCB->T_marco);
    unMCB->libre = true;
    unMCB->idPCB = -1;
    unMCB->marcoVictima = 0;
    for (int i = 0; i < MARCOS_POR_PROCESO; i++)
    {
        unMCB->bitMap[i] = 0;
        //printf("puse el bitMap %i en 0\n",i );
    }
    /*
    for (int i = 0; i < entradasPorTabla; i++)
    {
        int *tablaSegundoNivel = unMCB->tabla_paginas_primer_nivel[i];
        printf("voy hacer free de tablaSegundoNivel %i\n",i );
        free(tablaSegundoNivel);
    }
    */
    limpiarListaTMarcos(unMCB->T_marco, MARCOS_POR_PROCESO);
}
