void tempoScorre() {
	// Incrementa i minuti e aggiorna ore e giorni quando necessario
	minuti++;
	if (minuti == 60) {
		minuti = 0;
		ore++;
		if (ore == 24) {
			ore = 0;
			giorno++;
		}
	}
}