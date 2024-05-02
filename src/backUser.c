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
    
    if (mkfifo(PIPE, O_CREAT | O_EXCL | 0600) == -1) {
        perror("Erro ao criar o pipe");
        //escreverLog("ERROR: não foi possível criar o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }

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

    //close(fd_pipe);
    unlink(PIPE);
    //shmdt(shrmem);
    //shmctl(shmid, IPC_RMID, NULL);
    //fclose(logFile);
    //fclose(f);

    printf("Limpeza realizada. Saindo...\n");
}

void printMenu(){
	printf("---------MENU---------\n");
	printf("data_stats - Apresentar estatísticas\n");
	printf("reset - Limpar todas as estatísticas\n");
    printf("----------------------\n");
    //logFile = fopen("../files/log.txt", "a");
}

void gerarComandos(char *comando) {
    printf("> 1#");
    fgets(comando, BUFLEN, stdin);
    comando[strlen(comando) - 1] = '\0';
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
}
