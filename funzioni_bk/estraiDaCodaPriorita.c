// Funzione per estrarre un paziente dalla coda corrispondente alla sua priorità
Paziente estraiDaCodaPriorita(CodePriorita* code, CodicePriorita priorita) {
	return estraiDaCoda(&(code->code[priorita - 1]));
}
