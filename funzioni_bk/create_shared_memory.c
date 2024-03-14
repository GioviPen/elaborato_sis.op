// Funzione per la creazione della memoria condivisa
int create_shared_memory(size_t size) {
	int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
	if (shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	return shmid;
}
