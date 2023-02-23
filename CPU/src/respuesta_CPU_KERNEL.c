#include "../include/respuesta_CPU_KERNEL.h"
//calco de kernel-cpu



void comunicacion_kernel(void*  void_args){
    comunicacion_kernel_args* args_kernel = (comunicacion_kernel_args*) void_args;
    char* IP_KERNEL = args_kernel->IP_KERNEL;
    char* PUERTO_KERNEL_CPU = args_kernel->PUERTO_KERNEL_CPU;
    respuesta_a_kernel* respuesta_kernel = args_kernel->respuesta;
    t_log* log = args_kernel->log; 
    sem_wait(&SEM_KERNEL_ENCENDIDO);
    int socket;
    socket = crear_conexion(log,"KERNEL-CPU",IP_KERNEL,PUERTO_KERNEL_CPU);
    //RUNNING = pcbDeprueba();
    while(true){
        sem_wait(&SEM_RESPUESTA);
        
        enviar_respuesta_kernel(socket,respuesta_kernel);
        log_info(log, ANSI_COLOR_MAGENTA"Se le envia una respuesta a Kernel"ANSI_COLOR_RESET);
    }
    
}

void enviar_respuesta_kernel(int socket,respuesta_a_kernel* respuesta){
    size_t size;
    void* stream = serializar_respuesta(&size,respuesta);
    send(socket, stream, size, 0);
}

void* serializar_respuesta(size_t* size,respuesta_a_kernel* respuesta){
    int cop = 0;
    //op_code cop = RECIBIR_RESPUESTA;
    tipo_de_respuesta COOP_respuesta = respuesta->tipo_de_respuesta;
    int PC = respuesta->PC ;
    int tiempo_espera = respuesta->tiempo_espera;  
    *size = sizeof(int)+sizeof(codigo_de_ejecucion)+sizeof(int)+sizeof(int);
    void* stream = malloc(*size);
    memcpy(stream, &cop, sizeof(int));
    memcpy(stream+sizeof(int), &COOP_respuesta, sizeof(tipo_de_respuesta));
    memcpy(stream+sizeof(int)+sizeof(tipo_de_respuesta), &PC, sizeof(int));
    memcpy(stream+sizeof(int)+sizeof(tipo_de_respuesta)+sizeof(int), &tiempo_espera, sizeof(int));
    return stream;
}
