//Lucas Miguel Simões Loberto 2021219107
//Simão Tadeu Ricacho Reis Moreira 2020218319

#include "backUser.h"

#define BUFLEN 1024
#define PIPE "../files/BACK_PIPE"

//FILE *logFile;


int main(int argc, char const *argv[]){
    if (argc != 1) {
        printf("Numero de parametros errado\n./backoffice_user\n");
        exit(-1);
    }
    arranque();
    printMenu();
    
    if (fork() == 0) {
        signal(SIGINT, sigint);
        //escreverLog("BACKOFFICE_USER SIMULATOR STARTING");
        backoffice();
        exit(0);
    }
    wait(NULL);

    return 0;
}

void sigint(int signum){
    printf(" recebido\nA terminar o programa\n");

    limpeza();
    exit(0);
}

void limpeza(){
    printf("\nA realizar limpeza\n");

    unlink(PIPE);
    msgctl(glMsqId, IPC_RMID, NULL);
    
    //close(fd_pipe);
    //shmdt(shrmem);
    //shmctl(shmid, IPC_RMID, NULL);
    //fclose(logFile);
    //fclose(f);

    printf("Limpeza realizada. Saindo...\n");
}

void arranque() {
    if (mkfifo(PIPE, O_CREAT | O_EXCL | 0600) == -1) {
        perror("Erro ao criar o pipe");
        //escreverLog("ERROR: não foi possível criar o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }

    glMsqId = msgget(123, IPC_CREAT | 0777);
}

void printMenu(){
	printf("---------MENU---------\n");
	printf("ID#data_stats - Apresentar estatísticas\n");
	printf("ID#reset - Limpar todas as estatísticas\n");
    printf("----------------------\n");
    //logFile = fopen("../files/log.txt", "a");
}

void gerarComandos(char *comando, char *token) {
    printf("> ");
    fgets(comando, BUFLEN, stdin);
    comando[strlen(comando) - 1] = '\0';

    strtok(comando, "#");
    strcpy(token, comando);
    strcpy(comando, strtok(NULL, "#"));
}

/*
void escreverLog(char *message){
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);
    fprintf(logFile, "%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
    
    fflush(stdout);
    fflush(logFile);
}
*/

void backoffice(){
    //TODO: Implementar
    if (pthread_create(&stats_t, NULL, stats, NULL) != 0) {
        perror("Erro ao criar a thread stats");
        //escreverLog("ERROR: não foi possível criar a thread stats");
        limpeza();
        exit(-1);
    }

    if (pthread_create(&command_t, NULL, command, NULL) != 0) {
        perror("Erro ao criar a thread stats");
        //escreverLog("ERROR: não foi possível criar a thread stats");
        limpeza();
        exit(-1);
    }

    pthread_join(command_t, NULL);
    pthread_join(stats_t, NULL);
}

void * stats(void * arg){
    while (1) {
        printf("Recebi stats\n");
        msgrcv(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0, 0);
        printf("oi\n");
        printf("Total de pedidos de autenticação de música: %d\n", msgQueue.totalAuthReqsMusic);
        printf("Total de pedidos de autenticação de social: %d\n", msgQueue.totalAuthReqsSocial);
        printf("Total de pedidos de autenticação de video: %d\n", msgQueue.totalAuthReqsVideo);
        printf("Total de dados de música: %d\n", msgQueue.totalDataMusic);
        printf("Total de dados de social: %d\n", msgQueue.totalDataSocial);
        printf("Total de dados de video: %d\n", msgQueue.totalDataVideo);
        
    }
    

    pthread_exit(NULL);
    return NULL;
}

void * command(void * arg){
    while (1) {
        char comando[BUFLEN], token[BUFLEN];
        gerarComandos(comando, token);

        if (strcmp(comando, "data_stats") == 0) {
            printf("Comando certo\n");

        } else if (strcmp(comando, "reset") == 0) {
            printf("Comando certo\n");

        } else {
            printf("Comando inválido\n");
            continue;
        }
    }



    pthread_exit(NULL);
    return NULL;
}