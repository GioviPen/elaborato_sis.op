// Funzione per inizializzare la coda
void inizializzaCoda(CodaPazienti* coda, int capacita) {
	coda->elementi = (Paziente*)malloc(capacita * sizeof(Paziente));
	if (coda->elementi == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	coda->capacita = capacita;
	coda->testa = 0;
	coda->coda = -1;
	coda->numeroElementi = 0;
	if (sem_init(&coda->semaforo, 0, 1) == -1) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
}