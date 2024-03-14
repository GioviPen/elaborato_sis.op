// Caricamento casuale di un solo paziente dal file
Paziente caricaCasuale(FILE* file, CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti) {
	Paziente paziente;
	int nRand = rand() % NUM_NOMI;
	rewind(file); // Riavvolgi il file all'inizio
	for (int riga = 0; riga < nRand; riga++) {
		char buffer[MAX_LEN * 4];
		if (fgets(buffer, sizeof(buffer), file) == NULL) {
			gotoxy(55, 7);
			printf(ROSSO "Errore durante la lettura del file alla riga %d\n" RESET, nRand);
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
	}
	if (fscanf(file, "%s %s %s %d", paziente.nome, paziente.cognome, paziente.provincia, &paziente.eta) == 4) {
		inserisciPazientePriorita(code, elencoPazienti, numPazienti, &paziente);
	}
	else { // Se non riesce a leggere correttamente i dati
		gotoxy(55, 7);
		printf(ROSSO "Errore durante la lettura del paziente dalla riga %d\n" RESET, nRand);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	return paziente;
}
