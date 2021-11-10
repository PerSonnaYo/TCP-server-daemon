#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLOW_SIZE 1024


int read_file(char* filename, int sock)
{
    FILE * fp;
    char * line = malloc(sizeof(char) * MAX_FLOW_SIZE);
    char * line_new = NULL;
    size_t len = MAX_FLOW_SIZE;

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((line_new = fgets(line, len, fp)) != NULL) {
        if (send(sock, line_new, strlen(line_new), 0) < 0) {
            close(sock);
            fprintf(stderr, "Connection lost\n");
            fclose(fp);
            return 1;
        }
        // free(line_new);
    }
    close(sock);
    fprintf(stdout, "Mission complete\n");
    if (line)
        free(line);
    if (line_new)
        free(line_new);
    fclose(fp);
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {//примерно также как и у сервера

    int sock, rv;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (argc != 4)
    {
        printf("Usage: ip, host, filename\n");
        return(2);
    }

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "Client: %s\n", gai_strerror(rv));
        return(1);
    }
    for(p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0) {
            continue;
        }
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) { //разница с сервером
            fprintf(stderr, "Client: %s\n", strerror(errno));
            close(sock);
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "Client: failed to connection\n");
        return(1);
    }
    freeaddrinfo(res);
    int op = read_file(argv[3], sock);
    return (op);
}
