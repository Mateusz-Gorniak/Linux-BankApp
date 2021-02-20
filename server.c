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


#if defined(_MSC_VER)
#include <direct.h>
#define getcwd _getcwd
#elif defined(__GNUC__)
#include <unistd.h>
#endif
#define PORT 8080


struct Klient{
    char login[30];
    char haslo[30];
    char imie[30];
    char nazwisko[30];
    float saldo;

}typedef Klient;

int main() {

    // Get the current working directory:
    char* Path;
    if( (Path=getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        printf("%s \nLength: %zu\n", Path, strlen(Path));
    }


  //Create socket
    Klient klient;
    char user[30];
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


    //Customer Service
    while (newSocket) {// petla do rozdzielania nowych uzytkownikow na pojedyncze procesy
        newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
        if (newSocket < 0) {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n",
               inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0) {
            close(sockfd);
			
			FILE *file;//deklaracja wskaznika na plik
            strcat(Path,"/Users/");

            //Komunikacja server-klient
            while (newSocket) {
                while (newSocket) {

                    read(newSocket, buffer, sizeof(buffer));
                    strncpy(user,buffer, sizeof(user));//skopiowanie loginu od klienta
                    file= fopen(strcat(Path,user),"r");
                    fscanf(file,"%g %s %s %s %s",&klient.saldo,klient.imie,klient.nazwisko,klient.login,klient.haslo);
                    printf("Saldo: %g\n",klient.saldo);
                    printf("Imie:: %s\n",klient.imie);
                    printf("Nazwisko: %s\n",klient.nazwisko);
                    printf("Login: %s\n",klient.login);
                    printf("Haslo: %s\n",klient.haslo);
                    
                    fclose(file);
                    
                    if (strcmp(buffer,klient.login) == 0) {
                        printf("Logging: %s\n",buffer);
                        bzero(buffer, sizeof(buffer));
                        read(newSocket, buffer, sizeof(buffer));
                        if (strcmp(buffer,klient.haslo) == 0) {
                            bzero(buffer, sizeof(buffer));
                            printf("Logged from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                            send(newSocket, Logged, strlen(Logged), 0);
                            break;
                        } else {
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