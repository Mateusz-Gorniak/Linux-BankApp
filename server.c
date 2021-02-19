#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

#define PORT 8080
typedef enum {
    false, true
} bool;

int main() {

    int sockfd, ret;
    struct sockaddr_in serverAddr;
    struct sockaddr_in newAddr;
    int newSocket;
    char buffer[1024] = {0};
    socklen_t addr_size;
    pid_t childpid;
    char Logged[] = "Log";
    char NLogged[] = "NLog";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ret = bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", 8080);

    if (listen(sockfd, 10) == 0) {
        printf("[+]Listening....\n");
    } else {
        printf("[-]Error in binding.\n");
    }

    FILE *file; // pointer to flie with logs
    char logpath[] = "/home/student/PULX /PROJEKT_LINUX/log.txt";


    while (newSocket) {// petla do rozdzielania nowych uzytkownikow na pojedyncze procesy
        newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
        if (newSocket < 0) {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n",
               inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0) {
            close(sockfd);

            while (1) {//Komunikacja server-klient 
                while (1) {

                    read(newSocket, buffer, sizeof(buffer));
                    if (strcmp(buffer, "user") == 0) {
                        bzero(buffer, sizeof(buffer));
                        read(newSocket, buffer, sizeof(buffer));
                        if (strcmp(buffer, "pass") == 0) {
                            bzero(buffer, sizeof(buffer));
                            printf("Logged from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                            send(newSocket, Logged, strlen(Logged), 0);
                            break;
                        }
                            else{
                                send(newSocket, NLogged, strlen(NLogged), 0);
                            }
                    } else {
                        printf("Logging failed %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        send(newSocket, NLogged, strlen(NLogged), 0);
                    }
                }
                read(newSocket, buffer, sizeof(buffer));
                if ((strcmp(buffer, "exit") == 0)) {
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    break;
                }
            }
        }

         close(newSocket);
    }

    return 0;
}

