//Lucas Miguel Simões Loberto 2021219107
//Simão Tadeu Ricacho Reis Moreira 2020218319

#include "mobileUser.h"

int main(int argc, char *argv[]){
    pid_principal = getpid();
    signal(SIGINT, sigint);

    detecaoErros(argc, argv);


    if ((fd_pipe = open(USER_PIPE, O_WRONLY)) < 0) {
        perror("Erro ao abrir o pipe\n");
        exit(-1);
    }
    sprintf(message, "%d#%d", getpid(), PLAFOND);
    write(fd_pipe, &message, sizeof(message));

    printf("Esperando aprovação para registar...\n");
    msqid = msgget(123, 0777);
    if (msqid == -1) {
        perror("Erro ao acessar a Message Queue");
        return -1;
    }


    msgrcv(msqid, &msg, sizeof(msg), getpid(), 0);
    if (msg.sucesso == 1)
        printf("Registado com sucesso\n");
    else if (msg.sucesso == 0){
        printf("Falha ao registar. Número de maximo de utilizadores atingido.\n");
        return -1;
    } else if (msg.sucesso == -1) {
        printf("Falha ao registar. Tempo maximo de processamento atingido.\n");
        return -1;
    }

    //while(1);
    //sleep(1);


    if (fork() == 0) {
        //escreverLog("MOBILE_USER SIMULATOR STARTING");
        mobile();
        exit(0);
    }
    wait(NULL);


    
    
    return 0;
}

void sigint(int signum) {
    if (signum){};

    if (getpid() == pid_principal) {
        printf(" recebido\nA terminar o programa\n");
        pthread_mutex_destroy(&mutex);
        
        write(fd_pipe, "exit", sizeof("exit"));
    } else if (N_PEDIDOS <= 0){
        printf("Número maximo de pedidos atingido\n");
        pthread_mutex_destroy(&mutex);
        write(fd_pipe, "exit", sizeof("exit"));
    }
    
    exit(0);
}

void detecaoErros(int n, char *args[]) {
    if (n != 7) {
        printf("Numero de parametros errado\n./mobile_user {plafond inicial} {número de pedidos de autorização} {intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL} {dados a reservar}\n");
        exit(-1);
    }

    int letras = 0;
    for (int i = 1; i < n; i++) {
        for (int j = 0; args[i][j] != '\0'; j++) {
            if (!isdigit(args[i][j])) {
                letras = 1;
                break;
            }
        }
        
        switch (i) {
            case 1:
                if (!letras) PLAFOND = atoi(args[i]);
                else printf("Erro: o argumento {plafond inicial} não é válido.\n");
                
                break;
            case 2:
                if (!letras) N_PEDIDOS = atoi(args[i]);
                else printf("Erro: o argumento {número de pedidos de autorização} não é válido.\n");
                break;
            case 3:
                if (!letras) INTERVALO_VIDEO = atoi(args[i]);
                else printf("Erro: o argumento {intervalo VIDEO} não é válido.\n");
                break;
            case 4:
                if (!letras) INTERVALO_MUSIC = atoi(args[i]);
                else printf("Erro: o argumento {intervalo MUSIC} não é válido.\n");
                break;
            case 5:
                if (!letras) INTERVALO_SOCIAL = atoi(args[i]);
                else printf("Erro: o argumento {intervalo SOCIAL} não é válido.\n");
                break;
            case 6:
                if (!letras) DADOS_RESERVAR = atoi(args[i]);
                else printf("Erro: o argumento {dados a reservar} não é válido.\n");
                break;
            default:
                break;
        }
        if (letras) {
            printf("./mobile_user {plafond inicial} {número de pedidos de autorização} {intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL} {dados a reservar}\n");
            exit(-1);
        }
    }
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

void mobile() {
    pthread_create(&video_t, NULL, video, NULL);
    pthread_create(&music_t, NULL, music, NULL);
    pthread_create(&social_t, NULL, social, NULL);
    pthread_create(&alertas_t, NULL, alertas, NULL);



    pthread_join(video_t, NULL);
    pthread_join(music_t, NULL);
    pthread_join(social_t, NULL);
}


void * video(void *args) {
    if ((int *)args){};

    int id = getppid();
    while (1) {
        printf("VIDEO REQUEST\n");

        pthread_mutex_lock(&mutex);
        --N_PEDIDOS;

        sprintf(message, "%d#VIDEO#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));

        if (N_PEDIDOS <= 0)
            sigint(2);
        pthread_mutex_unlock(&mutex);

        sleep(INTERVALO_VIDEO);
    }

    pthread_exit(NULL);
    return NULL;
}


void * music(void *args) {
    if ((int *)args){};

    int id = getppid();
    while (1) {
        printf("MUSIC REQUEST\n");

        pthread_mutex_lock(&mutex);
        --N_PEDIDOS;
        
        sprintf(message, "%d#MUSIC#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));

        if (N_PEDIDOS <= 0) 
            sigint(2);
        pthread_mutex_unlock(&mutex);

        sleep(INTERVALO_MUSIC);
    }

    pthread_exit(NULL);
    return NULL;
}


void * social(void *args) {
    if ((int *)args){};
    
    int id = getppid();
    while (1) {
        printf("SOCIAL REQUEST\n");

        pthread_mutex_lock(&mutex);
        --N_PEDIDOS;

        sprintf(message, "%d#SOCIAL#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));

        if (N_PEDIDOS <= 0) 
            sigint(2);
        pthread_mutex_unlock(&mutex);

        sleep(INTERVALO_SOCIAL);
    }   

    pthread_exit(NULL);
    return NULL;
}

void * alertas(void *args) {
    if ((int *)args){};

    int id = getppid();
    while (1) {
        msgrcv(msqid, &msg, sizeof(msg), id, 0);
        if (msg.sucesso == 2) 
            printf("\n-----------ALERTA----------\n80%% atingido\n---------------------------\n");
        else if (msg.sucesso == 3)
            printf("\n-----------ALERTA----------\n90%% atingido\n---------------------------\n");
        else if (msg.sucesso == 4) {
            printf("\n-----------ALERTA----------\n100%% atingido\n---------------------------\n\n");
            printf("A terminar o programa\n");
            sigint(2);
        }
    }
}