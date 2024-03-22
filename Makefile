all: bin bin/5g_auth_platform bin/backoffice_user bin/mobile_user

bin:
	mkdir -p bin

bin/5g_auth_platform: src/sysMan.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

bin/backoffice_user: src/backUser.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

bin/mobile_user: src/mobileUser.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

