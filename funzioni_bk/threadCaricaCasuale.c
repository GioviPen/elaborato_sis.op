void* threadCaricaCasuale(void* args) {
	ThreadCaricaCasualeArgs* threadArgs = (ThreadCaricaCasualeArgs*)args;
	int j = 0, q = 0;
	while (1) {
		int nRand = 1 + rand() % 7;
		for (int i = 0; i < nRand; i++) {
			Paziente ultimoPaziente = caricaCasuale(threadArgs->fileAnagrafica, threadArgs->code, threadArgs->elencoPazienti, threadArgs->numPazienti);
			pthread_mutex_lock(&muto);
			if (j == 10) {
				for (int h = 0; h < 10; h++) {
					gotoxy(1, 3 + h);
					printf("                                                ");
				}
				j = 0;
			}
			gotoxy(1, 3 + j);
			printf("Paziente %d, %s %s in arrivo.\n\n\n", q + 1, ultimoPaziente.nome, ultimoPaziente.cognome);
			q++;
			j++;
			pthread_mutex_unlock(&muto);
			fflush(stdout);
			sleep(1);
		}
		fflush(stdout);
		sleep(6);
	}
	pthread_exit(NULL);
}



void* threadCaricaCasuale(void* args) {
	ThreadCaricaCasualeArgs* threadArgs = (ThreadCaricaCasualeArgs*)args;
	int j = 0, q = 0;
	while (1) {
		Paziente ultimoPaziente = caricaCasuale(threadArgs->fileAnagrafica, threadArgs->code, threadArgs->elencoPazienti, threadArgs->numPazienti);
		pthread_mutex_lock(&muto);
		memcpy(shared_memall, &threadArgs->code->code, sizeof(CodePriorita));
		if (j == 10) { // Se la console è piena 
			for (int h = 0; h < 10; h++) {
				gotoxy(1, 3 + h);
				printf("                                                ");
			} // cancella 10 righe
			j = 0;
		}
		gotoxy(1, 3 + j);
		printf("Paziente %d, %s %s in arrivo.\n\n\n", q + 1, ultimoPaziente.nome, ultimoPaziente.cognome);
		q++;
		j++;
		pthread_mutex_unlock(&muto);
		fflush(stdout);
		sleep(1 + rand() % 5); //simula il tempo che passa casuale tra l'arrivo di due pazienti

		// Incrementa il contatore delle iterazioni
		threadArgs->contatoreIterazioni++;

		// Controlla se è il momento di terminare
		if (threadArgs->contatoreIterazioni >= 100) {
			// Notifica al thread principale che è il momento di terminare
			pthread_cond_signal(&threadArgs->condizioneTerminazione); // variabile di condizione
			break;
		}
	}
	pthread_exit(NULL);
}
