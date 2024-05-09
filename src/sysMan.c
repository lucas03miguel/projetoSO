//Lucas Miguel Simões Loberto 2021219107
//Simão Tadeu Ricacho Reis Moreira 2020218319
#include "sysMan.h"


int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Numero de parametros errado\n./5g_auth_platform {config-file}\n");
        exit(-1);
    }
    pid_principal = getpid();
    signal(SIGINT, sigint);

    arranque(argv[1]);

    /*
    printf("oiiiiiiiiiiiiiii\n");
    while (1) {
        printf("N_USERS: %d\nN_SLOTS: %d\nAUTH_SERVERS_MAX: %d\nAUTH_PROC_TIME: %d\nMAX_VIDEO_WAIT: %d\nMAX_OTHERS_WAIT: %d\n\n", N_USERS, N_SLOTS, AUTH_SERVERS_MAX, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT);
        sleep(2);
    }
    */
    printf("vou sair\n");
    return 0;
}

void arranque(char *argv){
    int invalido = 0;
    char message[BUFLEN];
    char filename[BUFLEN] = "../files/";
    strcat(filename, argv);

    logFile = fopen("../files/log.txt", "w");
    if (logFile == NULL){
        perror("Erro ao abrir o ficheiro log");
        limpeza();
        exit(-1);
    }
    escreverLog("5G_AUTH_PLATFORM SIMULATOR STARTING");

    f = fopen(filename, "r");
    if (f == NULL) {
        sprintf(message, "Erro ao abrir o ficheiro %s", argv);
        perror(message);
        sprintf(message, "ERROR ao abrir o ficheiro %s", argv);
        escreverLog(message);
        fclose(logFile);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    if(fileSize == 0){
        printf("Erro: o %s ficheiro está vazio.\n", argv);
        sprintf(message, "ERROR: o ficheiro %s está vazio", argv);
        escreverLog(message);
        limpeza();
        exit(-1);
    }
    fseek(f, 0, SEEK_SET);

    char linhas[6][BUFLEN];
    for (int i = 0; i < 6; i++) {
        fgets(linhas[i], BUFLEN, f);
        for (int j = 0; linhas[i][j] != '\n' && linhas[i][j] != '\0'; j++) {
            if (!isdigit(linhas[i][j])){
                invalido = 1;
                strcpy(linhas[i], "-1");
                break;
            }
        }
        if (invalido) break;
    }

    N_USERS = atoi(linhas[0]);
    if (N_USERS < 1){
        printf("Erro: o número de utilizadores tem de ser maior que 0 e inteiro.\n");
        escreverLog("ERROR: número de utilizadores inválido");
        limpeza();
        exit(-1);
    }

    N_SLOTS = atoi(linhas[1]);
    if (N_SLOTS < 0){
        printf("Erro: o número de slots nas filas tem de ser maior ou igual que 0 e inteiro.\n");
        escreverLog("ERROR: número de slots inválido");
        limpeza();
        exit(-1);
    }

    AUTH_SERVERS_INIT = atoi(linhas[2]);
    if (AUTH_SERVERS_INIT < 1){
        printf("Erro: o número de Authorization Engines tem de ser maior ou igual que 1 e inteiro.\n");
        escreverLog("ERROR: número de Authorization Engines inválido");
        limpeza();
        exit(-1);
    }
    
    AUTH_PROC_TIME = atoi(linhas[3]);
    if (AUTH_PROC_TIME < 0){
        printf("Erro: o tempo de processamento dos Authorization Engine é inválido e inteiro.\n");
        escreverLog("ERROR: tempo de processamento dos Authorization Engine inválido");
        limpeza();
        exit(-1);
    }
    
    MAX_VIDEO_WAIT = atoi(linhas[4]);
    if (MAX_VIDEO_WAIT < 1){
        printf("Erro: o tempo que os pedidos de autorização do serviço de vídeo podem aguardar tem de ser maior ou igual que 1 e inteiro.\n");
        escreverLog("ERROR: tempo que os pedidos de autorização do serviço de vídeo podem aguardar inválido");
        limpeza();
        exit(-1);
    }

    MAX_OTHERS_WAIT = atoi(linhas[5]);
    if (MAX_OTHERS_WAIT < 1){
        printf("Erro: o tempo que os outros pedidos de autorização podem aguardar tem de ser maior ou igual que 1 e inteiro.\n");
        escreverLog("ERROR: tempo que os outros pedidos de autorização podem aguardar inválido");
        limpeza();
        exit(-1);
    }

    int size = N_USERS * sizeof(MobileUser) + sizeof(Stats) + sizeof(MemStruct); 
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT|0777);
    if (shmid == -1) {
        perror("Erro ao criar a memória partilhada");
        escreverLog("ERROR: não foi possível criar a memória partilhada");
        limpeza();
        exit(-1);
    }

    shrmem = (MemStruct *) shmat(shmid, NULL, 0);
    if (shrmem == (MemStruct *) -1) {
        perror("Erro ao aceder à memória partilhada");
        escreverLog("ERROR: não foi possível aceder à memória partilhada");
        limpeza();
        exit(-1);
    }

    shrmem->mobileUsers = (MobileUser *)((void *)shrmem + sizeof(MobileUser));
    shrmem->stats = (Stats *)((void *)shrmem + N_USERS * sizeof(MobileUser) + sizeof(MemStruct));

    glMsqId = msgget(123, IPC_CREAT | 0700);


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
    
    
    /*
    sem = sem_open("SEM", O_CREAT | O_EXCL, 0700, 1);
    if(sem == SEM_FAILED){
        printf("Erro ao criar o semaforo\n");
        escreverLog("ERROR: criação do semáforo falhou");
        sigint(0);
        exit(-1);
    }
    */
    

    for(int i = 0; i < 2; ++i)
    	wait(NULL);

    //fclose(f);
}

void sigint(int signum){
    if (signum){};

    if (getpid() == pid_principal) {
        printf(" recebido\nA terminar o programa\n");
        escreverLog("SIGNAL SIGINT RECEIVED. TERMINATING PROGRAM");

        limpeza();
    }
    exit(0);
}

void limpeza(){
    printf("\nA realizar limpeza\n");
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex_mem);
    pthread_mutex_destroy(&mutex_video_queue);
    pthread_mutex_destroy(&mutex_others_queue);

    free(vsq);
    free(osq);

    close(fd_back_pipe);
    close(fd_user_pipe);
    unlink(BACK_PIPE);
    unlink(USER_PIPE);
    shmdt(shrmem);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(logFile);
    fclose(f);


    printf("Limpeza realizada. Saindo...\n");
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

void authorizationEngine(int id) {
    if (id){};

}

void authorizationRequestManager() {
    vsq = (Video_Streaming_Queue *) malloc(N_SLOTS * sizeof(Video_Streaming_Queue));
    if (vsq == NULL) {
        printf("Erro ao alocar memoria para a Video_Streaming_Queue\n");
        escreverLog("Erro ao alocar memoria para a Video_Streaming_Queue");
        limpeza();
        exit(-1);
    }

    osq = (Other_Services_Queue *) malloc(N_SLOTS * sizeof(Other_Services_Queue));
    if (osq == NULL) {
        printf("Erro ao alocar memoria para a Other_Services_Queue\n");
        escreverLog("Erro ao alocar memoria para a Other_Services_Queue");
        limpeza();
        exit(-1);
    }

    if (mkfifo(BACK_PIPE, O_CREAT | O_EXCL | 0600) == -1) {
        perror("Erro ao criar o pipe BACK_PIPE");
        escreverLog("ERROR: não foi possível criar o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }

    if (mkfifo(USER_PIPE, O_CREAT | O_EXCL | 0600) == -1) {
        perror("Erro ao criar o pipe USER_PIPE");
        escreverLog("ERROR: não foi possível criar o pipe USER_PIPE");
        limpeza();
        exit(-1);
    }


    if(pthread_create(&receiver_t, NULL, receiver, NULL) == -1){
    	perror("Erro ao criar a thread Receiver");
  		escreverLog("ERROR: não foi possível criar a thread Receiver");
 
    	exit(-1);
    }
    printf("Thread Receiver criada\n");
    escreverLog("THREAD RECEIVER CREATED");
    
    if(pthread_create(&sender_t, NULL, sender, NULL) == -1){
    	perror("Erro ao criar a thread Sender\n");
  		escreverLog("ERROR: não foi possível criar a thread Sender");
 
    	exit(-1);
    }
    printf("Thread Sender criada\n");
    escreverLog("THREAD SENDER CREATED");



    //TODO: completar

    pthread_join(receiver_t, NULL);
    pthread_join(sender_t, NULL);
}

void * receiver(void *arg) {
    if((int *)arg) {};

    if ((fd_back_pipe = open(BACK_PIPE, O_RDONLY)) == -1) {
        perror("Erro ao abrir o pipe BACK_PIPE");
        escreverLog("ERROR: não foi possível abrir o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }
    if ((fd_user_pipe = open(USER_PIPE, O_RDONLY)) == -1) {
        perror("Erro ao abrir o pipe USER_PIPE");
        escreverLog("ERROR: não foi possível abrir o pipe USER_PIPE");
        limpeza();
        exit(-1);
    }

    FD_ZERO(&read_set);
    FD_SET(fd_back_pipe, &read_set);
    FD_SET(fd_user_pipe, &read_set);

    while (1) {
        if (select(fd_user_pipe + 1, &read_set, NULL, NULL, NULL) > 0) {
            if (FD_ISSET(fd_back_pipe, &read_set)) {
                //TODO: nao sei o que fazer
            }

            if (FD_ISSET(fd_user_pipe, &read_set)) {
                // opa ya, testa aí
            }
        }
    }

    pthread_exit(NULL);
    return NULL;
}

void * sender(void *arg) {
    if((int *)arg) {};
    //TODO: completar

    pthread_exit(NULL);
    return NULL;
}

void monitorEngine() {
    //TODO: completar
    pthread_t stats_t, alert_t;
    pthread_create(&stats_t, NULL, gerarEstatisticas, NULL);
    pthread_create(&alert_t, NULL, gerarAlertas, NULL);

    pthread_join(stats_t, NULL);
    pthread_join(alert_t, NULL);
}

void * gerarEstatisticas(void * arg) {
    if((int *)arg) {};

    while (1) {
        pthread_mutex_lock(&mutex_mem);
        msgQueue.type = 1;
        msgQueue.totalAuthReqsMusic = shrmem->stats->totalAuthReqsMusic;
        msgQueue.totalAuthReqsSocial = shrmem->stats->totalAuthReqsSocial;
        msgQueue.totalAuthReqsVideo = shrmem->stats->totalAuthReqsVideo;
        msgQueue.totalDataMusic = shrmem->stats->totalDataMusic;
        msgQueue.totalDataSocial = shrmem->stats->totalDataSocial;
        msgQueue.totalDataVideo = shrmem->stats->totalDataVideo;
        pthread_mutex_unlock(&mutex_mem);
        printf("Vou enviar\n");
        msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
        printf("Enviei\n");
        sleep(5);

        pthread_mutex_lock(&mutex_mem);
        shrmem->stats->totalAuthReqsMusic++;
        shrmem->stats->totalAuthReqsSocial++;
        shrmem->stats->totalAuthReqsVideo++;
        shrmem->stats->totalDataMusic++;
        shrmem->stats->totalDataSocial++;
        shrmem->stats->totalDataVideo++;
        pthread_mutex_unlock(&mutex_mem);
    }
    
    pthread_exit(NULL);
    return NULL;
}

void * gerarAlertas(void *arg) {
    if((int *)arg) {};

    while (1) {
        //pthread_mutex_lock(&mutex_mem);
        while (/*TODO: completar */1) {
            pthread_cond_wait(&cond, &mutex_mem);
        }


    }


    pthread_exit(NULL);
    return NULL;
}