#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "sysMan.h"

#define BUFLEN 1024

int N_USERS, N_SLOTS, AUTH_SERVERS_MAX, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT;
FILE *logFile;

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Numero de parametros errado\n./5g_auth_platform {config-file}\n");
        exit(-1);
    }

    arranque(argv[1]);
    
    while (1) {
        printf("N_USERS: %d\nN_SLOTS: %d\nAUTH_SERVERS_MAX: %d\nAUTH_PROC_TIME: %d\nMAX_VIDEO_WAIT: %d\nMAX_OTHERS_WAIT: %d\n\n", N_USERS, N_SLOTS, AUTH_SERVERS_MAX, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT);
        sleep(2);
    }

    return 0;
}

void arranque(char *argv){
    FILE *f;
    int invalido = 0;
    char message[BUFLEN];
    char filename[BUFLEN] = "../files/";
    strcat(filename, argv);

    logFile = fopen("../files/log.txt", "w");
    if (logFile == NULL){
        printf("Erro ao abrir o ficheiro log\n");
        sprintf(message, "ERROR ao abrir o ficheiro log");
        escreverLog(message);
        //sigint(0);
        fclose(logFile);
        exit(-1);
    }
    escreverLog("5G_AUTH_PLATFORM SIMULATOR STARTING");

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erro ao abrir o ficheiro %s.\n", argv);
        sprintf(message, "ERROR ao abrir o ficheiro %s", argv);
        escreverLog(message);
        //sigint(0);
        fclose(logFile);
        exit(-1);
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    if(size == 0){
        printf("Erro: o %s ficheiro está vazio.\n", argv);
        sprintf(message, "ERROR: o ficheiro %s está vazio", argv);
        escreverLog(message);
        //sigint(0);
        fclose(logFile);
        exit(-1);
    }
    fseek(f, 0, SEEK_SET);

    
    char linhas[6][BUFLEN];

    fgets(linhas[0], BUFLEN, f);
    N_USERS = atoi(linhas[0]);
    if (N_USERS < 1){
        printf("Erro: o número de utilizadores tem de ser maior que 0.\n");
        sprintf(message, "ERROR: o número de utilizadores tem de ser maior que 0");
        escreverLog(argv);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }

    fgets(linhas[1], BUFLEN, f);
    N_SLOTS = atoi(linhas[1]);
    for (int i = 0; linhas[1][i]; i++) {
        if (isalpha(linhas[1][i])){
            invalido = 1;
            break;
        }
    }
    if (N_SLOTS < 0 || invalido){
        printf("Erro: o número de slots nas filas tem de ser maior ou igual que 0.\n");
        sprintf(message, "ERROR: o número de slots tem de ser maior que 0");
        escreverLog(message);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }

    fgets(linhas[2], BUFLEN, f);
    AUTH_SERVERS_MAX = atoi(linhas[2]);
    if (AUTH_SERVERS_MAX < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        sprintf(message, "ERROR: o número de servidores de autorização tem de ser maior que 0");
        escreverLog(message);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }
    
    fgets(linhas[3], BUFLEN, f);
    AUTH_PROC_TIME = atoi(linhas[3]);
    if (AUTH_PROC_TIME < 1){
        printf("Erro: o tempo de processamento dos servidores de autorização tem de ser maior que 0.\n");
        sprintf(message, "ERROR: o tempo de processamento dos servidores de autorização tem de ser maior que 0");
        escreverLog(message);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }
    
    fgets(linhas[4], BUFLEN, f);
    MAX_VIDEO_WAIT = atoi(linhas[4]);
    if (MAX_VIDEO_WAIT < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        sprintf(message, "ERROR: o número de servidores de autorização tem de ser maior que 0");
        escreverLog(message);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }

    fgets(linhas[5], BUFLEN, f);
    MAX_OTHERS_WAIT = atoi(linhas[5]);
    if (MAX_OTHERS_WAIT < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        sprintf(message, "ERROR: o número de servidores de autorização tem de ser maior que 0");
        escreverLog(message);
        fclose(f);
        fclose(logFile);
        exit(-1);
    }

    if (fork() == 0) {
        escreverLog("PROCESS AUTHORIZATION_REQUEST_MANAGER CREATED");
        authorizationRequestManager();
        exit(0);
    }
    if (fork() == 0) {
        escreverLog("PROCESS MONITOR_ENGINE CREATED");
        monitorEngine();
        exit(0);
    }
    





    fclose(f);
}

void escreverLog(char *message){
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);
    fprintf(logFile, "%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
    
    fflush(stdout);
    fflush(logFile);
}

void authorizationRequestManager() {
    //TODO: completar
}

void monitorEngine() {
    //TODO: completar
}