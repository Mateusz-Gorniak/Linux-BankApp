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

#define clear() printf("\033[H\033[J")//czyszczenie ekranu

int main() {

    //zmienne pomocnicze
    char login[30];
    char haslo[30];
    char imie[30];
    char nazwisko[30];
    float saldo;
    char failLogin[] = "fail";
    char logpath[]="/home/student/PULX /PROJEKT_LINUX/log.txt";
    char user[30];
    char Logged[] = "Log";
    char NLogged[] = "NLog";
    char cmdNotDect[] = "Command no detected";
    char cmdSucces[] = "Operation was successful";
    char cmdFailed[] = "Operation failed";

    // Get the current working directory:
    char *Path;//wskznik na sciezke katalogu w ktorym pracuje server
    if ((Path = getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        printf("%s \nLength: %zu\n", Path, strlen(Path));
    }

    //File with server logs

    time_t myclock = time(NULL);
    struct tm *dataclock;
    char mytime[1000];
    FILE *logfile;
    //sprintf(logpath,"%s%s", Path, "/log.txt");
    printf("LOGPath: %s\n", logpath);

    logfile = fopen(logpath,"a");
    if(!logfile){
        printf("%s","FAILURE");
    }
    dataclock = localtime(&myclock);
    strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
    fprintf(logfile,"\n%s START PRACY SERVERA\n",mytime);
    fclose(logfile);
    clear();


    //Create socket
    int sockfd, ret;//uchwyt do socketa
    struct sockaddr_in serverAddr;
    struct sockaddr_in newAddr;
    int newSocket;
    char buffer[1024] = {0};
    socklen_t addr_size;//rozmiar adresu
    pid_t childpid;// proces do rozdzielania wielu uzytkownikow

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {// lub sockfd ==-1
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");


    memset(&serverAddr, '\0', sizeof(serverAddr));
    /*
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);//przypisanie portu
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
     */
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons( PORT );
    //Powiązanie nowo utworzonego gniazda z podanym adresem IP i weryfikacja
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
        printf("Connection accepted from %s:%d\n",inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        logfile = fopen(logpath,"a");
        fprintf(logfile,"\n%s Nawiazanie polaczenia z klientem: %s %d\n",mytime,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
        fclose(logfile);

        if ((childpid = fork()) == 0) {
            close(sockfd);

            //Petla logowania
            while (newSocket) {

                logfile = fopen(logpath,"a");
                FILE *file;//deklaracja wskaznika na plik z danymi klienta
                strcat(Path, "/Users/");
                char path[50];
                //Komunikacja server-klient
                while (newSocket) {


                    read(newSocket, buffer, sizeof(buffer));
                    printf("User input login: %s\n", buffer);
                    fprintf(logfile,"%s User input login: %s\n",mytime,buffer);
                    strncpy(user, buffer, sizeof(user));//skopiowanie loginu od klienta
                    sprintf(path, "%s%s", Path, user);
                    printf("Path: %s\n", path);
                    file = fopen(path, "r");
                    if (file) {
                        fscanf(file, "%g %s %s %s %s", &saldo, imie, nazwisko, login, haslo);
                        printf("%s\n", "User file opened");
                    } else {
                        printf("%s\n", "User file not opened");
                    }
/*
                    printf("Saldo: %g\n", saldo);
                    printf("Imie:: %s\n", imie);
                    printf("Nazwisko: %s\n", nazwisko);
                    printf("Login: %s\n", login);
                    printf("Haslo: %s\n", haslo);
*/
                    read(newSocket, buffer, sizeof(buffer));
                    printf("User input password: %s\n", buffer);
                    fprintf(logfile,"%s User input password: %s\n",mytime,buffer);
                    if ((strcmp(user, login) == 0) && (strcmp(buffer, haslo) == 0)) {
                        printf("Logging: %s\n", user);
                        fprintf(logfile,"%s Logging user: %s\n",mytime,user);
                        bzero(buffer, sizeof(buffer));
                        // bzero(user,sizeof(user));
                        printf(" %s Logged %s from %s:%d\n",mytime,user,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        fprintf(logfile," %s Logged from %s:%d\n",mytime,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        send(newSocket, Logged, strlen(Logged), 0);

                        // send(newSocket, imie, strlen(imie), 0);
                        // send(newSocket, nazwisko, strlen(nazwisko), 0);
                        fclose(file);
                        break;

                    } else {
                        printf("Logging failed %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        fprintf(logfile,"%s Logging failed %s:%d\n",mytime,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
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
            fclose(logfile);


            //wyslanie aplikcji klienta stanu konta po zalogowaniu
            read(newSocket, buffer, sizeof(buffer));
            if ((strcmp(buffer, "account_state") == 0)) {
                printf("%s\n","Socket client requested account_state");
                send(newSocket, imie, sizeof (imie), 0);
                send(newSocket, nazwisko, sizeof (nazwisko), 0);
                send(newSocket, &saldo, sizeof (float), 0);
            }
            bzero(buffer, sizeof(buffer));
            logfile = fopen(logpath,"a");
            // Petla obslugi polecen klienta
            while (newSocket) {

                read(newSocket, buffer, sizeof(buffer));

                //bzero(buffer, sizeof(buffer));
                if((strcmp(buffer,"help") == 0)) {
                    printf("Client asked for help %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    fprintf(logfile, "\n%s %s uruchomil pomoc: %s %d\n", mytime,user, inet_ntoa(newAddr.sin_addr),
                            ntohs(newAddr.sin_port));
                    send(newSocket, cmdSucces, strlen(cmdSucces), 0);
                }
                if ((strcmp(buffer,"exit") == 0)) {
                    printf("%s %s Logged out from %s:%d\n",mytime,user,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    fprintf(logfile,"\n%s Wylogowano %s: %s %d\n",mytime,user,inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    break;
                }
                else {
                    printf("%s\n", "Command not detected");
                    send(newSocket, cmdNotDect, strlen(cmdNotDect), 0);
                }

            }
            if (read(newSocket, buffer, sizeof(buffer)) < 0) {
                printf("Disconected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                break;
            }

            //read(newSocket, buffer, sizeof(buffer));
            //if ((strcmp(buffer, "logout") == 0)) {
            //    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            //    break;
            //}
            fprintf(logfile,"\n%s KONIEC PRACY SERVERA\n",mytime);
            fclose(logfile);
        }

        close(newSocket);
    }

    return 0;
}