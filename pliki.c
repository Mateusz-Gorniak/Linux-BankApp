#include<stdio.h>
#include<string.h>
#include <stdlib.h>

struct Klient{
char login[30];
char haslo[30];
char imie[30];
char nazwisko[30]; 
float saldo; 

}typedef Klient;

int main(){

	FILE *plik;
	int liczba_uzytkownikow;
	int i =0;
	Klient klient[100];

        plik= fopen( "/home/student/PULX /PROJEKT_LINUX/clients.txt","r" ) ;
	if (!plik){
	   fprintf(stderr , "Nie mozna otworzyc pliku\n");
            exit(1);
	}
	else{
	printf("Otworzono plik\n");
	}
	while(fscanf(plik,"%s %s %g %s %s",klient[i].imie,klient[i].nazwisko,&klient[i].saldo,klient[i].login,klient[i].haslo)>0) {
	++liczba_uzytkownikow;	
        printf("%s %s %g %s %s\n",klient[i].imie,klient[i].nazwisko,klient[i].saldo,klient[i].login,klient[i].haslo);
	i++;
	}
        printf("Liczba uzytkownikow: %d\n",liczba_uzytkownikow);
	printf("Saldo klienta 1: %g\n",klient[0].saldo);
	printf("Imie klienta 1: %s\n",klient[0].imie);
	printf("Login klienta 1: %s\t",klient[0].login);
	printf("Haslo klienta 1: %s\t",klient[0].haslo);
	fclose(plik);
return 0;
}
