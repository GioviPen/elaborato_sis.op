// Funzione per inserire un paziente nella coda
void inserisciInCoda(CodaPazienti* coda, Paziente* paziente) {
	sem_wait(&coda->semaforo);  // Wait sul semaforo, garantisce mutua esclusione
	if (coda->numeroElementi == coda->capacita) {
		gotoxy(55,7);
		printf(GIALLO "La coda %d e' piena, aumento la capacita'\n" RESET, paziente->priorita);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                     ");
		coda->capacita *= 2;
		coda->elementi = (Paziente*)realloc(coda->elementi, coda->capacita * sizeof(Paziente));
	}
	coda->coda = (coda->coda + 1) % coda->capacita;
	coda->elementi[coda->coda] = *paziente;
	coda->numeroElementi++;
	sem_post(&coda->semaforo);  // Signal sul semaforo (rilascio)
}
