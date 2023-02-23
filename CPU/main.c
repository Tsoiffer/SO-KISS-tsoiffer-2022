#include "include/main.h"
#define MODULENAME "CPU"
#define CONFIG_FILE_PATH "CPU.config"

t_log* cpu_log;
t_config* config ;

int fd_kernel;
int fd_cpu_dispatch ;
int fd_cpu_interrupt ;
int memoria_fd;

bool* interrupcion_global;




pthread_t hilo_kernel;
pthread_t hilo_interrupcion;

PCB* PCB_en_ejecucion;
respuesta_a_kernel* respuestaGlobal;
comunicacion_kernel_args* args_kernel;
interrupcion_args* args_interrupcion;
TLB* tlbDelCPU;
comunicacion_MEMSWAP_args* args_memswap;

void sighandlerPiola(int s){

    cerrarProgramaPiola();

}

void cerrarProgramaPiola(){

    log_info(cpu_log, "Cerrando programa");
    config_destroy(config);
    log_info(cpu_log, "Libere config");

    log_info(cpu_log, "Liberando Hilos");
    pthread_detach(hilo_kernel);
    log_info(cpu_log, "Libere hilo_kernel");

    log_info(cpu_log, "Liberando Conexiones");
    close(memoria_fd);
    log_info(cpu_log, "Libere memoria_fd");
    close(fd_cpu_dispatch);
    log_info(cpu_log, "Libere fd_cpu_dispatch");
	//close(fd_cpu_interrupt);
    //log_info(kernel_log, "Libere fd_cpu_interrupt");

    inicializarFrees();
    log_info(cpu_log, "Hice frees");

    log_info(cpu_log, "Voy a liberar log");
	log_destroy(cpu_log);

    exit(0);
}

void inicializarFrees(){

    log_info(cpu_log, "Voy a hacer free respuestaGlobal");
    free(respuestaGlobal);
    log_info(cpu_log, "Voy a hacer free args_kernel");
    free(args_kernel);
    log_info(cpu_log, "Voy a hacer free args_interrupcion");
    free(args_interrupcion);
    log_info(cpu_log, "Voy a hacer free PCB_en_ejecucion");
    free(PCB_en_ejecucion);
    log_info(cpu_log, "Voy a hacer free interrupcion_global");
    free(interrupcion_global);
    log_info(cpu_log, "Voy a hacer free TLB* tlbDelCPU;");
    free(tlbDelCPU);

}

int main() {
    signal(SIGINT, sighandlerPiola);
    iniciarSemaforos();
	char* IP_ESCUCHA;
	char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
    cpu_log = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
    config = config_create(CONFIG_FILE_PATH);

    int ENTRADAS_TLB = config_get_int_value(config,"ENTRADAS_TLB");
    char* REEMPLAZO_TLB = config_get_string_value(config,"REEMPLAZO_TLB");
    char* RETARDO_NOOP = config_get_string_value(config,"RETARDO_NOOP");

           
    
    log_info(cpu_log,
    "\n"ANSI_BACKGROUND_WHITE 
    ANSI_COLOR_MAGENTA   "LAS CONFIGURACIONES INICIALES DEL CPU SON:\n"
    ANSI_COLOR_RED 
     "ENTRADAS_TLB: %i \n"
     "REEMPLAZO_TLB: %s\n"
     "RETARDO_NOOP: %s "ANSI_COLOR_RESET "\n",
    ENTRADAS_TLB,REEMPLAZO_TLB,RETARDO_NOOP);
           

    //------------------------- incializarTLB

    tlbDelCPU = incializarTLB(ENTRADAS_TLB,REEMPLAZO_TLB);
    log_info(cpu_log,ANSI_COLOR_MAGENTA"Se incializo una TLB con %i entradas" ANSI_COLOR_RESET,ENTRADAS_TLB );
    //


    /* PCB* */PCB_en_ejecucion = malloc(sizeof(PCB));
    /* respuesta_a_kernel* */ respuestaGlobal =  malloc(sizeof(codigo_de_ejecucion)+sizeof(int)+sizeof(int));
    /* bool* */interrupcion_global = malloc(sizeof(bool));
    *interrupcion_global = false;
    

    //-------------------------Cargando configuracion Puerto KERNEL

    char* IP_KERNEL;
	char* PUERTO_KERNEL_CPU;
    IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    PUERTO_KERNEL_CPU = config_get_string_value(config, "PUERTO_KERNEL_CPU");
    

    //pthread_t hilo_kernel;
    /* comunicacion_kernel_args* */args_kernel = malloc((sizeof(int*)+sizeof(int*)+sizeof(int*)+sizeof(int*)));// REVISAR malloc(sizeof(int)+sizeof(respuesta_a_kernel*)+sizeof(t_log*)); REVISAR
    args_kernel->log = cpu_log;
    args_kernel->IP_KERNEL = IP_KERNEL;
    args_kernel->PUERTO_KERNEL_CPU = PUERTO_KERNEL_CPU;
    args_kernel->respuesta = respuestaGlobal;
    pthread_create(&hilo_kernel, NULL, (void*) comunicacion_kernel, (void*) args_kernel);
    pthread_detach(hilo_kernel);

  
  //------------------------- Cargando configuracion Puerto ESCUCHA

    //int fd_cpu_dispatch ;
    //int fd_cpu_interrupt ;
    IP_ESCUCHA = config_get_string_value(config, "IP_ESCUCHA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    fd_cpu_dispatch = iniciar_servidor(cpu_log, "CPU_DISPATCH", IP_ESCUCHA, PUERTO_ESCUCHA_DISPATCH);
    fd_cpu_interrupt = iniciar_servidor(cpu_log, "CPU_INTERRUPT", IP_ESCUCHA, PUERTO_ESCUCHA_INTERRUPT);


//------------------------- Cargando configuracion Puerto Memoria

    //int memoria_fd;
    char* IP_MEMORIA;
	char* PUERTO_DISPATCH_MEMORIA;
    IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    PUERTO_DISPATCH_MEMORIA = config_get_string_value(config, "PUERTO_DISPATCH_MEMORIA");
    char* PUERTO_ESCUCHA_MEMORIA = config_get_string_value(config, "PUERTO_ESCUCHA_MEMORIA");

    mensaje_MEMSWAP* mensaje_MEMSWAP_Global = malloc(sizeof(tipo_mensaje_MEMSWAP ) + sizeof(int) *8+sizeof(datosMemoria*) );
    mensaje_MEMSWAP_Global->datos = malloc(sizeof(int)*2);
    pthread_t hilo_memoria;
    /* memswap_conexion_args* */ args_memswap = malloc(sizeof(t_log*) + sizeof(mensaje_MEMSWAP*) + sizeof(char*)+ sizeof(char*)+ sizeof(char*)+ sizeof(char*) );
    args_memswap->log = cpu_log;
    args_memswap->mensaje_MEMSWAP = mensaje_MEMSWAP_Global;
    args_memswap->puerto_escucha_memoria = PUERTO_ESCUCHA_MEMORIA;
    args_memswap->puerto_dispatch_memoria = PUERTO_DISPATCH_MEMORIA;
    args_memswap->ip_memoria = IP_MEMORIA;
    args_memswap->ip_kernel = IP_ESCUCHA;
    pthread_create(&hilo_memoria, NULL, (void *)comunicacion_MEMSWAP, (void *)args_memswap);
    pthread_detach(hilo_memoria);





//------------------------- INICIANDO CPU

    args_interrupcion = malloc ((sizeof(int*)+sizeof(int*)+sizeof(int*)+sizeof(int)));
    args_interrupcion->log = cpu_log;
    args_interrupcion->server_name = "CPU_INTERRUPT";
    args_interrupcion->fd_interrupt = fd_cpu_interrupt;
    args_interrupcion->interrupcionGlobal = interrupcion_global;
    pthread_create(&hilo_interrupcion, NULL, (void*) comunicacion_interrupcion, (void*) args_interrupcion);
    pthread_detach(hilo_kernel);

    

    while(server_escuchar(cpu_log,"CPU_DISPATCH",fd_cpu_dispatch,PCB_en_ejecucion,respuestaGlobal,interrupcion_global,tlbDelCPU,mensaje_MEMSWAP_Global)){}
    
    cerrarProgramaPiola();

    return 0;
}

