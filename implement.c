#include "include.h"
void handle_get(char* filename, int fd) {

    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        char error[] = "SERVER RESPONSE\n404 file not found\n";
        send(fd, error, strlen(error), 0);
        return;
    }

    struct stat st;
    stat(filename, &st);
    int size = st.st_size;
    char response[NUM];
    snprintf(response, NUM, "\nSERVER RESPONSE\nCHLP 200: OK\nbody size: %d\n\n", size);

    send(fd, response, strlen(response), 0);

    char read_buffer[MSG_FILE];
    int read_bytes;
    while ((read_bytes = fread(read_buffer, 1, MSG_FILE, file)) > 0) {
        send(fd, read_buffer, read_bytes, 0);
    }

    fclose(file);
}

void handle_header(int fd) {

    char buf[RR];
    snprintf(buf, sizeof(buf), "%s", "----CHLP PROTOCOL PRESENT----\n-->USAGE <request> </filename> <protocolname><--\nrequests-->POST,HEADERS,GET<--\n-->GET-Server give you file size(body-size) and content\nPOST-write to file, server give you writed file<--\n-->RETURN CODES 404:file not found, 419:Error opening file, 200:OK, codes\nserver listen to many clients\n");
    
    int len = strlen(buf); 
    if (send(fd, buf, len, 0) == -1) { 
        perror("send error: ");
        exit(1);
    }
    printf("resposne sendit succsesfuly\n");
}
void handle_post(char* filename, int fd) {
    printf("handling POST request\n");
    char recv_buffer[MSG_FILE];
    int read_bytes = recv(fd, recv_buffer, MSG_FILE, 0);
    if (read_bytes < 0) {
        perror("recv error: ");
        return; 
    }
    recv_buffer[read_bytes] = '\0';
    
    int file = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0644);  
    if (file < 0) {
        perror("file opening error: ");
        char statuss[] = "SERVER RESPONSE\nCHLP 419 file error";
        send(fd, statuss, strlen(statuss), 0);
        return; 
    }

    int len = strlen(recv_buffer);
    if (write(file, recv_buffer, len) < 0) {
        perror("write error: ");
        close(file); 
        return; 
    }
    close(file);

    struct stat st;
    stat(filename, &st);

    
    int size = st.st_size;

    char status[100];
    snprintf(status, sizeof(status), "\n%s\n%s\n%s%d\n\n", "SERVER RESPONSE", "CHLP 200: OK_ADDED", "body size:", size);

    if (send(fd, status, strlen(status), 0) == -1) {
        perror("send error: ");
        return; 
    }

    printf("POST request handled successfully\n");
}


void handle_delete(char* filename,int fd){
    
    int f = open(filename,O_RDWR);
    if(f < 0){
    
        perror("file opening error: ");
        char statuss[] = "SERVER RESPONSE\nCHLP 419 file error";
        send(fd, statuss, strlen(statuss), 0);
        exit(EXIT_FAILURE);
    }
    remove(filename);
    char status[NUM];
    snprintf(status, NUM, "\n%s\n%s\n", "SERVER RESPONSE", "CHLP 200: OK_DELETED");

    if (send(fd, status, strlen(status), 0) == -1) {
        perror("send error: ");
        exit(EXIT_FAILURE);
    }
    printf("resposne sendit succsesfuly\n");
}

void handle_request(int fd){
    while(1){
    fflush(stdout); 
    fflush(stdin); 
   
    char request[REQUEST_SIZE];
    int readed = recv(fd,request,REQUEST_SIZE - 1,0);
    if(readed < 0){
        perror("recv error: ");
        exit(EXIT_FAILURE);
    }
    char request_type[MAX_REQUEST_TSIZE];
    char filename[MAX_FILENAME_SIZE];
    char protocol[MAX_PROTOCOL];
    char* token = strtok(request," ");
    strcpy(request_type,token);
    token = strtok(NULL," /");
    strcpy(filename,token);
    token = strtok(NULL," ");
    strcpy(protocol,token);
    

    
    if(strcmp(request_type,"GET") == 0){
        handle_get(filename,fd,protocol);
    }
    else if(strcmp(request,"POST") == 0){
        handle_post(filename,fd);

    }
    else if(strcmp(request,"OPTIONS") == 0){
        handle_header(fd,protocol);
    }
    else if(strcmp(request,"DELETE") == 0){
        handle_delete(filename,fd,protocol);
    }
    }
}
