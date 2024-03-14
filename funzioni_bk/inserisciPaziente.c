// Funzione per inserire il paziente nella coda e nell'elenco
void inserisciPaziente(CodaPazienti* coda, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente) {
	paziente->priorita = assegnaPrioritaCasuale(); // Assegnazione casuale del codice di priorità
	elencoPazienti[*numPazienti][paziente->priorita] = *paziente; // Aggiungo il paziente all'elenco corrispondente al suo codice di priorità
	inserisciInCoda(coda, paziente); // Inserimento del paziente nella coda
	(*numPazienti)++;
	printf("Paziente inserito nella coda.\n");
}
