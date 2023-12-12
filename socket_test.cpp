#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h> 
#include <errno.h> 

int PORT = 12345;

/** 
 * @param node bind() するホスト名. NULL の場合, INADDR_ANY or IN6ADDR_ANY_INIT. 
 * @param port ポート番号. 0 の場合、空き番号を利用. 
 * @return INVALID_SOCKET エラー 
 *         >=0  listenしているソケット 
 */ 
int tcp_listen(const char* node, int port) 
{ 
    if ( port < 0 ) {
        fprintf(stderr, "tcp_listen(): invalid port number %d\n", port); 
        return -1;
    }
 
    int err; 
    struct addrinfo hints; 
    struct addrinfo* res = NULL;
    struct addrinfo* ai; 
    int sock; 
 
    memset(&hints, 0, sizeof(hints)); 
    if (!node) {
        hints.ai_family = AF_INET6;    // AF_INET6は、IPv4/IPv6両方を受け付ける。 
    }
    hints.ai_socktype = SOCK_STREAM; 
    // AI_PASSIVE をセットせずに, node = NULL の場合は, loopbackアドレス 
    // (INADDR_LOOPBACK or IN6ADDR_LOOPBACK_INIT). 
    // AI_PASSIVE をセットして, node = NULLのときは, INADDR_ANY, IN6ADDR_ANY_INIT. 

    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
 
    // (node, service) の両方を nullptr にすると getaddrinfo() が失敗するが, 
    // このようにすれば, ephemeral port から取れる. 
    char service[11]; 
    sprintf(service, "%d", port); //service = "12345";
    err = getaddrinfo(node, service, &hints, &res); 
    if (err != 0) { 
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err)); 
        return -1; 
    } 
 
    ai = res;

    /**********************************/
    /* Step 2. socket()               */
    /**********************************/
    // sock_print("create socket", ai->ai_family, ai->ai_socktype); 
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol); 
    if ( sock < 0 ) { 
        freeaddrinfo(res); 
        return -1; 
    }

    int on = 1;
    // if ( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, 
    if ( setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, 
                    sizeof(on)) < 0 ) { 
        close(sock);
        freeaddrinfo(res); 
        return -1; 
    } 
    printf("set SO_REUSEADDR\n"); 
 
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
 
    return sock; 
}

void test_server() 
{ 
    int listen_sock; 
 
    listen_sock = tcp_listen(NULL, PORT);  // [::]:12345 
    if (listen_sock == -1) { 
        perror("server"); 
        exit(1); 
    } 
 
    printf("wait...\n"); 
 
    while (1) { 
        int cs; 
        // クライアントの情報を得る場合 
        struct sockaddr_storage client_sa;  // sockaddr_in 型ではない。 
        socklen_t len = sizeof(client_sa);   
        cs = accept(listen_sock, (struct sockaddr*) &client_sa, &len); 
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
}

int main() 
{
    test_server();
    return 0; 
}