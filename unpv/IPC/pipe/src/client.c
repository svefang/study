/**
 * @Description: 
 * @Version: 2.0
 * @Autor: sve_fang
 * @Date: 2022-11-11 08:05:14
 * @LastEditors: sve_fang
 * @LastEditTime: 2022-11-27 00:06:12
 * @Code: UTF-8
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

extern ssize_t writen(int fd, void * vptr, size_t n);
extern ssize_t readn(int fd, void * vptr, size_t n);
extern ssize_t readline(int fd, void *vptr, size_t n);

void str_cli(FILE *fp, int sockfd)
{
    char   sendline[4096], recvline[4096];

    while (fgets(sendline, 4096, fp) != NULL)
    {
        /**
         *  @ 模拟触发SIGPIPE信号，该信号触发的规则是，当一个socket收到RST报文后，
         *  进程继续向这个socket进行写操作，那么内核会产生一个SIGPIPE信号给进程
         * 
         *  @ 模拟代码如下：
         *      write(socket, sendfile, 1) //引发RST 
         *      sleep(1)
         *      write(socket, sendfile + 1, strlen(sendline) - 1) 
         *    
         *  @ 解决办法: 忽略该信号，或者在信号处理函数中判断，当write返回EPIPE错误时，
         *  进行相应的处理操作 
         * 
        */


        writen(sockfd, sendline, strlen(sendline));
        if ((readline(sockfd, recvline, 4096)) == 0)
        {
            printf("str_cli: server terminated permaturely!");
        }
        // printf是不可重入的，这里只是打印，最好不要使用
        printf("%s", recvline);
    }
}

int max(int a, int b)
{
    return  a > b ? a : b;
}

void str_select_cli(FILE *fp, int sockfd)
{
    int         maxfd;
    fd_set      rset;
    int         ret = 0;
    char        sendline[4096], recvline[4096];

    FD_ZERO(&rset);

    for (;;)
    {
        // select返回时，会将就绪的文件描述符标志为清0，所以这里重新设置
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);

        maxfd = max(fileno(fp), sockfd);

        if ((ret = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                printf("select error\n");
                return;
            }
        }

        // sockfd readable
        if (FD_ISSET(sockfd, &rset))
        {
            if ((readline(sockfd, recvline, 4096)) == 0)
            {
                printf("str_cli: server terminated permaturely!\n");
                return;
            }
            printf("%s", recvline);
        }

        if (FD_ISSET(fileno(fp), &rset))
        {
            if (fgets(sendline, 4096, fp) == NULL)
            {
                return;
            }

            writen(sockfd, sendline, strlen(sendline));
        }
    }

}



int main(int argc, char *argv[])
{
    char recvbuf[4096];
    int n = 0;
    int ret = 0;
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        printf("please input ip\n");
        return 0;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        printf("socket error\r\n");
        return -1;
    } 
    
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0 )
    {
        printf("inet_pton error\r\n");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))//进行三次握手
    {
        printf("connect error:%s\r\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    //str_cli(stdin, sockfd);
    str_select_cli(stdin, sockfd);

    return 0;
}
