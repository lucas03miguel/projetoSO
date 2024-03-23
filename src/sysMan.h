#ifndef SYSMAN_H
#define SYSMAN_H

typedef struct {
	//TODO
} MemStruct;

void arranque(char *filename);
void escreverLog(char *message);
void authorizationRequestManager();
void monitorEngine();

#endif // SYSMAN_H