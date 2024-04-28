/*
 * @Date: 2024-04-28 22:43:09
 * @code: UTF-8
 * @Description: 
 */
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int main(int argc, char *argv[])
{
    struct hostent * hostent = gethostbyname((char *)"fangxing.site");
    if (hostent == NULL) {
        perror("gethostbyname");
        return -1;
    }


    printf("ip:%s\n", inet_ntoa(*(struct in_addr*)hostent->h_addr_list[0])); 
    printf("name:%s\n", hostent->h_name); 
    printf("length:%d\n", hostent->h_length);
    printf("alias:%s\n", hostent->h_aliases[0]); 
    printf("type:%d\n", hostent->h_addrtype); //AF_INET  = 2

    return 0;
}