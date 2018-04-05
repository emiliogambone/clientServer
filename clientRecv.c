/*
    VR386763
    Emilio Gambone
    16/06/2017
    clientRecv.c
*/
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//dimensione massima del messaggio
#define DIMMAX 256

//struttura che contiene il messaggio e l'mtype da inviare o ricevere
typedef struct __msg 
{
    long mtype;
    char testo[DIMMAX];
} msg;

//id della coda
int idcoda;

//struttura contenente il messaggio da inviare
msg messaggio;

int main(int argc, char* argv[]) 
{
    	
    if (argc != 2) 
    {
        printf("Numero di argomenti errato\n");
        return 1;
    }

    //apro la coda
    idcoda = msgget(0x42, 0666);
    if (idcoda == -1) 
    {
        perror("clientRecv");
        return 1;
    }

    //converto il primo argomento da stringa ad intero
    int mtype = atoi(argv[1]);
    if (mtype <=1) 
    {
        printf("mtype non valido\n");
        return 1;
    }

    //ricevo il messaggio
    if (msgrcv(idcoda, &messaggio, 256, mtype, 0) == -1) 
    {
        perror("clientSend");
        return 1;
    }

    //stampo il messaggio
    printf("Messaggio: %s\n", messaggio.testo);

    return 0;

}
