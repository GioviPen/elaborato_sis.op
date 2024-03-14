void inviaRichiesta(int client_socket, pid_t client_pid, Paziente* pazienti) {
    RichiestaClient richiesta;
    richiesta.client_id = client_pid;
    // Recupera il paziente dalla coda del client (assumendo che il paziente da dimettere sia in testa alla coda)
    richiesta.paziente = pazienti[0];

	// Invia la richiesta al server
    if (send(client_socket, &richiesta, sizeof(RichiestaClient), 0) == -1) {
        /*write(client_socket, &richiesta, sizeof(RichiestaClient))*/
        perror("Errore nell'invio della richiesta al server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char conferma[BUFFER_SIZE]; // Ricevi il messaggio di conferma dal server
    if (recv(client_socket, conferma, sizeof(conferma), 0) == -1) {
        /*read(client_socket, conferma, sizeof(conferma))*/
        perror("Errore nella ricezione della conferma dal server");
        exit(EXIT_FAILURE);
    }
    printf("Risposta server: %s\n", conferma);
}
