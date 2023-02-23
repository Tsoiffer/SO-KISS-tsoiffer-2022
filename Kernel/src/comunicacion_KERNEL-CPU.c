#include "../include/comunicacion_KERNEL-CPU.h"

void comunicacion_CPU(void*  void_args){
    cpu_conexion_args* args_cpu = (cpu_conexion_args*) void_args;
    int dispatch = args_cpu->dispatch;
    int interrupt = args_cpu->interrupt; 
    PCB* RUNNING = args_cpu->unPCB; 
    t_log* log = args_cpu->log; 
    
    //RUNNING = pcbDeprueba();
    while(1){
      sem_wait(&SEM_COLA_RUNNING);
      enviarPCB(dispatch,RUNNING);
      log_info(log,ANSI_COLOR_MAGENTA "Se envio el PCB id: "ANSI_BACKGROUND_CYAN"%i"ANSI_COLOR_RESET ANSI_COLOR_MAGENTA" al CPU"ANSI_COLOR_RESET,RUNNING->id);
    }
    
}



void enviarPCB(int socket, PCB* unPCB){
    size_t size;
    void* stream = serializar_PCB(&size,unPCB);
    send(socket, stream, size, 0);
}

void* serializar_PCB(size_t* size,PCB* unPCB){
    op_code cop = RECIBIR_PCB;
    int id = unPCB->id;
    size_t sizePCB = unPCB->size;
    t_list* instruccionesPCB = unPCB->instruccionesPCB;
    int PC = unPCB->PC;
    int tabla_paginas = unPCB->tabla_paginas;
    int estimacion_rafaga = unPCB->estimacion_rafaga;
    int cantidad_instrucciones = list_size(unPCB->instruccionesPCB);
    size_t size_instrucciones = ( sizeof(int)+sizeof(int)+sizeof(char)*6) * cantidad_instrucciones + sizeof(int) ;
    size_t size_pcb_sin_tlist = sizeof(int)+sizeof(size_t)+sizeof(int)+sizeof(int)+sizeof(int);
    size_t size_pcb = sizeof(int)+sizeof(size_t)+sizeof(t_list)+sizeof(int)+sizeof(int)+sizeof(int);
    *size = size_instrucciones + sizeof(op_code) + size_pcb_sin_tlist;

    void* stream = malloc(*size);
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id, sizeof(int));
    memcpy(stream+sizeof(op_code)+sizeof(int), &sizePCB, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(int)+sizeof(size_t), &PC, sizeof(int));
    memcpy(stream+sizeof(op_code)+sizeof(int)+sizeof(size_t)+sizeof(int), &tabla_paginas, sizeof(int));
    memcpy(stream+sizeof(op_code)+sizeof(int)+sizeof(size_t)+sizeof(int)+sizeof(int), &estimacion_rafaga, sizeof(int));
    memcpy(stream+sizeof(op_code)+size_pcb_sin_tlist, &cantidad_instrucciones, sizeof(int));

    for(int i = 0; i < cantidad_instrucciones ; i++ ){
      serializar_instruccion_pcb(stream,list_get(unPCB->instruccionesPCB, i),i );
    }
    return stream;
    
}

void* serializar_instruccion_pcb(void* stream,INSTRUCCION* instruccionCompleta, int numeroDeInstruccion){
  size_t size_pcb = sizeof(int)+sizeof(size_t)+sizeof(t_list)+sizeof(int)+sizeof(int)+sizeof(int);
  size_t size_pcb_sin_tlist = sizeof(int)+sizeof(size_t)+sizeof(int)+sizeof(int)+sizeof(int);
  char* instruccion = instruccionCompleta->codigo_de_operacion;
  int arg1 = instruccionCompleta->arg1;
  int arg2 = instruccionCompleta->arg2;
  memcpy(stream+sizeof(op_code)+size_pcb_sin_tlist+sizeof(int)+(sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion , instruccion, sizeof(char)*6);
  memcpy(stream+sizeof(op_code)+size_pcb_sin_tlist+sizeof(int)+((sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion)+(sizeof(char)*6), &arg1, sizeof(int));
  memcpy(stream+sizeof(op_code)+size_pcb_sin_tlist+sizeof(int)+((sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion)+(sizeof(char)*6) + sizeof(int), &arg2, sizeof(int));

}



PCB* pcbDeprueba(){
  
  size_t sizePCB  = sizeof(int)+sizeof(size_t)+sizeof(t_list)+sizeof(int)+sizeof(int)+sizeof(int);
  
  PCB* unPCB = malloc(sizePCB );
  unPCB->id =5;
  unPCB->size = 1024;
  unPCB->PC = 1;
  unPCB->tabla_paginas = 1;
  unPCB->estimacion_rafaga = 3;
  t_list* unasInstrucciones = list_create();
  INSTRUCCION* instruccion1 = malloc(sizeof(int)+sizeof(int)+sizeof(char)*6 );
  INSTRUCCION* instruccion2 = malloc(sizeof(int)+sizeof(int)+sizeof(char)*6 );
  INSTRUCCION* instruccion3 = malloc(sizeof(int)+sizeof(int)+sizeof(char)*6 );
  instruccion1->codigo_de_operacion = "NO_OP";
  instruccion2->codigo_de_operacion = "I/O";
  instruccion3->codigo_de_operacion= "EXIT";
  //strcpy(instruccion1->codigo_de_operacion, "NO_OP");
  //strcpy(instruccion2->codigo_de_operacion, "I/O");
  //strcpy(instruccion3->codigo_de_operacion, "EXIT");
  //instruccion1->codigo_de_operacion = {'N','O','_','O','P','\0'};
  //instruccion2->codigo_de_operacion = {'I','/','O','\0','\0','\0'};
  //instruccion3->codigo_de_operacion = {'E','X','I','T','\0','\0'};
  instruccion1->arg1 = 5;
  instruccion1->arg2 = NULL;
  instruccion2->arg1 =3000;
  instruccion2->arg2 =NULL;
  instruccion3->arg1 =NULL;
  instruccion3->arg2 =NULL;
 
  list_add(unasInstrucciones, instruccion1);
  list_add(unasInstrucciones, instruccion2);
  list_add(unasInstrucciones, instruccion3);
  
  unPCB->instruccionesPCB = unasInstrucciones;
 return unPCB;
}
