void rimuoviPaziente(CodePriorita* code, Paziente elencoPazienti[][5], int* numPazienti, Paziente pazienteRim) {
	CodicePriorita priorita = pazienteRim.priorita;


	if (priorita < CODICE_ROSSO || priorita > CODICE_BIANCO) {
		
		gotoxy(55, 7);
		printf(ROSSO "Errore: Codice di priorita' %d non valido." RESET, priorita);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                        ");
		return;
	}
	if (code->code[priorita - 1].numeroElementi == 0) { //se la coda è vuota
		
		gotoxy(55, 7);
		printf(ROSSO "Errore: Nessun paziente trovato nella coda di priorita' %d." RESET, priorita);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                                   ");
		return;
	}

	Paziente pazienteEstratto = estraiDaCodaPriorita(code, priorita); // Rimuovi il paziente dalla coda corrispondente alla priorita

	if (memcmp(&pazienteEstratto, &pazienteRim, sizeof(Paziente)) == 0) {
		gotoxy(55, 5);
		printf("                                                     ");
		printf("Paziente rimosso con successo.");
	}
	else {
		gotoxy(55, 5);
		printf("                                                     ");
		printf("Paziente non corrispondente.  ");
	}
}
