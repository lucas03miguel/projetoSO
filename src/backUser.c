#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFLEN 1024

FILE *logFile;

void escreverLog(FILE *f, char *message);
void printMenu();
void gerarComandos(char *comando);

int main(int argc, char const *argv[]){
    if (argc != 1) {
        printf("Numero de parametros errado\n./backoffice_user\n");
        exit(-1);
    }
    printMenu();

    while (1) {
        char comando[BUFLEN];
        gerarComandos(comando);

        if (strcmp(comando, "data_stats") == 0) {
            printf("Comando certo\n");

        } else if (strcmp(comando, "reset") == 0) {
            printf("Comando certo\n");

        } else {
            printf("Comando inválido\n");
            continue;
        }
    }


    return 0;
}

void printMenu(){
	printf("---------MENU---------\n");
	printf("data_stats - Apresentar estatísticas\n");
	printf("reset - Limpar todas as estatísticas\n");
    printf("----------------------\n");
}

void gerarComandos(char *comando) {
    printf("> 1#");
    fgets(comando, BUFLEN, stdin);
    comando[strlen(comando) - 1] = '\0';
}

void escreverLog(FILE *f, char *message){
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);
    fprintf(f, "%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
    
    fflush(stdout);
    fflush(f);
}
