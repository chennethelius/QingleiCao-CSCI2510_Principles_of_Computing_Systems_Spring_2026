#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 200

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: ./open <filename>\n", 25);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (1) {
        bytesRead = read(fd, buffer, BUFFER_SIZE);

        if (bytesRead == 0) {
            break;
        }

        if (bytesRead == -1) {
            perror("Error reading file");
            close(fd);
            return -1;
        }

        write(STDOUT_FILENO, buffer, bytesRead);
    }

    close(fd);

    return 0;
}
