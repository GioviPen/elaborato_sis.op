// Funzione per inizializzare le 5 code di priorità
void inizializzaCodePriorita(CodePriorita* code, int capacita) {
	for (int i = 0; i < 5; i++) {
		inizializzaCoda(&(code->code[i]), capacita);
	}
}