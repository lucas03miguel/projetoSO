all: mobile_user backoffice_user 5g_auth_platform #clean

5g_auth_platform: sysMan.c
	gcc -Wall -Wextra -pthread -D_REENTRANT -g sysMan.c -o 5g_auth_platform

backoffice_user: backUser.c
	gcc -Wall -Wextra  -pthread -D_REENTRANT -g backUser.c -o backoffice_user

mobile_user: mobileUser.c
	gcc -Wall -Wextra -pthread -D_REENTRANT -g mobileUser.c -o mobile_user

#sensor.o: sensor.c
#	gcc -Wall -g -c sensor.c -o sensor.o

#funcoes.o: funcoes.c
#	gcc -Wall -pthread -D_REENTRANT -g -c funcoes.c -o funcoes.o
	
#clean:
#	rm -f funcoes.o sensor.o userConsole.o SystemManager.o
