#include <stdio.h>
#define BUFLEN 1024
int N_USERS, N_SLOTS, AUTH_SERVERS_MAX, AUTH_PROC_TIME, MAX_VIDEO_WAIT, MAX_OTHERS_WAIT;


void arranque(char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL){
        printf("Erro ao abrir o ficheiro de configuração\n");
        return;
    }

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erro ao abrir o ficheiro.\n");
        //sprintf(message, "Erro ao abrir o ficheiro %s", filename);
        //escrever_log(f, message);
        //sigint(0);
        exit(-1);
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    if(size == 0){
        printf("Erro: o ficheiro está vazio.\n");
        //sprintf(message, "Erro: o ficheiro %s está vazio", filename);
        //escrever_log(f, message);
        //sigint(0);
        exit(-1);
    }
    fseek(f, 0, SEEK_SET);

    char linhas[6][BUFLEN];

    fgets(linhas[0], BUFLEN, f);
    N_USERS = atoi(linhas[0]);
    if (N_USERS < 1){
        printf("Erro: o número de utilizadores tem de ser maior que 0.\n");
        //sprintf(message, "Erro: o número de utilizadores tem de ser maior que 0");
        //escrever_log(f, message);
        exit(-1);
    }

    fgets(linhas[1], BUFLEN, f);
    N_SLOTS = atoi(linhas[1]);
    if (N_SLOTS < 0){
        printf("Erro: o número de slots nas filas tem de ser maior ou igual que 0.\n");
        //sprintf(message, "Erro: o número de slots tem de ser maior que 0");
        //escrever_log(f, message);
        exit(-1);
    }

    fgets(linhas[2], BUFLEN, f);
    AUTH_SERVERS_MAX = atoi(linhas[2]);
    if (AUTH_SERVERS_MAX < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        //sprintf(message, "Erro: o número de servidores de autorização tem de ser maior que 0");
        //escrever_log(f, message);
        exit(-1);
    }
    
    fgets(linhas[3], BUFLEN, f);
    AUTH_PROC_TIME = atoi(linhas[3]);
    
    fgets(linhas[4], BUFLEN, f);
    MAX_VIDEO_WAIT = atoi(linhas[4]);
    if (MAX_VIDEO_WAIT < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        //sprintf(message, "Erro: o número de servidores de autorização tem de ser maior que 0");
        //escrever_log(f, message);
        exit(-1);
    }

    fgets(linhas[5], BUFLEN, f);
    MAX_OTHERS_WAIT = atoi(linhas[5]);
    if (MAX_OTHERS_WAIT < 1){
        printf("Erro: o número de servidores de autorização tem de ser maior que 0.\n");
        //sprintf(message, "Erro: o número de servidores de autorização tem de ser maior que 0");
        //escrever_log(f, message);
        exit(-1);
    }







    fclose(f);
}

int main(int argc, char const *argv[]){
    if (argc != 2)
        printf("Numero de parametros errados\n./5g_auth_platform {config-file}\n");




    return 0;
}
