//Lucas Miguel Simões Loberto 2021219107
//Simão Tadeu Ricacho Reis Moreira 2020218319

#include "mobileUser.h"



//FILE *logFile;


int main(int argc, char *argv[]){
    signal(SIGINT, sigint);

    detecaoErros(argc, argv);


    if ((fd_pipe = open(USER_PIPE, O_WRONLY)) < 0) {
        perror("Erro ao abrir o pipe\n");
        exit(-1);
    }
    sprintf(message, "%d#%d", getpid(), PLAFOND);
    write(fd_pipe, &message, sizeof(message));

    printf("Esperando aprovação para registar...\n");
    printf("mypid: %d\n", getpid());
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
    
    pthread_mutex_destroy(&mutex);
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
                else {
                    printf("Erro: o argumento {plafond inicial} não é válido.\n");
                    //sprintf(message, "ERROR: o argumento {plafond inicial} não é válido.");
                    //escreverLog(message);
                }
                break;
            case 2:
                if (!letras) N_PEDIDOS = atoi(args[i]);
                else {
                    printf("Erro: o argumento {número de pedidos de autorização} não é válido.\n");
                    //sprintf(message, "ERROR: o argumento {número de pedidos de autorização} não é válido.");
                    //escreverLog(message);
                }
                break;
            case 3:
                if (!letras) INTERVALO_VIDEO = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo VIDEO} não é válido.\n");
                    //sprintf(message, "ERROR: o argumento {intervalo VIDEO} não é válido.");
                    //escreverLog(message);
                }
                break;
            case 4:
                if (!letras) INTERVALO_MUSIC = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo MUSIC} não é válido.\n"); 
                    //sprintf(message, "ERROR: o argumento {intervalo MUSIC} não é válido.");
                    //escreverLog(message);
                }
                break;
            case 5:
                if (!letras) INTERVALO_SOCIAL = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo SOCIAL} não é válido.\n");
                    //sprintf(message, "ERROR: o argumento {intervalo SOCIAL} não é válido.");
                    //escreverLog(message);
                }
                break;
            case 6:
                if (!letras) DADOS_RESERVAR = atoi(args[i]);
                else {
                    printf("Erro: o argumento {dados a reservar} não é válido.\n");
                    //sprintf(message, "ERROR: o argumento {dados a reservar} não é válido.");
                    //escreverLog(message);
                }
                break;
            default:
                break;
        }
        if (letras) {
            printf("./mobile_user {plafond inicial} {número de pedidos de autorização} {intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL} {dados a reservar}\n");
            //fclose(logFile);
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



    pthread_join(video_t, NULL);
    pthread_join(music_t, NULL);
    pthread_join(social_t, NULL);
    /*
    while (1) {
        printf("PLAFOND: %d\nN_PEDIDOS: %d\nINTERVALO_VIDEO: %d\nINTERVALO_MUSIC: %d\nINTERVALO_SOCIAL: %d\nDADOS_RESERVAR: %d\n\n", PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR);
        sleep(2);
    }
    
    */
}


void * video(void *args) {
    if ((int *)args){};

    printf("%d\n", getppid());
    int id = getppid();
    while (1) {
        printf("VIDEO\n");

        pthread_mutex_lock(&mutex);
        sprintf(message, "%d#VIDEO#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));
        pthread_mutex_unlock(&mutex);


        sleep(INTERVALO_VIDEO);
    }



    pthread_exit(NULL);
    return NULL;
}


void * music(void *args) {
    if ((int *)args){};

    printf("%d\n", getppid());
    int id = getppid();
    while (1) {
        pthread_mutex_lock(&mutex);
        sprintf(message, "%d#MUSIC#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));
        pthread_mutex_unlock(&mutex);

        sleep(INTERVALO_MUSIC);
    }



    pthread_exit(NULL);
    return NULL;
}


void * social(void *args) {
    if ((int *)args){};
    
    printf("%d\n", getppid());
    int id = getppid();
    while (1) {
        printf("SOCIAL\n");
        pthread_mutex_lock(&mutex);
        sprintf(message, "%d#SOCIAL#%d", id, DADOS_RESERVAR);
        write(fd_pipe, &message, sizeof(message));
        pthread_mutex_unlock(&mutex);

        sleep(INTERVALO_SOCIAL);
    }   


    pthread_exit(NULL);
    return NULL;
}