// Max Chen
// 02/18/2026
// Prints a Hello World message using the write() system call

#include <unistd.h>

int main(int argc, char* argv[]) {

    char message[] = "Hello, world!\n";

    write(STDOUT_FILENO, message, 14);

    return 0;
}
