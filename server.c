#include "include.h"

void* server_handling(void* arg){
    int client_fd = *((int*)arg);
    handle_request(client_fd);
    free(arg); 
    return NULL;
}


void handle_sigpipe(int sig) {
    printf("Caught SIGPIPE signal. Client closed the connection.\n");
    exit(1);
}

int main(){
    signal(SIGSEGV,handle_sigpipe);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket creation error: ");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in protocol_addres;
    protocol_addres.sin_family = AF_INET;
    protocol_addres.sin_port = htons(PORT);
    protocol_addres.sin_addr.s_addr = INADDR_ANY;
    
    int returned = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&returned,(socklen_t)sizeof(returned));

    socklen_t len = sizeof(protocol_addres);
    
     
    if(bind(sockfd,(struct sockaddr*)&protocol_addres,len) < 0){
        perror("bind error: ");
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd,MAX_CLIENT_COUNT) == -1){
        perror("listen error: ");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[MAX_CLIENT_COUNT];
    for(int i = 0;i < MAX_CLIENT_COUNT;++i){
        struct sockaddr_in *client_addr = malloc(sizeof(struct sockaddr_in));
        socklen_t cl_len = sizeof(struct sockaddr_in);

        int accept_fd = accept(sockfd,(struct sockaddr*)&client_addr,&cl_len);
        if(accept_fd < 0){
            perror("accept error");
            exit(1);
        }
        int *current_fd = malloc(sizeof(int));
        if(current_fd == NULL){
            printf("malloc error\n");
            exit(EXIT_FAILURE);
        }
        *current_fd = accept_fd;
        int th = pthread_create(&threads[i],NULL,server_handling,(void*)current_fd);
        if(th < 0){
            perror("creat: ");
            exit(1);
        }

    }

    for(int i = 0;i < MAX_CLIENT_COUNT;++i){
        pthread_join(threads[i],NULL);
    }
    close(sockfd);
}