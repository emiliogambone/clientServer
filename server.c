/*
    VR386763
    Emilio Gambone
    16/06/2017
    server.c
*/
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

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

//pid del processo figlio
int pid_figlio;

void figlio();

//funzione per gestire i segnali del processo padre
void gestisci_segnale(int segnale) 
{
    switch (segnale) 
    {
        //termino il figlio, chiudo la coda ed esco
        case SIGINT:
        case SIGUSR1: 
        {
            kill(pid_figlio, SIGINT);
            msgctl(idcoda, IPC_RMID, NULL);
            exit(0);
        }
        break;

        //esco solo se non ci sono più messaggi
        case SIGUSR2: 
        {
            struct msqid_ds buffer;
            if (msgctl(idcoda, IPC_STAT, &buffer) == -1) 
            {
                perror("server");
                return;
            }
            if (buffer.msg_qnum == 0) {
                kill(pid_figlio, SIGINT);
                msgctl(idcoda, IPC_RMID, NULL);
                exit(0);
            }
        }
        break;
    }
}

int main() 
{
    //apro la coda
    idcoda = msgget(0x42, IPC_CREAT | IPC_EXCL | 0666);
    if (idcoda == -1) {
        perror("server");
        return 1;
    }
    
    //creo il proceso figlio
    int pid = fork();
    if (pid < 0) 
    {
        printf("Errore, fork fallita.\n");
        exit(1);
    }

    if (pid == 0) 
    {
        //eseguo la funzione del processo figlio 
        figlio();
    }
    else 
    {
        pid_figlio = pid;
     
        //gestisco i segnali
        signal(SIGINT, gestisci_segnale);
        signal(SIGUSR1, gestisci_segnale);
        signal(SIGUSR2, gestisci_segnale);
        
        //loop infinito per evitare la terminazione del programma
        while (1) 
        {
            sleep(1);
        }
    }

    return 0;
}

//gestore del timeout del figlio
void gestione_sigalrm(int segnale) {
    kill(getppid(), SIGUSR1);
}

//funzione 'main' del figlio
void figlio() 
{
    //gestisco il segnale SIGALRM
    signal(SIGALRM, gestione_sigalrm);
    while (1) 
    {
        //ricevo un messaggio
        if (msgrcv(idcoda, &messaggio, DIMMAX, 1, 0) == -1) 
        {
            //ignoro l'errore EINTR, causato quando viene inviato il segnale SIGALRM
            if (errno != EINTR)
                perror("server");
        }
        else 
        {
            printf("Comando: %s\n", messaggio.testo);

            //implementazione dei comandi
            if (strcmp("niceclose", messaggio.testo) == 0) 
            {
                //dico al padre di provare a terminare se non ci sono messaggi
                kill(getppid(), SIGUSR2);
            }
            else if (strcmp("freeall", messaggio.testo) == 0) 
            {
                //leggo i messaggi fino a svuotare la coda, ignorando il testo del messaggio
                int tmp = 1;
                while (tmp) {
                    if (msgrcv(idcoda, &messaggio, DIMMAX, 0, IPC_NOWAIT) < 0)
                        tmp = 0;
                }
            }
            else if (strstr(messaggio.testo, "closetime")) // controllo se nel comando c'è la sottostringa closetime 
            {
                //leggo e imposto il tempo di chiusura
                int tempo = atoi(messaggio.testo + 9);
                if (tempo >= 0)
                    alarm(tempo);
                else
                    printf("Errore, tempo di chiusura non valido\n");
            }
        }
    }
}
