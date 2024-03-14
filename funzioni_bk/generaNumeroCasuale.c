// Funzione per generare un numero casuale compreso tra minimo e massimo
int generaNumeroCasuale(int minimo, int massimo) {
	return rand() % (massimo - minimo + 1) + minimo;
}
