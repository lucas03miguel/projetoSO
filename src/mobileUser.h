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


#define BUFLEN 1024
#define USER_PIPE "/tmp/USER_PIPE"

int PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR;
pthread_t video_t, music_t, social_t;
fd_set read_set;
int fd_pipe, msqid;
char message[BUFLEN];

void mobile();
//void escreverLog(char *message);
void detecaoErros(int n, char *args[]);
void * video(void * args);
void * music(void * args);
void * social(void * args);

#endif // MOBILEUSER_H