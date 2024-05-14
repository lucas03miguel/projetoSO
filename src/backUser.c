//Lucas Miguel Simões Loberto 2021219107
//Simão Tadeu Ricacho Reis Moreira 2020218319

#include "backUser.h"

int main(int argc, char const *argv[]){
    if (argc != 1 && argv != NULL) {
        printf("Numero de parametros errado\n./backoffice_user\n");
        exit(-1);
    }
    arranque();
    printMenu();

    if ((fd_pipe = open(PIPE, O_WRONLY)) < 0) {
        perror("Erro ao abrir o pipe");
        limpeza();
        exit(-1);
    }
    
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
    if (signum){};
    printf(" recebido\nA terminar o programa\n");

    limpeza();
    exit(0);
}

void limpeza(){
    printf("\nA realizar limpeza\n");

    
    //
    //msgctl(glMsqId, IPC_RMID, NULL);
    //unlink(PIPE);
    //shmdt(shrmem);
    //shmctl(shmid, IPC_RMID, NULL);
    //fclose(logFile);
    //fclose(f);

    close(fd_pipe);
    sem_unlink(SEM_NAME);
    sem_close(sem);


    printf("Limpeza realizada. Saindo...\n");
}

void arranque() {
    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0777, 1);
    if (sem == SEM_FAILED) {
        printf("Só pode ter um backoffice a correr de cada vez\n");
        exit(-1);
    }

    glMsqId = msgget(123, 0777);
    if (glMsqId == -1) {
        perror("Erro ao acessar a Message Queue");
        limpeza();
        exit(-1);
    }
}

void printMenu(){
	printf("---------MENU---------\n");
	printf("1#data_stats - Apresentar estatísticas\n");
	printf("1#reset - Limpar todas as estatísticas\n");
    printf("----------------------\n");
}

int countChar(const char *str, char ch) {
    int count = 0;
    while (*str) {
        if (*str == ch)
            count++;
        str++;
    }
    return count;
}

void gerarComandos(char *comando, char *token) {
    printf("> ");
    if (fgets(comando, BUFLEN, stdin) == NULL) {
        perror("Erro ao ler o comando");
        strcpy(comando, "erro");
        return;
    }

    size_t len = strlen(comando);
    if (len > 0 && comando[len - 1] == '\n') {
        comando[len - 1] = '\0';
    }

    if (countChar(comando, '#') != 1) {
        strcpy(comando, "erro");
        return;
    }

    char *temp = strtok(comando, "#");
    if (temp == NULL) {
        strcpy(comando, "erro");
        return;
    }
    strcpy(token, temp);

    temp = strtok(NULL, "#");
    if (temp == NULL) {
        strcpy(comando, "erro");
        return;
    }
    strcpy(comando, temp);
}

void backoffice(){
    if (pthread_create(&stats_t, NULL, stats, NULL) != 0) {
        perror("Erro ao criar a thread stats");
        limpeza();
        exit(-1);
    }

    if (pthread_create(&command_t, NULL, command, NULL) != 0) {
        perror("Erro ao criar a thread stats");
        limpeza();
        exit(-1);
    }

    pthread_join(command_t, NULL);
    pthread_join(stats_t, NULL);
}

void * stats(void * arg){
    if ((int *)arg){};

    while (1) {
        if (msgrcv(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 1, 0) < 0) {
            perror("Erro ao receber mensagem");
            limpeza();
            exit(-1);
        }

        printf("\n-----------------------------------\n");
        printf("Total de pedidos de autenticação: %d\n", msgQueue.totalAuthReqs);
        printf("Total de pedidos de autenticação de música: %d\n", msgQueue.totalAuthReqsMusic);
        printf("Total de pedidos de autenticação de social: %d\n", msgQueue.totalAuthReqsSocial);
        printf("Total de pedidos de autenticação de video: %d\n", msgQueue.totalAuthReqsVideo);
        printf("Total de dados de música: %d\n", msgQueue.totalDataMusic);
        printf("Total de dados de social: %d\n", msgQueue.totalDataSocial);
        printf("Total de dados de video: %d\n", msgQueue.totalDataVideo);
        printf("-----------------------------------\n");
    }
    
    pthread_exit(NULL);
    return NULL;
}

void * command(void * arg){
    if ((int *)arg){};

    while (1) {
        char comando[BUFLEN], token[BUFLEN];
        gerarComandos(comando, token);

        if (strcmp(comando, "data_stats") == 0) {
            write(fd_pipe, comando, strlen(comando));

            msgrcv(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 2, 0);

            printf("Service\t%-12s\t%-10s\n", "Total data", "Auth Reqs");
            printf("%-6s\t%-12d\t%-10d\n", "VIDEO", msgQueue.totalDataVideo, msgQueue.totalAuthReqsVideo);
            printf("%-6s\t%-12d\t%-10d\n", "MUSIC", msgQueue.totalDataMusic, msgQueue.totalAuthReqsMusic);
            printf("%-6s\t%-12d\t%-10d\n", "SOCIAL", msgQueue.totalDataSocial, msgQueue.totalAuthReqsSocial);

        } else if (strcmp(comando, "reset") == 0) {
            write(fd_pipe, comando, strlen(comando));

        } else {
            printf("Comando inválido\n");
            continue;
        }
    }

    pthread_exit(NULL);
    return NULL;
}