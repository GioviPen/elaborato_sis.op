#include "common.h"

// Definizione dei codici di priorità
typedef enum {
    CODICE_ROSSO = 1,
    CODICE_ARANCIONE,
    CODICE_AZZURRO,
    CODICE_VERDE,
    CODICE_BIANCO = 5
} CodicePriorita;

// Struttura del paziente da anagrafica
typedef struct {
    char nome[MAX_LEN];
    char cognome[MAX_LEN];
    char provincia[5];
    int eta;
    CodicePriorita priorita;
} Paziente;

// Struttura per rappresentare un paziente dimesso
typedef struct {
    Paziente paziente;
    time_t dataDimissione; // Aggiungi la data e l'orario della dimissione
} PazienteDimesso;

// Struttura della coda
typedef struct {
    Paziente* elementi;
    int capacita;
    int testa;
    int coda;
    int numeroElementi;
    sem_t semaforo;
    PazienteDimesso* dimessi;
    int dimessiCapacita;
    int dimessiNumeroElementi;
} CodaPazienti;

// Struttura contenente cinque code, una per ogni priorità
typedef struct {
    CodaPazienti code[5];
} CodePriorita;

// Struttura per rappresentare una richiesta del client
typedef struct {
    int client_id;
    int tipoRichiesta;
    Paziente paziente;
} RichiestaClient;

// Variabili globale
int shmid; // Identificatore della memoria condivisa
CodePriorita* shared_memall; // Puntatore alla memoria condivisa

// Prototipi delle funzioni
pid_t getPIDByName(const char* processName);
void inizializzaCoda(CodaPazienti* coda, int capacita);
int inizializzaConnessione();
void inviaRichiesta(int client_socket, int client_pid, Paziente* pazienti, int priocode);
void stampaElencoPazientiPerPriorita(CodePriorita* code);
void stampaElencoPazientiSpecifici(CodePriorita* code, int prio);
void gestisciAttivitaSpecifiche(Paziente* p);
void* threadGestorePriorita(void* arg);
void gestisciPriorita(Paziente* p);
Paziente estraiDaCoda(CodaPazienti* coda);
int getPaziente(CodaPazienti* coda, Paziente* paziente);
void dimettiPaziente(CodaPazienti* coda, Paziente* paziente);
void stampaElencoPazientiDimessi(CodaPazienti* coda);
void gestioneSegnaleUSR1(int sig, siginfo_t* info, void* context);
void inviaSegnalePersonalizzato(int client_pid);
//void inviaSegnaleAlarm(int client_pid);
void exec(int codicePrioritaClient, int checkPrimario, int terzoParametro);
void stampaCodiceMenu(int cod, int checkPrimario);
char getch();

int main() {
    system("clear");
    printf("Connessione in corso...\n");
    sleep(1);
    fflush(stdout);
    int client_socket = inizializzaConnessione();

    const char* processName = "primario";  // qui il nome del processo desiderato

    pid_t pid = getPIDByName(processName);
    if (pid != -1) {
        printf("Il PID del processo %s è %d\n", processName, pid); // Stampa di controllo
        printf("Premi un tasto per continuare...\n");
        fflush(stdout);
        getch();
    }
    else {
        printf("Il medico %s non e' connesso\nPremi un tasto per continuare ugualmente...\n", processName);
        fflush(stdout);
        getch();
    }
    
    struct sigaction sa;
    sa.sa_sigaction = gestioneSegnaleUSR1;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Errore nella registrazione della gestione del segnale");
        return 1;
    }

    pid_t client_id = getpid(); // Ottieni il PID del client    

    // Ricevi dal server le informazioni della memoria condivisa
    if (recv(client_socket, &shmid, sizeof(int), 0) == -1) {
        perror("Errore nella ricezione della shmid");
        exit(EXIT_FAILURE);
    } if(shmid == -1) {
		perror("Errore id memoria condivisa\n");
		exit(EXIT_FAILURE);
	}
    shared_memall = (CodePriorita*)shmat(shmid, NULL, 0); // Assegna la struttura ricevuta a shared_memall
    if (shared_memall == (CodePriorita*)-1) { // verifica collegamento alla memoria condivisa
        perror("Errore nel collegamento alla memoria condivisa");
        exit(EXIT_FAILURE);
    }

    int check = 0; // Registra la funzione di gestione del segnale
    if (pid == client_id)    check = 1;

    int codicePrioritaClient; //scelta del codice di priorità
    do {
        system("clear");
        fflush(stdout);
        printf("Connesso al Server!\n\n");
        printf("1| Codice Rosso\n");
        printf("2| Codice Arancione\n");
        printf("3| Codice Azzurro\n");
        printf("4| Codice Verde\n");
        printf("5| Codice Bianco\n");

        printf("\nScegli il codice di priorita' che verra' gestito da questo client: ");
        scanf("%d", &codicePrioritaClient);
        fflush(stdout);
        if (codicePrioritaClient < 1 || codicePrioritaClient > 5) {
            printf(ROSSO "\nCodice di priorita' non valido\n" RESET);
            sleep(1);
        }
    } while (codicePrioritaClient < 1 || codicePrioritaClient > 5);
    sleep(1);
    printf("Sincronizzo la coda con il server...\n");
    inizializzaCoda(&(shared_memall->code[codicePrioritaClient]), NUM_NOMI);
    char scelta;
    sleep(1);
    system("clear");
    fflush(stdout);
    do {
        exec(codicePrioritaClient, check, client_id);  // esegue il programma "menuclient"
        scelta = getch(); // Leggi la scelta dell'utente
        fflush(stdout);
        switch (scelta) {
            case '1': { // Elenco di tutti i Pazienti ottenuti
                system("clear"); 
                printf("| Elenco Tutti Pazienti |");
                stampaCodiceMenu(codicePrioritaClient, check);
                // Stampare a schermo l'elenco dei pazienti per ogni codice di priorità
                stampaElencoPazientiPerPriorita(shared_memall);
                printf("\nPremi un tasto per continuare...");
                fflush(stdout);
                getch();
                break;
            }
            case '2': { // Elenco dei Pazienti del solo codice priorità selezionato
                system("clear");
                printf("| Elenco Pazienti del Codice selezionato |");
                stampaCodiceMenu(codicePrioritaClient, check);
                // Stampare a schermo l'elenco dei pazienti per il codice di priorità selezionato
                stampaElencoPazientiSpecifici(shared_memall, codicePrioritaClient);
                printf("\nPremi un tasto per continuare...");
                fflush(stdout);
                getch();
                break;
            }
            case '3': { // Ottieni Paziente
                system("clear");
                printf("| Ottieni e gestisci Paziente |");
                stampaCodiceMenu(codicePrioritaClient, check);
                // Ottiene dalla coda del client un paziente da gestire (FIFO)
                Paziente paziente;
                int result = getPaziente(shared_memall->code, &paziente);

                if (result == -1) { // La coda è vuota
                    printf("Non ci sono pazienti in coda!");
                    printf("\nPremi un tasto per continuare...");
                    fflush(stdout);
                    getch();
                    break; // Termina il case
                }
                // Stampa a schermo le informazioni del paziente
                printf("\nNome: %s\n", paziente.nome);
                printf("Cognome: %s\n", paziente.cognome);
                printf("Provincia: %s\n", paziente.provincia);
                printf("Eta': %d\n", paziente.eta);
                printf("Codice di Priorita': %d\n", paziente.priorita);
                sleep(1);
                //gestione del paziente
                printf("\nInizio gestione del paziente...\n");
                sleep(1);
                // Gestione dei pazienti nella coda
                while (shared_memall->code[codicePrioritaClient].numeroElementi > 0) {
                    Paziente pazienteCorrente = estraiDaCoda(&shared_memall->code[codicePrioritaClient]);
                    gestisciPriorita(&pazienteCorrente);
                }
                sleep(1);
                printf("\nPremi un tasto per continuare...");
                fflush(stdout);
                getch();
                break;
            }
            case '4': { // Dimetti Paziente
                system("clear");
                printf("| Dimetti Paziente |");
                stampaCodiceMenu(codicePrioritaClient, check);
                //dimettiPaziente(&shared_memall->code[codicePrioritaClient], &shared_memall->code[codicePrioritaClient].elementi[0]);

                // Invia la richiesta al server
                inviaRichiesta(client_socket, client_id, &shared_memall->code[codicePrioritaClient].elementi[0], codicePrioritaClient);
                // ricevi la conferma dal server
                char conferma[3];
                if (recv(client_socket, conferma, sizeof(conferma), 0) == -1) {
                    perror("Errore nella ricezione della conferma dal server");
                    exit(EXIT_FAILURE);
                }
                printf("\nConferma dal server: %s\n", conferma); // Stampa la conferma a schermo
                if (strcmp(conferma, "OK") == 0) {
                    printf("Paziente dimesso con successo\n");
                } else printf("Errore nella dimissione del paziente\n");
                sleep(1);
                printf("\nSei stato disconnesso");
                printf("\nPremi un tasto per continuare...");
                fflush(stdout);
                getch();
                break;
            }
            case '5': { // Pazienti Dimessi
                system("clear");
                printf("| Pazienti Dimessi |");
                stampaCodiceMenu(codicePrioritaClient, check);
                // stampa l'elenco dei pazienti dimessi
                //stampaElencoPazientiDimessi(&shared_memall->code[codicePrioritaClient]);

                printf("\nPremi un tasto per continuare...");
                fflush(stdout);
                getch();
                break;
            }
            case '6': { // Richiedi Assistenza
                char risp;
                do { // invia segnali tra processi (altri client) e medico primario
                    system("clear");
                    printf("| Richiedi Assistenza |");
                    stampaCodiceMenu(codicePrioritaClient, check);
                    if (check == 1) {
                        printf(ROSSO "Sei tu il medico Primario!" RESET);
                        fflush(stdout);
                        sleep(2);
                        break;
                    }
                    printf("\nRichiedere assistenza al Medico primario? (usare SOLO se il primario si connette per primo)\n\n1| Si\n0| Menu' principale.\n\n");
                    fflush(stdout);
                    risp = getch();
                    if (risp != '1' && risp != '0') {  // Corretto da '1' e '0'
                        printf(ROSSO "Opzione non valida!" RESET);
                        fflush(stdout);
                        sleep(1);
                    }
                    else if (risp == '1') {
                        inviaSegnalePersonalizzato(pid);
                    }
                } while (risp != '0' && risp != '1');
                printf("\nPremi un tasto per tornare al menu principale...");
                fflush(stdout);
                getch();
                break;
            }
            case '0': { // Chiudi Programma
                printf("\n\n\nStai terminando il programma, inserisci nuovamente 0 per confermare: \n");
                fflush(stdout);
                scelta = getch();
                if (scelta != '0') {
                    printf("\nUscita annullata!\n");
                    fflush(stdout);
                    sleep(1);
                }
                break;
            }
            default: {
                printf("\n");
                printf(ROSSO "Scelta non valida" RESET);
                fflush(stdout);
                sleep(1);
                system("clear");
                break;
            }
        }
    } while (scelta != '0');

    if (close(client_socket) == -1) {// Chiusura del socket del client
        perror("Errore nella chiusura del socket del client");
        exit(EXIT_FAILURE);
    }
    if (shmdt(shared_memall) == -1) {// Chiusura della memoria condivisa
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {// Rimozione della memoria condivisa
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    printf("\n| Client concluso |\n\n");
    exit(EXIT_SUCCESS);
}

pid_t getPIDByName(const char* processName) {
    FILE* fp;
    char buffer[128];
    pid_t pid = -1;

    // Creare il comando per ottenere il PID del processo desiderato
    char command[256];
    snprintf(command, sizeof(command), "pgrep %s", processName);

    // Aprire il comando con popen
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Errore durante l'esecuzione del comando");
        exit(EXIT_FAILURE);
    }

    // Leggere il PID dal risultato del comando
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        pid = (pid_t)atoi(buffer);
    }

    pclose(fp);// Chiudere il file pointer
    return pid;
}

// Funzione per inizializzare la coda
void inizializzaCoda(CodaPazienti* coda, int capacita) {
    printf("Inizializzazione della coda...\n");
    coda->elementi = (Paziente*)malloc(capacita * sizeof(Paziente));
    if (coda->elementi == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    coda->capacita = capacita;
    coda->testa = 0;
    coda->coda = -1;
    coda->numeroElementi = 0;
    if (sem_init(&coda->semaforo, 0, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

// Funzione per inizializzare la connessione al server
int inizializzaConnessione() {
    int client_socket;
    struct sockaddr_in server_address;

    // Creazione del socket del client
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Errore nella creazione del socket del client");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);  // Indirizzo IP del server
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Indirizzo del server non valido o errore conversione");
        exit(EXIT_FAILURE);
    }

    // Connessione al server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

// Funzione per inviare la richiesta (di dimissioni) al server
void inviaRichiesta(int client_socket, int client_pid, Paziente* pazienti, int priocode) {
    RichiestaClient richiesta;
    richiesta.client_id = client_pid;
    richiesta.tipoRichiesta = priocode;
    richiesta.paziente = *pazienti;
    // Recupera il paziente dalla coda del client (assumendo che il paziente da dimettere sia in testa alla coda)
    /*richiesta.paziente = pazienti[0];*/

    printf("Client %d invio richiesta al server...\n", client_pid);
    if (send(client_socket, &richiesta, sizeof(RichiestaClient), 0) == -1) {
        perror("Errore nell'invio della richiesta al server");  
        exit(EXIT_FAILURE);
    }
}

// Funzione per stampare l'elenco dei pazienti per priorità
void stampaElencoPazientiPerPriorita(CodePriorita* code) {
    for (int codice = CODICE_ROSSO; codice <= CODICE_BIANCO; codice++) {
        int numPazientiPerCodice = 0;
        for (int i = 0; i < code->code[codice].numeroElementi; i++) {
            Paziente paziente = code->code[codice].elementi[i];
            if (paziente.priorita == codice) {
                numPazientiPerCodice++;
            }
        }
        if (numPazientiPerCodice > 0) {
            printf("\n\n%d pazienti con codice %s(%d):\n", numPazientiPerCodice, codice == CODICE_ROSSO ? "ROSSO" :
                (codice == CODICE_ARANCIONE ? "ARANCIONE" :
                    (codice == CODICE_AZZURRO ? "AZZURRO" :
                        (codice == CODICE_VERDE ? "VERDE" : "BIANCO"))), codice);
            for (int i = 0; i < code->code[codice].numeroElementi; i++) {
                Paziente paziente = code->code[codice].elementi[i];
                if (paziente.priorita == codice) {
                    printf("- %s %s\n", paziente.nome, paziente.cognome);
                }
            }
        }
        else {
            printf("\nNessun codice %s\n", codice == CODICE_ROSSO ? "ROSSO" :
                (codice == CODICE_ARANCIONE ? "ARANCIONE" :
                    (codice == CODICE_AZZURRO ? "AZZURRO" :
                        (codice == CODICE_VERDE ? "VERDE" : "BIANCO"))));
        }
    }
}

void stampaElencoPazientiSpecifici(CodePriorita* code, int prio) {
        int numPazientiPerCodice = 0;
        for (int i = 0; i < code->code[prio].numeroElementi; i++) {
            Paziente paziente = code->code[prio].elementi[i];
            if (paziente.priorita == prio) {
                numPazientiPerCodice++;
            }
        }
        if (numPazientiPerCodice > 0) {
            printf("\n\n%d pazienti con codice %s(%d):\n", numPazientiPerCodice, prio == CODICE_ROSSO ? "ROSSO" :
                (prio == CODICE_ARANCIONE ? "ARANCIONE" :
                    (prio == CODICE_AZZURRO ? "AZZURRO" :
                        (prio == CODICE_VERDE ? "VERDE" : "BIANCO"))), prio);
            for (int i = 0; i < code->code[prio].numeroElementi; i++) {
                Paziente paziente = code->code[prio].elementi[i];
                if (paziente.priorita == prio) {
                    printf("- %s %s\n", paziente.nome, paziente.cognome);
                }
            }
        }
        else {
            printf("\nNessun codice %s\n", prio == CODICE_ROSSO ? "ROSSO" :
                (prio == CODICE_ARANCIONE ? "ARANCIONE" :
                    (prio == CODICE_AZZURRO ? "AZZURRO" :
                        (prio == CODICE_VERDE ? "VERDE" : "BIANCO"))));
        }
}

// Funzione per la gestione delle attività specifiche per il paziente
void gestisciAttivitaSpecifiche(Paziente* p) {
    // Simulazione di un'attività separata (ad esempio, visita medica) utilizzando fork
    pid_t pid = fork();
    if (pid == -1) {
        perror(ROSSO "Errore durante la fork" RESET);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Processo figlio: gestisci l'attività specifica (cartella clinica, visita medica, azioni, ecc.)
        printf("Processo [%d] per gestire il paziente %s %s\n", getpid(), p->nome, p->cognome);
        sleep(1);  // Simulazione robe?
        printf("Inizio visita medica per il paziente %s %s\n", p->nome, p->cognome);
        sleep(2);  // Simulazione della visita medica
        printf("Paziente %s %s dimesso: processo [%d] terminato\n", p->nome, p->cognome, getpid());
        exit(0);  // Termina il processo figlio
    }
    else {
        // Processo genitore: attende la terminazione del processo figlio
        waitpid(pid, NULL, 0);
        printf("Medico [%d] continua dopo aver dimesso %s %s\n", getpid(), p->nome, p->cognome);
    }
}
// Funzione ausiliaria che contiene la logica specifica per ogni livello di priorità
void* threadGestorePriorita(void* arg) {
    Paziente* p = (Paziente*)arg;
    // definizione delle azioni specifiche per ciascuna priorità
    switch (p->priorita) {
    case CODICE_ROSSO:
        printf("\nPaziente %s %s con priorita' ROSSA\n", p->nome, p->cognome);
        // qui la logica specifica per il codice ROSSO - emergenza
        printf("-tempo medio di permanenza in ospedale 4gg: attendi\n");
        break;
    case CODICE_ARANCIONE:
        printf("\nPaziente %s %s con priorita' ARANCIONE\n", p->nome, p->cognome);
        // qui la logica specifica per il codice ARANCIONE - urgenza
        printf("-tempo medio di permanenza in ospedale 2gg: attendi\n");
        break;
    case CODICE_AZZURRO:
        printf("\nPaziente %s %s con priorita' AZZURRO\n", p->nome, p->cognome);
        // qui la logica specifica per il codice AZZURRO - urgenza differibile
        printf("-tempo medio di permanenza in ospedale 1gg: attendi\n");
        break;
    case CODICE_VERDE:
        printf("\nPaziente %s %s con priorita' VERDE\n", p->nome, p->cognome);
        // qui la logica specifica per il codice VERDE - urgenza minore
        printf("-tempo medio di permanenza in ospedale 12h: attendi\n");
        break;
    case CODICE_BIANCO:
        printf("\nPaziente %s %s con priorita' BIANCO\n", p->nome, p->cognome);
        // qui la logica specifica per il codice BIANCO - non urgenza
        printf("-tempo medio di permanenza in ospedale 2h: attendi\n");
        break;
    default:
        printf("Codice di priorita' non valido per il paziente %s %s\n", p->nome, p->cognome);
        break;
    }
    pthread_exit(NULL);
}
// Funzione per gestire i codici di priorità: crea un thread per ogni paziente e gestisce la sua priorità.
void gestisciPriorita(Paziente* p) {
    pthread_t thread;  // Dichiarazione dell'identificatore del thread

    // Creazione del thread e passaggio del paziente come argomento
    if (pthread_create(&thread, NULL, threadGestorePriorita, (void*)p) != 0) {
        fprintf(stderr, "Errore durante la creazione del thread per il paziente %s %s\n", p->nome, p->cognome);
        return;
    }

    // Attendere la terminazione del thread prima di continuare
    if (pthread_join(thread, NULL) != 0) {
        fprintf(stderr, "Errore durante l'attesa della terminazione del thread per il paziente %s %s\n", p->nome, p->cognome);
        return;
    }

    // Eventuale ulteriore terminazione del thread
    pthread_detach(thread);

    // Chiamare la funzione per gestire le attività specifiche in un processo figlio
    gestisciAttivitaSpecifiche(p);
}

// Funzione per estrarre un paziente dalla coda
Paziente estraiDaCoda(CodaPazienti* coda) {
    sem_wait(&coda->semaforo);  // Wait sul semaforo

    Paziente pazienteEstratto = coda->elementi[coda->testa];
    coda->testa = (coda->testa + 1) % coda->capacita;
    coda->numeroElementi--;

    sem_post(&coda->semaforo);  // Signal sul semaforo

    return pazienteEstratto;
}

// Funzione per ottenere un paziente dalla coda
int getPaziente(CodaPazienti* coda, Paziente* paziente) {
    if (sem_wait(&coda->semaforo) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
    if (coda->numeroElementi == 0) {
        // La coda è vuota, restituisci un segnale speciale
        printf("\nLa coda è vuota\n");
        if (sem_post(&coda->semaforo) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
        return -1; // Indica che la coda è vuota
    }
    else {
        // La coda contiene elementi, procedi normalmente
        *paziente = coda->elementi[coda->testa];
        coda->testa = (coda->testa + 1) % coda->capacita;
        coda->numeroElementi--;
        if (sem_post(&coda->semaforo) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
        return 0; // Indica che l'operazione è andata a buon fine
    }
}

void dimettiPaziente(CodaPazienti* coda, Paziente* paziente) {
    PazienteDimesso pazienteDimesso;
    pazienteDimesso.paziente = *paziente;
    pazienteDimesso.dataDimissione = time(NULL);

    if (coda->dimessiNumeroElementi < coda->dimessiCapacita) {
        coda->dimessi[coda->dimessiNumeroElementi++] = pazienteDimesso;
    }
    else {
        // Gestisci il caso in cui la lista dei pazienti dimessi è piena
        printf("La lista dei pazienti dimessi è piena. Impossibile aggiungere nuovi pazienti.\n");
    }
}

// Funzione per stampare l'elenco dei pazienti dimessi
void stampaElencoPazientiDimessi(CodaPazienti* coda) {
    if (coda->dimessiNumeroElementi > 0) {
        printf("\n\nElenco pazienti dimessi:\n");
        for (int i = 0; i < coda->dimessiNumeroElementi; i++) {
            PazienteDimesso pazienteDimesso = coda->dimessi[i];
            printf("- %s %s (Dimesso il %s)", pazienteDimesso.paziente.nome, pazienteDimesso.paziente.cognome, ctime(&pazienteDimesso.dataDimissione));
        }
    }
    else {
        printf("\nNessun paziente dimesso\n");
    }
}

void gestioneSegnaleUSR1(int sig, siginfo_t* info, void* context) {
    if (sig == SIGUSR1) {
        printf(ROSSO "\n\nATTENZIONE, RICHIESTA ASSISTENZA DA %ld!\n" RESET, (long int)info->si_pid);
        fflush(stdout);
        getch();
    }
}

void inviaSegnalePersonalizzato(int client_pid) {
    if (kill(client_pid, SIGUSR1) == -1) {
        perror("Errore nell'invio del segnale personalizzato");
        exit(EXIT_FAILURE);
    }
    printf(VERDE "Richiesta di assistenza inviata al medico primario: %ld" RESET"\n", (long int)client_pid);
}

void exec(int codicePrioritaClient, int checkPrimario, int terzoParametro) {
    pid_t childPid = fork();
    if (childPid == -1) {
        perror("Errore nella fork");
        exit(EXIT_FAILURE);
    }
    if (childPid == 0) {
        // Questo è il processo figlio
        char codicePrioritaClientStr[5];
        char checkPrimarioStr[5];
        char terzoParametroStr[5];

        snprintf(codicePrioritaClientStr, sizeof(codicePrioritaClientStr), "%d", codicePrioritaClient);
        snprintf(checkPrimarioStr, sizeof(checkPrimarioStr), "%d", checkPrimario);
        snprintf(terzoParametroStr, sizeof(terzoParametroStr), "%d", terzoParametro);

        // Esegui il nuovo programma con execl
        execl("./menuclient", "menuclient", codicePrioritaClientStr, checkPrimarioStr, terzoParametroStr, NULL);
    }
    else { // Questo è il processo genitore
        wait(NULL);// Attendere che il processo figlio termini
    }
}

void stampaCodiceMenu(int cod, int checkPrimario) {
    if (checkPrimario == 1) {
        switch (cod) {
        case 1:
            printf(" Gestione codice " ROSSO "ROSSO " RESET "|" GIALLO" Medico Primario "RESET"|\n");
            break;
        case 2:
            printf(" Gestione codice " ARANCIONE "ARANCIONE " RESET "|" GIALLO" Medico Primario "RESET"|\n");
            break;
        case 3:
            printf(" Gestione codice " AZZURRO "AZZURRO " RESET "|" GIALLO" Medico Primario "RESET"|\n");
            break;
        case 4:
            printf(" Gestione codice " VERDE "VERDE " RESET "|" GIALLO" Medico Primario "RESET"|\n");
            break;
        case 5:
            printf(" Gestione codice BIANCO " RESET "|" GIALLO" Medico Primario "RESET"|\n");
            break;
        }
    }
    else {
        switch (cod) {
        case 1:
            printf(" Gestione codice " ROSSO "ROSSO" RESET" | Pronto Soccorso |\n");
            break;
        case 2:
            printf(" Gestione codice " ARANCIONE "ARANCIONE" RESET" | Pronto Soccorso |\n");
            break;
        case 3:
            printf(" Gestione codice " AZZURRO "AZZURRO" RESET" | Pronto Soccorso |\n");
            break;
        case 4:
            printf(" Gestione codice " VERDE "VERDE" RESET" | Pronto Soccorso |\n");
            break;
        case 5:
            printf(" Gestione codice BIANCO | Pronto Soccorso |\n");
            break;
        }
    }
}

char getch() {
    char buf = 0;
    struct termios old = { 0 }, new_termios;

    // Salva la configurazione corrente del terminale
    if (tcgetattr(STDIN_FILENO, &old) < 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }   

    new_termios = old;

    // Imposta il terminale in modalità "raw" senza buffering
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;

    // Applica le nuove impostazioni del terminale
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) < 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    // Leggi un singolo carattere
    if (read(STDIN_FILENO, &buf, 1) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Ripristina le impostazioni originali del terminale
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old) < 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    return buf;
}