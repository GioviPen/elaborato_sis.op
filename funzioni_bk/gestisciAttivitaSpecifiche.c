// Funzione per la gestione delle attività specifiche (cartella clinica, visita medica, ecc.)
void gestisciAttivitaSpecifiche(Paziente* p) {
	// Simulazione di un'attività separata (ad esempio, visita medica) utilizzando fork
	pid_t pid = fork();

	if (pid == -1) {

		gotoxy(55, 7);
		perror(ROSSO "Errore durante la fork" RESET);
		fflush(stdout);
		sleep(3);
		gotoxy(55, 7);
		printf("                                                   ");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0) {
		// Processo figlio: gestisci l'attività specifica (cartella clinica, visita medica, ecc.)
		printf("Processo [%d] per gestire il paziente %s %s\n", getpid(), p->nome, p->cognome);
		sleep(1);  // Simulazione della creazione della cartella clinica
		printf("Cartella clinica creata per il paziente %s %s\n", p->nome, p->cognome);
		// robe?
		printf("Inizio visita medica per il paziente %s %s\n", p->nome, p->cognome);
		sleep(2);  // Simulazione della visita medica
		printf("Paziente %s %s dimesso: processo [%d] terminato\n", p->nome, p->cognome, getpid());
		exit(0);  // Termina il processo figlio
	}
	else {
		// Processo genitore: attendi la terminazione del processo figlio
		waitpid(pid, NULL, 0);
		printf("Medico [%d] continua dopo aver dimesso %s %s\n", getpid(), p->nome, p->cognome);
	}
}
