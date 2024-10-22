#include "log.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int     opt, flags;
    mqd_t   mqd;  
    
    flags = O_CREAT | O_RDWR;

    while ((opt = getopt(argc, argv, "eh")) != -1) {
        switch (opt) {
            case 'e':
                flags |= O_EXCL;
                break;
            case 'h':
                  fprintf(stderr, "Usage: %s [-h] [-e filename ]\n",
                           argv[0]);
                  return 0;
            default:
                  fprintf(stderr, "Usage: %s [-h] [-e filename ]\n",
                           argv[0]);
                  return -1;
        }
    }
    if (optind != argc -1) {
        fprintf(stderr, "Usage: %s [-h] [-e filename ]\n",
                 argv[0]);
        return -1;
    }
    // filename: /same-link...  max length is NAME_MAX, and mqd in the /dev/mqueue/filename , manipulate by ls or rm ...
    // mqd in virtual filesystem  /dev/mqueue df -T /dev/mqueue ---> type: mqueue
    mqd = mq_open(argv[optind], flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, NULL);
    if (mqd < 0) {
        log_error("mq_open error:%s", strerror(errno));
        return -1;
    }

    mq_close(mqd);
    return 0;
}
