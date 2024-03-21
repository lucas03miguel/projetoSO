CC = gcc
CFLAGS = -Wall -Wextra -pthread -D_REENTRANT -g
SRC_DIR = src
BIN_DIR = $(SRC_DIR)/bin

all: $(BIN_DIR)/5g_auth_platform $(BIN_DIR)/backoffice_user $(BIN_DIR)/mobile_user

$(BIN_DIR)/5g_auth_platform: $(SRC_DIR)/sysMan.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

$(BIN_DIR)/backoffice_user: $(SRC_DIR)/backUser.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

$(BIN_DIR)/mobile_user: $(SRC_DIR)/mobileUser.c 
	gcc -Wall -Wextra -pthread -D_REENTRANT -g $< -o $@

