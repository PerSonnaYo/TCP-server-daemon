#include "server.h"

static void *get_in_addr(struct sockaddr *sa) //возвращаем ipv4 или ipv6 адрес
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int recieve_msg(fd_set *master, int i, FILE* fd)
{
    int nbytes;
    char buf[1024];

    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
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
        fprintf(fd, "%s", buf);
        fseek(fd, strlen(buf), SEEK_END);// передвигаем каретку
        memset(buf, 0, sizeof(buf)); 
    }
    return(nbytes);
}

static int new_connection(fd_set *master, int fdmax, int listener)
{
    char remoteIP[INET6_ADDRSTRLEN];
    struct sockaddr_storage remoteaddr; // адрес клиента
    int newfd; // дескриптор для новых соединений после accept()

    socklen_t addrlen = sizeof remoteaddr;
    newfd = accept  (listener,
                    (struct sockaddr *)&remoteaddr,
                    &addrlen);
    if (newfd == -1) {
        fprintf(stderr, SD_ERR "accept %s\n", strerror(errno));
    } 
    else 
    {
        FD_SET(newfd, master); // добавляем в мастер-сет
        if (newfd > fdmax) {    // продолжаем отслеживать самый большой номер дескиптора
            fdmax = newfd;
        }
        fprintf(stderr, SD_INFO "Server: new connection from %s on socket %d\n",
                inet_ntop(  remoteaddr.ss_family, 
                            get_in_addr((struct sockaddr *)&remoteaddr),
                            remoteIP, 
                            INET6_ADDRSTRLEN),
                newfd);
    }
    return(fdmax);
}

int initialize_socket (char **param1) {
    fd_set master;    // главный сет дескрипторов
    fd_set read_fds;  // временный сет дескрипторов для select()
    int fdmax;        // макс. число дескрипторов

    int listener;     // дескриптор слушающего сокета
    
    int yes=1;        // для setsockopt() SO_REUSEADDR, ниже
    int i, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // очищаем оба сета
    FD_ZERO(&read_fds);

    // получаем сокет и биндим его
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    FILE* fd = fopen(param1[1], "a"); //открываем текстовый файл
    if (fd == 0)
    {
        fprintf(stderr, SD_ERR "Cannot open txt file:{ %s } %s\n", param1[1], strerror(errno));
        fd = stderr;
    }

    if ((rv = getaddrinfo(NULL, param1[0], &hints, &ai)) != 0) {
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
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
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
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { // есть!
                if (i == listener)
                {
                    // обрабатываем новые соединения
                    fdmax = new_connection(&master, fdmax, listener);
                }
                else
                {
                    // обрабатываем данные клиента
                    recieve_msg(&master, i, fd);
                }
            } // Закончили обрабатывать новое входящее соединение
        } // Закончили цикл по дескрипторам
    }
    fclose(fd);
    return 0;
}