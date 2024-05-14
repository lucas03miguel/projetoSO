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
#define SEM_ALERTAS "/tmp/sem_alertas"

typedef struct {
    int plafondInicial;
    int plafondAtual;
    //int pedidosMax;
    int pedidosAtual;
    int pedidosVideo;
    int pedidosMusic;
    int pedidosSocial;
    int id_user;
} MobileUser;

typedef struct {
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
    int totalAuthReqs;
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

    pthread_mutex_t mutex_mem, mutex_log, mutex_engine_free, mutex_more_engines; //, mutex;
    pthread_cond_t cond_engine_free, cond_more_engines;
    sem_t *sem_alerts;
    
} MemStruct;

typedef struct {
    long type; // 1 - estatistica periodicas; 2 - estatisticas pedidas; pid - user
    int sucesso; // 0 - falha; 1 - sucesso; 2 - alerta 0.8; 3 - alerta 0.9; 4 - alerta 1
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
    int totalAuthReqs;
    
} glMessageQueue;

typedef struct {
    int id;
    char servico[BUFLEN]; // video 
    int dados_reservar;
    time_t enqueued_time; // tempo em que foi inserido na fila
} Video_Streaming_Queue;

typedef struct node_video{
    Video_Streaming_Queue vsq;
    struct node_video *next;
} Node_video;

typedef struct {
    int id;
    char servico[BUFLEN]; // register; music; social; stats; null
    int dados_reservar;
    time_t enqueued_time; // tempo em que foi inserido na fila
} Other_Services_Queue;

typedef struct node_other{
    Other_Services_Queue osq;
    struct node_other *next;
} Node_other;

struct enviar_pipe {
    int id;
    char servico[BUFLEN]; // register; music; social; data_stats; video; null
    int dados_reservar;
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
pthread_mutex_t mutex_queues = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
FILE *logFile, *f;


void arranque(char *filename);
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