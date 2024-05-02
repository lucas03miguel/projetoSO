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
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>


//void escreverLog(char *message);
void sigint(int signum);
void limpeza();
void printMenu();
void gerarComandos(char *comando);
void backoffice();

#endif // BACKUSER_H