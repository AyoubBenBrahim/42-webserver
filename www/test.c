#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    pid_t childPid;
    int kq, nev;
    struct kevent chlist[1];   // Event we want to monitor
    struct kevent evlist[1];   // Event that was triggered
    struct timespec timeout = { 5, 0 };   // 5 seconds timeout

    // Create a new kqueue
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue() failed");
        exit(1);
    }

    // Fork a child process
    childPid = fork();
    if (childPid == -1) {
        perror("fork() failed");
        exit(1);
    }

    if (childPid == 0) {
        // Child process
        sleep(3);   // Simulating some work
        exit(0);
    } else {
        // Parent process
        EV_SET(&chlist[0], childPid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);

        // Wait for events or timeout
        nev = kevent(kq, chlist, 1, evlist, 1, &timeout);
        if (nev == -1) {
            perror("kevent() failed");
            exit(1);
        } else if (nev == 0) {
            // Timeout occurred, child process took too long
            printf("Child process timed out\n");
        } else {
            // Event occurred, child process exited
            printf("Child process exited\n");
        }
    }

    return 0;
}
