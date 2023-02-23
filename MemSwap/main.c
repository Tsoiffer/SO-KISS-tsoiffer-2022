#include "include/main.h"
#define MODULENAME "MEMSWAP"
#define CONFIG_FILE_PATH "MEMSWAP.config"

t_log *memswap_log;
t_config *config;
int fd_kernel;

pthread_t hilo_memoria_Kernel;
pthread_t hilo_memoria_CPU;
memoria_CPU_conexion_args *args_memoria_CPU;
memoria_Kernel_conexion_args *args_memoria_Kernel;

MCB *unMCB;

int ID = 0;
void sighandlerPiola(int s)
{
    cerrarProgramaPiola();
}

void cerrarProgramaPiola()
{

    log_info(memswap_log, "Cerrando programa");

    config_destroy(config);
    log_info(memswap_log, "Libere config");

    log_info(memswap_log, "Liberando Hilos");

    pthread_detach(hilo_memoria_Kernel);
    log_info(memswap_log, "Libere hilo_memoria_Kernel");
    pthread_detach(hilo_memoria_CPU);
    log_info(memswap_log, "Libere hilo_memoria_CPU");
    // pthread_detach();
    // log_info(memswap_log, "Libere ");

    log_info(memswap_log, "Liberando Conexiones");

    close(fd_kernel);
    log_info(memswap_log, "Libere fd_kernel");
    // close(cpu_dispatch_fd);
    // log_info(memswap_log, "Libere cpu_dispatch_fd");
    // close(cpu_interrupt_fd);
    // log_info(memswap_log, "Libere cpu_interrupt_fd");
    //  close(memswap_fd);
    // log_info(memswap_log, "Libere memswap_fd");
    // close(socket_kernel_CPU);
    // log_info(memswap_log, "Libere socket_kernel_CPU");

    inicializarFrees();
    log_info(memswap_log, "Hice frees");

    log_info(memswap_log, "Voy a liberar log");
    log_destroy(memswap_log);

    exit(0);
}

void inicializarFrees()
{

    log_info(memswap_log, "Voy a hacer free args_memoria_CPU");
    free(args_memoria_CPU);
    log_info(memswap_log, "Voy a hacer free args_memoria_Kernel");
    free(args_memoria_Kernel);

    // log_info(memswap_log, "Voy a hacer free args_cpu");
    //  free(args_cpu->dispatch);
    //  free(args_cpu->interrupt);
    //  free(args_cpu->unPCB);
    //  free(args_cpu->log);
    // free(args_cpu);
    // log_info(memswap_log, "Voy a hacer free args_memswap");
    //  free(args_memswap->memswap);
    //  free(args_memswap->log);
    //  free(args_memswap->lista_PCB);
    // free(args_memswap);

    // log_info(kernel_log, "Voy a hacer free planificador_args_ok");             ----------- CHEQUERAR PORQUE TIRA SEGFAULT
    // free(planificador_args_ok->NEW);
    // free(planificador_args_ok->READY);
    // free(planificador_args_ok->RUNNING);
    // free(planificador_args_ok->EXIT);
    // free(planificador_args_ok);

    // log_info(kernel_log, "Voy a hacer free args_respuesta_CPU_ok");          ----------- CHEQUERAR PORQUE TIRA SEGFAULT
    // free(args_respuesta_CPU_ok);
}
int main()
{

    signal(SIGINT, sighandlerPiola);
    iniciarSemaforos();
    char *IP_ESCUCHA;
    char *IP_CPU;
    char *IP_KERNEL;
    int TAM_MEMORIA;
    int TAM_PAGINA;
    int ENTRADAS_POR_TABLA;
    int RETARDO_MEMORIA;
    char *ALGORITMO_REEMPLAZO;
    int MARCOS_POR_PROCESO;
    int RETARDO_SWAP;
    char *PATH_SWAP;
    char *PUERTO_ESCUCHA_KERNEL;
    char *PUERTO_DISPATCH_KERNEL;
    char *PUERTO_ESCUCHA_CPU;
    char *PUERTO_DISPATCH_CPU;

    memswap_log = log_create("memswap.log", "MEMSWAP", true, LOG_LEVEL_INFO);
    config = config_create(CONFIG_FILE_PATH);
    IP_ESCUCHA = config_get_string_value(config, "IP_ESCUCHA");
    IP_CPU = config_get_string_value(config, "IP_CPU");
    IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    PUERTO_ESCUCHA_KERNEL = config_get_string_value(config, "PUERTO_ESCUCHA_KERNEL");
    PUERTO_DISPATCH_KERNEL = config_get_string_value(config, "PUERTO_DISPATCH_KERNEL");
    PUERTO_ESCUCHA_CPU = config_get_string_value(config, "PUERTO_ESCUCHA_CPU");
    PUERTO_DISPATCH_CPU = config_get_string_value(config, "PUERTO_DISPATCH_CPU");
    TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    ENTRADAS_POR_TABLA = config_get_int_value(config, "ENTRADAS_POR_TABLA");
    RETARDO_MEMORIA = config_get_int_value(config, "RETARDO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    MARCOS_POR_PROCESO = config_get_int_value(config, "MARCOS_POR_PROCESO");
    RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
    PATH_SWAP = config_get_string_value(config, "PATH_SWAP");

    log_info(memswap_log,
             "\n" ANSI_BACKGROUND_WHITE
                 ANSI_COLOR_MAGENTA "LAS CONFIGURACIONES INICIALES DE MEMSWAP SON:\n" ANSI_COLOR_RED
             "TAM_MEMORIA: %i \n"
             "TAM_PAGINA: %i\n"
             "ENTRADAS_POR_TABLA: %i\n"
             "RETARDO_MEMORIA: %i\n"
             "ALGORITMO_REEMPLAZO: %s\n"
             "MARCOS_POR_PROCESO: %i\n"
             "RETARDO_SWAP: %i\n"
             "PATH_SWAP: %s" ANSI_COLOR_RESET "\n",
             TAM_MEMORIA, TAM_PAGINA, ENTRADAS_POR_TABLA, RETARDO_MEMORIA, ALGORITMO_REEMPLAZO, MARCOS_POR_PROCESO, RETARDO_SWAP, PATH_SWAP);

    int server_socket_kernel = iniciar_servidor(memswap_log, "PUERTO_ESCUCHA_KERNEL", IP_ESCUCHA, PUERTO_ESCUCHA_KERNEL);
    int server_socket_cpu = iniciar_servidor(memswap_log, "PUERTO_ESCUCHA_CPU", IP_ESCUCHA, PUERTO_ESCUCHA_CPU);
    int dispatch_kernel = crear_conexion(memswap_log, "PUERTO_DISPATCH_KERNEL", IP_KERNEL, PUERTO_DISPATCH_KERNEL);
    int dispatch_cpu = crear_conexion(memswap_log, "PUERTO_DISPATCH_CPU", IP_CPU, PUERTO_DISPATCH_CPU);

    //------------------------------------------- Inicializar
    t_list* lista_ProcesosEnMemoria = list_create();

    void *memoria_Principal = inicializarMemoriaUsuario(TAM_MEMORIA, TAM_PAGINA, MARCOS_POR_PROCESO, ENTRADAS_POR_TABLA, lista_ProcesosEnMemoria);

    // pthread_t hilo_memoria_Kernel;
    args_memoria_Kernel = malloc(sizeof(t_log *) + sizeof(int) + sizeof(char*) + sizeof(int) + sizeof(t_list*) + sizeof(int) + sizeof(int)+ sizeof(int));
    args_memoria_Kernel->log = memswap_log;
    args_memoria_Kernel->lista_ProcesosEnMemoria = lista_ProcesosEnMemoria;
    args_memoria_Kernel->server_socket = server_socket_kernel;
    args_memoria_Kernel->server_name = "MEMSWAP_KERNEL";
    args_memoria_Kernel->dispatch_socket = dispatch_kernel;
    args_memoria_Kernel->ENTRADAS_POR_TABLA = ENTRADAS_POR_TABLA;
    args_memoria_Kernel->TAM_PAGINA = TAM_PAGINA;
    args_memoria_Kernel->RETARDO_SWAP = RETARDO_SWAP;

    pthread_create(&hilo_memoria_Kernel, NULL, (void *)comunicacion_MEMSWAP_Kernel, (void *)args_memoria_Kernel);
    pthread_detach(hilo_memoria_Kernel);

    // pthread_t hilo_memoria_CPU;
    args_memoria_CPU = malloc(sizeof(t_log*) + sizeof(int) + sizeof(char*) + sizeof(int)+ sizeof(t_list*) + sizeof(int) + sizeof(int));   
    args_memoria_CPU->log = memswap_log;
    args_memoria_CPU->lista_ProcesosEnMemoria = lista_ProcesosEnMemoria;
    args_memoria_CPU->server_socket = server_socket_cpu;
    args_memoria_CPU->server_name = "MEMSWAP_CPU";
    args_memoria_CPU->dispatch_socket = dispatch_cpu;
    args_memoria_CPU->ENTRADAS_POR_TABLA = ENTRADAS_POR_TABLA;
    args_memoria_CPU->TAM_PAGINA = TAM_PAGINA;

    pthread_create(&hilo_memoria_CPU, NULL, (void *)comunicacion_MEMSWAP_CPU, (void *)args_memoria_CPU);
    pthread_detach(hilo_memoria_CPU);

    log_error(memswap_log, "El socket de dispatch_cpu es %i\n", dispatch_cpu);
    log_error(memswap_log, "El socket de server_socket_cpu es %i\n", server_socket_cpu);
    log_error(memswap_log, "El socket de dispatch_kernel es %i\n", dispatch_kernel);
    log_error(memswap_log, "El socket de server_socket_kernel es %i\n", server_socket_kernel);

   
    sem_wait(&SEM_FIN_MEMORIA);
    
    //
    cerrarProgramaPiola();

    return 0;
}

void* inicializarMemoriaUsuario(int TAM_MEMORIA, int TAM_PAGINA, int MARCOS_POR_PROCESO, int ENTRADAS_POR_TABLA, t_list* procesosEnMemoria)
{

    int cantidadDeMarcosDeLaMemoria = floor(TAM_MEMORIA / TAM_PAGINA);
    void *memoriaPrincipal = malloc(TAM_MEMORIA);

    for (int i = 0; i < cantidadDeMarcosDeLaMemoria / MARCOS_POR_PROCESO; i++)
    {
        MCB *nuevoMCB = generarEstructuraDeProceso(ENTRADAS_POR_TABLA, i * MARCOS_POR_PROCESO, MARCOS_POR_PROCESO, TAM_PAGINA, memoriaPrincipal);
        asignarTablasSegundoNivel(nuevoMCB->tabla_paginas_primer_nivel, ENTRADAS_POR_TABLA);
        ID++;
        //imprimirTablaSegNivel(nuevoMCB, ENTRADAS_POR_TABLA);
        list_add(procesosEnMemoria, nuevoMCB);
    }
    //printf("la cantidad de procesos que tenemos en memoria es %i",list_size(procesosEnMemoria) );
    return memoriaPrincipal;
}

MCB *generarEstructuraDeProceso(int ENTRADAS_POR_TABLA, int posicionPrimerMarco, int MARCOS_POR_PROCESO, int tamanioDePagina, void *memoriaPrincipal)
{
    //printf("BITMAP la cantidad de marcos de proceso es %d \n",MARCOS_POR_PROCESO);
    int bitMap[MARCOS_POR_PROCESO];
    for(int i = 0;i<MARCOS_POR_PROCESO;i++ ){
        bitMap[i] = 0;
    }
    

    unMCB = malloc(sizeof(int) + sizeof(bool) + sizeof(int)+ sizeof(uint32_t *)  + sizeof(int) + sizeof(int ) + sizeof(int*) + sizeof(int**)+ sizeof(t_list *)); 
    unMCB->id = ID;
    unMCB->libre = true;
    unMCB->idPCB = -1;
    unMCB->T_marco = list_create();
    unMCB->marcoVictima = -1; 
    unMCB->primerMarco = (uint32_t *)memoriaPrincipal + (posicionPrimerMarco * tamanioDePagina);
    unMCB->bitMap = bitMap;
    unMCB->tabla_paginas_primer_nivel = (int **)malloc(ENTRADAS_POR_TABLA * sizeof(int *));
    unMCB->tamanioDePagina = tamanioDePagina;
    generarListaTMarco(unMCB, MARCOS_POR_PROCESO);
    return unMCB;
}

void generarListaTMarco(MCB *unMCB, int MARCOS_POR_PROCESO)
{
    for (int i = 0; i < MARCOS_POR_PROCESO; i++)
    {
        T_marco *nuevoMarco;
        nuevoMarco = malloc(sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(uint32_t *));
        nuevoMarco->bitU = 0;
        nuevoMarco->bitM = 0;
        nuevoMarco->punteroAMarco = unMCB->primerMarco + (i * unMCB->tamanioDePagina  ); 
        nuevoMarco->idMarco = i;
        nuevoMarco->idPagina = -1;
        list_add(unMCB->T_marco, nuevoMarco);
        //printf("el marco de id %i\n",nuevoMarco->idMarco);
        //printf("tiene el puntero a memoria %i\n",nuevoMarco->punteroAMarco);
    }
}

int solicitudDeMarcoLibre(MCB *mcb, t_log *memswap_log)
{ 
    t_config *config;
    char *ALGORITMO_REEMPLAZO;
    config = config_create(CONFIG_FILE_PATH);
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    int cantidadMarcos = list_size(mcb->T_marco);
    int marcoADevolver = 0;
    //printf("TABLA ANTES DE ASIGNAR UN MARCO POR BITMAP \n");
    //imprimirInfoDeMarcos(mcb->T_marco);
    log_info(memswap_log, "Busco un marco libre");
    for (; marcoADevolver < cantidadMarcos; marcoADevolver++)
    {
        //printf("[%i]",mcb->bitMap[marcoADevolver]);
        if (mcb->bitMap[marcoADevolver] == 0)
        {
            log_info(memswap_log, "Se encontro un marco libre %d",marcoADevolver );
            return marcoADevolver;
        }
    }

    log_info(memswap_log, "No se encontro un marco libre, se procede a aplicar el algoritmo: %s \n", ALGORITMO_REEMPLAZO);

    
    int victimaFIFO = marcoMasAntiguo(mcb->T_marco, cantidadMarcos);
    //imprimirInfoDeMarcos(mcb->T_marco);
    marcoADevolver = aplicarAlgoritmoCorrespondiente(mcb, ALGORITMO_REEMPLAZO, cantidadMarcos, victimaFIFO);
    log_info(memswap_log, "El algoritmo devolvio el marco %d",marcoADevolver );
    
    //printf("TABLA ANTES DE ASIGNAR UN MARCO algoritmo \n");
    //imprimirInfoDeMarcos(mcb->T_marco);
    T_marco *inicializador = list_get(mcb->T_marco, marcoADevolver);   
    inicializador->bitU = 0;
    inicializador->bitM = 0;
    inicializador->idPagina = -1;
    return marcoADevolver;
}

void limpiarListaTMarcos(t_list *t_marcos, int MARCOS_POR_PROCESO)
{   
    //printf("Entro a limpiar lista T_marcos \n");
    T_marco *marcoRecorredor;
    for (int i = 0; i < MARCOS_POR_PROCESO; i++)
    {
        marcoRecorredor = list_get(t_marcos, i);
        marcoRecorredor->bitU = 0; 
        marcoRecorredor->bitM = 0;
        marcoRecorredor->idPagina = -1; 
        marcoRecorredor->antiguedad = -1;
    }
}

int horaDeEscritura()
{
    
    char *hora = temporal_get_string_time("%H:%M:%S:%MS");

    char **hora_split = string_split(hora, ":");

    int hora_miliSegundo = atoi(hora_split[0]) * 60 * 60 * 1000 + atoi(hora_split[1]) * 60 * 1000 + atoi(hora_split[2]) * 1000 + atoi(hora_split[3]);

    return hora_miliSegundo;
}

int marcoMasAntiguo(t_list *t_marcos, int MARCOS_POR_PROCESO)
{   
    //printf("entro en marco mas atiguo\n");
    T_marco *marcoRecorredor;
    marcoRecorredor = list_get(t_marcos, 0);

    int marcoAntiguo = marcoRecorredor->idMarco;
    int antiguedadMarco = marcoRecorredor->antiguedad;

    //printf("El marco id %d tiene la antiguedad de %d \n",marcoAntiguo,antiguedadMarco);

    for (int i = 1; i < MARCOS_POR_PROCESO; i++)
    {
        marcoRecorredor = list_get(t_marcos, i);
        //printf("El marco id %d tiene la antiguedad de %d \n",marcoRecorredor->idMarco,marcoRecorredor->antiguedad);     
        if (antiguedadMarco > marcoRecorredor->antiguedad)
        {
            marcoAntiguo = marcoRecorredor->idMarco;
            //printf("la antiguedad %d es remplazada por antiguedad  %d \n",antiguedadMarco,marcoRecorredor->antiguedad);     
            antiguedadMarco = marcoRecorredor->antiguedad;
        }
    }

    return marcoAntiguo;
}



//-------------------------------------PRUEBAS

void imprimirInfoDeMarcos(t_list* marcos){
    printf("idM | idP | bU | bM | Antig\n");

    for(int i = 0; i< list_size(marcos); i++ ){
        T_marco* unMarco = list_get(marcos,i);
        printf("%i | %i | %i | %i | %i \n",
        unMarco->idMarco, unMarco->idPagina, 
        unMarco->bitU, unMarco->bitM, 
        unMarco->antiguedad  );
    }
}

void imprimirTablaSegNivel(MCB* nuevoMCB, int entradasPorTabla){
    for(int i = 0; i< entradasPorTabla ; i++){
        int * tablaSegNiv =  nuevoMCB->tabla_paginas_primer_nivel[i];
        printf("en la entrada de nivel 1 posicion  %i estan las hojas:\n",i);
        for(int j=0; j<entradasPorTabla;j++ ){
            printf(" %i -",tablaSegNiv[j]);
        }
        printf(" \n");
    }
    
}