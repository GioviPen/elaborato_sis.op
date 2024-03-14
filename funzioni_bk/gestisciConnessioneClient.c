// Funzione per gestire la connessione del client al server
void gestisciConnessioneClient(int client_socket, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	RichiestaClient richiesta;

	if (recv(client_socket, &richiesta, sizeof(RichiestaClient), 0) == -1) {// Ricevi la richiesta del client
		gotoxy(55, 7);
		perror(ROSSO "Errore nella ricezione della richiesta del client" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                     ");
		close(client_socket);
		exit(EXIT_FAILURE);
	}

	rimuoviPaziente(code, elencoPazienti, numPazienti, richiesta.paziente);
	gotoxy(55, 5);
	printf("Client %d: Paziente rimosso con successo\n", richiesta.client_id);


	// Invia un messaggio di conferma al client
	if (send(client_socket, "OK", sizeof("OK"), 0) == -1) {
		
		gotoxy(55, 7);
		perror(ROSSO "Errore nell'invio della conferma al client" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                   ");
		exit(EXIT_FAILURE);
	}

	// Chiudi la connessione con il client
	close(client_socket);
	exit(EXIT_SUCCESS);
}
