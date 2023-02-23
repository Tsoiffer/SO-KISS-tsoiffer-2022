#include "include/main.h"
#define MODULENAME "KERNEL"
#define CONFIG_FILE_PATH "Kernel.config"

t_log *kernel_log;
t_config *config;

pthread_t hilo_cpu;
pthread_t hilo_memoria;
pthread_t hilo_planificador;
pthread_t hilo_respuesta_CPU;

int cpu_dispatch_fd;
int cpu_interrupt_fd;
int fd_kernel;
int socket_kernel_CPU;




respuesta_a_kernel *respuestaGlobal;
PCB *RUNNING;
PCB *P_EXIT;
cpu_conexion_args *args_cpu;
planificador_args *planificador_args_ok;
memswap_conexion_args *args_memswap;
args_respuesta_CPU *args_respuesta_CPU_ok;

void sighandlerPiola(int s)
{
    cerrarProgramaPiola();
}

void cerrarProgramaPiola()
{

    log_info(kernel_log, "Cerrando programa");
    config_destroy(config);
    log_info(kernel_log, "Libere config");

    log_info(kernel_log, "Liberando Hilos");
    pthread_detach(hilo_cpu);
    log_info(kernel_log, "Libere hilo_cpu");
    // pthread_detach(hilo_memoria);
    log_info(kernel_log, "Libere hilo_memoria");
    pthread_detach(hilo_planificador);
    log_info(kernel_log, "Libere hilo_planificador");

    log_info(kernel_log, "Liberando Conexiones");
    close(fd_kernel);
    log_info(kernel_log, "Libere fd_kernel");
    close(cpu_dispatch_fd);
    log_info(kernel_log, "Libere cpu_dispatch_fd");
    close(cpu_interrupt_fd);
    log_info(kernel_log, "Libere cpu_interrupt_fd");
    close(socket_kernel_CPU);
    log_info(kernel_log, "Libere socket_kernel_CPU");

    inicializarFrees();
    log_info(kernel_log, "Hice frees");

    log_info(kernel_log, "Voy a liberar log");
    log_destroy(kernel_log);

    exit(0);
}

void inicializarFrees()
{

    log_info(kernel_log, "Voy a hacer free RUNNING");
    free(RUNNING);
    log_info(kernel_log, "Voy a hacer free P_EXIT");
    free(P_EXIT);

    log_info(kernel_log, "Voy a hacer free args_cpu");
    // free(args_cpu->dispatch);
    // free(args_cpu->interrupt);
    // free(args_cpu->unPCB);
    // free(args_cpu->log);
    free(args_cpu);

    log_info(kernel_log, "Voy a hacer free args_memswap");
    // free(args_memswap->memswap);
    // free(args_memswap->log);
    // free(args_memswap->lista_PCB);
    free(args_memswap);

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

    iniciarSemaforos();
    signal(SIGINT, sighandlerPiola);

    kernel_log = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    config = config_create(CONFIG_FILE_PATH);
    char *ALGORITMO_PLANIFICACION;
    ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    char *ESTIMACION_INICIAL;
    ESTIMACION_INICIAL = config_get_string_value(config, "ESTIMACION_INICIAL");
    char *ALFA;
    ALFA = config_get_string_value(config, "ALFA");
    char *GRADO_MULTIPROGRAMACION;
    GRADO_MULTIPROGRAMACION = config_get_string_value(config, "GRADO_MULTIPROGRAMACION");
    char *TIEMPO_MAXIMO_BLOQUEADO;
    TIEMPO_MAXIMO_BLOQUEADO = config_get_string_value(config, "TIEMPO_MAXIMO_BLOQUEADO");

    t_list *NEW = list_create();
    t_list *READY = list_create();
    t_list *BLOCKED = list_create();
    t_list *SUSPENDED_BLOCKED = list_create();
    t_list *SUSPENDED_READY = list_create();
    /* PCB* */ RUNNING = (void *)malloc(sizeof(int) + sizeof(size_t) + sizeof(t_list) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int));
    RUNNING->id = NULL;
    /* PCB* */ P_EXIT = (void *)malloc(sizeof(int) + sizeof(size_t) + sizeof(t_list) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int));

    log_info(kernel_log,
             "\n" ANSI_BACKGROUND_WHITE ANSI_COLOR_MAGENTA "LAS CONFIGURACIONES INICIALES DEL KERNEL SON:\n" ANSI_COLOR_RED ANSI_BACKGROUND_WHITE "ALGORITMO_PLANIFICACION: %s \n" ANSI_COLOR_RED ANSI_BACKGROUND_WHITE "ESTIMACION_INICIAL:  %s \n" ANSI_COLOR_RED ANSI_BACKGROUND_WHITE "ALFA: %s \n" ANSI_COLOR_RED ANSI_BACKGROUND_WHITE "GRADO_MULTIPROGRAMACION:  %s \n" ANSI_COLOR_RED ANSI_BACKGROUND_WHITE "TIEMPO_MAXIMO_BLOQUEADO:  %s " ANSI_COLOR_RESET "\n",
             ALGORITMO_PLANIFICACION, ESTIMACION_INICIAL, ALFA, GRADO_MULTIPROGRAMACION, TIEMPO_MAXIMO_BLOQUEADO);

    //------------------------- Cargando configuracion Puerto Kernel

    char *IP_KERNEL;
    char *PUERTO_KERNEL_CONSOLA;
    char *PUERTO_KERNEL_CPU;
    // int fd_kernel;
    IP_KERNEL = config_get_string_value(config, "IP_ESCUCHA");
    PUERTO_KERNEL_CONSOLA = config_get_string_value(config, "PUERTO_KERNEL_CONSOLA");
    PUERTO_KERNEL_CPU = config_get_string_value(config, "PUERTO_KERNEL_CPU");
    fd_kernel = iniciar_servidor(kernel_log, "KERNEL_CONSOLA", IP_KERNEL, PUERTO_KERNEL_CONSOLA);
    socket_kernel_CPU = iniciar_servidor(kernel_log, "KERNEL_CPU", IP_KERNEL, PUERTO_KERNEL_CPU);
    respuestaGlobal = malloc(sizeof(codigo_de_ejecucion) + sizeof(int) + sizeof(int));

    //------------------------- Cargando configuracion Puerto Kernel-CPU

    //------------------------- Cargando configuracion Puerto CPU

    char* IP_CPU;
    char* PUERTO_CPU_DISPATCH;
    char* PUERTO_CPU_INTERRUPT;
    // int* cpu_dispatch_fd;
    // int* cpu_interrupt_fd;
    IP_CPU = config_get_string_value(config, "IP_CPU");
    PUERTO_CPU_DISPATCH = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    PUERTO_CPU_INTERRUPT = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    cpu_dispatch_fd = crear_conexion(kernel_log, "CPU_DISPATCH", IP_CPU, PUERTO_CPU_DISPATCH);
    cpu_interrupt_fd = crear_conexion(kernel_log, "CPU_INTERRUPT", IP_CPU, PUERTO_CPU_INTERRUPT);

    // pthread_t hilo_cpu;
    /* cpu_conexion_args* */ args_cpu = malloc((sizeof(int) + sizeof(int) + sizeof(int*) + sizeof(int*)));
    args_cpu->dispatch = cpu_dispatch_fd;
    args_cpu->interrupt = cpu_interrupt_fd;
    args_cpu->unPCB = RUNNING;
    args_cpu->log = kernel_log;
    pthread_create(&hilo_cpu, NULL, (void *)comunicacion_CPU, (void*)args_cpu);
    pthread_detach(hilo_cpu);
    // comunicacion_CPU(cpu_dispatch_fd,NEW);

    //-------------------------Cargando Recibir Respuesta CPU

    // pthread_t hilo_respuesta_CPU;
    /* args_respuesta_CPU* */ args_respuesta_CPU_ok = malloc(sizeof(t_log* ) + sizeof(char* ) + sizeof(int) + sizeof(respuesta_a_kernel*));
    args_respuesta_CPU_ok->log = kernel_log;
    char* server_name_RC = "RESPUESTA_CPU";
    args_respuesta_CPU_ok->server_name = server_name_RC;
    args_respuesta_CPU_ok->server_socket = socket_kernel_CPU;
    args_respuesta_CPU_ok->respuestaGlobal = respuestaGlobal;
    pthread_create(&hilo_respuesta_CPU, NULL, (void *)recibir_respuesta_CPU, (void*)args_respuesta_CPU_ok);
    pthread_detach(hilo_respuesta_CPU);

    //-------------------------Cargando configuracion Puerto Memoria

    char* IP_MEMORIA;
    char* PUERTO_DISPATCH_MEMORIA;
    IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    PUERTO_DISPATCH_MEMORIA = config_get_string_value(config, "PUERTO_DISPATCH_MEMORIA");
    char *PUERTO_ESCUCHA_MEMORIA = config_get_string_value(config, "PUERTO_ESCUCHA_MEMORIA");

    mensaje_MEMSWAP *mensaje_MEMSWAP_Global = malloc(sizeof(tipo_mensaje_MEMSWAP ) + sizeof(int)*3);

    pthread_t hilo_memoria;
    /* memswap_conexion_args* */ args_memswap = malloc(sizeof(t_log*) + sizeof(mensaje_MEMSWAP*) + sizeof(char*)+ sizeof(char*)+ sizeof(char*)+ sizeof(char*) );
    args_memswap->log = kernel_log;
    args_memswap->mensaje_MEMSWAP = mensaje_MEMSWAP_Global;
    args_memswap->puerto_escucha_memoria = PUERTO_ESCUCHA_MEMORIA;
    args_memswap->puerto_dispatch_memoria = PUERTO_DISPATCH_MEMORIA;
    args_memswap->ip_memoria = IP_MEMORIA;
    args_memswap->ip_kernel = IP_KERNEL;
    pthread_create(&hilo_memoria, NULL, (void *)comunicacion_MEMSWAP, (void*)args_memswap);
    pthread_detach(hilo_memoria);

    //------------------------- Iniciando Planificador

    // pthread_t hilo_planificador;
    /* planificador_args* */ planificador_args_ok = malloc(sizeof(t_log*) + sizeof(t_list*) + sizeof(t_list*) + sizeof(PCB*) + sizeof(PCB*) + sizeof(respuesta_a_kernel*) + sizeof(t_list*) + sizeof(t_list*) + sizeof(t_list*) + sizeof(tipo_planificador) + sizeof(int)+ sizeof(mensaje_MEMSWAP*) );
    planificador_args_ok->log = kernel_log;
    planificador_args_ok->NEW = NEW;
    planificador_args_ok->READY = READY;
    planificador_args_ok->RUNNING = RUNNING;
    planificador_args_ok->EXIT = P_EXIT;
    planificador_args_ok->respuestaGlobal = respuestaGlobal;
    planificador_args_ok->BLOCKED = BLOCKED;
    planificador_args_ok->SUSPENDED_BLOCKED = SUSPENDED_BLOCKED;
    planificador_args_ok->SUSPENDED_READY = SUSPENDED_READY;
    planificador_args_ok->tipoPlanificador = decode(ALGORITMO_PLANIFICACION);
    planificador_args_ok->interrupt_fd = cpu_interrupt_fd;
    planificador_args_ok->mensaje_MEMSWAP = mensaje_MEMSWAP_Global;


    log_error(kernel_log,"el socket de cpu_interrupt_fd es %i\n",cpu_interrupt_fd);
    log_error(kernel_log,"el socket de cpu_dispatch_fd es %i\n",cpu_dispatch_fd);
    log_error(kernel_log,"el socket de socket_kernel_CPU es %i\n",socket_kernel_CPU);
    log_error(kernel_log,"el socket de server_name_CONSOLA es %i\n",fd_kernel);


    pthread_create(&hilo_planificador, NULL, (void *)planificador, (void*)planificador_args_ok);
    pthread_detach(hilo_planificador);

    //------------------------- Esperando Consolas

    while (server_escuchar(kernel_log, "KERNEL", fd_kernel, NEW));

    cerrarProgramaPiola();
    return 0;
}
