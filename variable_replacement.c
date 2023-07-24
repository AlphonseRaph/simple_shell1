#include "variable_replacement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int last_command_status = 0;

void replace_variables(char *command) {
    char *pos;
    char pid[20];

    // Replace $? with the last command status
    while ((pos = strstr(command, "$?")) != NULL) {
        *pos = '\0';
        sprintf(pid, "%d", last_command_status);
        strcat(command, pid);
        strcat(command, pos + 2);
    }

    // Replace $$ with the process ID of the shell
    while ((pos = strstr(command, "$$")) != NULL) {
        *pos = '\0';
        sprintf(pid, "%d", getpid());
        strcat(command, pid);
        strcat(command, pos + 2);
    }
}
