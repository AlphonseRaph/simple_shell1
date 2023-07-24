#include "get_full_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

char *path;
char *dir;
char full_path[MAX_PATH_LENGTH + 1];
char *result;
char *get_full_path(char *command) {
    if (command == NULL) {
        return NULL;
    }

    // Get the PATH environment variable
   path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }

    // Tokenize the PATH variable to get individual directories
    dir = strtok(path, ":");
    while (dir != NULL) {
        // Concatenate the directory path with the command
        
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

        // Check if the file exists and is executable
        if (access(full_path, X_OK) == 0) {
            // Allocate memory for the full path and return it
            result = (char *)malloc(strlen(full_path) + 1);
            if (result != NULL) {
                strcpy(result, full_path);
            }
            return result;
        }

        // Move to the next directory in the PATH
        dir = strtok(NULL, ":");
    }

    // If the command is not found in any directory in the PATH, return NULL
    return NULL;
}

