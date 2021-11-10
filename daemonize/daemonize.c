#include "daemonize.h"
#include "../server/server.h"
/*
 * @brief Defines the daemon behavior for different signals
 * @params sig POSIX signal
 * @params sigint Function to be executed if signal is SIGINT
 * @params sighup Function to be executed if signal is SIGHUP
 * @params sigchld Function to be executed if signal is SIGCHLD
 */

void handle_signal(int sig, void (* sighup)(), void (* sigterm)()) {

    switch (sig) {
        // case SIGINT:
        //     syslog(LOG_INFO, "DEBUG: stopping daemon");
        //     sigint();
        //     signal(SIGINT, SIG_DFL);
        //     break;
        case SIGHUP:
            // syslog(LOG_INFO, "DEBUG: reloading daemon config file");
            fprintf(stderr, SD_NOTICE "NOTICE: reloading daemon config file\n");
            sighup();
            break;
        case SIGTERM:
            // end = false;
            fprintf(stderr, SD_NOTICE "NOTICE: received SIGTERM signal\n");
            // perror("NOTICE: received SIGTERM signal");
            // syslog(LOG_INFO, "DEBUG: received SIGTERM signal");
            sigterm();
            // signal(SIGTERM, SIG_DFL);
            break;
    }

}

/*
 * @brief Defines the daemon behavior for different signals
 * @params reaload Boolean value to define if the daemon will be realoaded 
 * @params confFilename Name of the conf file
 * @params port Number of the server port
 */

int read_conf_file(int reload, char* confFilename) {

    FILE* confFile = NULL;
    int ret = -1;

    if (confFilename == NULL) return 0;

    confFile = fopen(confFilename, "r");

    if (confFile == NULL) {
        // syslog(LOG_ERR, "Cannot open config file: %s", strerror(errno));
        fprintf(stderr, SD_ERR "Cannot open config file: %s", strerror(errno));
        return 1;
    }

    ret = fscanf(confFile, "%s", port);
    
    if (ret > 0) {
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

/*
 * @brief Opens a log file
 * @params logStream FILE pointer to log file
 * @params logFilename Name of the log file
 */

// void open_log_file(FILE *logStream, char *logFilename) {

//     if (logFilename != NULL) {
//         logStream = fopen(logFilename, "w+");
//         if (logStream == NULL) {
//             fprintf(stderr, SD_ERR "Cannot open log file: %s", strerror(errno));
//             // syslog(LOG_ERR, "Cannot open log file: %s", strerror(errno));
//             logStream = stdout;
//         } else logStream = stdout;
//     }

// }
