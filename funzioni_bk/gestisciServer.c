// Funzione per gestire il server
void gestisciServer(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	int server_socket, client_socket;
	struct sockaddr_in server_address, client_address;
	socklen_t client_address_len = sizeof(client_address);

	// Creazione del socket del server
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		
		gotoxy(55, 7);
		perror(ROSSO "Errore nella creazione del socket del server" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                   ");
		exit(EXIT_FAILURE);
	}

	// Inizializzazione della struttura dell'indirizzo del server
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(SERVER_IP);/*INADDR_ANY*/
	server_address.sin_port = htons(SERVER_PORT);

	// Associazione del socket all'indirizzo del server
	if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		
		gotoxy(55, 7);
		perror(ROSSO "Errore nell'associazione del socket all'indirizzo del server" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                                      ");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	fflush(stdout);
	gotoxy(55, 3);
	printf("In attesa della connessione del client...\n");// Messaggio di attesa della connessione del client

	// Mettere in ascolto il socket del server
	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
		
		gotoxy(55, 7);
		perror(ROSSO "Errore nella messa in ascolto del socket del server" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                          ");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	gotoxy(55, 4);
	printf("Server in ascolto %s:%d...\n", SERVER_IP, SERVER_PORT);

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

	while (server_running) {
		// Accettazione di una nuova connessione
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len)) == -1) {
			
			gotoxy(55, 7);
			perror(ROSSO "Errore nell'accettazione della connessione del client" RESET);
			fflush(stdout);
			sleep(3);
			gotoxy(55, 7);
			printf("                                                              ");
			close(server_socket);
			close(client_socket);
			exit(EXIT_FAILURE);
		} 
		gotoxy(55, 3); 
		printf("Client connesso: %s                 \n", inet_ntoa(client_address.sin_addr));
		gotoxy(55, 4);
		printf("                                     ");

		// Creazione di un processo figlio per gestire la connessione del client
		pid_t child_pid;
		if ((child_pid = fork()) == -1) {
			perror("client/server fork error");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		else if (child_pid == 0) { // Processo figlio
			close(server_socket);  // Il processo figlio chiude il socket del server (non ne ha bisogno)
			// Chiamata alla funzione per gestire la connessione del client
			gestisciConnessioneClient(client_socket, code, elencoPazienti, numPazienti);
			close(client_socket); // Chiude il socket del client quando la gestione è completata
			exit(EXIT_SUCCESS); // Terminazione del processo figlio
		}
		else { // Processo padre
			waitpid(child_pid, NULL, WNOHANG); // WNOHANG per non bloccarsi se non ci sono processi figli inattivi
			close(client_socket);  // Il processo padre chiude il socket del client
		}
	}
}
