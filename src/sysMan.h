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
#include <sys/msg.h>

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

typedef struct {
    long type;
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
} glMessageQueue;

int N_USERS, N_SLOTS, AUTH_SERVERS_MAX, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT;
int shmid, fd_pipe, glMsqId;
MemStruct *shrmem;
glMessageQueue msgQueue;
pthread_t receiver_t, sender_t;
pthread_mutex_t mutex_mem = PTHREAD_MUTEX_INITIALIZER, mutex_video_queue = PTHREAD_MUTEX_INITIALIZER, mutex_others_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;
sem_t *sem_monitor, *sem_auth_engine, *sem_auth_request, *sem_sys_manager;
FILE *logFile, *f;

void arranque(char *filename);
void sigint(int signum);
void limpeza();
void escreverLog(char *message);
void authorizationRequestManager();
void monitorEngine();
void * receiver(void *arg);
void * sender(void *arg);

#endif // SYSMAN_H