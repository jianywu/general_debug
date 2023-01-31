#include <errno.h>  // errno
#include <signal.h> // sigaction()
#include <stdio.h>  // printf()
#include <string.h> // strerror()
#include <stdlib.h> // exit()
#include <stdbool.h> // true, false

// Format: const char *, unsigned int, const char *
#define LOG_LOCATION __FILE__, __LINE__, __func__ 
#define LOG_FORMAT_STR "file: %s, line: %u, func: %s: "

static bool s_flag = false;

static void termination_handler(const int signal)
{
    switch (signal)
    {
    case SIGUSR1:
        s_flag = s_flag ? false : true;
        printf("\nSIGUSR1 (%i) signal caught, s_flag:%d.\n", signal, s_flag);
        break;
    case SIGUSR2:
        s_flag = s_flag ? false : true;
        printf("\nSIGUSR2 (%i) signal caught, s_flag:%d.\n", signal, s_flag);
        break;
    default:
        printf("\nOther signal (%i) caught.\n", signal);
        break;
    }
}

int main(int argc, char *argv[])
{
    int ret_code;
    struct sigaction new_action;
    // memset(&new_action, 0, sizeof(new_action));
    new_action.sa_handler = termination_handler; // set callback function
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    errno = 0;

    // SIGUSR1: custom user signal 1
    ret_code = sigaction(SIGUSR1, &new_action, NULL);
    if (ret_code == -1)
    {
        printf(LOG_FORMAT_STR "sigaction failed when setting signal to "
               "%i; errno = %i: %s\n", 
               LOG_LOCATION, SIGUSR1, errno, strerror(errno));
    }
   
    // SIGUSR2: custom user signal 2
    ret_code = sigaction(SIGUSR2, &new_action, NULL);
    if (ret_code == -1)
    {
        printf(LOG_FORMAT_STR "sigaction failed when setting signal to "
               "%i; errno = %i: %s\n", 
               LOG_LOCATION, SIGUSR2, errno, strerror(errno));
    }

    while(1);

    return 0;
}

