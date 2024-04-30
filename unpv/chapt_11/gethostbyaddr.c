#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/**
 * 此接口查询的是/etc/resolv.conf和/etc/hosts文件中对应的主机信息
 * ./gethostbyaddr 8.8.8.8
 * 
*/

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        printf("Usage: %s address\n", argv[0]);
        return -1;
    }

    struct in_addr inp;
    char    **pptr;
    char    *str;
    const char *ip = argv[1];

    if (inet_aton(ip, &inp) == 0) {
        printf("inet_aton error\n");
        return -1;
    }

    struct hostent *hostent = gethostbyaddr(&inp, 4, AF_INET);
    if (hostent == NULL) {
        printf("gethostbyaddr error\n");
        return -1;
    }

    printf("offical name:%s\n", hostent->h_name);

    switch (hostent->h_addrtype) {
        case AF_INET:
            for (pptr = hostent->h_addr_list; *pptr != NULL; pptr++) {
                printf("address:%s\n", inet_ntop(hostent->h_addrtype, *pptr,
                        str, sizeof(str)));
            }
            break;
        default:
            printf("unknow address type\n");
            break;
    }

    return 0;
}