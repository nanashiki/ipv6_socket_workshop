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
    /**********************************/
    /* Step 1. socket()               */
    /**********************************/
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent* host = gethostbyname(hostname); 
    if (!host) { 
        printf("gethostbyname() failed\n"); 
        return -1; 
    }

    unsigned int **addrptr = (unsigned int **)host->h_addr_list;

    struct sockaddr_in server;
	server.sin_addr.s_addr = *(*addrptr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

    /**********************************/
    /* Step 2. connect()               */
    /**********************************/
	if ( connect(sock,(struct sockaddr *)&server, sizeof(server))  < 0) {
        printf("connect faild\n");
        close(sock); 
        sock = -1; 
        return -1;
    } else {
        printf("connect success\n");
        sockaddr_print((struct sockaddr *)&server, sizeof(server));
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