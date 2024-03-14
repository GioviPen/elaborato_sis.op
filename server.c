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
	char provincia[4];
	int eta;
	CodicePriorita priorita;
} Paziente;

// Struttura della coda
typedef struct {
	Paziente* elementi;
	int capacita;
	int testa;
	int coda;
	int numeroElementi;
	sem_t semaforo;
} CodaPazienti;

// Struttura contenente cinque code, una per ogni priorità
typedef struct {
	CodaPazienti code[5];
} CodePriorita;

// Struttura per rappresentare una richiesta del client
typedef struct {
	int client_id; 
	int tipoRichiesta; // indica il codice di priorità (indice della coda)
	Paziente paziente;
} RichiestaClient;

//semafori
sem_t* mutex;
sem_t* empty;
sem_t* full;

// Variabili globali
CodePriorita* shared_memall;
int shmid; // Identificatore della memoria condivisa
int giorno = 1; // Variabile per memorizzare il giorno
int ore = 0, minuti = 0; // Variabili per memorizzare l'ora e i minuti
int currentX = 1, currentY = 1; // Posizione iniziale del cursore (per goto)
// Dichiarazione del mutex orologio e della condizione di terminazione
pthread_mutex_t muto = PTHREAD_MUTEX_INITIALIZER; // Mutex per l'accesso alla console
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Variabile di condizione per la terminazione del thread dell'orologio

// Prototipi delle funzioni
void inizializzaCoda(CodaPazienti* coda, int capacita);
void inizializzaCodePriorita(CodePriorita* code, int capacita);
void inserisciInCoda(CodaPazienti* coda, Paziente* paziente);
void inserisciInCodaPriorita(CodePriorita* code, Paziente* paziente);
void stampaElencoPazientiPerPriorita(CodePriorita* code);
Paziente estraiDaCoda(CodaPazienti* coda);
Paziente estraiDaCodaPriorita(CodePriorita* code, CodicePriorita priorita);
void rimuoviPaziente(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente pazienteRim);
bool confrontaPazienti(const Paziente* p1, const Paziente* p2);
int generaNumeroCasuale(int minimo, int massimo);
void inserisciPaziente(CodaPazienti* coda, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente);
void inserisciPazientePriorita(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente);
Paziente caricaCasuale(FILE* file, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti);
void caricaTuttoFile(FILE* file, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti);
CodicePriorita assegnaPrioritaCasuale();

void gestisciConnessioneClient(int client_socket, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti);
void gestisciServer(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti);

int create_shared_memory(size_t size);
void* attach_shared_memory(int shmid);
void initialize_semaphores();
void distruggiSemafori(CodePriorita* codePriorita);
void liberaMemoriaCondivisa();

void tempoScorre();
void* orologio(void* args);
void gotoxy(int x, int y);

int main() {
	// Inizializzazione del generatore di numeri casuali
	srand(time(NULL));
	system("clear");
	// Creazione e attach della memoria condivisa
	shmid = create_shared_memory(sizeof(CodePriorita));
	shared_memall = (CodePriorita*)attach_shared_memory(shmid);

	initialize_semaphores(); // Inizializzazione dei semafori

	FILE* fileAnagrafica = NULL;
	if ((fileAnagrafica = fopen("anagrafica.txt", "r")) == NULL) {
		perror(ROSSO "apertura del file\n" RESET);
		fflush(stdout);
		return -1;
	}

	gotoxy(1, 1); // posiziona il cursore e stampa il tempo
	printf("========== Pazienti In Arrivo ==========");
	pthread_t orologio_tid; // Crea il thread per l'orologio
	pthread_mutex_init(&muto, NULL); // Inizializza il mutex
	pthread_cond_init(&cond, NULL); // Inizializza la variabile di condizione
	if (pthread_create(&orologio_tid, NULL, orologio, NULL) != 0) {
		perror(ROSSO "pthread_create orologio" RESET);
		exit(EXIT_FAILURE);
	}
	CodePriorita codePriorita;// Creazione delle code di priorità
	inizializzaCodePriorita(&codePriorita, BUFFER_SIZE);

	// Lettura dei pazienti dal file e assegnazione priorita
	int numPazienti = 0; //totale

	// Array per tenere traccia dei pazienti per ogni codice di priorità
	Paziente elencoPazienti[NUM_NOMI][5] = { 0 }; // 5 è il numero di codici di priorità
	sleep(1);
	for (int i = 0; i < (5 + rand() % NUM_NOMI); i++) { //carica un numero casuale di pazienti
		caricaCasuale(fileAnagrafica, &codePriorita, elencoPazienti, &numPazienti);
	} printf("\nPazienti caricati: %d\n", numPazienti);
	for (int i = 0; i < 5; i++) { //salva le cinque code nella memoria condivisa
		memcpy(&(shared_memall->code[i]), &(codePriorita.code[i]), sizeof(CodaPazienti));
	} printf("Code salvate nella memoria condivisa");
	stampaElencoPazientiPerPriorita(&codePriorita);
	gestisciServer(&codePriorita, elencoPazienti, &numPazienti); // Avvio del server
	// Il resto del codice non sarà eseguito finché il server è in esecuzione

	pthread_mutex_lock(&muto);
	pthread_cond_wait(&cond, &muto); // Attende la notifica dal thread dell'orologio
	pthread_mutex_unlock(&muto);
	pthread_cond_destroy(&cond); // Distrugge la variabile di condizione
	// Chiusura del thread dell'orologio
	if (pthread_join(orologio_tid, NULL) != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	if (pthread_cancel(orologio_tid) != 0) {
		perror("pthread_cancel");
		exit(EXIT_FAILURE);
	}
	fclose(fileAnagrafica);
	distruggiSemafori(&codePriorita);  // Liberazione delle risorse del semaforo
	pthread_mutex_destroy(&muto); // Distrugge il mutex prima di uscire
	liberaMemoriaCondivisa(); // Libera la memoria condivisa
	for (int i = 0; i < 5; i++) { //libera la memoria allocata per le code
		free(codePriorita.code[i].elementi);
	}
	printf("\nProgramma terminato con successo.\n");
	return 0;
}

// Funzione per inizializzare la coda
void inizializzaCoda(CodaPazienti* coda, int capacita) {
	coda->elementi = (Paziente*)malloc(capacita * sizeof(Paziente));
	if (coda->elementi == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	coda->capacita = capacita;
	coda->testa = 0;
	coda->coda = -1;
	coda->numeroElementi = 0;
	if (sem_init(&coda->semaforo, 0, 1) == -1) {// Inizializzazione del semaforo per l'accesso alla coda
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
}

// Funzione per inizializzare le 5 code di priorità
void inizializzaCodePriorita(CodePriorita* code, int capacita) {
	for (int i = 0; i < 5; i++) {
		inizializzaCoda(&(code->code[i]), capacita);
	}
}

// Funzione per inserire un paziente nella coda
void inserisciInCoda(CodaPazienti* coda, Paziente* paziente) {
	sem_wait(&coda->semaforo);  // Wait sul semaforo, garantisce mutua esclusione
	if (coda->numeroElementi == coda->capacita) {
		gotoxy(50, 7);
		printf(GIALLO "La coda %d e' piena, aumento la capacita'\n" RESET, paziente->priorita);
		fflush(stdout);
		sleep(3);
		gotoxy(50, 7);
		printf("                                                     ");
		coda->capacita *= 2;
		coda->elementi = (Paziente*)realloc(coda->elementi, coda->capacita * sizeof(Paziente));
	}
	coda->coda = (coda->coda + 1) % coda->capacita;
	coda->elementi[coda->coda] = *paziente;
	coda->numeroElementi++;
	sem_post(&coda->semaforo);  // Signal sul semaforo (rilascio)
}

// Funzione per inserire un paziente nella coda corrispondente alla sua priorità
void inserisciInCodaPriorita(CodePriorita* code, Paziente* paziente) {
	inserisciInCoda(&(code->code[paziente->priorita - 1]), paziente);
}

void stampaElencoPazientiPerPriorita(CodePriorita* code) {
	for (int codice = CODICE_ROSSO; codice <= CODICE_BIANCO; codice++) {
		int numPazientiPerCodice = 0;
		/*for (int i = 0; i < numPazienti; i++)
			if (elencoPazienti[i][codice].priorita == codice)*/
		for (int i = 0; i < code->code[codice - 1].numeroElementi; i++) {
			Paziente paziente = code->code[codice - 1].elementi[i];
			if (paziente.priorita == codice) {
				numPazientiPerCodice++;
			}
		}
		if (numPazientiPerCodice > 0) {
			printf("\n\n%d pazienti con codice %s(%d):\n", numPazientiPerCodice, codice == CODICE_ROSSO ? "ROSSO" :
				(codice == CODICE_ARANCIONE ? "ARANCIONE" :
					(codice == CODICE_AZZURRO ? "AZZURRO" :
						(codice == CODICE_VERDE ? "VERDE" : "BIANCO"))), codice);
			/*for (int i = 0; i < numPazienti; i++) {
				if (elencoPazienti[i][codice].priorita == codice) {
					printf("- %s %s\n", elencoPazienti[i][codice].nome, elencoPazienti[i][codice].cognome);*/
			for (int i = 0; i < code->code[codice - 1].numeroElementi; i++) {
				Paziente paziente = code->code[codice - 1].elementi[i];
				if (paziente.priorita == codice) {
					printf("- %s %s\n", paziente.nome, paziente.cognome);
				}
			}
		}
		else printf("\nNessun codice %s\n", codice == CODICE_ROSSO ? "ROSSO" :
			(codice == CODICE_ARANCIONE ? "ARANCIONE" :
				(codice == CODICE_AZZURRO ? "AZZURRO" :
					(codice == CODICE_VERDE ? "VERDE" : "BIANCO"))));
	}
}


// Funzione per estrarre un paziente dalla coda generale
Paziente estraiDaCoda(CodaPazienti* coda) {
	Paziente pazienteEstratto;
	sem_wait(&coda->semaforo);  // Wait sul semaforo
	if (coda->numeroElementi == 0) {
		gotoxy(50, 7);
		printf("La coda è vuota, impossibile estrarre elementi.\n");
	}
	else {
		pazienteEstratto = coda->elementi[coda->testa];
		coda->testa = (coda->testa + 1) % coda->capacita;
		coda->numeroElementi--;
	}
	sem_post(&coda->semaforo);  // Signal sul semaforo
	return pazienteEstratto;
}

// Funzione per estrarre un paziente dalla coda corrispondente alla sua priorità
Paziente estraiDaCodaPriorita(CodePriorita* code, CodicePriorita priorita) {
	return estraiDaCoda(&(code->code[priorita - 1]));
}

void rimuoviPaziente(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente pazienteRim) {
	CodicePriorita priorita = pazienteRim.priorita;

	if (priorita < CODICE_ROSSO || priorita > CODICE_BIANCO) {
		gotoxy(50, 7);
		printf(ROSSO "Errore: Codice di priorita' %d non valido." RESET, priorita);
		fflush(stdout);
		return;
	}
	if (code->code[priorita - 1].numeroElementi == 0) { //se la coda è vuota
		gotoxy(50, 7);
		printf(ROSSO "Errore: Nessun paziente trovato nella coda di priorita' %d." RESET, priorita);
		fflush(stdout);
		return;
	}

	Paziente pazienteEstratto = estraiDaCodaPriorita(code, priorita); // Rimuovi il paziente dalla coda corrispondente alla priorita

	if (memcmp(&pazienteEstratto, &pazienteRim, sizeof(Paziente)) == 0) {
		gotoxy(50, 5);
		printf("                                                     ");
		printf("Paziente rimosso con successo.");
	} //utilizzare confrontaPazienti 
	else {
		gotoxy(50, 5);
		printf("                                                     ");
		printf("Paziente non corrispondente.  ");
	}
}

bool confrontaPazienti(const Paziente* p1, const Paziente* p2) {
	return strcmp(p1->nome, p2->nome) == 0 &&
		strcmp(p1->cognome, p2->cognome) == 0 &&
		strcmp(p1->provincia, p2->provincia) == 0 &&
		p1->eta == p2->eta &&
		p1->priorita == p2->priorita;
}

// Funzione per generare un numero casuale compreso tra minimo e massimo
int generaNumeroCasuale(int minimo, int massimo) {
	return rand() % (massimo - minimo + 1) + minimo;
}

// Funzione per inserire il paziente nella coda e nell'elenco
void inserisciPaziente(CodaPazienti* coda, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente) {
	paziente->priorita = assegnaPrioritaCasuale(); // Assegnazione casuale del codice di priorità
	elencoPazienti[*numPazienti][paziente->priorita] = *paziente; // Aggiungo il paziente all'elenco corrispondente al suo codice di priorità
	inserisciInCoda(coda, paziente); // Inserimento del paziente nella coda
	(*numPazienti)++;
	printf("Paziente inserito nella coda.\n");
}

// Funzione per inserire il paziente nella coda e nell'elenco
void inserisciPazientePriorita(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente) {
	paziente->priorita = assegnaPrioritaCasuale(); // Assegnazione casuale del codice di priorità
	elencoPazienti[*numPazienti][paziente->priorita - 1] = *paziente; // Aggiungo il paziente all'elenco corrispondente al suo codice di priorità
	inserisciInCodaPriorita(code, paziente); // Inserimento del paziente nella coda
	(*numPazienti)++;
}

// Caricamento casuale di un solo paziente dal file
Paziente caricaCasuale(FILE* file, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	Paziente paziente;
	int nRand = rand() % NUM_NOMI;
	rewind(file); // Riavvolgi il file all'inizio
	for (int riga = 0; riga < nRand; riga++) {
		char buffer[MAX_LEN * 4];
		if (fgets(buffer, sizeof(buffer), file) == NULL) {
			gotoxy(50, 7);
			printf(ROSSO "Errore durante la lettura del file alla riga %d\n" RESET, nRand);
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
	}
	if (fscanf(file, "%s %s %s %d", paziente.nome, paziente.cognome, paziente.provincia, &paziente.eta) == 4) {
		inserisciPazientePriorita(code, elencoPazienti, numPazienti, &paziente);
	}
	else { // Se non riesce a leggere correttamente i dati
		gotoxy(50, 7);
		printf(ROSSO "Errore durante la lettura del paziente dalla riga %d\n" RESET, nRand);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	return paziente;
}

// Funzione per caricare tutti i pazienti dal file (lettura)
void caricaTuttoFile(FILE* file, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	Paziente paziente;
	*numPazienti = 0;
	rewind(file); // Riavvolgi il file all'inizio
	while (*numPazienti < NUM_NOMI && !feof(file)) {
		if (fscanf(file, "%s %s %s %d", paziente.nome, paziente.cognome, paziente.provincia, &paziente.eta) == 4) {
			inserisciPazientePriorita(code, elencoPazienti, numPazienti, &paziente);
		}
	}
}

// Funzione per l'assegnazione casuale della priorita
CodicePriorita assegnaPrioritaCasuale() {
	return (CodicePriorita)generaNumeroCasuale(CODICE_ROSSO, CODICE_BIANCO);
}

// Funzione per gestire la connessione del client al server
void gestisciConnessioneClient(int client_socket, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	RichiestaClient richiesta;
	richiesta.client_id = 0;
	richiesta.tipoRichiesta = 0;
	if (recv(client_socket, &richiesta, sizeof(RichiestaClient), 0) == -1) {// Ricevi la richiesta del client
		gotoxy(50, 7);
		perror(ROSSO "Errore nella ricezione della richiesta del client" RESET);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	gotoxy(50, 5);
	//printf("Server %d: Richiesta coda%d ricevuta dal client %d\n", getpid(), richiesta.tipoRichiesta, richiesta.client_id);  //debug
	fflush(stdout);
	if (richiesta.client_id <=0 || richiesta.tipoRichiesta<=0) {
		gotoxy(50, 7);
		printf(ROSSO "Errore: Richiesta non valida\n" RESET);
		sleep(2);
		fflush(stdout);
		return;
	}
	rimuoviPaziente(code, elencoPazienti, numPazienti, richiesta.paziente); //utilizare tipoRichiesta (priocode)
	gotoxy(50, 5);
	printf("Client %d: Paziente %s %s rimosso con successo\n", richiesta.client_id, richiesta.paziente.nome, richiesta.paziente.cognome);
	fflush(stdout);
	// Invia un messaggio di conferma al client
	if (send(client_socket, "OK", sizeof("OK"), 0) == -1) {
		gotoxy(50, 7);
		perror(ROSSO "Errore nell'invio della conferma al client" RESET);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	gotoxy(50, 10);
	printf("Conferma inviata al client %d\n", richiesta.client_id);
	fflush(stdout);
}

// Funzione per gestire il server
void gestisciServer(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	int server_socket, client_socket;
	struct sockaddr_in server_address, client_address;
	socklen_t client_address_len = sizeof(client_address);
	time_t ticks; // Variabile per memorizzare il tempo
	pid_t child_pid[MAX_CONNECTIONS]; // Array per tenere traccia dei processi figli
	int ascoltati = 0; // Numero di client ascoltati
	// Creazione del socket del server
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		gotoxy(50, 7);
		perror(ROSSO "Errore nella creazione del socket del server" RESET);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	// Inizializzazione della struttura dell'indirizzo del server
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(SERVER_IP);/*INADDR_ANY*/
	server_address.sin_port = htons(SERVER_PORT);
	// Associazione del socket all'indirizzo del server
	if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		gotoxy(50, 12);
		perror(ROSSO "Errore bind socket (associazione indirizzo server)" RESET);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	fflush(stdout);
	gotoxy(50, 3);
	printf("In attesa della connessione del client...\n");// Messaggio di attesa della connessione del client
	fflush(stdout);
	// Mettere in ascolto il socket del server per le connessioni in ingresso
	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
		gotoxy(50, 7);
		perror(ROSSO "Errore listen!" RESET);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	ascoltati++;
	gotoxy(50, 4);
	printf("Server in ascolto %d:%d...\n", ntohs(server_address.sin_addr.s_addr)/*SERVER_IP*/, server_address.sin_port/*SERVER_PORT*/);
	fflush(stdout);

	{
		// Handle SIGCHLD signal for reaping zombie processes
		struct sigaction sa;
		sa.sa_handler = SIG_IGN; // Ignore the signal
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("Error setting up SIGCHLD handler");
			close(server_socket);
			exit(EXIT_FAILURE);
		}
	}

	int i = 1;
	int nc = 0;
	while (i <= MAX_CONNECTIONS) {// Ciclo per gestire le connessioni dei client
		// Accetta la connessione del client
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len)) == -1) {
			gotoxy(50, 7);
			perror(ROSSO "Errore nella connessione del client" RESET);
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
		gotoxy(50, 3);
		printf("%d - Client %d connesso: %s                 \n", i, client_socket, inet_ntoa(client_address.sin_addr));
		gotoxy(50, 4);
		printf("                                      ");
		fflush(stdout);
		if (i >= MAX_CONNECTIONS) {
			gotoxy(0, 18);
			printf(ROSSO "Attenzione: Numero massimo di connessioni raggiunto." RESET);
			close(client_socket);
			continue;
		} i++;
		gotoxy(50, 8 + nc);
		printf("Connessione %d accettata dal client %s:%d\n", client_socket, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
		fflush(stdout);
		
		child_pid[i] = fork();
		if (child_pid[i] == -1) {
			gotoxy(50, 7);
			perror(ROSSO "Errore nella creazione del processo figlio" RESET);
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
		else if (child_pid[i] == 0) { // Processo figlio
			close(server_socket); // Il processo figlio chiude il socket del server (non ne ha bisogno)
			if (send(client_socket, &shmid, sizeof(int), 0) == -1) { // Invia al client la shmid
				perror("Errore invio shmid al client");
				exit(EXIT_FAILURE);
			}
			gestisciConnessioneClient(client_socket, code, elencoPazienti, numPazienti); // Gestisce la connessione del client con la funzione apposita
			//aspetta la chiusura del client prima di continuare
			if(client_socket==0) printf("\ncoglione\n");
			wait(NULL); // Il processo figlio aspetta la terminazione del client (evita i processi zombie)
			if (close(client_socket) == 0) { // Chiude il socket del client quando la gestione è completata
				gotoxy(50, 8);
				printf("Client %d disconnesso.                                       \n", client_socket);
				fflush(stdout);
			}
			exit(EXIT_SUCCESS); //Termina il processo figlio
		}
		nc++;
	}
	// Chiude il socket del server
	close(client_socket); // Padre chiude il socket del client
	if (close(server_socket) == 0) {
		gotoxy(50, 8);
		printf("Server disconnesso.                                       \n");
		fflush(stdout);
	}
	// Termina tutti i processi figli
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (child_pid[i] != 0) {
			kill(child_pid[i], SIGTERM);
		}
	}
}

// Funzione per la creazione della memoria condivisa
int create_shared_memory(size_t size) {
	key_t chiave = ftok(".", 'w');
	if (chiave == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	int shmid = shmget(chiave/*IPC_PRIVATE*/, size, IPC_CREAT | 0666); /*S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH*/
	if (shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	return shmid;
}

// Funzione generica per l'attach della memoria condivisa
void* attach_shared_memory(int shmid) {
	void* memory = shmat(shmid, NULL, 0);
	if (memory == (void*)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	return memory;
}

// Funzione per l'inizializzazione dei semafori
void initialize_semaphores() {
	sem_unlink("/mutex");
	sem_unlink("/empty");
	sem_unlink("/full");

	mutex = sem_open("/mutex", O_CREAT | O_EXCL, 0666, 1);    // Mutex inizializzato a 1 (semaforo binario)
	empty = sem_open("/empty", O_CREAT | O_EXCL, 0666, BUFFER_SIZE);  // Numero massimo di slot vuoti
	full = sem_open("/full", O_CREAT | O_EXCL, 0666, 0);     // Nessun slot pieno all'inizio

	if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
}

// Funzione per liberare le risorse del semaforo
void distruggiSemafori(CodePriorita* codePriorita) {
	for (int i = 0; i < 5; i++) {
		sem_destroy(&(codePriorita->code[i].semaforo));// Funzione per distruggere i semafori di una struttura CodePriorita
	}
	sem_destroy(mutex);
	sem_destroy(empty);
	sem_destroy(full);
	sem_close(mutex);
	sem_close(empty);
	sem_close(full);
	sem_unlink("/mutex");
	sem_unlink("/empty");
	sem_unlink("/full");
}

void liberaMemoriaCondivisa() {
	// Chiusura della memoria condivisa
	if (shmdt(shared_memall) == -1) {
		perror("shmdt");
		exit(EXIT_FAILURE);
	}
	// Rimozione della memoria condivisa
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(EXIT_FAILURE);
	}
	free(shared_memall);
}

void tempoScorre() {
	// Incrementa i minuti e aggiorna ore e giorni quando necessario
	minuti++;
	if (minuti >= 60) {
		minuti = 0;
		ore++;
		if (ore >= 24) {
			ore = 0;
			giorno++;
		}
	}
}

void* orologio(void* args) { // Thread per l'orologio
	while (1) { 
		pthread_mutex_lock(&muto); // Blocca l'accesso alla console
		tempoScorre(); // Incrementa il tempo con l'apposita funzione
		gotoxy(42, 1);
		printf("   Day:%d | PRONTO SOCCORSO | [%02d:%02d] === Exit:CTRL + C\n", giorno, ore, minuti);
		pthread_mutex_unlock(&muto); // Sblocca l'accesso alla console
		usleep(6944); // 10 SECONDI veri -> UN GIORNO virtuale
		if (giorno == 30) { // Se è trascorso un mese
			pthread_mutex_lock(&muto); // Blocca l'accesso alla console
			fflush(stdout);
			gotoxy(50, 12);
			printf("\nE' trascorso un mese - l'orologio si ferma");
			pthread_cond_signal(&cond); // Notifica al thread principale che è il momento di terminare
			pthread_mutex_unlock(&muto); // Sblocca l'accesso alla console
			break;
		}
	}
	pthread_exit(NULL);
}

void gotoxy(int x, int y) {
	printf("\033[%d;%dH", y, x);
	fflush(stdout);
	currentX = x;
	currentY = y;
}