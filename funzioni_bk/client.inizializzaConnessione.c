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
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    return client_socket;
}
