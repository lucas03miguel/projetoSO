#ifndef MOBILEUSER_H
#define MOBILEUSER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFLEN 1024

int PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR;

void mobile();
//void escreverLog(char *message);
void detecaoErros(int n, char *args[]);

#endif // MOBILEUSER_H