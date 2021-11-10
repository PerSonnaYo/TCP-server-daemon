#include "daemonize.h"
#include "../server/server.h"

void handle_signal(int sig, void (* sighup)(), void (* sigterm)()) {

    switch (sig) {
        case SIGHUP:
            // syslog(LOG_INFO, "DEBUG: reloading daemon config file");
            fprintf(stderr, SD_NOTICE "NOTICE: reloading daemon config file\n");
            sighup();
            break;
        case SIGTERM:
            fprintf(stderr, SD_NOTICE "NOTICE: received SIGTERM signal\n");
            // syslog(LOG_INFO, "DEBUG: received SIGTERM signal");
            sigterm();
            break;
    }

}

static int handle_params(char **param1, FILE * confFile)
{
    char * line = malloc(sizeof(char) * MAX_FLOW_SIZE);
    char * line_new = NULL;
    size_t len = MAX_FLOW_SIZE;
    char **str;
    short index = 0;
    while ((line_new = fgets(line, len, confFile)) != NULL) {
        index++;
        str = ft_split(line_new, '=');
        switch(index){
            case 1: {
                if (strcmp(str[0], "port")) {
                        fprintf(stderr, SD_ERR "Invalid config parametrs (port=1111)\n");
                        ft_freetab(str, 2);
                        free(param1);
                        return (2);
                }
                else
                {
                    param1[0] = strdup(str[1]);
                    param1[0][strlen(param1[0]) - 1] = '\0';
                    // free_tab(str, 2);
                }
                break;
            }
            case 2: {
                
                if (strcmp(str[0], "path")) {
                        fprintf(stderr, SD_ERR "Invalid config parametrs (path=/etc/...)\n");
                        ft_freetab(str, 2);
                        // free(param1);
                        ft_freetab(param1, 1);
                        return (2);
                }
                else
                {
                    param1[1] = strdup(str[1]);
                    if (param1[1][strlen(param1[1]) - 1] == '\n')
                        param1[1][strlen(param1[1]) - 1] = '\0';
                    // free_tab(str, 2);
                }
                break;
            }
        }
        ft_freetab(str, 2);
    }
    if (line)
        free(line);
    if (line_new)
        free(line_new);

    return (0);
}

int read_conf_file(int reload, char* confFilename, char ** param1) {

    FILE* confFile = NULL;
    int ret = -1;
    if (reload == 1)
    {
        ft_freetab(param1, 2);
        param1 = malloc(sizeof(char*) * 2);
    }
    confFile = fopen(confFilename, "r");

    if (confFile == NULL) {
        // syslog(LOG_ERR, "Cannot open config file: %s", strerror(errno));
        fprintf(stderr, SD_ERR "Cannot open config file: %s", strerror(errno));
        free(param1);
        return 1;
    }
    ret = handle_params(param1, confFile);
    if (ret == 0) {
        if (reload == 1)
            fprintf(stderr, SD_NOTICE "Reloaded configuration file\n");
            // syslog(LOG_INFO, "Reloaded configuration file");
        else
            fprintf(stderr, SD_INFO  "Configuration file read\n");
            // syslog(LOG_INFO, "Configuration file read");
    }

    fclose(confFile); 

    return ret;
}