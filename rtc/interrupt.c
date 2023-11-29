#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void timer_handler(int signum) {
    printf("interrupt HANDLE?\n");
    // Add your desired actions or code here
}

int main() {
    struct sigaction sa;
    sa.sa_handler = &timer_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    alarm(3);

    // Keep the program running to handle the timer interrupt
    while (1) {
        // You can put other processing here or use sleep() to avoid high CPU usage
        sleep(1);
    }

    return EXIT_SUCCESS;
}
