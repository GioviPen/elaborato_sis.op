// common.h

#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h> //obsoleta
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 10
#define MAX_LEN 128
#define NUM_NOMI 100
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_CONNECTIONS 5

#define ROSSO     "\x1b[31m"
#define VERDE   "\x1b[32m"
#define GIALLO  "\x1b[33m"
#define BLU    "\x1b[34m"
#define RESET   "\x1b[0m"

#endif COMMON_H