#include "../include/algoritmos.h"

algoritmo_reemplazo_MP decodificadorAlgoritmo(char *algoritmo)
{   
    if (!strcmp(algoritmo,"CLOCK"))
        return CLOCK;

    if (!strcmp(algoritmo,"CLOCK-M"))
        return CLOCK_M;
}

int aplicarAlgoritmoCorrespondiente(MCB *mcb, char *algoritmo, int CantidadDeMarcos, int victimaInicial)
{
    algoritmo_reemplazo_MP ALGORITMO = decodificadorAlgoritmo(algoritmo);
    //printf("entro a aplicarAlgoritmoCorrespondiente\n");
    int marco = -1;
    switch (ALGORITMO)
    {
    case CLOCK:
        while (marco == -1) // si es -1 significa que todos los U estaban en 1
        { 
            //printf("ENTRO a case CLOCK:\n");
            marco = algoritmo_Sustitucion_Clock(mcb, CantidadDeMarcos, victimaInicial);
            //printf("SALGO de algoritmo_Sustitucion_Clock el marco es %i\n",marco);
        }

        return marco;

    case CLOCK_M:
        while (marco == -1) // si es -1 significa que todos los U y M estaban en 1 
        { 
            //printf("ENTRO a case CLOCK_M:\n");
            marco = algoritmo_Sustitucion_Clock_Modificado(mcb, CantidadDeMarcos, victimaInicial);
            //printf("SALGO de algoritmo_Sustitucion_Clock_Modificado el marco es %i\n",marco);
        }

        return marco;

    default:
        break;
    }
}

int algoritmo_Sustitucion_Clock(MCB *mcb, int CantidadDeMarcos, int fifoVictima)
{
    T_marco *marcoRecorredor;
    int restantes = fifoVictima;
    bool bandera = true;
    //printf("Entro al primer while de clock . fifoVictima %d cantidad de marcos %d\n",fifoVictima, CantidadDeMarcos);
    while (fifoVictima < CantidadDeMarcos)
    {
        marcoRecorredor = list_get(mcb->T_marco, fifoVictima);

        if (marcoRecorredor->bitU == 0) // Busco U = 0
        { 
            if (marcoRecorredor->bitM == 0)
            {
                return marcoRecorredor->idMarco;
            }
            else
            {
                pthread_mutex_lock(&MUTEX_SWAP);
                escribirPagina(mcb->idPCB, marcoRecorredor->idPagina, mcb->tamanioDePagina, marcoRecorredor->punteroAMarco);     // Bajo a swap
                pthread_mutex_unlock(&MUTEX_SWAP);
                return marcoRecorredor->idMarco;
            }
        }
        else    // Si U = 1, tiene una opoertunidad mas pero lo cambio a 0
        { 
            marcoRecorredor->bitU = 0;
        }

        fifoVictima++;
        if (fifoVictima == (CantidadDeMarcos) && bandera)
        { 
            bandera = false;
            fifoVictima = 0;
            CantidadDeMarcos = restantes;
        }
    }
    //printf("Devuelvo -1 y vuelvo a correr el argoritmo clock\n");
    return -1; // Si devulve -1, significa que tengo que volver a recorrerlo
}

int algoritmo_Sustitucion_Clock_Modificado(MCB *mcb, int CantidadDeMarcos, int fifoVictima)
{
    T_marco *marcoRecorredor;
    int restantes = fifoVictima;
    bool bandera = true;

    int fifoVictima2 = fifoVictima;
    int restantes2 = fifoVictima;
    bool bandera2 = true;
    int CantidadDeMarcos2 = CantidadDeMarcos;


    //printf("Entro al primer while de clock M. fifoVictima %d cantidad de marcos %d\n",fifoVictima, CantidadDeMarcos);
    while (fifoVictima < CantidadDeMarcos)  // busco (0,0) sin poner U en 0
    { 
        marcoRecorredor = list_get(mcb->T_marco, fifoVictima);
        if (marcoRecorredor->bitU == 0)
        {
            if (marcoRecorredor->bitM == 0)
            {
                return marcoRecorredor->idMarco;
            }
        }

        fifoVictima++;
        if (fifoVictima == (CantidadDeMarcos) && bandera)
        { 
            bandera = false;
            fifoVictima = 0;
            CantidadDeMarcos = restantes;
        }
    }

    //printf("No lo encontro en el primer while. fifoVictima2 %d cantidad de marcos2 %d\n",fifoVictima2, CantidadDeMarcos2);

    while (fifoVictima2 < CantidadDeMarcos2)    //  busco (0,1) poniendo U en 0
    { 
        marcoRecorredor = list_get(mcb->T_marco, fifoVictima2);
        if (marcoRecorredor->bitU == 0)
        {
            if (marcoRecorredor->bitM == 1)
            {
                pthread_mutex_lock(&MUTEX_SWAP);
                escribirPagina(mcb->idPCB, marcoRecorredor->idPagina, mcb->tamanioDePagina, marcoRecorredor->punteroAMarco); // Bajo a swap
                pthread_mutex_unlock(&MUTEX_SWAP);
                return marcoRecorredor->idMarco;
            }
        }
        else
        {
            marcoRecorredor->bitU = 0;
        }

        fifoVictima2++;
        if (fifoVictima2 == (CantidadDeMarcos2) && bandera2)
        { 
            bandera2 = false;
            fifoVictima2 = 0;
            CantidadDeMarcos2 = restantes2;
        }
    }

    //printf("Devuelvo -1 y vuelvo a correr el argoritmo clock M\n");
    return -1; // Si devulve -1, significa que tengo que volver a recorrerlo
}
