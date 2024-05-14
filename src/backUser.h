#ifndef BACKUSER_H
#define BACKUSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/msg.h>

#define BUFLEN 1024
#define PIPE "/tmp/BACK_PIPE"
#define SEM_NAME "/sem_bloqueante"

typedef struct {
    long type; // 1 - estatistica periodicas; 2 - estatisticas pedidas; 2 - alerta; pid - user
    int sucesso; // 0 - falha; 1 - sucesso
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
    int totalAuthReqs;
    
} glMessageQueue;

int glMsqId;
pthread_t stats_t, command_t;
sem_t *sem;
glMessageQueue msgQueue;
int fd_pipe;
char message[BUFLEN];

//void escreverLog(char *message);
void arranque();
void sigint(int signum);
void limpeza();
void printMenu();
void gerarComandos(char *comando, char *token);
void backoffice();
void * stats(void *arg);
void * command(void *arg);

#endif // BACKUSER_H