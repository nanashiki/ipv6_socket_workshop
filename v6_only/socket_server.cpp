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
    /**********************************/
    /* Step 1. socket()               */
    /**********************************/
    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); 
    if ( sock < 0 ) { 
        perror("server");
        return -1;
    }

    int on = 1;
    if ( setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on)) < 0 ) { 
        close(sock);
        perror("server");
        return -1;
    }

    sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr)); 
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(12345); 
 
    /**********************************/
    /* Step 3. bind()                 */
    /**********************************/
    if (bind(sock, (sockaddr*) &addr, sizeof(addr)) < 0) { 
        close(sock);
        perror("server");
        return -1;
    } 
 
    /**********************************/
    /* Step 3. listen()               */
    /**********************************/
    if (
        listen(
            sock,
            5 // 接続の待ち行列の最大数.
        ) < 0
    ) { 
        close(sock);
        perror("server");
        return -1;
    } 
    printf("Listen succeeded\n");
    printf("wait...\n"); 
 
    while (1) { 
        int cs; 
        // クライアントの情報を得る場合 
        struct sockaddr_in6 client_sa;
        socklen_t len = sizeof(client_sa);   
        cs = accept(sock, (struct sockaddr*) &client_sa, &len); 
        if ( cs == -1 ) { 
            if (errno == EINTR) 
                continue; 
            perror("accept"); 
            return -1;
        } 
 
        // accept した相手先を表示. 
        printf("accepted.\n");
        sockaddr_print((struct sockaddr*) &client_sa, len);
        // 親プロセス 
        close(cs);
    } 

    return 0; 
}