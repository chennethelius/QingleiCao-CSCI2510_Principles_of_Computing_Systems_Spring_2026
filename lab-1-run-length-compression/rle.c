//Runlength compression in C
// Usage: ./rle <input file> <output file> <runlength> <mode>
//        Where mode=0 is compress and mode=1 is decompress

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    // check arg count
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input file> <output file> <compression length> <mode>\n", argv[0]);
        exit(-1);
    }

    int comp_len = atoi(argv[3]);
    int mode = atoi(argv[4]);

    // check compression len
    if (comp_len < 1) {
        fprintf(stderr, "Error: compression length must be at least 1\n");
        exit(-1);
    }

    // check mode
    if (mode != 0 && mode != 1) {
        fprintf(stderr, "Error: mode must be 0 (compress) or 1 (decompress)\n");
        exit(-1);
    }

    // open input file
    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) {
        perror("Error opening input file");
        exit(-1);
    }

    // open output file
    int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd_out < 0) {
        perror("Error opening output file");
        exit(-1);
    }

    if (mode == 0) {
        // compress
        unsigned char *current = malloc(comp_len);
        unsigned char *next = malloc(comp_len);
        if (!current || !next) {
            perror("Error allocating memory");
            exit(-1);
        }

        // first pattern
        ssize_t bytes_read = read(fd_in, current, comp_len);
        if (bytes_read < 0) {
            perror("Error reading input file");
            exit(-1);
        }
        if (bytes_read == 0) {
            // empty file
            free(current);
            free(next);
            if (close(fd_in) < 0) { perror("Error closing input file"); exit(-1); }
            if (close(fd_out) < 0) { perror("Error closing output file"); exit(-1); }
            return 0;
        }

        ssize_t current_len = bytes_read;
        unsigned char counter = 1;

        while (1) {
            bytes_read = read(fd_in, next, comp_len);
            if (bytes_read < 0) {
                perror("Error reading input file");
                exit(-1);
            }

            if (bytes_read == current_len && memcmp(current, next, current_len) == 0) {
                counter++;
                if (counter == 255) {
                    // counter max, write out run
                    if (write(fd_out, &counter, 1) != 1) {
                        perror("Error writing to output file");
                        exit(-1);
                    }
                    if (write(fd_out, current, current_len) != current_len) {
                        perror("Error writing to output file");
                        exit(-1);
                    }
                    // restart on next pattern
                    bytes_read = read(fd_in, current, comp_len);
                    if (bytes_read < 0) {
                        perror("Error reading input file");
                        exit(-1);
                    }
                    if (bytes_read == 0) {
                        free(current);
                        free(next);
                        if (close(fd_in) < 0) { perror("Error closing input file"); exit(-1); }
                        if (close(fd_out) < 0) { perror("Error closing output file"); exit(-1); }
                        return 0;
                    }
                    current_len = bytes_read;
                    counter = 1;
                }
            } else if (bytes_read == 0) {
                // eof, write out current run
                if (write(fd_out, &counter, 1) != 1) {
                    perror("Error writing to output file");
                    exit(-1);
                }
                if (write(fd_out, current, current_len) != current_len) {
                    perror("Error writing to output file");
                    exit(-1);
                }
                break;
            } else {
                // new pattern, write out current run
                if (write(fd_out, &counter, 1) != 1) {
                    perror("Error writing to output file");
                    exit(-1);
                }
                if (write(fd_out, current, current_len) != current_len) {
                    perror("Error writing to output file");
                    exit(-1);
                }
                // new pattern = current
                memcpy(current, next, bytes_read);
                current_len = bytes_read;
                counter = 1;
            }
        }

        free(current);
        free(next);

    } else {
        // decompress
        unsigned char count_byte;
        unsigned char *pattern = malloc(comp_len);
        if (!pattern) {
            perror("Error allocating memory");
            exit(-1);
        }

        while (1) {
            // read count byte
            ssize_t bytes_read = read(fd_in, &count_byte, 1);
            if (bytes_read < 0) {
                perror("Error reading input file");
                exit(-1);
            }
            if (bytes_read == 0) {
                break; // eof
            }

            // read pattern
            ssize_t pattern_read = read(fd_in, pattern, comp_len);
            if (pattern_read < 0) {
                perror("Error reading input file");
                exit(-1);
            }
            if (pattern_read == 0) {
                fprintf(stderr, "Error: unexpected end of file during decompression\n");
                exit(-1);
            }

            // write pattern count_byte times
            for (unsigned char i = 0; i < count_byte; i++) {
                if (write(fd_out, pattern, pattern_read) != pattern_read) {
                    perror("Error writing to output file");
                    exit(-1);
                }
            }
        }

        free(pattern);
    }

    if (close(fd_in) < 0) {
        perror("Error closing input file");
        exit(-1);
    }
    if (close(fd_out) < 0) {
        perror("Error closing output file");
        exit(-1);
    }

    return 0;
}
