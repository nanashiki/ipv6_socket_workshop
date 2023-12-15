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
    int err; 
    struct addrinfo hints; 
    struct addrinfo* res = NULL;
    struct addrinfo* ai; 
    int sock; 
 
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
 
    err = getaddrinfo(NULL, "12345", &hints, &res); 
    if (err != 0) { 
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err)); 
        return -1; 
    } 
 
    ai = res;
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol); 
    if ( sock < 0 ) { 
        freeaddrinfo(res); 
        return -1; 
    }

    int on = 1;
    if ( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on)) < 0 ) { 
        close(sock);
        freeaddrinfo(res); 
        return -1; 
    }
 
    /**********************************/
    /* Step 3. bind()                 */
    /**********************************/
    if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) { 
        close(sock);
        freeaddrinfo(res); 
        return -1; 
    } 
 
    freeaddrinfo(res); 
    res = NULL; 
 
    // 表示する. port = 0 の場合は [::]:0 になってしまう。取得しなおす 
    sockaddr_storage addr;   // sockaddr_in は IPv4 only. 
    socklen_t socklen = sizeof(addr); 
    if ( getsockname(sock, (sockaddr*) &addr, &socklen) == -1 ) { 
        perror("get port failed"); 
        close(sock);
        return -1; 
    } 
    // sockaddr_print("listen socket", (sockaddr*) &addr, socklen); 
 
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
        return -1; 
    } 
    printf("Listen succeeded\n"); 

    printf("wait...\n"); 
 
    while (1) { 
        int cs; 
        // クライアントの情報を得る場合 
        struct sockaddr_storage client_sa;  // sockaddr_in 型ではない。 
        socklen_t len = sizeof(client_sa);   
        cs = accept(sock, (struct sockaddr*) &client_sa, &len); 
        if ( cs == -1 ) { 
            if (errno == EINTR) 
                continue; 
            perror("accept"); 
            exit(1); 
        } 
 
        // accept した相手先を表示. 
        printf("accepted.\n");
        // 親プロセス 
        close(cs);
    }
    return 0; 
}