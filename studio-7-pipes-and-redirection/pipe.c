#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];

    // Create the pipe before forking
    if (pipe(fd) == -1) {
        perror("Error creating pipe");
        return 1;
    }

    // Fork first child (Program 1 - the writer)
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error forking child 1");
        return 1;
    }

    if (pid1 == 0) {
        // Child 1: redirect stdout to write end of pipe
        dup2(fd[1], STDOUT_FILENO);
        // Close unused ends
        close(fd[0]); // Child 1 doesn't read from pipe
        close(fd[1]); // Already duplicated

        char *cmd = "./print";
        char *myargv[] = {"print", NULL};
        int ret = execvp(cmd, myargv);
        if (ret == -1) perror("Error exec'ing print");
        return 1;
    }

    // Fork second child (Program 2 - the reader)
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Error forking child 2");
        return 1;
    }

    if (pid2 == 0) {
        // Child 2: redirect stdin to read end of pipe
        dup2(fd[0], STDIN_FILENO);
        // Close unused ends
        close(fd[1]); // Child 2 doesn't write to pipe
        close(fd[0]); // Already duplicated

        char *cmd = "./input";
        char *myargv[] = {"input", NULL};
        int ret = execvp(cmd, myargv);
        if (ret == -1) perror("Error exec'ing input");
        return 1;
    }

    // Parent: close both ends of the pipe (parent doesn't use it)
    close(fd[0]);
    close(fd[1]);

    // Wait for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
