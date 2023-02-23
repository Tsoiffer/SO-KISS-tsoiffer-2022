#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_
#include "printfColores.h"

typedef struct
{
	//op_code codigo_de_operacion;
  char codigo_de_operacion[6];    //Le asignamos un tamaño fijo a cada instruccion
  int arg1;
  int arg2;
} INSTRUCCION;

typedef enum
{
    INSTRUCCIONES_PCB,
    RECIBIR_PCB,
	FIN  //instruccion
} op_code;




#endif
