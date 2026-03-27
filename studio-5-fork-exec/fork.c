#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (child_pid == 0) {
        // Child process
        sleep(3);
        printf("I am the child process. My PID is: %d\n", getpid());

        char* child_argv[] = {"ls", "-l", NULL};
        char* child_prog = child_argv[0];
        execvp(child_prog, child_argv);

        // If execvp returns, it failed
        perror("execvp failed");
        exit(1);
    } else {
        // Parent process
        waitpid(child_pid, NULL, 0);
        printf("I am the parent process. My PID is: %d\n", getpid());
    }

    return 0;
}
