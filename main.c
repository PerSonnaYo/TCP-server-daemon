#include "daemonize/daemonize.h"
#include "server/server.h"
#include <time.h>

bool end = true;
static char *confFilename = "/etc/TCPserver/TCPserver.conf";
char **param;

void server_signal_handler(int sig);

void init_mas()
{
    param = malloc(sizeof(char*) * 2);
}

int main (void) {

    int s = 0;
    int p;

    signal(SIGTERM, server_signal_handler);
    signal(SIGHUP, server_signal_handler);

    init_mas();
    if ((p = read_conf_file(0, confFilename, param)) != 0)
    {  
        s = p; //прерываем работу программы
        end = false;
    }
    if (!p){
        while(1)
        {
            s = initialize_socket(param);
            if (s == 4)
                continue;
            else if (s == 1)
            {
                ft_freetab(param, 2);
                break;
            }
            else if (s == 0 || s == 60)
            {
                s = 0;
                ft_freetab(param, 2);
                break;
            }
        }
    }
    return s;
}

void sigterm() {
    end = false;
    return;
}

void sighup() {
    int p;
    sleep(1);
    if ((p = read_conf_file(1, confFilename, param)) != 0)
        end = false;
}

void server_signal_handler(int sig) { //обработчик сигналов
    handle_signal(sig, sighup, sigterm);
    return;
}
