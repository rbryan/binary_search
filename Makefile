
CC=gcc
CFLAGS= -Wall -g -O0 -lm -lssh -lcrypto

slept: binary.c Makefile
	$(CC) $(CFLAGS) binary.c -o bin 
