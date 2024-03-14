// Funzione per liberare le risorse del semaforo
void distruggiSemafori(CodePriorita* codePriorita) {
	for (int i = 0; i < 5; i++) {
		sem_destroy(&(codePriorita->code[i].semaforo));// Funzione per distruggere i semafori di una struttura CodePriorita
	}
	sem_destroy(mutex);
	sem_destroy(empty);
	sem_destroy(full);
	sem_close(mutex);
	sem_close(empty);
	sem_close(full);
	sem_unlink("/mutex");
	sem_unlink("/empty");
	sem_unlink("/full");
}
