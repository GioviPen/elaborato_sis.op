// Funzione per estrarre un paziente dalla coda generale
Paziente estraiDaCoda(CodaPazienti* coda) {
	Paziente pazienteEstratto;
	sem_wait(&coda->semaforo);  // Wait sul semaforo
	if (coda->numeroElementi == 0) {
		gotoxy(55, 7);
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
