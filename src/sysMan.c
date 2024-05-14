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

    return 0;
}

void arranque(char *argv){
    int invalido = 0;
    char filename[BUFLEN] = "../files/";
    strcat(filename, argv);

    logFile = fopen("../files/log.txt", "w");
    if (logFile == NULL){
        perror("Erro ao abrir o ficheiro log");
        limpeza();
        exit(-1);
    }

    f = fopen(filename, "r");
    if (f == NULL) {
        sprintf(message, "Erro ao abrir o ficheiro %s", argv);
        perror(message);
        sprintf(message, "ERROR ao abrir o ficheiro %s", argv);
        fclose(logFile);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    if(fileSize == 0){
        printf("Erro: o %s ficheiro está vazio.\n", argv);
        sprintf(message, "ERROR: o ficheiro %s está vazio", argv);
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
        limpeza();
        exit(-1);
    }

    N_SLOTS = atoi(linhas[1]);
    if (N_SLOTS < 0){
        printf("Erro: o número de slots nas filas tem de ser maior ou igual que 0 e inteiro.\n");
        limpeza();
        exit(-1);
    }

    AUTH_SERVERS_INIT = atoi(linhas[2]);
    if (AUTH_SERVERS_INIT < 1){
        printf("Erro: o número de Authorization Engines tem de ser maior ou igual que 1 e inteiro.\n");
        limpeza();
        exit(-1);
    }
    
    AUTH_PROC_TIME = atoi(linhas[3]);
    if (AUTH_PROC_TIME < 0){
        printf("Erro: o tempo de processamento dos Authorization Engine é inválido e inteiro.\n");
        limpeza();
        exit(-1);
    }
    
    MAX_VIDEO_WAIT = atoi(linhas[4]);
    if (MAX_VIDEO_WAIT < 1){
        printf("Erro: o tempo que os pedidos de autorização do serviço de vídeo podem aguardar tem de ser maior ou igual que 1 e inteiro.\n");
        limpeza();
        exit(-1);
    }
   
    MAX_OTHERS_WAIT = atoi(linhas[5]);
    if (MAX_OTHERS_WAIT < 1){
        printf("Erro: o tempo que os outros pedidos de autorização podem aguardar tem de ser maior ou igual que 1 e inteiro.\n");
        limpeza();
        exit(-1);
    }
    
    int size = N_MAX_USERS * sizeof(MobileUser) + sizeof(Stats) + (AUTH_SERVERS_INIT + 1) * sizeof(AuthorizationEnginePipes) + sizeof(MemStruct); 
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
    if (shmid == -1) {
        perror("Erro ao criar a memória partilhada");
        limpeza();
        exit(-1);
    }
    shrmem = (MemStruct *) shmat(shmid, NULL, 0);
    if (shrmem == (MemStruct *) -1) {
        perror("Erro ao aceder à memória partilhada");
        limpeza();
        exit(-1);
    }

    void *base = (void *)shrmem;
    shrmem->mobileUsers = (MobileUser *)(base + sizeof(MemStruct));
    shrmem->stats = (Stats *)(shrmem->mobileUsers + N_MAX_USERS);
    shrmem->authEnginePipes = (AuthorizationEnginePipes *)(shrmem->stats + 1); //+1 porque é so um stat
    shrmem->n_users = 0;
    shrmem->stats->totalDataVideo = 0;
    shrmem->stats->totalAuthReqsVideo = 0;
    shrmem->stats->totalDataMusic = 0;
    shrmem->stats->totalAuthReqsMusic = 0;
    shrmem->stats->totalDataSocial = 0;
    shrmem->stats->totalAuthReqsSocial = 0;
    shrmem->stats->totalAuthReqs = 0;

    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shrmem->mutex_mem, &attr);

    pthread_mutexattr_t attr2;
    pthread_mutexattr_init(&attr2);
    pthread_mutexattr_setpshared(&attr2, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shrmem->mutex_log, &attr2);

    pthread_mutexattr_t attr3;
    pthread_mutexattr_init(&attr3);
    pthread_mutexattr_setpshared(&attr3, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shrmem->mutex_engine_free, &attr3);

    pthread_condattr_t attr4;
    pthread_condattr_init(&attr4);
    pthread_condattr_setpshared(&attr4, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shrmem->cond_engine_free, &attr4);

    pthread_condattr_t attr5;
    pthread_condattr_init(&attr5);
    pthread_condattr_setpshared(&attr5, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shrmem->cond_more_engines, &attr5);

    shrmem->sem_alerts = sem_open("sem", O_CREAT, 0777, 0);
    if (shrmem->sem_alerts == SEM_FAILED) {
        perror("Erro ao criar semáforo");
        limpeza();
        exit(-1);
    }

    escreverLog("5G_AUTH_PLATFORM SIMULATOR STARTING");


    glMsqId = msgget(123, IPC_CREAT | 0777);


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

    for(int i = 0; i < 2; ++i)
    	wait(NULL);

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
    escreverLog("5G_AUTH_PLATFORM SIMULATOR CLOSING");
    

    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&shrmem->cond_more_engines);
    pthread_cond_destroy(&shrmem->cond_engine_free);
    pthread_mutex_destroy(&shrmem->mutex_mem);
    pthread_mutex_destroy(&mutex_queues);
    pthread_mutex_destroy(&shrmem->mutex_engine_free);
    sem_close(shrmem->sem_alerts);
	sem_unlink("sem");

    destroy_other_queue(&other_queue);
    destroy_video_queue(&video_queue);

    msgctl(glMsqId, IPC_RMID, NULL);

    close(fd_back_pipe);
    close(fd_user_pipe);
    unlink(BACK_PIPE);
    unlink(USER_PIPE);
    shmdt(shrmem);
    shmctl(shmid, IPC_RMID, NULL);

    fclose(f);
    fclose(logFile);
    printf("Limpeza realizada. Saindo...\n");
}

void escreverLog(char *message){
    time_t currentTime;
    struct tm *localTime;

    pthread_mutex_lock(&shrmem->mutex_log);
    time(&currentTime);
    localTime = localtime(&currentTime);

    fprintf(logFile, "%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);

    fflush(stdout);
    fflush(logFile);
    pthread_mutex_unlock(&shrmem->mutex_log);
}

int countOccupiedSlots(int tipo) {
    int count = 0;
    if (tipo == 0) {
        Node_other *aux = other_queue;
        while (aux != NULL) {
            count++;
            aux = aux->next;
        }
    } else {
        Node_video *aux = video_queue;
        while (aux != NULL) {
            count++;
            aux = aux->next;
        }
    }
    return count;
}

void enqueue_other(Node_other **head, Other_Services_Queue other) {
    Node_other *newNode = malloc(sizeof(Node_other));
    if (newNode == NULL) {
        perror("Erro ao alocar memoria para o novo no da fila other");
        escreverLog("ERROR: nao foi possivel alocar memoria para o novo no da fila other");
        return;
    }

    int count = countOccupiedSlots(0);
    if (count >= N_SLOTS) {
        printf("Erro: Fila de outros serviços está cheia. Eliminando pedido...\n");
        escreverLog("ERROR: nao foi possivel escrever na fila de outros servicos.");
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
        return (Other_Services_Queue){-1, "null", -1, -1};

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

    int count = countOccupiedSlots(1);
    if (count >= N_SLOTS) {
        printf("Erro: Fila de outros serviços está cheia. Eliminando pedido...\n");
        escreverLog("ERROR: nao foi possivel escrever na fila de outros servicos.");
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
        return (Video_Streaming_Queue){-1, "null", -1, -1};

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

void criar_authEngine(int id) {
    pthread_mutex_lock(&shrmem->mutex_mem);
    ++enginesCounter;
    if (pipe(shrmem->authEnginePipes[id].fd) == -1) {
        pthread_mutex_unlock(&shrmem->mutex_mem);
        perror("Erro ao criar o pipe");
        escreverLog("ERROR: nao foi possivel criar o pipe");
        limpeza();
        exit(-1);
    }
    pthread_mutex_unlock(&shrmem->mutex_mem);

    if (fork() == 0) {
        pthread_mutex_lock(&shrmem->mutex_mem);
        shrmem->authEnginePipes[id].pid = getpid();
        shrmem->authEnginePipes[id].disponivel = 1;
        close(shrmem->authEnginePipes[id].fd[1]);
        pthread_mutex_unlock(&shrmem->mutex_mem);

        sprintf(message, "AUTHORIZATION_ENGINE %d STARTING", id + 1);
        escreverLog(message);
        authorizationEngine(id);
        exit(0);
    } else {
        pthread_mutex_lock(&shrmem->mutex_mem);
        close(shrmem->authEnginePipes[id].fd[0]);
        pthread_mutex_unlock(&shrmem->mutex_mem);
    
    }
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

    for (int i = 0; i < AUTH_SERVERS_INIT; i++) {
        criar_authEngine(i);
    }

    for (int i = 0; i < AUTH_SERVERS_INIT; i++) {
        wait(NULL);
    }
    

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
                char buffer[BUFLEN];
                read(fd_back_pipe, buffer, BUFLEN);

                Other_Services_Queue aux;
                aux.id = 3;
                strcpy(aux.servico, buffer);
                aux.dados_reservar = -1;

                pthread_mutex_lock(&mutex_queues);
                enqueue_other(&other_queue, aux);
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&mutex_queues);

            }

            if (FD_ISSET(fd_user_pipe, &read_set)) {
                char buffer[BUFLEN];
                int number_chars = read(fd_user_pipe, buffer, BUFLEN);
                buffer[number_chars - 1] = '\0';

                int quant_hash = countChar(buffer, '#');

                //mensagem de registo
                if (quant_hash == 0) {
                    Other_Services_Queue aux;
                    aux.id = -1;
                    strcpy(aux.servico, "exit");
                    aux.dados_reservar = -1;

                    pthread_mutex_lock(&mutex_queues);
                    enqueue_other(&other_queue, aux);
                    pthread_cond_signal(&cond);
                    pthread_mutex_unlock(&mutex_queues);
                    
                } else if (quant_hash == 1) {
                    int id = atoi(strtok(buffer, "#"));
                    int plafond = atoi(strtok(NULL, "#"));

                    Other_Services_Queue aux;
                    aux.id = id;
                    strcpy(aux.servico, "REGISTER");
                    aux.dados_reservar = plafond;

                    pthread_mutex_lock(&mutex_queues);
                    enqueue_other(&other_queue, aux);
                    pthread_cond_signal(&cond);
                    pthread_mutex_unlock(&mutex_queues);

                } else if (quant_hash == 2) { // pedido de autorizacao
                    int id = atoi(strtok(buffer, "#"));
                    char servico[BUFLEN];
                    strcpy(servico, strtok(NULL, "#"));
                    int pedido = atoi(strtok(NULL, "#"));

                    if (strcmp(servico, "VIDEO") == 0) {
                        Video_Streaming_Queue video;
                        video.id = id;
                        strcpy(video.servico, "VIDEO");
                        video.dados_reservar = pedido;

                        pthread_mutex_lock(&mutex_queues);
                        enqueue_video(&video_queue, video);
                        pthread_cond_signal(&cond);
                        pthread_mutex_unlock(&mutex_queues);

                    } else if (strcmp(servico, "MUSIC") == 0) {
                        Other_Services_Queue other;
                        other.id = id;
                        strcpy(other.servico, "MUSIC");
                        other.dados_reservar = pedido;

                        pthread_mutex_lock(&mutex_queues);
                        enqueue_other(&other_queue, other);
                        pthread_cond_signal(&cond);
                        pthread_mutex_unlock(&mutex_queues);

                    } else if (strcmp(servico, "SOCIAL") == 0) {
                        Other_Services_Queue other;
                        other.id = id;
                        strcpy(other.servico, "SOCIAL");
                        other.dados_reservar = pedido;

                        pthread_mutex_lock(&mutex_queues);
                        enqueue_other(&other_queue, other);
                        pthread_cond_signal(&cond);
                        pthread_mutex_unlock(&mutex_queues);
                    } 
                }   
            }
        }
    }
    
    pthread_exit(NULL);
    return NULL;
}

int verificar_tempo_other(Other_Services_Queue pedido_other) {
    time_t current_time = time(NULL);
    if (other_queue != NULL) {
        double diff = difftime(current_time, pedido_other.enqueued_time);
        if (diff > MAX_OTHERS_WAIT / 1000) {
            printf("Pedido de outro serviço expirado removido.\n");
            escreverLog("Pedido de outro serviço expirado removido.");
            return 1;
        }
    }
    return 0;
}

int verificar_tempo_video(Video_Streaming_Queue pedido_video) {
    time_t current_time = time(NULL);
    if (video_queue != NULL) {
        if (difftime(current_time, pedido_video.enqueued_time) > MAX_VIDEO_WAIT / 1000) {
            printf("Pedido de vídeo expirado removido.\n");
            escreverLog("Pedido de vídeo expirado removido.");
            return 1;
        }
    }
    return 0;
}

int pesquisa_engine_disponivel() {
    for (int i = 0; i < enginesCounter; i++) {
        pthread_mutex_lock(&shrmem->mutex_mem);
        if (shrmem->authEnginePipes[i].disponivel == 1) {
            pthread_mutex_unlock(&shrmem->mutex_mem);
            return i;
        }
        pthread_mutex_unlock(&shrmem->mutex_mem);
    }
    return -1;
}

void * sender(void *arg) {
    if((int *)arg) {};

    while (1) {
        pthread_mutex_lock(&mutex_queues);
        while (other_queue == NULL && video_queue == NULL) {
            pthread_cond_wait(&cond, &mutex_queues);
        }
        pthread_mutex_unlock(&mutex_queues);

        struct enviar_pipe aux;
        pthread_mutex_lock(&mutex_queues);
        if (video_queue != NULL) { // video
            Video_Streaming_Queue pedido_video = dequeue_video(&video_queue);

            strcpy(aux.servico, pedido_video.servico);
            aux.dados_reservar = pedido_video.dados_reservar;
            aux.id = pedido_video.id;

            if (verificar_tempo_video(pedido_video)) {
                pthread_mutex_unlock(&mutex_queues);
                continue;
            }

        } else { // other
            Other_Services_Queue pedido_other = dequeue_other(&other_queue);

            strcpy(aux.servico, pedido_other.servico);
            aux.dados_reservar = pedido_other.dados_reservar;
            aux.id = pedido_other.id;

            if(verificar_tempo_other(pedido_other)){
                pthread_mutex_unlock(&mutex_queues);
                if (aux.servico == 0) {
                    msgQueue.sucesso = -1;
                    msgQueue.type = aux.id;
                    msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
                }
                continue;
            }
        }
        pthread_mutex_unlock(&mutex_queues);
        

        int engine = pesquisa_engine_disponivel();
        if (engine == -1) {
            printf("Não há engines disponíveis. À espera que algum fique livre\n");
            pthread_mutex_lock(&shrmem->mutex_engine_free);
            while ((engine = pesquisa_engine_disponivel()) == -1) {
                pthread_cond_wait(&shrmem->cond_engine_free, &shrmem->mutex_engine_free);
            }
            pthread_mutex_unlock(&shrmem->mutex_engine_free);
            printf("Já há engines disponíveis %d\n", engine);
        }
        
        if (strcmp(aux.servico, "REGISTER") == 0) { // registo
            sprintf(message, "SENDER: REGISTRATION REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        } else if (strcmp(aux.servico, "VIDEO") == 0) { // video
            sprintf(message, "SENDER: VIDEO AUTHORIZATION REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        }
         else if (strcmp(aux.servico, "MUSIC") == 0) { // music
            sprintf(message, "SENDER: MUSIC AUTHORIZATION REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        } else if (strcmp(aux.servico, "SOCIAL") == 0) { // social
            sprintf(message, "SENDER: SOCIAL AUTHORIZATION REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        } else if (strcmp(aux.servico, "data_stats") == 0) { // stats
            sprintf(message, "SENDER: STATS REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        } else if (strcmp(aux.servico, "reset") == 0) { // stats
            sprintf(message, "SENDER: STATS REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);

        } else if (strcmp(aux.servico, "exit") == 0) { // exit
            sprintf(message, "SENDER: EXIT REQUEST (ID = %d) SENT FOR PROCESSING ON AUTHORIZATION_ENGINE %d", aux.id , engine);
            escreverLog(message);
        }        
        
        pthread_mutex_lock(&mutex_queues);
        write(shrmem->authEnginePipes[engine].fd[1], &aux, sizeof(aux));
        pthread_mutex_unlock(&mutex_queues);
    }
    
    pthread_exit(NULL);
    return NULL;
}


void authorizationEngine(int id) {
    struct enviar_pipe aux;
    
    ssize_t bytesRead;
    
    memset(&aux, 0, sizeof(aux));
    while ((bytesRead = read(shrmem->authEnginePipes[id].fd[0], &aux, sizeof(aux))) >= 0) {
        time_t processed_time = time(NULL);
        pthread_mutex_lock(&shrmem->mutex_mem);
        shrmem->authEnginePipes[id].disponivel = 0;
        shrmem->stats->totalAuthReqs++;
        pthread_mutex_unlock(&shrmem->mutex_mem);

        printf("Engine %d recebeu: ID %d\n", id, aux.id);
        if (strcmp(aux.servico, "exit") == 0) {
            pthread_mutex_lock(&shrmem->mutex_mem);
            shrmem->n_users--;
            pthread_mutex_unlock(&shrmem->mutex_mem);

            sprintf(message, "AUTHORIZATION_ENGINE %d: EXIT REQUEST (ID = %d) PROCESSING COMPLETED", id, aux.id);
            escreverLog(message);

        } else if (strcmp(aux.servico, "REGISTER") == 0) { // registo
            pthread_mutex_lock(&shrmem->mutex_mem);
            if (shrmem->n_users < N_MAX_USERS) {
                shrmem->mobileUsers[shrmem->n_users].id_user = aux.id;
                shrmem->mobileUsers[shrmem->n_users].plafondInicial = aux.dados_reservar;
                shrmem->mobileUsers[shrmem->n_users].plafondAtual = aux.dados_reservar;
                shrmem->mobileUsers[shrmem->n_users].pedidosAtual = 0;
                shrmem->mobileUsers[shrmem->n_users].pedidosVideo = 0;
                shrmem->mobileUsers[shrmem->n_users].pedidosMusic = 0;
                shrmem->mobileUsers[shrmem->n_users].pedidosSocial = 0;
                shrmem->n_users++;
                pthread_mutex_unlock(&shrmem->mutex_mem);
                msgQueue.sucesso = 1;

            } else {
                pthread_mutex_unlock(&shrmem->mutex_mem);
                printf("Numero de utilizadores maximo atingido.\n");
                escreverLog("Numero de utilizadores atingiu o limite. Pedido de registo removido.");
                msgQueue.sucesso = 0;
            }
            msgQueue.type = aux.id;
            msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0); // enviar mensagem ao user para saber se deu login ou nao
            
            sprintf(message, "AUTHORIZATION_ENGINE %d: REGISTRATION REQUEST (ID = %d) PROCESSING COMPLETED", id, aux.id);
            escreverLog(message);

        } else if(strcmp(aux.servico, "MUSIC") == 0) { // music
            int id_user = -1;
            pthread_mutex_lock(&shrmem->mutex_mem);
            for (int i = 0; i < shrmem->n_users; i++) {
                if (shrmem->mobileUsers[i].id_user == aux.id) {
                    id_user = i;
                    break;
                }
            }

            sem_post(shrmem->sem_alerts);
            int plafond_disponivel = shrmem->mobileUsers[id_user].plafondAtual;
            shrmem->stats->totalAuthReqsMusic++;
            shrmem->mobileUsers[id_user].pedidosMusic++;
            shrmem->mobileUsers[id_user].pedidosAtual++;
            if (plafond_disponivel > 0) {
                shrmem->mobileUsers[id_user].plafondAtual -= aux.dados_reservar;
                shrmem->stats->totalDataMusic += aux.dados_reservar;

                sprintf(message, "AUTHORIZATION_ENGINE %d: MUSIC AUTHORIZATION REQUEST (ID = %d) PROCESSING COMPLETED", id, aux.id);
                escreverLog(message);
            } else {
                sprintf(message, "AUTHORIZATION_ENGINE %d: MUSIC AUTHORIZATION REQUEST (ID = %d) REFUSED", id, aux.id);
                escreverLog(message);
            }
            pthread_mutex_unlock(&shrmem->mutex_mem);

            

        } else if (strcmp(aux.servico, "SOCIAL") == 0) { // social
            int id_user = -1;
            pthread_mutex_lock(&shrmem->mutex_mem);
            for (int i = 0; i < shrmem->n_users; i++) {
                if (shrmem->mobileUsers[i].id_user == aux.id) {
                    id_user = i;
                    break;
                }
            }

            sem_post(shrmem->sem_alerts);
            int plafond_disponivel = shrmem->mobileUsers[id_user].plafondAtual;
            shrmem->stats->totalAuthReqsSocial++;
            shrmem->mobileUsers[id_user].pedidosSocial++;
            shrmem->mobileUsers[id_user].pedidosAtual++;
            if (plafond_disponivel > 0) {
                shrmem->mobileUsers[id_user].plafondAtual -= aux.dados_reservar;
                shrmem->stats->totalDataSocial += aux.dados_reservar;

                sprintf(message, "AUTHORIZATION_ENGINE %d: SOCIAL AUTHORIZATION REQUEST (ID = %d) PROCESSING COMPLETED", id, aux.id);
                escreverLog(message);
            } else {
                sprintf(message, "AUTHORIZATION_ENGINE %d: SOCIAL AUTHORIZATION REQUEST (ID = %d) REFUSED", id, aux.id);
                escreverLog(message);
            }
            pthread_mutex_unlock(&shrmem->mutex_mem);

        } else if (strcmp(aux.servico, "data_stats") == 0){ // stats

            pthread_mutex_lock(&shrmem->mutex_mem);
            msgQueue.type = 2;
            msgQueue.sucesso = 1;
            msgQueue.totalAuthReqsMusic = shrmem->stats->totalAuthReqsMusic;
            msgQueue.totalAuthReqsSocial = shrmem->stats->totalAuthReqsSocial;
            msgQueue.totalAuthReqsVideo = shrmem->stats->totalAuthReqsVideo;
            msgQueue.totalDataMusic = shrmem->stats->totalDataMusic;
            msgQueue.totalDataSocial = shrmem->stats->totalDataSocial;
            msgQueue.totalDataVideo = shrmem->stats->totalDataVideo;
            pthread_mutex_unlock(&shrmem->mutex_mem);

            msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
 
        } else if (strcmp(aux.servico, "reset") == 0){ // reset

            pthread_mutex_lock(&shrmem->mutex_mem);
            shrmem->stats->totalAuthReqsMusic = 0;
            shrmem->stats->totalAuthReqsSocial = 0;
            shrmem->stats->totalAuthReqsVideo = 0;
            shrmem->stats->totalDataMusic = 0;
            shrmem->stats->totalDataSocial = 0;
            shrmem->stats->totalDataVideo = 0;
            shrmem->stats->totalAuthReqs = 0;
            pthread_mutex_unlock(&shrmem->mutex_mem);
 
        } else if (strcmp(aux.servico, "VIDEO") == 0) { // video

            int id_user = -1;
            pthread_mutex_lock(&shrmem->mutex_mem);
            for (int i = 0; i < shrmem->n_users; i++) {
                if (shrmem->mobileUsers[i].id_user == aux.id) {
                    id_user = i;
                    break;
                }
            }

            sem_post(shrmem->sem_alerts);
            int plafond_disponivel = shrmem->mobileUsers[id_user].plafondAtual;
            shrmem->stats->totalAuthReqsVideo++;
            shrmem->mobileUsers[id_user].pedidosVideo++;
            shrmem->mobileUsers[id_user].pedidosAtual++;
            if (plafond_disponivel > 0) {
                shrmem->mobileUsers[id_user].plafondAtual -= aux.dados_reservar;
                shrmem->stats->totalDataVideo += aux.dados_reservar;

                sprintf(message, "AUTHORIZATION_ENGINE %d: VIDEO AUTHORIZATION REQUEST (ID = %d) PROCESSING COMPLETED", id, aux.id);
                escreverLog(message);

            } else {
                sprintf(message, "AUTHORIZATION_ENGINE %d: VIDEO AUTHORIZATION REQUEST (ID = %d) REFUSED", id, aux.id);
                escreverLog(message);

            }
            pthread_mutex_unlock(&shrmem->mutex_mem);


        }

        time_t end_time = time(NULL);
        memset(&aux, 0, sizeof(aux));
        sleep((AUTH_PROC_TIME - (end_time - processed_time)) / 1000);

        pthread_mutex_lock(&shrmem->mutex_mem);
        
        pthread_mutex_lock(&shrmem->mutex_engine_free);
        shrmem->authEnginePipes[id].disponivel = 1;
        pthread_cond_signal(&shrmem->cond_engine_free);
        pthread_mutex_unlock(&shrmem->mutex_engine_free);

        pthread_mutex_unlock(&shrmem->mutex_mem);
        
    }
}


void monitorEngine() {
    pthread_t stats_t, alert_t;
    pthread_create(&stats_t, NULL, gerarEstatisticas, NULL);
    pthread_create(&alert_t, NULL, gerarAlertas, NULL);

    pthread_join(stats_t, NULL);
    pthread_join(alert_t, NULL);
}

void * gerarEstatisticas(void * arg) {
    if((int *)arg) {};

    while (1) {
        pthread_mutex_lock(&shrmem->mutex_mem);
        msgQueue.type = 1;
        msgQueue.sucesso = 1;
        msgQueue.totalAuthReqsMusic = shrmem->stats->totalAuthReqsMusic;
        msgQueue.totalAuthReqsSocial = shrmem->stats->totalAuthReqsSocial;
        msgQueue.totalAuthReqsVideo = shrmem->stats->totalAuthReqsVideo;
        msgQueue.totalDataMusic = shrmem->stats->totalDataMusic;
        msgQueue.totalDataSocial = shrmem->stats->totalDataSocial;
        msgQueue.totalDataVideo = shrmem->stats->totalDataVideo;
        msgQueue.totalAuthReqs = shrmem->stats->totalAuthReqs;
        pthread_mutex_unlock(&shrmem->mutex_mem);

        msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
        sleep(30);
    }

    pthread_exit(NULL);
    return NULL;
}

void * gerarAlertas(void *arg) {
    if((int *)arg) {};

    
    while (1) {
        sem_wait(shrmem->sem_alerts);

        pthread_mutex_lock(&shrmem->mutex_mem);
        for (int i = 0; i < shrmem->n_users; i++) {
            if (shrmem->mobileUsers[i].plafondAtual <= shrmem->mobileUsers[i].plafondInicial * 0) {
                msgQueue.type = shrmem->mobileUsers[i].id_user;
                msgQueue.sucesso = 4;
                msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
            } else if (shrmem->mobileUsers[i].plafondAtual < shrmem->mobileUsers[i].plafondInicial * 0.1) {
                msgQueue.type = shrmem->mobileUsers[i].id_user;
                msgQueue.sucesso = 3;
                msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
            } else if (shrmem->mobileUsers[i].plafondAtual < shrmem->mobileUsers[i].plafondInicial * 0.2) {
                msgQueue.type = shrmem->mobileUsers[i].id_user;
                msgQueue.sucesso = 2;
                msgsnd(glMsqId, &msgQueue, sizeof(glMessageQueue) - sizeof(long), 0);
            } 
        }
        pthread_mutex_unlock(&shrmem->mutex_mem);
    }

    pthread_exit(NULL);
    return NULL;
}

