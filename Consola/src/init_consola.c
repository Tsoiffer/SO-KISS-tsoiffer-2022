#include "../include/init_consola.h"

void cerrar_programa(t_log *consola_log, int *socket_consola, t_config *config)
{
  liberar_conexion(socket_consola);
  log_destroy(consola_log);
  config_destroy(config);
}

void finalizarConexion(int socket)
{
  size_t size = sizeof(op_code);
  void *stream = malloc(size);
  op_code cop = FIN;
  memcpy(stream, &cop, sizeof(op_code));
  send(socket, stream, size, 0);

  free(stream);
}