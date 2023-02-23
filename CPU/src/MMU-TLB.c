#include "../include/MMU-TLB.h"

int llamada_MMU_TLB(int direccion_logica, TLB *tlb, mensaje_MEMSWAP *mensaje_MEMSWAP_global, int idMCB)
{ 
    t_log *MMU_log = log_create("cpu.log", "MMU", true, LOG_LEVEL_INFO);
    int marcoDeMemoria;
    int num_pag = floor(direccion_logica / (mensaje_MEMSWAP_global->datos->tamanio_pagina));
    if (consultaATLB(&marcoDeMemoria, num_pag, tlb))
    {
        log_info(MMU_log,ANSI_COLOR_MAGENTA "TLB HIT"ANSI_COLOR_RESET);
        // pedidoDeDatoAMemoria(direccion_fisica);
        /*algoritmo_reemplazo_TLB algoritmo = decodificarStringPlanificadorTLB(tlb->algoritmoDeRemplazo);
        if(algoritmo == LRU){

        }*/
        log_destroy(MMU_log);
        return marcoDeMemoria;
    }
    

    else
    {
        log_info(MMU_log,ANSI_COLOR_MAGENTA "TLB MISS"ANSI_COLOR_RESET);
        marcoDeMemoria = consultaAMMU(direccion_logica, mensaje_MEMSWAP_global, idMCB,tlb); // TLB MISS       
        log_info(MMU_log,ANSI_COLOR_RED "LA MMU nos respondio con el marco: %i "ANSI_COLOR_RESET,marcoDeMemoria);        
        //escribirEntradaEnLaTLB(tlb, num_pag, marcoDeMemoria);
        log_info(MMU_log,ANSI_COLOR_MAGENTA "Agrego la entrada a la TLB"ANSI_COLOR_RESET);
        log_destroy(MMU_log);
        return marcoDeMemoria;
    }
}

bool consultaATLB(int *marcoDeMemoria, int direccion_logica, TLB *tlb)
{
    algoritmo_reemplazo_TLB algoritmo = decodificarStringPlanificadorTLB(tlb->algoritmoDeRemplazo);
    for(int i = 0; i < tlb->tamanio; i++)
    {
        if(tlb->matriz[i][0] == direccion_logica)
        {
            *marcoDeMemoria = tlb->matriz[i][1];
            if (algoritmo == LRU)
            {
                tlb->matriz[i][2] = horaDeEscritura();
            }
            return true;
        }
    }
    return false;
}

int consultaAMMU(int direccion_logica, mensaje_MEMSWAP *mensaje_MEMSWAP_global, int idMCB, TLB *tlb)
{
    //[entrada_tabla_1er_nivel | entrada_tabla_2do_nivel | desplazamiento]
    // hay que hacer 2 funciones? una para la primera llamada y otra para la segunda
    int num_pag = floor(direccion_logica / (mensaje_MEMSWAP_global->datos->tamanio_pagina));
    int entrada_tabla_1er_nivel = floor(num_pag / (mensaje_MEMSWAP_global->datos->cant_entradas_por_tabla));
    int entrada_tabla_2do_nivel = num_pag % (mensaje_MEMSWAP_global->datos->cant_entradas_por_tabla);
    int desplazamiento = direccion_logica - (num_pag * (mensaje_MEMSWAP_global->datos->tamanio_pagina));
    int marcoDeMemoria;
    mensaje_MEMSWAP_global->tipo_mensaje = COP_PEDIDO_T_NIVEL2;
    mensaje_MEMSWAP_global->id_MCB = idMCB;
    mensaje_MEMSWAP_global->referenciaATabla1er_nivel = entrada_tabla_1er_nivel;

    sem_post(&SEM_MENSAJE_A_MEMORIA);
    sem_wait(&SEM_RESPUESTA_DE_MEMORIA);
    mensaje_MEMSWAP_global->tipo_mensaje = COP_PEDIDO_D_FISICA;
    mensaje_MEMSWAP_global->id_MCB = idMCB;
    mensaje_MEMSWAP_global->referenciaATabla2do_nivel = mensaje_MEMSWAP_global->respuestaMemoria;
    mensaje_MEMSWAP_global->indiceDeATabla2do_nivel = entrada_tabla_2do_nivel;

    sem_post(&SEM_MENSAJE_A_MEMORIA);
    sem_wait(&SEM_RESPUESTA_DE_MEMORIA);
    marcoDeMemoria = mensaje_MEMSWAP_global->respuestaMemoria;
    escribirEntradaEnLaTLB(tlb, num_pag, marcoDeMemoria);
    
    return marcoDeMemoria;
}

TLB *incializarTLB(int tamanio_1, char *algoritmo)
{   
    //printf("Al inicializar la TLB el tamaño es %d \n",tamanio_1);

    TLB *nuevaTLB = malloc(sizeof(int) + sizeof(int **) + sizeof(char *));
    nuevaTLB->matriz = (int **)malloc(tamanio_1 * sizeof(int *));
    for (int i = 0; i < tamanio_1; i++)
    {
        nuevaTLB->matriz[i] = (int *)malloc(3 * sizeof(int *));
    }

    nuevaTLB->tamanio = tamanio_1;
    nuevaTLB->algoritmoDeRemplazo = algoritmo;
    limpiarCacheTLB(nuevaTLB);
    return nuevaTLB;
}

void limpiarCacheTLB(TLB *tlb)
{
    for (int i = 0; i < tlb->tamanio; i++)
    {
        tlb->matriz[i][0] = -1; // numero de paginas en 0
        tlb->matriz[i][1] = -1; // numero de marcos en 0s
        tlb->matriz[i][2] = -1;
    }
}

int pedidoDeDatoAMemoria(int marco, int direccion_logica, mensaje_MEMSWAP *mensaje_MEMSWAP_global, datosMemoria *datosMemoria, int idMCB)
{
    int num_pag = floor(direccion_logica / datosMemoria->tamanio_pagina);
    int offset = direccion_logica - num_pag * datosMemoria->tamanio_pagina;

    mensaje_MEMSWAP_global->tipo_mensaje = COP_LECTURA;
    mensaje_MEMSWAP_global->id_MCB = idMCB;
    mensaje_MEMSWAP_global->marco = marco;
    mensaje_MEMSWAP_global->offset = offset;
    sem_post(&SEM_MENSAJE_A_MEMORIA);
    sem_wait(&SEM_RESPUESTA_DE_MEMORIA);
    return mensaje_MEMSWAP_global->respuestaMemoria;
}
int pedidoEscribirDatoEnMemoria(int marco, int direccion_logica, mensaje_MEMSWAP *mensaje_MEMSWAP_global, datosMemoria *datosMemoria, int idMCB, int datoAEscribir)
{
    int num_pag = floor(direccion_logica / datosMemoria->tamanio_pagina);
    int offset = direccion_logica - num_pag * datosMemoria->tamanio_pagina;

    mensaje_MEMSWAP_global->tipo_mensaje = COP_ESCRITURA;
    mensaje_MEMSWAP_global->id_MCB = idMCB;
    mensaje_MEMSWAP_global->marco = marco;
    mensaje_MEMSWAP_global->offset = offset;
    mensaje_MEMSWAP_global->datoAEscribir = datoAEscribir;
    sem_post(&SEM_MENSAJE_A_MEMORIA);
    sem_wait(&SEM_RESPUESTA_DE_MEMORIA);
    return mensaje_MEMSWAP_global->respuestaMemoria;

    return 1;
}

void escribirEntradaEnLaTLB(TLB *tlb, int num_pag, int marco)
{
    t_log *TLB_log = log_create("cpu.log", "TLB", true, LOG_LEVEL_INFO);
    int posicionTLB = espacioEnLaTLB(tlb); // devuelve -1 si la TLB esta completa
    int antiguedad = horaDeEscritura();


    if (posicionTLB != -1)  // Quedan entradas libres en la TLB
    { 
        tlb->matriz[posicionTLB][0] = num_pag;
        tlb->matriz[posicionTLB][1] = marco;
        tlb->matriz[posicionTLB][2] = antiguedad;
    }
    else    // Busco entrada del TLB victima
    { 
        posicionTLB = devolverVictimaTLB(tlb);
        tlb->matriz[posicionTLB][0] = num_pag;
        tlb->matriz[posicionTLB][1] = marco;
        tlb->matriz[posicionTLB][2] = antiguedad;
    }
    
   //log_info("el tamanio de la tlb es %i\n", tlb->tamanio);
    log_info(TLB_log,ANSI_COLOR_GREEN"Estado TLB"ANSI_COLOR_RESET);
    log_info(TLB_log,ANSI_COLOR_YELLOW"NumP - NumM - Ant "ANSI_COLOR_RESET);
    for(int i = 0; i<tlb->tamanio ; i++){
        log_info(TLB_log,ANSI_COLOR_BLUE" %d   -  %d  -  %d  "ANSI_COLOR_RESET,tlb->matriz[i][0],tlb->matriz[i][1],tlb->matriz[i][2]);
    }

    log_destroy(TLB_log);
}

int devolverVictimaTLB(TLB *tlb)
{
    int victima = 0;
    
    for (int i = 1; i < tlb->tamanio; i++)
    {
        if (tlb->matriz[victima][2] > tlb->matriz[i][2])
        {
            victima = i;
        }
    }
        
    return victima;
}

algoritmo_reemplazo_TLB decodificarStringPlanificadorTLB(char *planificador)
{
    if (!strcmp(planificador, "FIFO"))
    {
        return FIFO;
    }
    if (!strcmp(planificador, "LRU"))
    {
        return LRU;
    }
}

int espacioEnLaTLB(TLB *tlb)
{

    for (int i = 0; i < tlb->tamanio; i++)
    {
        if (tlb->matriz[i][0] == -1)
        {
            return i;
        }
    }
    return -1; 
}

int horaDeEscritura()
{
    char *hora = temporal_get_string_time("%H:%M:%S:%MS");

    char **hora_split = string_split(hora, ":");

    int hora_miliSegundo = atoi(hora_split[0]) * 60 * 60 * 1000 + atoi(hora_split[1]) * 60 * 1000 + atoi(hora_split[2]) * 1000 + atoi(hora_split[3]);

    return hora_miliSegundo;
}