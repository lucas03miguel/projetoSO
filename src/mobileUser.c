#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#define BUFLEN 1024
//#include <string.h>

int PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR;
FILE *logFile;

void detecaoErros(int n, char *args[]);
void escreverLog(FILE *f, char *message);


int main(int argc, char *argv[]){
    detecaoErros(argc, argv);
    
    while (1)
    {
        printf("PLAFOND: %d\nN_PEDIDOS: %d\nINTERVALO_VIDEO: %d\nINTERVALO_MUSIC: %d\nINTERVALO_SOCIAL: %d\nDADOS_RESERVAR: %d\n\n", PLAFOND, N_PEDIDOS, INTERVALO_VIDEO, INTERVALO_MUSIC, INTERVALO_SOCIAL, DADOS_RESERVAR);
        sleep(2);
    }
    


    return 0;
}


void detecaoErros(int n, char *args[]) {
    if (n != 7) {
        printf("Numero de parametros errado\n./mobile_user {plafond inicial} {número de pedidos de autorização} {intervalo VIDEO} {intervalo MUSIC} {intervalo SOCIAL} {dados a reservar}\n");
        exit(-1);
    }

    logFile = fopen("../files/log.txt", "a");
    char message[BUFLEN];
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
                    sprintf(message, "ERROR: o argumento {plafond inicial} não é válido.");
                    escreverLog(logFile, message);
                }
                break;
            case 2:
                if (!letras) N_PEDIDOS = atoi(args[i]);
                else {
                    printf("Erro: o argumento {número de pedidos de autorização} não é válido.\n");
                    sprintf(message, "ERROR: o argumento {número de pedidos de autorização} não é válido.");
                    escreverLog(logFile, message);
                }
                break;
            case 3:
                if (!letras) INTERVALO_VIDEO = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo VIDEO} não é válido.\n");
                    sprintf(message, "ERROR: o argumento {intervalo VIDEO} não é válido.");
                    escreverLog(logFile, message);
                }
                break;
            case 4:
                if (!letras) INTERVALO_MUSIC = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo MUSIC} não é válido.\n"); 
                    sprintf(message, "ERROR: o argumento {intervalo MUSIC} não é válido.");
                    escreverLog(logFile, message);
                }
                break;
            case 5:
                if (!letras) INTERVALO_SOCIAL = atoi(args[i]);
                else {
                    printf("Erro: o argumento {intervalo SOCIAL} não é válido.\n");
                    sprintf(message, "ERROR: o argumento {intervalo SOCIAL} não é válido.");
                    escreverLog(logFile, message);
                }
                break;
            case 6:
                if (!letras) DADOS_RESERVAR = atoi(args[i]);
                else {
                    printf("Erro: o argumento {dados a reservar} não é válido.\n");
                    sprintf(message, "ERROR: o argumento {dados a reservar} não é válido.");
                    escreverLog(logFile, message);
                }
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


void escreverLog(FILE *f, char *message){
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);
    fprintf(f, "%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
    
    fflush(stdout);
    fflush(f);
}