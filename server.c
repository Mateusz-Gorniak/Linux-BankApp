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

char login[30];
char haslo[30];
char imie[30];
char nazwisko[30];
float saldo;
char failLogin[] = "fail";


int main() {

    // Get the current working directory:
    char *Path;
    if ((Path = getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        printf("%s \nLength: %zu\n", Path, strlen(Path));
    }


    //Create socket
    // Klient klient;
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
    char cmdNotDect[] = "Command no detected";

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

            //Komunikacja server-klient
            while (newSocket) {

                FILE *file;//deklaracja wskaznika na plik
                strcat(Path, "/Users/");
                char path[50];
                //Komunikacja server-klient
                while (newSocket) {


                    read(newSocket, buffer, sizeof(buffer));
                    printf("User input login: %s\n", buffer);
                    strncpy(user, buffer, sizeof(user));//skopiowanie loginu od klienta
                    sprintf(path, "%s%s", Path, user);
                    printf("Path: %s\n", path);
                    file = fopen(path, "r");
                    if (file) {
                        fscanf(file, "%g %s %s %s %s", &saldo, imie, nazwisko, login, haslo);
                        printf("%s\n", "Otworzono plik");
                    } else {
                        printf("%s\n", "Nie otworzono pliku z danymi");
                    }

                    printf("Saldo: %g\n", saldo);
                    printf("Imie:: %s\n", imie);
                    printf("Nazwisko: %s\n", nazwisko);
                    printf("Login: %s\n", login);
                    printf("Haslo: %s\n", haslo);

                    read(newSocket, buffer, sizeof(buffer));
                    printf("User input password: %s\n", buffer);

                    if ((strcmp(user, login) == 0) && (strcmp(buffer, haslo) == 0)) {
                        printf("Logging: %s\n", user);
                        bzero(buffer, sizeof(buffer));
                        // bzero(user,sizeof(user));
                        printf("Logged from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        send(newSocket, Logged, strlen(Logged), 0);

                        // send(newSocket, imie, strlen(imie), 0);
                        // send(newSocket, nazwisko, strlen(nazwisko), 0);
                        fclose(file);
                        break;

                    } else {
                        printf("Logging failed %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        send(newSocket, NLogged, strlen(NLogged), 0);
                        bzero(buffer, sizeof(buffer));
                        bzero(user, sizeof(user));
                        bzero(login, sizeof(login));
                        bzero(haslo, sizeof(haslo));
                        bzero(haslo, sizeof(haslo));
                    }
                }
                break;
            }


            while (newSocket) {

                read(newSocket, buffer, sizeof(buffer));

                if ((strcmp(buffer, "logout") == 0)) {
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    break;
                } else {
                    printf("%s\n", "Command not detected");
                    send(newSocket, cmdNotDect, strlen(cmdNotDect), 0);
                }
                bzero(buffer, sizeof(buffer));

            }
            //read(newSocket, buffer, sizeof(buffer));
            //if ((strcmp(buffer, "logout") == 0)) {
            //    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            //    break;
            //}

        }

        close(newSocket);
    }

    return 0;
}