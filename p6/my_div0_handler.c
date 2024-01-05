#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// global counter to keep track of the number of times it receives SIGUSR1
int counter = 0;

/*
 * sigfpe_handler()
 *
 * print a message stating that a divide by 0 operation was attempted,
 * print the number of successfully completed division operations,
 * and then exit the program using exit(0)
 */
void sigfpe_handler() {
    // print error message when a divide by 0 operation was attempted
    printf("Error: a division by 0 operation was attempted.\n");
    // print the number of successfully completed division operations and exit
    printf("Total number of operations completed successfully: %d\n", counter);
    printf("The program will be terminated.\n");
    exit(0);
}

/*
 * sigint_handler()
 *
 * when Ctrl+C is inputted, the handler prints the number of successfully
 * completed division operations, and then exit the program using exit(0)
 *
 */
void sigint_handler() {
    // print the number of successfully completed division operations and exit
    printf("\n");
    printf("Total number of operations completed successfully: %d\n", counter);
    printf("The program will be terminated.\n");
    exit(0);
}

int main(int argc, char** argv) {
    // declare the sigaction structures
    struct sigaction sig_fpe;
    struct sigaction sig_int;

    // clears the sigaction structures to zero before using it
    memset(&sig_fpe, 0, sizeof(sig_fpe));
    memset(&sig_int, 0, sizeof(sig_int));

    // initialize the handler for the SIGFPE signal
    sig_fpe.sa_handler = sigfpe_handler;
    // initialize the handler for the SIGINT signal
    sig_int.sa_handler = sigint_handler;

    // if the function sigaction call for SIGFPE fails,
    // print error message and exit
    if (sigaction(SIGFPE, &sig_fpe, NULL) != 0) {
        printf("Failed to send SIGFPE\n");
        exit(1);
    }

    // if the function sigaction call for SIGINT fails,
    // print error message and exit
    if (sigaction(SIGINT, &sig_int, NULL) != 0) {
        printf("Failed to send SIGINT\n");
        exit(1);
    }

    // infinite loop
    while (1) {
        // declare a buffer to store user input
        char buffer[100];

        // prompt user to type in first integer for numerator
        printf("Enter first integer: ");
        // if the line of input is null, print error message
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading first integer");
        }
        // convert c string to an integer
        int numer = atoi(buffer);

        // prompt user to type in second integer for numerator
        printf("Enter second integer: ");
        // if the line of input is null, print error message
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading second integer");
        }
        // convert c string to an integer
        int denomi = atoi(buffer);

        // calculate the quotient of int1/int2
        int quotient = numer / denomi;
        // calculate the remainder of int1/int2
        int remainder = numer % denomi;

        // print the results of quotient and remainder of int1/int2
        printf("%d / %d is %d with a remainder of %d\n", numer, denomi, quotient, remainder);

        // increment the counter for total division operation
        counter++;
    }

    return 0;
}
