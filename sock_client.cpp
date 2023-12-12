#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h> 
#include <errno.h> 

void sockaddr_print(struct	sockaddr *ai_addr, socklen_t ai_addrlen ) {
    char host[128];
    char port[128];
    printf("%s\n", ai_addr);
    if( getnameinfo(ai_addr, ai_addrlen, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST|NI_NUMERICSERV)<0 ) {
        return;
    }
    if( ai_addr->sa_family == PF_INET ) {
        printf("%s:%s\n", host, port );
    } else {
        printf("[%s]:%s\n", host, port );
    }
}

/** 
 * サーバに接続する。Block する. 
 * @return エラーのとき -1 
 */ 
int connect_to_server( 
    const char* hostname,  // IPv4 or IPv6ホスト名 
    int port )   // ポート番号 
{ 
    assert( hostname ); 
    if ( port <= 0 || port > 65535 ) 
        return -1; 
 
    int sockfd = -1; 
    int err; 
    struct addrinfo hints; 
    struct addrinfo* res = NULL; 
    struct addrinfo* ai; 
 
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_UNSPEC;     // IPv4/IPv6両対応 
    hints.ai_socktype = SOCK_STREAM; // TCP
    // AI_NUMERICSERV を指定しなければ、service は 'pop'などでもよい。 
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
        printf("connect...");
        sockaddr_print(ai->ai_addr, ai->ai_addrlen); 
        sockfd = socket(
            ai->ai_family, // AF_INET or AF_INET6
            // #define PF_INET         AF_INET
            // #define PF_INET6        AF_INET6
            // #define AF_INET         2               /* internetwork: UDP, TCP, etc. */
            // #define AF_INET6        30              /* IP version 6 */
            ai->ai_socktype, // SOCK_STREAM(TCP).
            ai->ai_protocol // IPPROTO_TCP.
        );
        if ( sockfd == -1 ) { 
            freeaddrinfo(res); 
            return -1; 
        } 
        // 実際に connect しないと正解か分からない. 
        if ( connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0 ) { 
            close(sockfd); 
            sockfd = -1; 
        } else { 
            printf("connect success ");
            sockaddr_print(ai->ai_addr, ai->ai_addrlen);
            // break;
        }
    }
    freeaddrinfo(res);
    return sockfd; 
}

int main() 
{ 
    int sockfd; 
    char ch; 
 
    // sockfd = connect_to_server("localhost", 12345);
    sockfd = connect_to_server("qiita.com", 80);
    if ( sockfd == -1 ) { 
        perror("client"); 
        return 1; 
    }
    close(sockfd); 
    return 0; 
}