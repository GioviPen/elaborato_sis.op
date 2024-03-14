// Funzione per inserire un paziente nella coda corrispondente alla sua priorità
void inserisciInCodaPriorita(CodePriorita* code, Paziente* paziente) {
	inserisciInCoda(&(code->code[paziente->priorita - 1]), paziente);
}
