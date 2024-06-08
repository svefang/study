/*
 * @Date: 2024-06-04 20:32:25
 * @code: UTF-8
 * @Description: 测试进程间传递文件描述符实现：socketpair 与recvmsg，sendmsg
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    int sv[2];

    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    if (ret < 0) {
        perror("socketpair error\n");
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork error\n");
        return -1;
    }
    else if (pid == 0) {/*child */
        /*recvmsg*/
        struct msghdr msg;

        union {
            struct cmsghdr cm;
            char space[CMSG_SPACE(sizeof(int))];
        } cmsg;


        memset(&msg, 0, sizeof(msg));

        msg.msg_control = &cmsg;
        msg.msg_controllen = sizeof(cmsg); 


        char buf[65535] = {0};
        struct iovec iov[1];
        iov[0].iov_base = buf;
        iov[0].iov_len = sizeof(buf);

        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;

        ssize_t size = recvmsg(sv[1], &msg, 0); 
        if (size < 0) {
            perror("recvmsg error\n");
            return -1;
        }
        printf("recv data:\"%s\" successfully!\n", buf);

        int fd = -1;
        memcpy(&fd, CMSG_DATA(&cmsg.cm), sizeof(int));

        lseek(fd, 0, SEEK_END); //定位到文件末尾处
        write(fd, buf, strlen(buf));
        
        if (close(fd) < 0) {
             perror("close fd error\n");
             return -1;
        }

    }

    else { //parent
        int fd = open("./test_file", O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) {
            perror("open  error\n");
            return -1;
        }
        union {
            struct cmsghdr cm;
            /**CMSG_SPACE：入参是cmsghdr头部后根据的数据长度，这里传递的是fd，所以使用sizeof(int)
             * 返回的是包含cmsghdr头部长度 + 填充数据 + 数据 + 填充数据
             */
            char    space[CMSG_SPACE(sizeof(int))];
        } cmsg;

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));

        msg.msg_control = &cmsg;
        msg.msg_controllen = sizeof(cmsg);


        /**
         * CMSG_LEN:入参是一个控制信息中数据大小，返回值是cmsghdr + 填充数据 + 数据
         * 这里数据大小是一个文件描述符的大小
        */
        cmsg.cm.cmsg_len = CMSG_LEN(sizeof(int));
        cmsg.cm.cmsg_level = SOL_SOCKET;
        cmsg.cm.cmsg_type = SCM_RIGHTS;//??
        /**
         *  CMSG_DATA:入参是控制信息结构，返回值为指向控制信息数据的指针
         * 
        */
        memcpy(CMSG_DATA(&cmsg.cm), &fd, sizeof(int));

        char buf[65535] = {0};
        if (argv[1] == NULL) {
            memcpy(buf, "test",  sizeof("test"));
        } else {
            memcpy(buf, argv[1],  strlen(argv[1]) + 1);
        }
        struct iovec  iov[1];
        iov[0].iov_base = buf;
        iov[0].iov_len = strlen(buf);

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_flags = 0;

        ssize_t size = sendmsg(sv[0], &msg, 0);  
        if (size < 0) {
            perror("sendmsg error\n");
            return -1;
        }
        printf("send data:\"%s\" successfully!\n", buf);
        if (close(fd) < 0) {
            perror("close fd error\n");
            return -1;
        }
    }
}
