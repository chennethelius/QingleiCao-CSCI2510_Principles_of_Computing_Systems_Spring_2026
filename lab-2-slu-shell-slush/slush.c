#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_LINE 256
#define MAX_ARGS 16
#define MAX_CMDS 64

static volatile sig_atomic_t interrupted = 0;

void sigint_handler(int sig) {
    (void)sig;
    interrupted = 1;
    write(STDOUT_FILENO, "\n", 1);
}

void print_prompt(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *home = getenv("HOME");
        if (home && strncmp(cwd, home, strlen(home)) == 0) {
            char *rel = cwd + strlen(home);
            if (*rel == '/')
                rel++;
            printf("slush|%s> ", rel);
        } else {
            printf("slush|%s> ", cwd);
        }
    } else {
        printf("slush> ");
    }
    fflush(stdout);
}

// parse a single command segment into argvs
// return how many args and -1 on error 
int parse_command(char *segment, char **argv) {
    int argc = 0;
    char *token = strtok(segment, " \t\n");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;
    return argc;
}

int main(void) {
    char line[MAX_LINE];
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        interrupted = 0;
        print_prompt();

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            if (interrupted) {
                clearerr(stdin);
                continue;
            }
            printf("\n");
            break; // EOF
        }

        if (interrupted) {
            continue;
        }

        // remove newline
        line[strcspn(line, "\n")] = '\0';

        // skip empty lines
        if (strlen(line) == 0)
            continue;

        // check cd characters 
        {
            char tmp[MAX_LINE];
            strncpy(tmp, line, MAX_LINE);
            tmp[MAX_LINE - 1] = '\0';
            char *first = strtok(tmp, " \t");
            if (first != NULL && strcmp(first, "cd") == 0) {
                char *dir = strtok(NULL, " \t\n");
                if (dir == NULL) {
                    dir = getenv("HOME");
                }
                if (chdir(dir) != 0) {
                    perror(dir);
                }
                continue;
            }
        }

        // split input by parentheses into commands
        char *segments[MAX_CMDS];
        int num_cmds = 0;
        char *rest = line;
        char *seg;

        while ((seg = strsep(&rest, "(")) != NULL) {
            segments[num_cmds++] = seg;
            if (num_cmds >= MAX_CMDS)
                break;
        }

        // separate each segment into argvs
        char *cmd_argv[MAX_CMDS][MAX_ARGS + 1];
        int cmd_argc[MAX_CMDS];
        int valid = 1;

        for (int i = 0; i < num_cmds; i++) {
            cmd_argc[i] = 0;
            char *token = strtok(segments[i], " \t\n");
            while (token != NULL && cmd_argc[i] < MAX_ARGS) {
                cmd_argv[i][cmd_argc[i]++] = token;
                token = strtok(NULL, " \t\n");
            }
            cmd_argv[i][cmd_argc[i]] = NULL;

            if (cmd_argc[i] == 0) {
                fprintf(stderr, "Invalid null command\n");
                valid = 0;
                break;
            }
        }

        if (!valid)
            continue;

        int num_children = 0;
        pid_t children[MAX_CMDS];
        int prev_pipe_read = -1; // read end of previous pipe

        // read from right to left
        for (int i = num_cmds - 1; i >= 0; i--) {
            int pipefd[2];

            // pipe if not last 
            if (i > 0) {
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    break;
                }
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                break;
            }

            if (pid == 0) {
                // reset signal handler
                signal(SIGINT, SIG_DFL);

                // previous input handler
                if (prev_pipe_read != -1) {
                    dup2(prev_pipe_read, STDIN_FILENO);
                    close(prev_pipe_read);
                }

                // pipe next command
                if (i > 0) {
                    close(pipefd[0]); // close read end
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }

                execvp(cmd_argv[i][0], cmd_argv[i]);
                perror(cmd_argv[i][0]);
                exit(EXIT_FAILURE);
            }

            // parents
            children[num_children++] = pid;

            // close last pipe
            if (prev_pipe_read != -1) {
                close(prev_pipe_read);
            }

            // save end of curr pipe
            if (i > 0) {
                close(pipefd[1]); // close write end in parent
                prev_pipe_read = pipefd[0];
            }
        }

        // wait for children
        for (int i = 0; i < num_children; i++) {
            int status;
            waitpid(children[i], &status, 0);
        }
    }

    return 0;
}
