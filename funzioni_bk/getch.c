char getch() {
    char buf = 0;
    struct termios old = { 0 }, new_termios;

    // Salva la configurazione corrente del terminale
    if (tcgetattr(STDIN_FILENO, &old) < 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    new_termios = old;

    // Imposta il terminale in modalità "raw" senza buffering
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;

    // Applica le nuove impostazioni del terminale
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) < 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    // Leggi un singolo carattere
    if (read(STDIN_FILENO, &buf, 1) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Ripristina le impostazioni originali del terminale
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old) < 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    return buf;
}