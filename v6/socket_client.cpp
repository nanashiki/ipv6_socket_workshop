#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h> 
#include <errno.h> 

void sockaddr_print(struct sockaddr *ai_addr, socklen_t ai_addrlen ) {
    char host[128];
    char port[128];
    if( getnameinfo(ai_addr, ai_addrlen, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST|NI_NUMERICSERV)<0 ) {
        return;
    }
    if( ai_addr->sa_family == PF_INET ) {
        printf("%s:%s\n", host, port );
    } else {
        printf("[%s]:%s\n", host, port );
    }
}

int main() { 
    const char* hostname = "qiita.com";
    int port = 80;

    int sock = -1; 
    int err; 
    struct addrinfo hints; 
    struct addrinfo* res = NULL; 
    struct addrinfo* ai; 
 
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_UNSPEC;     // IPv4/IPv6両対応 
    hints.ai_socktype = SOCK_STREAM; // TCP 
    hints.ai_flags = AI_NUMERICSERV; 
 
    char service[11]; 
    sprintf(service, "%d", port); 
    err = getaddrinfo(hostname, service, &hints, &res); 
    if (err != 0) { 
        printf("getaddrinfo() failed: %s\n", gai_strerror(err)); 
        return -1; 
    }
    // 接続試行
    for (ai = res; ai; ai = ai->ai_next) { 
        printf("connect...\n");
        sockaddr_print(ai->ai_addr, ai->ai_addrlen); 
        /**********************************/
        /* Step 1. socket()               */
        /**********************************/
        sock = socket(
            ai->ai_family, // AF_INET or AF_INET6
            // #define PF_INET         AF_INET
            // #define PF_INET6        AF_INET6
            // #define AF_INET         2               /* internetwork: UDP, TCP, etc. */
            // #define AF_INET6        30              /* IP version 6 */
            ai->ai_socktype, // SOCK_STREAM(TCP).
            ai->ai_protocol // IPPROTO_TCP.
        );
        if ( sock == -1 ) { 
            freeaddrinfo(res); 
            return -1; 
        }

        /**********************************/
        /* Step 2. connect()               */
        /**********************************/
        if ( connect(sock, ai->ai_addr, ai->ai_addrlen) < 0 ) { 
            close(sock); 
            sock = -1;
            printf("connect failed\n");
        } else { 
            printf("connect success\n");
            //break;
        }
    }
    /**********************************/
    /* Step 3. read(), write()        */
    /**********************************/
    // 今回は省略

    /**********************************/
    /* Step 4. close()               */
    /**********************************/
    close(sock); 
    return 0; 
}