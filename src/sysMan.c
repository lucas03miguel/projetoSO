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

    N_MAX_USERS = atoi(linhas[0]);
    if (N_MAX_USERS < 1){
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

    int size = N_MAX_USERS * sizeof(MobileUser) + sizeof(Stats) + sizeof(MemStruct); 
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
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
    shrmem->stats = (Stats *)((void *)shrmem + N_MAX_USERS * sizeof(MobileUser) + sizeof(MemStruct));

    glMsqId = msgget(123, IPC_CREAT | 0777);
    printf("id: %d\n", glMsqId);


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
        
        terminar();
        limpeza();
    }
    exit(0);
}

void terminar() {
    escreverLog("5G_AUTH_PLATFORM SIMULATOR WAITING FOR LAST TASKS TO FINISH");

    //TODO: faz la essa shit

}

void limpeza(){
    printf("\nA realizar limpeza\n");
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond_more_engines);
    pthread_mutex_destroy(&mutex_mem);
    pthread_mutex_destroy(&mutex_queues);
    pthread_mutex_destroy(&mutex_more_engines);

    destroy_other_queue(&other_queue);
    destroy_video_queue(&video_queue);

    msgctl(glMsqId, IPC_RMID, NULL);

    close(fd_back_pipe);
    close(fd_user_pipe);
    for (int i = 0; i < AUTH_SERVERS_INIT; i++) {
        printf("oiiiii\n");
        close(fd_pipes_auth[i][0]);
        printf("oiii\n");
        close(fd_pipes_auth[i][1]);
        printf("oi\n");
        free(fd_pipes_auth[i]);
        printf("o\n");
    }
    free(fd_pipes_auth);

    unlink(BACK_PIPE);
    unlink(USER_PIPE);
    shmdt(shrmem);
    shmctl(shmid, IPC_RMID, NULL);


    escreverLog("5G_AUTH_PLATFORM SIMULATOR CLOSING");
    fclose(f);
    fclose(logFile);
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

void enqueue_other(Node_other **head, Other_Services_Queue other) {
    Node_other *newNode = malloc(sizeof(Node_other));
    if (newNode == NULL) {
        perror("Erro ao alocar memoria para o novo no da fila other");
        escreverLog("ERROR: nao foi possivel alocar memoria para o novo no da fila other");
        return;
    }
    other.enqueued_time = time(NULL);
    newNode->osq = other;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    } 
    Node_other *current = *head;
    while (current->next != NULL)
        current = current->next;
    current->next = newNode;
}

Other_Services_Queue dequeue_other(Node_other **head) {
    if (*head == NULL)
        return (Other_Services_Queue){-1, -1, -1, time(NULL)};

    Node_other *temp = *head;
    Other_Services_Queue item = temp->osq;
    *head = (*head)->next;
    free(temp);
    return item;
}

void init_other_queue(Node_other **head) {
    *head = NULL;
}


void destroy_other_queue(Node_other **head) {
    Node_other *current = *head;
    Node_other *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

void enqueue_video(Node_video **head, Video_Streaming_Queue video) {
    Node_video *newNode = malloc(sizeof(Node_video));
    if (newNode == NULL) {
        perror("Erro ao alocar memoria para o novo no da fila other");
        escreverLog("ERROR: nao foi possivel alocar memoria para o novo no da fila other");
        return;
    }
    video.enqueued_time = time(NULL);
    newNode->vsq = video;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    } 
    Node_video *current = *head;
    while (current->next != NULL)
        current = current->next;
    current->next = newNode;
}

Video_Streaming_Queue dequeue_video(Node_video **head) {
    if (*head == NULL)
        return (Video_Streaming_Queue){"null", -1, -1, time(NULL)};

    Node_video *temp = *head;
    Video_Streaming_Queue item = temp->vsq;
    *head = (*head)->next;
    free(temp);
    return item;
}

void init_video_queue(Node_video **head) {
    *head = NULL;
}

void destroy_video_queue(Node_video **head) {
    Node_video *current = *head;
    Node_video *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}


void authorizationEngine(int id) {
    if (id){};

}

void authorizationRequestManager() {
    init_other_queue(&other_queue);
    init_video_queue(&video_queue);

    if ((mkfifo(BACK_PIPE, O_CREAT | O_EXCL | 0600) < 0) && (errno != EEXIST)) {
        perror("Erro ao criar o pipe BACK_PIPE");
        escreverLog("ERROR: não foi possível criar o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }

    if ((mkfifo(USER_PIPE, O_CREAT | O_EXCL | 0600) < 0) && (errno != EEXIST)) {
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

    fd_pipes_auth = malloc(AUTH_SERVERS_INIT * sizeof(int *));
    if (fd_pipes_auth == NULL) {
        perror("Erro ao alocar memória para os file descriptors dos authorization engines");
        escreverLog("ERROR: nao foi possivel alocar memória para os file descriptors dos authorization engines");
        limpeza();
        exit(-1);
    }
    for (int i = 0; i < AUTH_SERVERS_INIT; i++) {
        fd_pipes_auth[i] = malloc(2 * sizeof(int));
        if (fd_pipes_auth[i] == NULL) {
            perror("Erro ao alocar memoria para os canais dos file descriptors");
            escreverLog("ERROR: nao foi possivel alocar memoria para os canais dos file descriptors");
            limpeza();
            exit(-1);
        }
        if (pipe(fd_pipes_auth[i]) == -1) {
            perror("Erro ao criar o pipe");
            escreverLog("ERROR: nao foi possivel criar o pipe");
            limpeza();
            exit(-1);
        }
        if (fork() == 0) {
            authorizationEngine(i);
            exit(0);
        }
    }
    for (int i = 0; i < AUTH_SERVERS_INIT; i++)
    {
        wait(NULL);
    }
    printf("atao pa\n");
    
    
    // meter loop com variavel e tals
    while (1) {
        while (/* coiso e tals */1) {
            pthread_cond_wait(&cond_more_engines, &mutex_more_engines);
        }
        



    }


    //TODO: completar

    pthread_join(receiver_t, NULL);
    pthread_join(sender_t, NULL);
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

void * receiver(void *arg) {
    if((int *)arg) {};

    if ((fd_user_pipe = open(USER_PIPE, O_RDWR)) < 0) {
        perror("Erro ao abrir o pipe USER_PIPE");
        escreverLog("ERROR: não foi possível abrir o pipe USER_PIPE");
        limpeza();
        exit(-1);
    }
    
    if ((fd_back_pipe = open(BACK_PIPE, O_RDWR)) < 0) {
        perror("Erro ao abrir o pipe BACK_PIPE");
        escreverLog("ERROR: não foi possível abrir o pipe BACK_PIPE");
        limpeza();
        exit(-1);
    }
    
    while (1) {
        FD_ZERO(&read_set);
        FD_SET(fd_user_pipe, &read_set);
        FD_SET(fd_back_pipe, &read_set);

        if (select(fd_back_pipe + 1, &read_set, NULL, NULL, NULL) > 0) {
            if (FD_ISSET(fd_back_pipe, &read_set)) {
                //TODO: nao sei o que fazer
            }

            if (FD_ISSET(fd_user_pipe, &read_set)) {
                printf("Foi do user\n");
                // opa ya, testa aí

                char buffer[BUFLEN];
                int number_chars = read(fd_user_pipe, buffer, BUFLEN);
                buffer[number_chars - 1] = '\0';

                int quant_hash = countChar(buffer, '#');
                printf("hashtagggg: %d\n", quant_hash);

                //mensagem de registo
                if (quant_hash == 1) {
                    int id = atoi(strtok(buffer, "#"));
                    int plafond = atoi(strtok(NULL, "#"));

                    pthread_mutex_lock(&mutex_queues);

                    Other_Services_Queue aux;
                    aux.id = id;
                    aux.servico = 0;
                    aux.dados_reservar = plafond;
                    enqueue_other(&other_queue, aux);

                    pthread_cond_signal(&cond);
                    pthread_mutex_unlock(&mutex_queues);


                    /*
                    //pthread_mutex_lock(&mutex_mem);
                    if (shrmem->n_users < N_MAX_USERS) {
                        printf("Oiiiiiiiiii %d\n", shrmem->n_users);
                        pthread_mutex_unlock(&mutex_mem);

                        
                        
                        shrmem->mobileUsers[shrmem->n_users].id_user = id;
                        shrmem->mobileUsers[shrmem->n_users].plafondInicial = plafond;
                        shrmem->mobileUsers[shrmem->n_users].plafondAtual = plafond;
                        //shrmem->mobileUsers[shrmem->n_users - 1].pedidosMax = 0;
                        shrmem->mobileUsers[shrmem->n_users].pedidosAtual = 0;
                        shrmem->n_users++;
                        pthread_mutex_unlock(&mutex_mem);
                        
                    } else {
                        printf("aiaiaiai %d\n", shrmem->n_users);
                        pthread_mutex_unlock(&mutex_mem);
                        //pthread_mutex_lock(&mutex_queues);

                        printf("Recebi: %d %d\n", id, plafond);
                        Other_Services_Queue aux;
                        aux.id = id;
                        aux.pedido = -1;	
                        strcpy(aux.teste, "Sem coiso");

                        enqueue_other(&other_queue, aux);

                        if (other_queue_is_empty(other_queue)) {
                            pthread_mutex_lock(&mutex_queues);
                            pthread_cond_signal(&cond);
                            pthread_mutex_unlock(&mutex_queues);
                            printf("Alertei... acho eu....\n");
                        }

                        //pthread_mutex_unlock(&mutex_others_queue);
                    } */

                } else if (quant_hash == 2) { // pedido de autorizacao
                    int id = atoi(strtok(buffer, "#"));
                    char servico[BUFLEN];
                    strcpy(servico, strtok(NULL, "#"));
                    int pedido = atoi(strtok(NULL, "#"));




                } 

                
                


                
            }
        }
    }
    
    pthread_exit(NULL);
    return NULL;
}

void verificar_tempo_other() {
    time_t current_time = time(NULL);
    if (other_queue != NULL) {
        double diff = difftime(current_time, other_queue->osq.enqueued_time);
        printf("Diff: %f\n", diff);
        if (diff > MAX_OTHERS_WAIT / 1000) {
            Node_other *temp = other_queue;
            other_queue = other_queue->next;
            free(temp);
            printf("Pedido de outro serviço expirado removido.\n");
            escreverLog("Pedido de outro serviço expirado removido.");
        }
    }

}

void verificar_tempo_video() {
    time_t current_time = time(NULL);
    if (video_queue != NULL) {
        if (difftime(current_time, video_queue->vsq.enqueued_time) > MAX_VIDEO_WAIT / 1000) {
            Node_video *temp = video_queue;
            video_queue = video_queue->next;
            free(temp);
            printf("Pedido de vídeo expirado removido.\n");
            escreverLog("Pedido de vídeo expirado removido.");
        }
    }
}

void * sender(void *arg) {
    if((int *)arg) {};

    //TODO: completar
    while (1) {
        printf("%d %d %d\n", (other_queue == NULL), (video_queue == NULL), (other_queue == NULL && video_queue == NULL));

        pthread_mutex_lock(&mutex_queues);
        while (other_queue == NULL && video_queue == NULL) {
            pthread_cond_wait(&cond, &mutex_queues);
        }
        pthread_mutex_unlock(&mutex_queues);

        printf("Alertaste sim senhora :)\n");

        verificar_tempo_video();
        verificar_tempo_other();

        // ver se é preciso e tals e depois aumentar e tals

        dequeue_other(&other_queue);
        sleep(5);
    }
    
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
    }
    
    pthread_exit(NULL);
    return NULL;
}

void * gerarAlertas(void *arg) {
    if((int *)arg) {};


    /*
    while (1) {
        //pthread_mutex_lock(&mutex_mem);
        while (1) {
            pthread_cond_wait(&cond, &mutex_mem);
        }


    }
    */


    pthread_exit(NULL);
    return NULL;
}

