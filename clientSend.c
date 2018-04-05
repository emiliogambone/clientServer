/*
    VR386763	
    Emilio Gambone
    16/06/2017
    clientSend.c
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
    if (argc <= 2) 
    {
        printf("Numero di argomenti errato\n");
        return 1;
    }
    //apro la coda
    idcoda = msgget(0x42, 0666);
    if (idcoda == -1) 
    {
        perror("clientSend");
        return 1;
    }

    char mesg[256];

    char *posizione = mesg;
    
    //ciclo per unire i vari argomenti in un unica stringa
    for (int i = 2; i < argc; i++) 
    {
        strcpy(posizione, argv[i]); //copio l'argomento i-esimo
        posizione += strlen(argv[i]); //incremento la posizione della sua lunghezza
        posizione[0] = ' '; //inserisco uno spazio
        posizione++; //incremento ancora la posizione
    }
    //inserisco un carattere nullo alla fine (sovrascrivo lo spazio)
    posizione[-1] = '\0';

    //copio il messaggio nella struttura
    int lunghezza = strlen(mesg)+1;
    messaggio.mtype = atoi(argv[1]);
    strcpy(messaggio.testo, mesg);

    //invio il messaggio
    if (msgsnd(idcoda, &messaggio, lunghezza, 0)) 
    {
        perror("clientSend");
        return 1;
    }
    return 0;
}
