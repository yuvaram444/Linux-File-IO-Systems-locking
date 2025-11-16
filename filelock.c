#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

void display_lslocks() {
    printf("\nCurrent lslocks output:\n");
    fflush(stdout);
    system("lslocks");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file = argv[1];
    int fd;

    printf("Opening %s\n", file);

    // Open the file for writing
    fd = open(file, O_WRONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Acquire shared lock
    if (flock(fd, LOCK_SH) == -1) {
        perror("Error acquiring shared lock");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Acquired shared lock using flock\n");
    display_lslocks();

    sleep(1); // Wait before upgrading

    // Upgrade to exclusive lock
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        perror("Error upgrading to exclusive lock");
        flock(fd, LOCK_UN); // Release shared lock
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Acquired exclusive lock using flock\n");
    display_lslocks();

    sleep(1); // Wait before unlocking

    // Unlock the file
    if (flock(fd, LOCK_UN) == -1) {
        perror("Error unlocking");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Unlocked\n");
    display_lslocks();

    close(fd);
    return 0;
}

