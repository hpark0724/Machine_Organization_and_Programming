#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// global alarm time variable (number of seconds for the alarm)
int second = 5;
// global counter to keep track of the number of times it receives SIGUSR1
int counter = 0;

/*
 * signal_handler()
 *
 * signal_handler() prints process ID and current time,
 * restart a new alarm to go off again 5 seconds
 * if it fails to retrieve or convert current time,
 * handles SIGALRM signals by printing error message
 * and exit the program
 */
void signal_handler() {
    // initalize the variable of current time
    time_t current_time;
    // checks whether the time function call is successful
    // else if it fails, returns -1
    if (time(&current_time) != -1) {
        // initalize the variable of process ID
        pid_t pid = getpid();
        // convert the current time to string
        char *timestr = ctime(&current_time);
        if (timestr != NULL) {
            // print the pid (process id) of the program and the current time
            printf("PID: %d CURRENT TIME: %s", pid, timestr);
        } else {
            // if ctime returns NULL, print error message and exit
            printf("Error: failed to convert time to string\n");
            exit(1);
        }
    } else {
        // if time function fails, print error message and exit
        printf("Error: time is invalid\n");
        exit(1);
    }

    // re-arm a new alarm to go off again 5 seconds
    alarm(second);
}

/*
* user_handler()
*
* user_handler() is a user signal handler for SIGUSR1
* increment global counter and handles error by printing a message
# on receiving a SIGUSR1 signal
*/
void user_handler() {
    // increment the counter for SIGUSR1
    counter++;
    // print error message
    printf("SIGUSR1 handled and counted!\n");
}

/*
 * sigint_handler()
 *
 * signint_handler handles ctrl-c,the keyboard interrupt signal by
 * printing the number of times it received the SIGUSR1 signal
 * and then call exit(0)
 */
void sigint_handler() {
    printf("\n");
    // print the message while SIGINT is handled
    printf("SIGINT handled.\n");
    printf("SIGUSR1 was handled %d times. Exiting now.\n", counter);
    exit(0);
}

/*
 * main function
 *
 * the program prints the process ID and current time every 5 seconds
 * with SIGALRM signal. It counts and the number every time SIGUSR1
 * signal is received. The user can end the program with Ctrl-C
 * which calls the SIGINT signal
 *
 */
int main() {
    // instruction of what program prints and how it can be ended
    printf("PID and time print every 5 seconds\n");
    printf("Type Ctrl-C to end the program.\n");

    // declare the sigaction structures
    struct sigaction act_alarm;
    struct sigaction act_sigusr1;
    struct sigaction act_sigint;

    // clears the sigaction structures to zero before using it
    memset(&act_alarm, 0, sizeof(act_alarm));
    memset(&act_sigusr1, 0, sizeof(act_sigusr1));
    memset(&act_sigint, 0, sizeof(act_sigint));

    // initialize the handler for the SIGALRM signal
    act_alarm.sa_handler = signal_handler;
    // initialize the handler for the SIGUSR1 signal
    act_sigusr1.sa_handler = user_handler;
    // initialize the handler for the SIGINT signal
    act_sigint.sa_handler = sigint_handler;

    // if the function sigaction call for SIGALRM fails,
    // print error message and exit
    if (sigaction(SIGALRM, &act_alarm, NULL) != 0) {
        printf("Failed to set SIGALRM handler\n");
        exit(1);
    }
    // if the function sigaction call for SIGUSR1 fails,
    // print error message and exit
    if (sigaction(SIGUSR1, &act_sigusr1, NULL) != 0) {
        printf("Failed to set SIGUSR1 handler\n");
        exit(1);
    }
    // if the function sigaction call for SIGINT fails,
    // print error message and exit
    if (sigaction(SIGINT, &act_sigint, NULL) != 0) {
        printf("Failed to set SIGNIT handler\n");
        exit(1);
    }

    // a new alarm to go off again 5 seconds later
    alarm(second);
    // continues infinite loop
    while (1) {
    }
    return 0;
}