#include "server.h"

/*
 * @brief Behavior of the server thread receiving and sending message from an to the client
 * @params id Client unique file descriptor (id) 
 */

// void* server_thread (void* id) {

//     int sentBytes, recvBytes;
//     char buf[MAX_FLOW_SIZE];
//     FILE *fd = fopen("/etc/TCPserver/TCPrecieve.txt", "a");

//     do {

//         memset(buf, 0, sizeof(buf));
//         recvBytes = recv((int) id, buf, MAX_FLOW_SIZE, 0);
//         if (recvBytes <= 0) {
//             if (recvBytes < 0)
//                 syslog(LOG_INFO, "Thread %d: connection lost\n", (int) id);
//             else
//                 syslog(LOG_INFO, "Thread %d: ending connection\n", (int) id);
//         } else {
//             sentBytes = send((int) id, buf, strlen(buf), 0);
//             fprintf(fd, "%s\n", buf);
//             if (recvBytes < 0) {
//                 syslog(LOG_INFO, "Thread %d: connection lost\n", (int) id);
//             }
//             else
//                 syslog(LOG_INFO, "Thread %d - message send: [%s]\n", (int) id, buf);
//         }

//     } while (recvBytes > 0);

//     close((int) id);
//     fclose(fd);
//     pthread_exit(0);

// }

/*
 * @brief Initialize TCP socket
 * @params port TCP port number
 */

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int recieve_msg(fd_set *master, int i, FILE* fd)
{
    int nbytes;
    // char *buf = calloc(1, 1);
    char buf[1024];

    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) { //recv плевать на размер буфера, поэтому возвращаем полный размер переданной строки
    // получена ошибка или соединение закрыто клиентом
        if (nbytes == 0) {
        // соединение закрыто
            fprintf(stderr, SD_INFO "selectserver: socket %d hung up\n", i);
            // fclose(fd);
        }
        else 
        {
            fprintf(stderr, SD_ERR "recv %s\n", strerror(errno));
        }
        close(i);
        FD_CLR(i, master); // удаляем из мастер-сета
    }
    else{
        // write(fd, buf, strlen(buf));
        // write(fd, "as\n", 3);
        fprintf(fd, "%s", buf);
        fseek(fd, strlen(buf), SEEK_END);
        // free(buf);
        // buf = NULL;
        memset(buf, 0, 1024);
        fprintf(stderr, SD_INFO "selectserver: socket %d hung up\n", nbytes);
    }
    return(nbytes);
} 

int initialize_socket () {
        fd_set master;    // главный сет дескрипторов
    fd_set read_fds;  // временный сет дескрипторов для select()
    int fdmax;        // макс. число дескрипторов

    int listener;     // дескриптор слушающего сокета
    int newfd;        // дескриптор для новых соединений после accept()
    struct sockaddr_storage remoteaddr; // адрес клиента
    socklen_t addrlen;

    // char buf[1024];    // буфер для данных клиента
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // для setsockopt() SO_REUSEADDR, ниже
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // очищаем оба сета
    FD_ZERO(&read_fds);

    // получаем сокет и биндим его
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    // int fd = open("/etc/TCPserver/TCPrecieve.txt",  O_RDWR | O_APPEND | O_SYNC); //открываем текстовый файл
    FILE* fd = fopen("/etc/TCPserver/TCPrecieve.txt", "a");
    if (fd == 0)
    {
        fprintf(stderr, SD_ERR "Cannot open txt file: %s", strerror(errno));
        fd = stderr;
    }

    if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
        fprintf(stderr, SD_ERR "selectserver: %s\n", gai_strerror(rv));
        return(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

    // избегаем ошибки «address already in use»
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

// если мы попали сюда, значит мы не смогли забиндить сокет
    if (p == NULL) {
        fprintf(stderr, SD_ERR "selectserver: failed to bind\n");
        return(1);
    }

    freeaddrinfo(ai); // с этим мы всё сделали

// слушаем
    if (listen(listener, SOMAXCONN) == -1) {
        fprintf(stderr, SD_ERR "listen %s\n", strerror(errno));
        return(1);
    }

// добавляем слушающий сокет в мастер-сет
    FD_SET(listener, &master);

    // следим за самым большим номером дескриптора
    fdmax = listener; // на данный момент это этот

    // главный цикл
    while(1) 
    {
        read_fds = master; // копируем его
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            fclose(fd);
            // close(listener);
            for(i = 0; i <= fdmax; i++) //select возвращает ошибку если поступают сигналы
            {
                if (FD_ISSET(i, &read_fds)) {
                    close(i);
                }
            }
            if (end == false)
                return(60);
            return 4;
        }

// проходим через существующие соединения, ищем данные для чтения
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // есть!
                if (i == listener) {
    // обрабатываем новые соединения
                    addrlen = sizeof remoteaddr;
                    newfd = accept  (listener,
                                    (struct sockaddr *)&remoteaddr,
                                    &addrlen);

                    if (newfd == -1) {
                        fprintf(stderr, SD_ERR "accept %s\n", strerror(errno));
                    } 
                    else 
                    {
                        FD_SET(newfd, &master); // добавляем в мастер-сет
                        if (newfd > fdmax) {    // продолжаем отслеживать самый большой номер дескиптора
                            fdmax = newfd;
                        }
                    }
                }
                else
                {
                    // обрабатываем данные клиента
                   nbytes =  recieve_msg(&master, i, fd);
                    // if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                    // // получена ошибка или соединение закрыто клиентом
                    //     if (nbytes == 0) {
                    // // соединение закрыто
                    //         fprintf(stderr, SD_INFO "selectserver: socket %d hung up\n", i);
                    //     }
                    //     else 
                    //     {
                    //         fprintf(stderr, SD_ERR "recv %s\n", strerror(errno));
                    //     }
                    //     close(i);
                    //     FD_CLR(i, &master); // удаляем из мастер-сета
                    // } 
                    // else
                    // {
                        // while (nbytes > 0)
                        // {
                        //     if (nbytes == 1024)
                        //         nbytes = recieve_msg(&master, i, fd);
                        //     else
                        //         break;
                        // }
                    // }
                } // Закончили обрабатывать данные от клиента
            } // Закончили обрабатывать новое входящее соединение
        } // Закончили цикл по дескрипторам
    }
    fclose(fd);
    return 0;
}
    // int s;
    // unsigned int sockLen;
    // struct sockaddr_in server;

    // s = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(s, F_SETFL, O_NONBLOCK);
    // if (s < 0) {
    //     syslog(LOG_INFO, "Failed opening socket\n");
    //     return 1;
    // }

    // server.sin_family = AF_INET;
    // server.sin_addr.s_addr = INADDR_ANY;

    // if (port <= 0) {
    //     syslog(LOG_INFO, "Invalid port\n");
    //     close(s);
    //     return 1;
    // }
    // else server.sin_port = htons(port);

    // if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
    //     syslog(LOG_INFO, "Bind failed\n");
    //     close(s);
    //     return 1;
    // }

    // sockLen = sizeof(server);
    // if (getsockname(s, (struct sockaddr *) &server, &sockLen) < 0) {
    //     syslog(LOG_INFO, "Failed getting socket name\n");
    //     close(s);
    //     return 1;
    // }

    // listen(s, QUEUE_LENGTH);
    // return s;

// }

/*
 * @brief Accept client connection and creates a client thread
 * @params s Socket file descriptor
 */

// int establish_connection (int s) {

//     int id;
//     unsigned int sockLen;
//     struct sockaddr_in client;

//     sockLen = sizeof(client);
//     id = accept(s, (struct sockaddr *) &client, &sockLen);
//     if (id < 0)
//         syslog(LOG_INFO, "Socket can't accept connections\n");
//     else {
//         pthread_t thd;
//         pthread_create(&thd, 0, (void *)server_thread, (void*)id);
//     }

// }

/*
 * @brief Closes TCP socket
 * @params s Socket file descriptor
 */

// int socket_close (int s) {

//     return close(s);

// }
