#include "daemonize/daemonize.h"
#include "server/server.h"
#include <regex.h>

bool end = true;
char port[6] = {0,0,0,0,0,0};
static char *confFilename = "/etc/TCPserver/TCPserver.conf";
// static char *logFilename = "/var/log/TCPserver.log";
static char *daemonName  = "TCPserver";

void server_signal_handler(int sig);

int main (void) {

    int s = 0;
    int p;
    // FILE *logStream;
    // openlog(daemonName, LOG_PID|LOG_CONS, LOG_DAEMON);

    // syslog(LOG_INFO, "Started %s", daemonName);
    // fprintf(stderr, SD_NOTICE "Started %s", daemonName);

    // open_log_file(logStream, logFilename);

    signal(SIGTERM, server_signal_handler);
    signal(SIGHUP, server_signal_handler);

    if ((p = read_conf_file(0, confFilename)) == -1)
        end = false;
        // if ((p = read_conf_file(0, confFilename)) == -1)
        // end = false;
    // s = initialize_socket(port);
    // while (running == 1)
    //     establish_connection(s);//ListenStream=/etc/TCPserver/TCPserver.conf
    while(1)
    {
        s = initialize_socket();
        if (s == 4)
            continue;
        else if (s == 60 || s == 1)
        {
            s = 1;
            break;
        }
        else if (s == 0)
            break;
    }

    // syslog(LOG_INFO, "Stopped %s", daemonName);
    // closelog();
    // if (logStream != stdout)
        // fclose(logStream);

    return s;
    
}

void sigterm() {
    end = false;
    return;
}

void sighup() {
    int p;
    if ((p = read_conf_file(1, confFilename)) == -1)
        end = false;
}

void server_signal_handler(int sig) {
    handle_signal(sig, sighup, sigterm);
    return;
}
