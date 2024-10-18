#include "log.h"
#include <netinet/in.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS     1024
#define BUFF_SIZE       1024

int main() {
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) {
        log_error("create socket error:%s", strerror(errno));
        return -1;
    }
    int use = 1;    
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &use, sizeof(use));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &use, sizeof(use));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    
    socklen_t addrlen = sizeof(servaddr);
    if (bind(sockfd, (struct sockaddr*)&servaddr, addrlen) == -1) {
        log_error("bind addr error:%s", strerror(errno));
        return -1;
    }
    
    if (listen(sockfd, 10) < 0) {
        log_error("listen erorr:%s", strerror(errno));
        return -1;
    }

    int     retval;
    int     maxfd = 0;
    int     clientfds[MAX_CLIENTS];
    fd_set  rfds, wfds;
    struct timeval val;
    
    for (int i = 0; i < MAX_CLIENTS; i++) 
        clientfds[i] = -1; 
        
    FD_ZERO(&rfds); 
    FD_ZERO(&wfds); 

    maxfd = sockfd;
    while (1) {
        //fd_set rfds_bak = rfds;  //select返回后，需要重新设置
        FD_SET(sockfd, &rfds);
        val.tv_sec = 5;
        val.tv_usec = 0;

        retval = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1) {
            log_error("select error:%s", strerror(errno));
            return -1;
        } else if (retval) {/*>0, return nums of ready fd*/
            if (FD_ISSET(sockfd, &rfds)) {
                struct sockaddr_in cliaddr;
                socklen_t   cliaddr_len;
                int client_fd = accept(sockfd, (struct sockaddr*)&cliaddr, &cliaddr_len); 
                if (client_fd < 0) {
                    log_error("accept error:%s", strerror(errno)); 
                    return -1;
                }
                getpeername(client_fd, (struct sockaddr*)&cliaddr, &cliaddr_len);
                log_info("client %s:%d connected server", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                int j = 0;
                for (j = 0; j < MAX_CLIENTS; j++) {
                    if (clientfds[j] == -1) {
                        clientfds[j] = client_fd; 
                        break;
                    }
                }
                if (j == MAX_CLIENTS) {
                    log_info("too many clients, dorp connection");
                    close(client_fd);
                    continue;
                }
                FD_SET(client_fd, &rfds);
                maxfd = client_fd > maxfd ? client_fd : maxfd;
            } 
            //recv data
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientfds[i] == -1) break;
                if (FD_ISSET(clientfds[i], &rfds)) {
                    char data_buff[BUFF_SIZE] = {0};
                    int n = recv(clientfds[i], data_buff, BUFF_SIZE, 0);
                    if (n == 0) {
                        FD_CLR(clientfds[i], &rfds);
                        close(clientfds[i]);
                        clientfds[i] = -1;
                        struct sockaddr_in cliaddr;
                        socklen_t cliaddr_len;
                        getpeername(clientfds[i], (struct sockaddr*)&cliaddr, &cliaddr_len);
                        log_info("client %s:%d exit", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                    } else {
                        log_info("recvfrom data:%s", data_buff);
                        send(clientfds[i], data_buff, n, 0);
                    }
                }
            }
        } else {
            log_info("not recv data with in timeout"); 
        }
    } 

    return 0;
}
