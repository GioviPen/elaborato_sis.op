// Funzione generica per l'attach della memoria condivisa
void* attach_shared_memory(int shmid) {
	void* memory = shmat(shmid, NULL, 0);
	if (memory == (void*)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	return memory;
}
