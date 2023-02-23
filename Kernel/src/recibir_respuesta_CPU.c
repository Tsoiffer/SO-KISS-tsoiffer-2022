//calco a cpu-kernel recibir la respuesta de respuesta_CPU_KERNEL.h en un hilo
#include "../include/recibir_respuesta_CPU.h"

void recibir_respuesta_CPU(void* void_args) {
    
    args_respuesta_CPU* args = (args_respuesta_CPU*) void_args;

    t_log* kernel_log = args->log;
    char* server_name = args->server_name;
    int server_socket = args->server_socket;
    respuesta_a_kernel* respuestaGlobal = args->respuestaGlobal;
    free(args);

    int cliente_socket = esperar_cliente(kernel_log, server_name, server_socket);
    
    int cop;
    while (cliente_socket != -1) {
        log_info(kernel_log, ANSI_COLOR_CYAN"Nos hemos conectado corectamente con el CPU desde el socket "ANSI_BACKGROUND_MAGENTA"%i"ANSI_COLOR_RESET,cliente_socket);
    	recv(cliente_socket, &cop, sizeof(int), MSG_WAITALL);
        switch (cop) {
            case 0:
                        //--------------- Recibir PCB ------------------
                        recibir_respuesta(cliente_socket,respuestaGlobal);
                        sem_post(&SEM_RESPUESTA_CPU);
                        log_info(kernel_log, ANSI_COLOR_CYAN"El CPU nos ha enviado una respuesta"ANSI_COLOR_RESET);
                        break;

            // Errores

            case -1:
                log_error(kernel_log, "Cliente desconectado de MODULO %s", server_name);
                return;
            default:
                log_error(kernel_log, "Algo anduvo mal en el server de MODULO %s \n", server_name);
                log_info(kernel_log,ANSI_COLOR_CYAN "C_OP: "ANSI_BACKGROUND_MAGENTA"%d"ANSI_COLOR_RESET, cop);
                return;
        }
    }

    log_warning(kernel_log, "El cliente se desconecto de MODULO %s server \n", server_name);
    return;
}

void recibir_respuesta(int  fd,respuesta_a_kernel* respuestaGlobal){
    tipo_de_respuesta tipo_de_respuesta;
    if (recv(fd, &tipo_de_respuesta, sizeof(tipo_de_respuesta), 0) != sizeof(tipo_de_respuesta))
        return;
    respuestaGlobal->tipo_de_respuesta = tipo_de_respuesta;
    int PC;
    if (recv(fd, &PC, sizeof(int), 0) != sizeof(int))
        return;
    respuestaGlobal->PC = PC;
    int tiempo_espera;
    if (recv(fd, &tiempo_espera, sizeof(int), 0) != sizeof(int))
        return;
    respuestaGlobal->tiempo_espera = tiempo_espera;

}
