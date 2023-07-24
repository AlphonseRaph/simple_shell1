#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define MAX_COMMAND_LENGTH 100

// Function to start the shell
void start_shell(char *program_name);

// Function to tokenize the input into commands
int my_tokenize(char *input, char *tokens[], int max_tokens);

// Function to execute a command
void execute_command(char *command, char *program_name, char *args[]);

#endif /* SHELL_H */

