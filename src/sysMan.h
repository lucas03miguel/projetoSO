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

#define BUFLEN 1024
#define BACK_PIPE "/tmp/BACK_PIPE"
#define USER_PIPE "/tmp/USER_PIPE"

typedef struct {
    int plafondInicial;
    int plafondAtual;
    //int pedidosMax;
    int pedidosAtual;
    int id_user;
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
    int pid;
    int disponivel; // 0 - ocupado; 1 - disponivel
    int fd[2];
} AuthorizationEnginePipes;

typedef struct {
    int n_users;
	MobileUser *mobileUsers;
    Stats *stats;
    AuthorizationEnginePipes *authEnginePipes;

    pthread_mutex_t mutex_mem, mutex_log, mutex_engine_free, mutex_more_engines;
    pthread_cond_t cond_engine_free, cond_more_engines;
} MemStruct;

typedef struct {
    long type; // 1 - estatistica; 2 - alerta; pid - user
    int sucesso; // 0 - falha; 1 - sucesso
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
} glMessageQueue;

typedef struct {
    //int servico; // 0 - registo; 1 - video; 2 - music; 3 - social; -1 - null
    int dados_reservar;
    int id;
    time_t enqueued_time; // tempo em que foi inserido na fila
} Video_Streaming_Queue;

typedef struct node_video{
    Video_Streaming_Queue vsq;
    struct node_video *next;
} Node_video;

typedef struct {
    int servico; // 0 - registo; 1 - music; 2 - social; -1 - null
    int dados_reservar;
    int id;
    time_t enqueued_time; // tempo em que foi inserido na fila
} Other_Services_Queue;

typedef struct node_other{
    Other_Services_Queue osq;
    struct node_other *next;
} Node_other;

struct enviar_pipe {
    int servico; // 0 - registo; 1 - music; 2 - social; 3 - video; -1 - null
    int dados_reservar;
    int id;
};


char message[BUFLEN];
int N_MAX_USERS, N_SLOTS, AUTH_SERVERS_INIT, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT;
int shmid, fd_back_pipe, fd_user_pipe, glMsqId, enginesCounter = 0, engineExtra = 0;
fd_set read_set;
pid_t pid_principal;
MemStruct *shrmem;
glMessageQueue msgQueue;
Node_video *video_queue;
Node_other *other_queue;
pthread_t receiver_t, sender_t, stats_t, alert_t;
pthread_mutex_t mutex_queues = PTHREAD_MUTEX_INITIALIZER, mutex_auth_request = PTHREAD_MUTEX_INITIALIZER, mutex_auth_engine = PTHREAD_MUTEX_INITIALIZER, mutex_monitor = PTHREAD_MUTEX_INITIALIZER, mutex_log = PTHREAD_MUTEX_INITIALIZER, mutex_stats = PTHREAD_MUTEX_INITIALIZER, mutex_alert = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
sem_t *sem_monitor, *sem_auth_engine, *sem_auth_request, *sem_sys_manager;
FILE *logFile, *f;

void arranque(char *filename);
void terminar();
void limpeza();
void sigint(int signum);
void escreverLog(char *message);

void enqueue_other(Node_other **head, Other_Services_Queue osq);
Other_Services_Queue dequeue_other(Node_other **head);
void init_other_queue(Node_other **head);
void destroy_other_queue(Node_other **head);

void enqueue_video(Node_video **head, Video_Streaming_Queue vsq);
Video_Streaming_Queue dequeue_video(Node_video **head);
void init_video_queue(Node_video **head);
void destroy_video_queue(Node_video **head);

void authorizationRequestManager();
void authorizationEngine(int id);
void monitorEngine();
void * receiver(void *arg);
void * sender(void *arg);
void * gerarEstatisticas(void *arg);
void * gerarAlertas(void *arg);

#endif // SYSMAN_H