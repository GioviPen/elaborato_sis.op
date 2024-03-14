// Funzione per inizializzare la memoria condivisa
Paziente* inizializzaMemoriaCondivisa(size_t size) {
    int shmid;
    key_t chiave = ftok(".", 'a');
    Paziente* pazienti;

    // Creazione della memoria condivisa
    if ((shmid = shmget(chiave/*IPC_PRIVATE*/, size, IPC_CREAT | 0666)) < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // Collegamento alla memoria condivisa
    if ((pazienti = shmat(shmid, NULL, 0)) == (Paziente*)-1) {
        perror("Errore nel collegamento alla memoria condivisa");
        exit(EXIT_FAILURE);
    }
    return pazienti;
}
