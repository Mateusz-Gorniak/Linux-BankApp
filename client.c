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


#define PORT 8080
#define clear() printf("\033[H\033[J")
char systemCashRequest[] ="account_state";
char imie[30];
char nazwisko[30];
float saldo;

typedef enum {
    false, true
} bool;

int main() {
    int L=0; // flaga logged
    int clientSocket, ret;//uchwyt do socketa
    struct sockaddr_in serverAddr;
    char buffer[1024];
    bool logged = false;

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

    while (L!=1) {
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
        read(clientSocket, buffer,sizeof(buffer));

        if ((strcmp(buffer, "Log") == 0)) {
            L=1;
            break;;
        }
        else{
            printf("%s\n","Login lub haslo zostaly wprowadzone niepoprawnie");
            printf("%s\n","SPROBUJ JESZCZE RAZ");
            clear();
            sleep(1);

        }

    }
    send(clientSocket, systemCashRequest, strlen(systemCashRequest), 0);
    read(clientSocket,imie,sizeof (imie));
    read(clientSocket,nazwisko,sizeof (nazwisko));
    read(clientSocket,&saldo,sizeof(float));

    clear();
    printf("%s\n", "SZACH-MAT BANK");
    printf("\n\nUZYTKOWNIK: %s %s",imie,nazwisko);
    printf("\nSTAN KONTA: %g PLN",saldo);

    puts("\n\nDYSPOZYCJE:");
    puts("1 - STAN KONTA");
    puts("2 - WPLAC PIENIADZE");
    puts("3 - WYPLAC PIENIADZE");
    puts("logout - WYLOGUJ SIE\n");
    puts("WYBOR OPERACJI: \n");
    
    while (1) {
        printf("$>:");
        scanf("%s", buffer);
        send(clientSocket, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "logout") == 0) {
            close(clientSocket);
            printf("[-]Disconnected from server.\n");
            exit(1);
        }
        bzero(buffer,sizeof(buffer));
        read(clientSocket, buffer,sizeof(buffer));
        if (strcmp(buffer, "Command no detected") == 0) {
            printf("[?]Command not detected.\n");
        }
    }

    close(clientSocket);
    return 0;
}


