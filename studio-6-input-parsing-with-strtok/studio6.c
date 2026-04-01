#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buffer[256];
    int max_args = 15;
    int max_argv_size = max_args + 2;
    char* cmd;
    char* my_argv[max_argv_size];

    printf("Enter input: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Strip trailing newline
    char* newline = strchr(buffer, '\n');
    if (newline) {
        *newline = '\0';
    }

    // Parse first token - this is the command
    cmd = strtok(buffer, " ");
    if (cmd == NULL) {
        printf("No input provided.\n");
        return 1;
    }

    // Build my_argv
    my_argv[0] = cmd;
    int i = 1;
    char* res = strtok(NULL, " ");
    while (res != NULL) {
        my_argv[i] = res;
        i++;
        res = strtok(NULL, " ");
    }
    my_argv[i] = NULL;

    // Execute the command
    execvp(cmd, my_argv);

    // If execvp returns, there was an error
    perror("execvp failed");
    return 1;
}
