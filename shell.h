#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_PATH_LENGTH 100

/*
 * Function to start the shell in interactive mode or file execution mode

 */int main(int argc, char *argv[]);

/*
 * Function to start the shell in interactive mode
 */
void start_shell(char *program_name);

/*
 * Function to tokenize input into multiple commands
*/
 int my_tokenize(char *input, char *tokens[], int max_tokens);

/*
 * Function to execute a command
*/
 void execute_command(char *command, char *program_name, char *args[]);

/* 
 * Function to print all environment variables
*/
 void print_environment();

/*
 * Declare environ if it is not provided by the system
*/
 extern char **environ;

/*
 * Global variable for the status of the last command executed
*/
 int last_command_status;
bool interactive_mode;
#endif /* SHELL_H */

