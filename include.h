#ifndef HEADER
#define HEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#define PORT 8081
#define MAX_CLIENT_COUNT 10
#define REQUEST_SIZE 128
#define MAX_REQUEST_TSIZE 10
#define MAX_FILENAME_SIZE 20
#define MSG_FILE 30
#define NUM 100
#define POST_RES 1024
#define RR 512
#define MAX_PROTOCOL 20
void handle_request(int );
void handle_get(char* ,int );
void handle_post(char*, int);
void handle_header(int);
void handle_delete(char*,int);









#endif


