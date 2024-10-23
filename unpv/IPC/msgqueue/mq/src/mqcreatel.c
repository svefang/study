#include "log.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int     opt, flags;
    mqd_t   mqd;  
    
    flags = O_CREAT | O_RDWR;
    struct mq_attr mq_attr;
    memset(&mq_attr, 0, sizeof(mq_attr));

    // 参数后跟: ,则 optarg可以指向后面的值
    while ((opt = getopt(argc, argv, "em:z:h")) != -1) {
        switch (opt) {
            case 'e':
                flags |= O_EXCL;
                break;
            case 'm':
                mq_attr.mq_maxmsg = atol(optarg);
                break;
            case 'z':
                mq_attr.mq_msgsize = atol(optarg);
                break;
            case 'h':
                  fprintf(stderr, "Usage: %s [ -h ] [ -e ] [-m msgmax ] [-z msgsize ] <filename>\n",
                           argv[0]);
                  return 0;
            default:
                  fprintf(stderr, "Usage: %s [-h]\n",
                           argv[0]);
                  return -1;
        }
    }
    if (optind != argc -1) {
        fprintf(stderr, "Usage: %s [-h]\n", argv[0]);
        return -1;
    }
    log_info("filename :%s %ld %ld", argv[optind], mq_attr.mq_maxmsg, mq_attr.mq_msgsize);
    // filename: /same-link...  max length is NAME_MAX, and mqd in the /dev/mqueue/filename , manipulate by ls or rm ...
    // mqd in virtual filesystem  /dev/mqueue df -T /dev/mqueue ---> type: mqueue
    mqd = mq_open(argv[optind], flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, \
                 (mq_attr.mq_maxmsg != 0) ? &mq_attr : NULL);
    if (mqd < 0) {
        log_error("mq_open error:%s", strerror(errno));
        return -1;
    }
    
    //get mq attr
    //         struct mq_attr {
    //         long mq_flags;       /* Flags: 0 or O_NONBLOCK */
    //         long mq_maxmsg;      /* Max. # of messages on queue */
    //         long mq_msgsize;     /* Max. message size (bytes) */
    //         long mq_curmsgs;     /* # of messages currently in queue */
    //     };

    struct mq_attr attr;  
    if (mq_getattr(mqd, &attr) < 0) {
        log_error("mq_getattr error:%s", strerror(errno));
        mq_close(mqd);
        return -1;
    }
    
    printf("mq_flags:%ld  mq_maxmsg:%ld mq_msgsize:%ld mq_curmsgs:%ld\n",
            attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs); 

    mq_close(mqd);
    return 0;
}
