// Funzione per gestire i codici di priorità: crea un thread per ogni paziente e gestisce la sua priorità.
void gestisciPriorita(Paziente* p) {
	pthread_t thread;  // Dichiarazione dell'identificatore del thread

	// Creazione del thread e passaggio del paziente come argomento
	if (pthread_create(&thread, NULL, threadGestorePriorita, (void*)p) != 0) {
		fprintf(stderr, "Errore durante la creazione del thread per il paziente %s %s\n", p->nome, p->cognome);
		return;
	}

	// Attendere la terminazione del thread prima di continuare
	if (pthread_join(thread, NULL) != 0) {
		fprintf(stderr, "Errore durante l'attesa della terminazione del thread per il paziente %s %s\n", p->nome, p->cognome);
		return;
	}

	// Eventuale ulteriore terminazione del thread
	pthread_detach(thread);

	// Chiamare la funzione per gestire le attività specifiche in un processo figlio
	gestisciAttivitaSpecifiche(p);
}
