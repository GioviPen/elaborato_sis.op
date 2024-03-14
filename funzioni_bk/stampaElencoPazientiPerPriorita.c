void stampaElencoPazientiPerPriorita(CodePriorita* code) {
	for (int codice = CODICE_ROSSO; codice <= CODICE_BIANCO; codice++) {
		int numPazientiPerCodice = 0;
		/*for (int i = 0; i < numPazienti; i++)
			if (elencoPazienti[i][codice].priorita == codice)*/
		for (int i = 0; i < code->code[codice - 1].numeroElementi; i++) {
			Paziente paziente = code->code[codice - 1].elementi[i];
			if (paziente.priorita == codice) {
				numPazientiPerCodice++;
			}
		}
		if (numPazientiPerCodice > 0) {
			printf("\n\n%d pazienti con codice %s(%d):\n", numPazientiPerCodice, codice == CODICE_ROSSO ? "ROSSO" :
				(codice == CODICE_ARANCIONE ? "ARANCIONE" :
					(codice == CODICE_AZZURRO ? "AZZURRO" :
						(codice == CODICE_VERDE ? "VERDE" : "BIANCO"))), codice);
			/*for (int i = 0; i < numPazienti; i++) {
				if (elencoPazienti[i][codice].priorita == codice) {
					printf("- %s %s\n", elencoPazienti[i][codice].nome, elencoPazienti[i][codice].cognome);*/
			for (int i = 0; i < code->code[codice - 1].numeroElementi; i++) {
				Paziente paziente = code->code[codice - 1].elementi[i];
				if (paziente.priorita == codice) {
					printf("- %s %s\n", paziente.nome, paziente.cognome);
				}
			}
		}
		else printf("\nNessun codice %s\n", codice == CODICE_ROSSO ? "ROSSO" :
			(codice == CODICE_ARANCIONE ? "ARANCIONE" :
				(codice == CODICE_AZZURRO ? "AZZURRO" :
					(codice == CODICE_VERDE ? "VERDE" : "BIANCO"))));
	}
}
