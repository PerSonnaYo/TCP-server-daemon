#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLOW_SIZE 1024


int read_file(char* filename, int sock)
{
    FILE * fp;
    char * line = malloc(sizeof(char) * MAX_FLOW_SIZE);
    char * line_new = NULL;
    size_t len = MAX_FLOW_SIZE;
    ssize_t read;
    int dl;
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((line_new = fgets(line, len, fp)) != NULL) {
        printf("%s\n", line);
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
    // if (line)
    //     free(line);
    fclose(fp);
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {

    int s, port;
    char buf[MAX_FLOW_SIZE];
    struct sockaddr_in client;
    struct hostent *hp;

    if (argc != 4)
    {
        printf("Usage: ip, host, filename\n");
        return(2);
    }
    hp = gethostbyname(argv[1]);
    if (((char *) hp) == NULL) {
        printf("Invalid host\n");
        return 2;
    }

    memcpy((char*)&client.sin_addr, (char*)hp->h_addr, hp->h_length);
    client.sin_family = AF_INET;
    port = atoi(argv[2]);
    if (port <= 0) {
        printf("Invalid port\n");
        return 2;
    }
    client.sin_port = htons(port);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf("Failed opening socket\n");
        return 1;
    }

    if (connect(s, (struct sockaddr *)&client, sizeof(client)) == -1) {
        printf("Connection failed\n");
        close(s);
        return 1;
    }

    // while (1) {
        int op = read_file(argv[3], s);
        return (op);
        // printf("Type message (type END to close): ");
        // scanf("%s", buf);

        // if (!strcmp(buf, "END")) {
        //     close(s);
        //     return 0;
        // }

        // printf("Typed message: [%s]\n", buf);
        // printf("Buffer size: %zu\n", strlen(buf));

        // if (send(s, buf, strlen(buf), 0) < 0) {
        //     close(s);
        //     printf("Connection lost");
        //     return 1;
        // }
        // memset(buf, 0, sizeof(buf));

        // if(recv(s, buf, MAX_FLOW_SIZE, 0) <= 0) {
        //     close(s);
        //     return 1;
        // }

        // printf("Server returned %zu char: [%s]\n", strlen(buf), buf);
        // memset(buf, 0, sizeof(buf));

    // } 
    
}
