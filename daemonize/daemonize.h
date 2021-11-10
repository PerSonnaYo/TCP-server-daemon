#ifndef DAEMONIZE_H
#define DAEMONIZE_H

#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

void handle_signal(int sig, void (* sighup)(), void (* sigterm)());
int read_conf_file(int reload, char* confFilename);
void open_log_file(FILE *logStream, char *logFilename);

#endif
