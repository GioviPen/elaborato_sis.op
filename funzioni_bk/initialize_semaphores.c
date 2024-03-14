// Funzione per l'inizializzazione dei semafori
void initialize_semaphores() {
	sem_unlink("/mutex");
	sem_unlink("/empty");
	sem_unlink("/full");

	mutex = sem_open("/mutex", O_CREAT | O_EXCL, 0666, 1);    // Mutex inizializzato a 1 (semaforo binario)
	empty = sem_open("/empty", O_CREAT | O_EXCL, 0666, BUFFER_SIZE);  // Numero massimo di slot vuoti
	full = sem_open("/full", O_CREAT | O_EXCL, 0666, 0);     // Nessun slot pieno all'inizio

	if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
}