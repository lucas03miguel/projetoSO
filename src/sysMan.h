#ifndef SYSMAN_H
#define SYSMAN_H

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
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>

typedef struct {
    int plafondInicial;
    int plafondAtual;
    int pedidosMax;
    int pedidosAtual;
    int id;
} MobileUser;

typedef struct {
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
} Stats;

typedef struct {
	MobileUser *mobileUsers;
    Stats *stats;
    
} MemStruct;

void arranque(char *filename);
void sigint(int signum);
void limpeza();
void escreverLog(char *message);
void authorizationRequestManager();
void monitorEngine();
void * receiver(void *arg);
void * sender(void *arg);

#endif // SYSMAN_H