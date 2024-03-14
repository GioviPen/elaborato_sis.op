void* orologio(void* args) {
	while (giorno <= 30) { //dopo "1 mese" si ferma
		pthread_mutex_lock(&muto); // Blocca l'accesso alla console
		tempoScorre(); // Incrementa il tempo con l'apposita funzione
		gotoxy(1, 1); // posiziona il cursore e stampa il tempo
		printf("========== Pazienti In Arrivo ========== Day: %d |PRONTO SOCCORSO| [%02d:%02d] === Exit:CTRL+C\n", giorno, ore, minuti);
		pthread_mutex_unlock(&muto); // Sblocca l'accesso alla console
		usleep(6944); // 10 SECONDI UN GIORNO
		if (giorno == 30) {
			printf("\nE' trascorso un mese, il programma termina alla fine del giorno\n");
		}
	}
}