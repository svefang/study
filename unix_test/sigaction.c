/**
 * describe: test sigaction
*/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig, siginfo_t *info, void *arg) {
    printf("recvive sig %d, sender is %d\n", sig, info->si_pid);
}


int main(int argc, char *argv[])
{
    int ret = 0;

    struct sigaction sa;

    sa.sa_sigaction = sig_handler;
    sa.sa_flags    = SA_SIGINFO;

    ret = sigaction(SIGUSR1, &sa, NULL);

    getchar();

    return 0;
}