/*
 * @Description: 
 * @Version: 2.0
 * @Autor: sve_fang
 * @Date: 2022-11-25 23:23:01
 * @LastEditors: sve_fang
 * @LastEditTime: 2022-11-26 00:14:08
 * @Code: UTF-8
 */
#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
/*
 * @return :read bytes size
 */

ssize_t 
readn(int fd,  void *vptr, size_t n)
{
    size_t nleft = n;
    char *ptr = vptr;    

    ssize_t nread = 0;

    while(nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)/*如果没有读完，继续读取剩下的字节(nleft-nread)*/
        {
            if (errno == EINTR) /* 处理中断的系统调用导致的错误 */
                nread = 0;
            
            else 
                return (-1);
        }
        else if (nread == 0)
            break;
        
        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);
}


ssize_t
writen(int fd, void *vptr, size_t n)
{
    size_t nleft = n;
    char * ptr = vptr;
    ssize_t nwrite = 0;

    while( nleft > 0)
    {
        if ((nwrite = write(fd, ptr, nleft)) <=0 )
        {
            if (nwrite < 0 && errno == EINTR)
            {
                nwrite = 0;
            }
            else
                return -1;
        }

        nleft -= nwrite;
        ptr += nwrite;
    }
    
    return n;
}


ssize_t
readline(int fd,  void *vptr, size_t maxlen)
{
    ssize_t        n, rc;

    char            c, *ptr;

    ptr = vptr;

    for ( n = 1; n < maxlen; n++)
    {
        again:

        if ( (rc = read(fd, &c, 1)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
            {
                break;
            }
        }
        else if (rc == 0)
        {
            *ptr = 0;
            return (n - 1);
        }
        else 
        {
            if (rc == EINTR)
            {
                goto again;
            }
            return -1;
        }
    }

    *ptr = 0;
    return n;
}