// Funzione ausiliaria che contiene la logica specifica per ogni livello di priorità
void* threadGestorePriorita(void* arg) {
	Paziente* p = (Paziente*)arg;
	// definizione delle azioni specifiche per ciascuna priorità
	switch (p->priorita) {
	case CODICE_ROSSO:
		printf("\nPaziente %s %s con priorita' ROSSA\n", p->nome, p->cognome);
		// qui la logica specifica per il codice ROSSO - emergenza
		printf("-tempo medio di permanenza in ospedale 4gg: attendi\n");
		sleep(20);
		break;
	case CODICE_ARANCIONE:
		printf("\nPaziente %s %s con priorita' ARANCIONE\n", p->nome, p->cognome);
		// qui la logica specifica per il codice ARANCIONE - urgenza
		printf("-tempo medio di permanenza in ospedale 2gg: attendi\n");
		sleep(10);
		break;
	case CODICE_AZZURRO:
		printf("\nPaziente %s %s con priorita' AZZURRO\n", p->nome, p->cognome);
		// qui la logica specifica per il codice AZZURRO - urgenza differibile
		printf("-tempo medio di permanenza in ospedale 1gg: attendi\n");
		sleep(5);
		break;
	case CODICE_VERDE:
		printf("\nPaziente %s %s con priorita' VERDE\n", p->nome, p->cognome);
		// qui la logica specifica per il codice VERDE - urgenza minore
		printf("-tempo medio di permanenza in ospedale 12h: attendi\n");
		sleep(2);
		break;
	case CODICE_BIANCO:
		printf("\nPaziente %s %s con priorita' BIANCO\n", p->nome, p->cognome);
		// qui la logica specifica per il codice BIANCO - non urgenza
		printf("-tempo medio di permanenza in ospedale 2h: attendi\n");
		sleep(1);
		break;
	default:
		printf("Codice di priorita' non valido per il paziente %s %s\n", p->nome, p->cognome);
		break;
	}
	pthread_exit(NULL);
}
