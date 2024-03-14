// Funzione per inserire il paziente nella coda e nell'elenco
void inserisciPazientePriorita(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente* paziente) {
	paziente->priorita = assegnaPrioritaCasuale(); // Assegnazione casuale del codice di priorità
	elencoPazienti[*numPazienti][paziente->priorita - 1] = *paziente; // Aggiungo il paziente all'elenco corrispondente al suo codice di priorità
	inserisciInCodaPriorita(code, paziente); // Inserimento del paziente nella coda
	(*numPazienti)++;
}
