/*
 * @Date: 2024-01-23 23:32:37
 * @code: UTF-8
 * @Description: test nonblock 
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int set_fl(int fd, int flags)
{
    int   val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        printf("fcntl get flags error\n");
        return -1;
    }

    val |= flags;

    if ((fcntl(fd, F_SETFL, val)) < 0)
    {
        printf("fcntl set flags error\n");
        return -1;
    }

    return 0;
}

int clr_fl(int fd, int flags)
{
    int   val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        printf("fcntl get flags error\n");
        return -1;
    }

    val &= ~flags;

    if ((fcntl(fd, F_SETFL, val)) < 0)
    {
        printf("fcntl set flags error\n");
        return -1;
    }

    return 0;
}

char buf[500000];

int main()
{
    int ntowrite, nwrite;
    char *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));

    fprintf(stderr, "read %d bytes\n", ntowrite);   

    set_fl(STDOUT_FILENO, O_NONBLOCK);
    ptr = buf;
    while (ntowrite)
    {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, error = %d\n", nwrite, errno);   

        if (nwrite > 0) 
        {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    clr_fl(STDOUT_FILENO, O_NONBLOCK);
    return 0;
}
