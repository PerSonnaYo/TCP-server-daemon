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
#define MAX_FLOW_SIZE 1024

void handle_signal(int sig, void (* sighup)(), void (* sigterm)());
int read_conf_file(int reload, char* confFilename, char **param1);
void open_log_file(FILE *logStream, char *logFilename);
char		**ft_split(char const *s, char c);
void	ft_freetab(char **str, size_t s);

#endif
