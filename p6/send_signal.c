#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * main function
 *
 * the program sends SIGINT or SIGUSR1 to the process with
 * specific PID that user typed with (-i for SIGINT or -u for SIGUSR1)
 * signal type
 *
 */
int main(int argc, char **argv) {
    // check if the number of arguments are correct
    if (argc != 3) {
        // else print the usage message and exit
        printf("Usage: send_signal <signal type> <pid>\n");
        exit(1);
    }

    // if the signal type is SIGINT
    if (strcmp(argv[1], "-i") == 0) {
        // send SIGINT to the process with pid that
        // user inputted
        if (kill(atoi(argv[2]), SIGINT) == -1) {
            // if fails, print error message and exit
            printf("Failed to send SIGINT\n");
            exit(1);
        }
    }
    // if the signal type is SIGUSR1
    else if (strcmp(argv[1], "-u") == 0) {
        // send SIGUSR1 to the process with pid that
        // user inputted
        if (kill(atoi(argv[2]), SIGUSR1) == -1) {
            // if fails, print error message and exit
            printf("Failed to send SIGUSR1\n");
            exit(1);
        }
    } else {
        // if the signal type that user inputted
        // is other than -i or -u, print error message
        // and exit
        printf("invalid signal type");
        exit(1);
    }

    return 0;
}