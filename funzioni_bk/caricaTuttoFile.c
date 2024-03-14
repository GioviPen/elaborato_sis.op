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
