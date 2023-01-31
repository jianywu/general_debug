/* Only support SIGUSR1, send SIGUSR2 will cause program exit.
 *
 * */
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool s_flag = false;

static void sigusr_handle(int sigNumber,
                           siginfo_t* sigInfoPtr,
                           void* contextPtr) {
    (void)contextPtr;

    s_flag = s_flag ? false : true;
    printf("\nsignal(%i) caught, sender pid:%d, s_flag:%d.\n", sigNumber, sigInfoPtr->si_pid, s_flag);
}

static bool sigusr_register(void) {
    printf("main proc registering SIGUSR1 signal handler\n");
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigusr_handle;
    // sa_sigaction need flag SA_SIGINFO
    act.sa_flags |= SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGUSR1, &act, 0) != 0) {
        return false;
    }
    if (sigaction(SIGUSR2, &act, 0) != 0) {
        return false;
    }

    return true;
}

int main() {
    printf("test start!\n");
    if (!sigusr_register()) {
        printf("SIGUSR register failed!\n");
    } else {
        printf("SIGUSR register successful!\n");
    }

    while (1);

    return 0;
}
