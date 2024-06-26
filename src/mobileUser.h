#ifndef MOBILEUSER_H
#define MOBILEUSER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/msg.h>
#include <signal.h>


#define BUFLEN 1024
#define USER_PIPE "/tmp/USER_PIPE"

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

int PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR;
pthread_t video_t, music_t, social_t, alertas_t;
glMessageQueue msg;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
fd_set read_set;
int fd_pipe, msqid, pid_principal;
char message[BUFLEN];

void mobile();
void sigint(int signum);
void detecaoErros(int n, char *args[]);
void * video(void * args);
void * music(void * args);
void * social(void * args);
void * alertas(void * args);

#endif // MOBILEUSER_H