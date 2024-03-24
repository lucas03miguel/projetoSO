#ifndef SYSMAN_H
#define SYSMAN_H

typedef struct {
    int plafondInicial;
    int plafondAtual;
    int pedidosMax;
    int pedidosAtual;
    int id;
} MobileUser;

typedef struct {
    int totalDataVideo;
    int totalAuthReqsVideo;
    int totalDataMusic;
    int totalAuthReqsMusic;
    int totalDataSocial;
    int totalAuthReqsSocial;
} Stats;

typedef struct {
	MobileUser *mobileUsers;
    Stats *stats;
    
} MemStruct;

void arranque(char *filename);
void limpeza(FILE *f);
void escreverLog(char *message);
void authorizationRequestManager();
void monitorEngine();
void * receiver(void *arg);
void * sender(void *arg);

#endif // SYSMAN_H