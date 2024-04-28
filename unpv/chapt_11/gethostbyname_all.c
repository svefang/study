/*
 * @Date: 2024-04-28 22:58:25
 * @code: UTF-8
 * @Description: 
 */
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * argv[0]: domain name
*/
int main(int argc, char *argv[])
{
    if (argc != 2 || !argv[1]) {
        printf("Usage: ./gethostbyname_all domain_name!\n");
        return -1;
    }

    char *domain_name;
    char **pptr;
    char str_ip[32] = {0};
    struct hostent *phostent;
    
    
    while(--argc > 0) {
        domain_name = *(++argv);
        if ((phostent = gethostbyname(domain_name)) == NULL) {
            printf("gethostbyname error\n");
            continue;
        }

        printf("official name:%s\n", phostent->h_name);

        for (pptr = phostent->h_aliases; *pptr != NULL; pptr++) {
            printf("alias name:%s\n", *pptr);
        }

        switch (phostent->h_addrtype) {
            case AF_INET:
                for (pptr = phostent->h_addr_list; *pptr != NULL; pptr++) {
                    printf("address: %s\n", inet_ntop(phostent->h_addrtype,
                           *pptr, str_ip, sizeof(str_ip)));
                  //printf("address: %s\n", inet_ntoa(*(struct in_addr*)*pptr));
                }
                break;
            default:
                printf("unknow type");
                break;
        }
    }



    return 0;
}