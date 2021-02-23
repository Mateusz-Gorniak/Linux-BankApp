#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#if defined(_MSC_VER)
#include <direct.h>
#define getcwd _getcwd
#elif defined(__GNUC__)
#include <unistd.h>
#endif

#define PORT 8080
#define clear() printf("\033[H\033[J")

//typedef enum {
//    false, true
//} bool;

int main() {

    //zmienne pomocnicze
    char systemCashRequest[] = "account_state";
    char imie[30];
    char nazwisko[30];
    float saldo;
    char siano_plus[10];
    char siano_minus[10];
    char path[40];
    FILE *file;
    char helpbuffer[255];

    // get curret working directory
    char *Path;//wskznik na sciezke katalogu w ktorym pracuje server
    if ((Path = getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        printf("%s \nLength: %zu\n", Path, strlen(Path));
    }

    //socket program
    int L = 0; // flaga logged
    int clientSocket, ret;//uchwyt do socketa
    struct sockaddr_in serverAddr;
    char buffer[1024];


    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Client Socket is created.\n");

    //memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.23");

    ret = connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Connected to Server.\n");
    clear();
    sleep(1);//opoznienie systemowe
    printf("WITAMY SZANOWNY KLIENCIE W BANKU SZACH-MAT BANK \n");
    printf("-----------------------------------------------------\n");
    printf("JEZELI CHCESZ ZAPOZNAC SIE INSTRUKCJA OBSLUGI\n");
    printf("APLIKACJI BANKOWEJ PO ZALOGOWANIU WPISZ 'help'\n");
    printf("-----------------------------------------------------\n");

    while (L != 1) {
        printf("Logowanie\n");
        printf("LOGIN:\n");
        char login[20];
        scanf("%s", login);
        send(clientSocket, login, strlen(login), 0);

        printf("HASLO\n");
        char haslo[20];
        scanf("%s", haslo);
        send(clientSocket, haslo, strlen(haslo), 0);

        bzero(buffer, sizeof(buffer));
        sleep(1);
        read(clientSocket, buffer, sizeof(buffer));

        if ((strcmp(buffer, "Log") == 0)) {
            L = 1;
            break;;
        } else {
            printf("%s\n", "Login lub haslo zostaly wprowadzone niepoprawnie");
            printf("%s\n", "SPROBUJ JESZCZE RAZ");
            clear();
            sleep(1);

        }

    }
    send(clientSocket, systemCashRequest, strlen(systemCashRequest), 0);
    read(clientSocket, imie, sizeof(imie));
    read(clientSocket, nazwisko, sizeof(nazwisko));
    read(clientSocket, &saldo, sizeof(float));

    clear();
    printf("%s\n", "SZACH-MAT BANK");
    printf("\n\nUZYTKOWNIK: %s %s", imie, nazwisko);
    printf("\nSTAN KONTA: %g PLN", saldo);

    puts("\nDYSPOZYCJE:");
    puts("status - STAN KONTA");
    puts("siano+ - WPLAC PIENIADZE");
    puts("siano- - WYPLAC PIENIADZE");
    puts("help - POMOC UZYTKOWNIKA");
    puts("exit - WYLOGUJ SIE\n");
    puts("WYBOR OPERACJI: \n");

    while (clientSocket) {
        printf("$>:");
        scanf("%s", buffer);
        send(clientSocket, buffer, strlen(buffer), 0);
        if ((strcmp(buffer, "status") == 0) || (strcmp(buffer, "siano+") == 0) || (strcmp(buffer, "siano-") == 0) ||
        (strcmp(buffer, "help") == 0) || (strcmp(buffer, "exit") == 0)) {

            //obsluga sprawdzenia stanu konta
            if ((strcmp(buffer, "status") == 0)) {
                sleep(1);
                printf("Aktualny stan konta uzytkownika -> %g\n",saldo);
            }

            //obsluga wplacenia siana
            if (strcmp(buffer, "siano+") == 0) {

                printf("Money[$->]");
                bzero(buffer, sizeof(buffer));
                scanf("%s", siano_plus);
                saldo = saldo + atof(siano_plus);
                sprintf(buffer, "%g", saldo);
                send(clientSocket, buffer, strlen(buffer), 0);
                sleep(1);
            }

            //obsluga wyplacenia siana
            if (strcmp(buffer, "siano-") == 0) {

                printf("Money[$->]");
                bzero(buffer, sizeof(buffer));
                scanf("%s", siano_plus);
                saldo = saldo - atof(siano_plus);
                sprintf(buffer, "%g", saldo);
                send(clientSocket, buffer, strlen(buffer), 0);
                sleep(1);
            }

            if (strcmp(buffer, "exit") == 0) {
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }
            if (strcmp(buffer, "help") == 0) {
                sprintf(path, "%s%s", Path, "/help.txt");
                //printf("%s",path);
                //printf("%s",Path);
                file = fopen(path, "r");
                if (file) {
                    while (fgets(helpbuffer, 255, file) > 0) {
                        printf("%s", helpbuffer);
                    }
                } else {
                    printf("%s\n", "Nie otworzono pliku z pomoca");
                }
                printf("\n\n");
                fclose(file);
                bzero(helpbuffer, sizeof(helpbuffer));
            }
        }
        else
        {
            printf("[?]Command not detected.\n");
        }
        bzero(buffer, sizeof(buffer));
        read(clientSocket, buffer, sizeof(buffer));
        if (strcmp(buffer, "Operation was successful") == 0) {
            printf("[+]Operation was successful.\n");
        }
        if (strcmp(buffer, "Command no detected") == 0) {
            printf("[?]Command not detected.\n");
        }

    }

    close(clientSocket);
    return 0;
}