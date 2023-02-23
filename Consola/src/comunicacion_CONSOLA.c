#include "../include/comunicacion_CONSOLA.h"

void parsear_codigo_ejecucion(char* codigoEjecucion,t_list* lista_ejecucion ){
  int tamanio = 0;
  char** codigoSeparadoPorLinea = string_split(codigoEjecucion,"\n");
  for(int i=0; codigoSeparadoPorLinea[i] != NULL; i++ ){
    //spliteo por argumentos
    char** instruccionEnString = string_split(codigoSeparadoPorLinea[i]," ");
    INSTRUCCION* instruccion = (INSTRUCCION*) malloc(sizeof(INSTRUCCION));
    inicializar_codigo_de_operacion(instruccion->codigo_de_operacion);
    strcpy(instruccion->codigo_de_operacion,instruccionEnString[0]);

    if(1 == string_array_size(instruccionEnString)){ 
      instruccion->arg1 =  NULL;
      instruccion->arg2 = NULL;
    }

    if(2 == string_array_size(instruccionEnString)){ 
      instruccion->arg1 =  atoi(instruccionEnString[1]);
      instruccion->arg2 = NULL;
    }

    if(3 == string_array_size(instruccionEnString)){
      instruccion->arg1 =  atoi(instruccionEnString[1]);
      instruccion->arg2 =  atoi(instruccionEnString[2]);
    }

    list_add(lista_ejecucion, instruccion);
  
  }

  return ;

}

void inicializar_codigo_de_operacion(char *codigo_de_operacion){
  for(int i= 0; i < 6 ;i++){
    codigo_de_operacion[i] = '\0';
  }
}

void enviarInstrucciones(int socket,t_list* instrucciones){
  size_t size;
  void* stream = serializar_instrucciones(&size,instrucciones);
  send(socket, stream, size, 0);
}

void* serializar_instrucciones(size_t* size,t_list* instrucciones){
  int cantidad_instrucciones = list_size(instrucciones);
  op_code cop = INSTRUCCIONES_PCB;
  *size = (sizeof(int)+sizeof(int)+sizeof(char)*6) * cantidad_instrucciones  + sizeof(op_code) + sizeof(int) ;

  void* stream = malloc(*size);

  memcpy(stream, &cop, sizeof(op_code));
  memcpy(stream+sizeof(op_code), &cantidad_instrucciones, sizeof(int));

  for(int i = 0; i < cantidad_instrucciones ; i++ ){
    serializar_instruccion(stream,list_get(instrucciones, i),i );
  }

  return stream;

}

void* serializar_instruccion(void* stream,INSTRUCCION* instruccionCompleta, int numeroDeInstruccion){
  char* instruccion = instruccionCompleta->codigo_de_operacion;
  int arg1 = instruccionCompleta->arg1;
  int arg2 = instruccionCompleta->arg2;
  memcpy(stream+sizeof(op_code)+sizeof(int)+(sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion , instruccion, sizeof(char)*6);
  memcpy(stream+sizeof(op_code)+sizeof(int)+(sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion+sizeof(char)*6, &arg1, sizeof(int));
  memcpy(stream+sizeof(op_code)+sizeof(int)+(sizeof(int)+sizeof(int)+sizeof(char)*6)*numeroDeInstruccion+sizeof(char)*6 + sizeof(int), &arg2, sizeof(int));
  return stream;
}
