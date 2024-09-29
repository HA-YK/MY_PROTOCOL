#include "include.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("wrong usage: usage ./executable <ip_addres>");
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation error: ");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in protocol_address;
    protocol_address.sin_family = AF_INET;
    protocol_address.sin_port = htons(PORT);
    protocol_address.sin_addr.s_addr = inet_addr(argv[1]); 
    socklen_t len = sizeof(protocol_address);

    int connect_fd = connect(sockfd, (struct sockaddr*)&protocol_address, len);
    if (connect_fd < 0) {
        perror("connect error: ");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("type request GET POST OPTIONS(OPTIONS 0 PROTOCOL NAME) DELETE or exit\n");
        char buffer[REQUEST_SIZE];
        fgets(buffer, REQUEST_SIZE, stdin);
        if(strcmp(buffer,"exit\n") == 0){
            break;
        }
        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("send error");
            exit(EXIT_FAILURE);
        }

        char* token = strtok(buffer, " ");
        if (strcmp("GET", token) == 0) {
            char response[NUM];
            int read_bytes = 0;
            while ((read_bytes = recv(sockfd, response, NUM - 1, 0)) > 0) {
                response[read_bytes] = '\0';
                printf("%s\n", response);
                if (strcmp(response, "404 error protocol\n") == 0 || 
                    strcmp(response, "SERVER RESPONSE\n404 file not found\n") == 0) {
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                break;
            }
            if (read_bytes < 0) {
                perror("recv error: ");
                exit(EXIT_FAILURE);
            }
            printf("enter 'enter' for entering new request\n");
            getchar();
        } else if (strcmp("POST", token) == 0) {
            printf("enter line to post into file\n");
            char post_buf[REQUEST_SIZE];
            fgets(post_buf, REQUEST_SIZE, stdin);

            int len = strlen(post_buf);
            if (len > 0 && post_buf[len - 1] == '\n') {
                post_buf[len - 1] = '\0';
            }

            if (send(sockfd, post_buf, strlen(post_buf), 0) == -1) {
                perror("send error: ");
                exit(EXIT_FAILURE);
            }

            char success_response[RR];
            int read_bytes = 0;
            while ((read_bytes = recv(sockfd, success_response, RR - 1, 0)) > 0) {
                success_response[read_bytes] = '\0';
                printf("%s\n", success_response);
            }

            char response[NUM];
            while ((read_bytes = recv(sockfd, response, NUM - 1, 0)) > 0) {
                response[read_bytes] = '\0';
                printf("%s", response);
            printf("enter 'enter' for entering new request\n");
            getchar();
            }
            if (read_bytes < 0) {
                perror("recv error: ");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp("OPTIONS", token) == 0 || strcmp("DELETE", token) == 0) {
            char present[RR];
            int read_bytes = recv(sockfd, present, sizeof(present) - 1, 0);  
            if (read_bytes < 0) {
                perror("read error: ");
                exit(EXIT_FAILURE);
            }
            present[read_bytes] = '\0';  
            printf("%s\n", present);  
            if (strcmp(present, "404 error protocol\n") == 0) {
                close(sockfd);
                exit(EXIT_FAILURE);
            }
        }
        printf("enter 'enter' for entering new request\n");
        getchar();
    }

    close(sockfd);
    return 0;
}
