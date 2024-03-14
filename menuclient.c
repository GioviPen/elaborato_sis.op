#include "common.h"

// Posizione iniziale del cursore (per goto) e parametri orologio
int currentX = 1;
int currentY = 1;

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
    currentX = x;
    currentY = y;
}

int main(int argc, char* argv[]) {
    char* endptr1;
    char* endptr2;
    char* endptr3;

    if (argc != 4) {
        fprintf(stderr, "Utilizzo: %s <codicePrioritaClient> <check> <terzoParametro>\n", argv[0]);
        return 1;
    }

    long codicePrioritaClient = strtol(argv[1], &endptr1, 10);
    long check = strtol(argv[2], &endptr2, 10);
    long pid = strtol(argv[3], &endptr3, 10);

    // Verifica eventuali errori di conversione
    if (*endptr1 != '\0' || *endptr2 != '\0' || *endptr3 != '\0') {
        fprintf(stderr, "Errore di conversione dei parametri.\n");
        return 1;
    }

    system("clear"); //printf("\033[H\033[J"); // Pulisce il terminale
    if (check == 1) {
        switch (codicePrioritaClient) {
        case 1:
            printf("Benvenuto nel Client - " GIALLO "Medico Primario %ld" RESET " - Gestione codice " ROSSO "ROSSO" RESET, pid);
            break;

        case 2:
            printf("Benvenuto nel Client - " GIALLO "Medico Primario %ld" RESET " - Gestione codice " ARANCIONE "ARANCIONE" RESET, pid);
            break;

        case 3:
            printf("Benvenuto nel Client - " GIALLO "Medico Primario %ld" RESET " - Gestione codice " AZZURRO "AZZURRO" RESET, pid);
            break;

        case 4:
            printf("Benvenuto nel Client - " GIALLO "Medico Primario %ld" RESET " - Gestione codice " VERDE "VERDE" RESET, pid);
            break;

        case 5:
            printf("Benvenuto nel Client - " GIALLO "Medico Primario %ld" RESET " - Gestione codice BIANCO", pid);
            break;
        }
    }
    else {
        switch (codicePrioritaClient) {
        case 1:
            printf("Benvenuto nel Client %ld - Pronto Soccorso - Gestione codice " ROSSO "ROSSO" RESET, pid);
            break;

        case 2:
            printf("Benvenuto nel Client %ld - Pronto Soccorso - Gestione codice " ARANCIONE "ARANCIONE" RESET, pid);
            break;

        case 3:
            printf("Benvenuto nel Client %ld - Pronto Soccorso - Gestione codice " AZZURRO "AZZURRO" RESET, pid);
            break;

        case 4:
            printf("Benvenuto nel Client %ld - Pronto Soccorso - Gestione codice " VERDE "VERDE" RESET, pid);
            break;

        case 5:
            printf("Benvenuto nel Client %ld - Pronto Soccorso - Gestione codice BIANCO", pid);
            break;
        }
    }

    gotoxy(3, 3);
    printf("1 | Elenco Tutti Pazienti"); // Elenco dei pazienti in attesa per priorità
    gotoxy(3, 4);
    printf("2 | Elenco Pazienti codice selezionato"); // Elenco dei pazienti in attesa per priorità
    gotoxy(3, 5);
    printf("3 | Ottieni e gestisci Paziente"); // Ottiene un paziente da gestire
    gotoxy(3, 6);
    printf("4 | Dimetti Paziente"); // Dimette il paziente gestito, lo inserisce nella lista dei dimessi e lo rimuove dalla coda
    gotoxy(3, 7);
    printf("5 | Pazienti Dimessi"); // Elenco dei pazienti dimessi
    gotoxy(3, 8);
    printf("6 | Richiedi Assistenza"); // Richiesta di assistenza tramite segnali tra processi medici
    gotoxy(3, 9);
    printf("0 | Chiudi Programma\n");
    fflush(stdout);

    return 0;
}