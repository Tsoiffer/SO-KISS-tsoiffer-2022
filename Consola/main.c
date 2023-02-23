#include "include/main.h"
#define MODULENAME "CONSOLA"
#define CONFIG_FILE_PATH "consola.config"



int main(int argc, char** argv) {
  t_log* consola_log;
	FILE *archivoEjecucion;
	char *codigoEjecucion;
	char* IP_KERNEL;
	char* PUERTO_CONSOLA_KERNEL;
  int kernel_fd;
  t_list* instrucciones = list_create();
  consola_log = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_INFO);
  

  //----------- Lectrura archivo ----------------------
  t_config* config = config_create(CONFIG_FILE_PATH);
  IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
  PUERTO_CONSOLA_KERNEL = config_get_string_value(config, "PUERTO_CONSOLA_KERNEL");
  
  log_info(consola_log, ANSI_COLOR_GREEN"IP_KERNEL:%s, PUERTO_CONSOLA_KERNEL:%s"ANSI_COLOR_RESET,IP_KERNEL,PUERTO_CONSOLA_KERNEL);
  kernel_fd = crear_conexion(consola_log,"KERNEL",IP_KERNEL,PUERTO_CONSOLA_KERNEL);
  

  if ((archivoEjecucion = fopen(argv[1], "rt")) == NULL){
    	log_error(consola_log, "Error archivo de entrada");
  }

  //----------- Pasando informacion del archivo a String ----------------------
  struct stat stat_file;
  stat(argv[1], &stat_file);
  codigoEjecucion = calloc(1, stat_file.st_size + 1); //similar al malloc, pero sin llamar al memset
  fread(codigoEjecucion, stat_file.st_size, 1, archivoEjecucion);
  
  //----------- parsear codigoEjecucion ----------------------
  log_info(consola_log, ANSI_COLOR_YELLOW"Iniciando parseo del Pseudocodigo"ANSI_COLOR_RESET);
  parsear_codigo_ejecucion(codigoEjecucion,instrucciones);
  
  //----------- Enviando instruccion ----------------------
  log_info(consola_log, ANSI_COLOR_GREEN"Enviando instrucciones"ANSI_COLOR_RESET);
  int tamanioDeProceso = strtol(argv[2], NULL, 10);
  size_t size = (size_t) tamanioDeProceso;
  log_info(consola_log, ANSI_COLOR_YELLOW"El tamanio del proceso es %i"ANSI_COLOR_RESET,tamanioDeProceso);
  void * stream = malloc(sizeof(size_t));
  memcpy(stream,&size,sizeof(size_t));
  send(kernel_fd,stream,sizeof(size_t),0);

  enviarInstrucciones(kernel_fd,instrucciones);
  
  int fin;
  log_info(consola_log, ANSI_COLOR_GREEN"ESPERO LA RESPUESTA DEL KERNEL!!!!!!!"ANSI_COLOR_RESET);
  recv(kernel_fd, &fin, sizeof(int), MSG_WAITALL);
  
  finalizarConexion(kernel_fd);

  log_info(consola_log, ANSI_COLOR_BLUE"SE FINALIZO LA EJECUCION DEL PCB %i"ANSI_COLOR_RESET,fin);
  cerrar_programa(consola_log, &kernel_fd, config);
	return EXIT_SUCCESS;
}
