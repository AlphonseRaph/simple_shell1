#include "myshell.h"
#include "alias.h"
#include "variable_replacement.h"
#include <stdio.h>
#include "get_full_path.h"
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_PATH_LENGTH 100



int main(int argc, char *argv[]) {
    char *program_name = argv[0];
    bool interactive_mode = true;

    if (argc > 1) {
        // File execution mode
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        interactive_mode = false;
        char input[MAX_COMMAND_LENGTH];

        // Read and execute commands from the file
        while (fgets(input, MAX_COMMAND_LENGTH, file) != NULL) {
            // Remove the trailing newline character, if present
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }

            // Remove any comments from the input
            char *comment_pos = strchr(input, '#');
            if (comment_pos != NULL) {
                *comment_pos = '\0';
            }

            // Tokenize the input into multiple commands
            char *commands[MAX_COMMAND_LENGTH];
            int command_count = my_tokenize(input, commands, MAX_COMMAND_LENGTH);
            if (command_count > 0) {
                // Execute each command separately
                bool prev_command_success = true;
                for (int i = 0; i < command_count; i++) {
                    // Check if the command contains && or ||
                    char *and_token = strstr(commands[i], "&&");
                    char *or_token = strstr(commands[i], "||");
                    // Skip the command if it's part of a logical AND or OR and the previous command failed
                    if ((and_token != NULL && !prev_command_success) || (or_token != NULL && prev_command_success)) {
                        continue;
                    }

                    char *args[MAX_COMMAND_LENGTH];
                    int token_count = my_tokenize(commands[i], args, MAX_COMMAND_LENGTH);
                    if (token_count > 0) {
                        replace_variables(args[0]); // Replace variables before executing the command
                        execute_command(args[0], program_name, args);
                        prev_command_success = (WIFEXITED(status) && WEXITSTATUS(status) == 0);
                    }
                }
            }
        }

        fclose(file);
    } else {
        // Interactive mode
        start_shell(program_name);
    }

    return 0;
}


char *get_full_path(char *command) {
    char *path = getenv("PATH");
    char *path_dirs[MAX_PATH_LENGTH];
    int dir_count = 0;

    // Tokenize the PATH to extract directories
    char *token = strtok(path, ":");
    while (token != NULL && dir_count < MAX_PATH_LENGTH - 1) {
        path_dirs[dir_count++] = token;
        token = strtok(NULL, ":");
    }
    path_dirs[dir_count] = NULL; // Set the last element to NULL

    // Check if the command exists in each directory in the PATH
    for (int i = 0; i < dir_count; i++) {
        char full_path[MAX_PATH_LENGTH + MAX_COMMAND_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dirs[i], command);

        if (access(full_path, X_OK) == 0) {
            return strdup(full_path);
        }
    }

    // Command not found in any directory in the PATH
    return NULL;
}

void print_environment() {
    // Loop through the environment variables and print them
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}
int my_tokenize(char *input, char *tokens[], int max_tokens) {
    int command_count = 0;
    char *current_command = strtok(input, ";");
    
    while (current_command != NULL && command_count < max_commands - 1) {
        commands[command_count++] = current_command;
        current_command = strtok(NULL, ";");
    }
    commands[command_count] = NULL;
    
    // Tokenize the individual commands to separate && and ||
    char *subcommands[MAX_COMMAND_LENGTH];
    for (int i = 0; i < command_count; i++) {
        int subcommand_count = 0;
        char *current_subcommand = strtok(commands[i], "&&||");
        
        while (current_subcommand != NULL && subcommand_count < MAX_COMMAND_LENGTH - 1) {
            subcommands[subcommand_count++] = current_subcommand;
            current_subcommand = strtok(NULL, "&&||");
        }
        subcommands[subcommand_count] = NULL;
        
        // Concatenate the subcommands back together with && or || as appropriate
        char *separator = strtok(NULL, "");
        if (separator != NULL) {
            for (int j = 0; j < subcommand_count; j++) {
                strcat(subcommands[j], separator);
            }
        }
        
        // Replace the original command with the concatenated subcommands
        strcpy(commands[i], subcommands[0]);
        for (int j = 1; j < subcommand_count; j++) {
            strcat(commands[i], separator);
            strcat(commands[i], subcommands[j]);
        }
    }
    
    return command_count;
}
void execute_command(char *command, char *program_name) {
if (strcmp(command, "exit") == 0) {
         if (strcmp(command, "exit") == 0) {
        // Handle the exit built-in command with arguments
        int status = 0;
        if (args[1] != NULL) {
            status = atoi(args[1]);
        }
        printf("Exiting shell with status %d\n", status);
        exit(status);
    } else if (strcmp(command, "setenv") == 0) {
        // Handle the setenv built-in command
        if (args[1] == NULL || args[2] == NULL) {
            fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
        } else if (setenv(args[1], args[2], 1) != 0) {
            perror("setenv");
        }
    } else if (strcmp(command, "unsetenv") == 0) {
        // Handle the unsetenv built-in command
        if (args[1] == NULL) {
            fprintf(stderr, "Usage: unsetenv VARIABLE\n");
        } else if (unsetenv(args[1]) != 0) {
            perror("unsetenv");
        }
    }
	else if (strcmp(command, "cd") == 0) {
        // Handle the cd built-in command
        char *directory = args[1];

        if (directory == NULL) {
            // If no argument is given, change to $HOME directory
            directory = getenv("HOME");
            if (directory == NULL) {
                fprintf(stderr, "cd: %s: No $HOME directory set.\n", program_name);
                return;
            }
        } else if (strcmp(directory, "-") == 0) {
            // Handle "cd -" to change to the previous directory
            directory = getenv("OLDPWD");
            if (directory == NULL) {
                fprintf(stderr, "cd: OLDPWD not set.\n");
                return;
            }
        }

        // Save the current directory in OLDPWD
        char current_directory[MAX_PATH_LENGTH];
        if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
            perror("getcwd");
            return;
        }
        setenv("OLDPWD", current_directory, 1);

        // Change to the new directory
        if (chdir(directory) != 0) {
            perror("cd");
        } else {
            // Update the PWD environment variable
            char new_directory[MAX_PATH_LENGTH];
            if (getcwd(new_directory, sizeof(new_directory)) != NULL) {
                setenv("PWD", new_directory, 1);
            } else {
                perror("getcwd");
            }
        }
    }
	else if (strcmp(command, "alias") == 0) {
        // Handle the alias built-in command
        if (args[1] == NULL) {
            // Print all aliases
            print_aliases();
        } else {
            // Process alias arguments
            for (int i = 1; args[i] != NULL; i++) {
                char *name = args[i];
                char *value = strchr(name, '=');

                if (value == NULL) {
                    // Print the alias if only the name is provided
                    Alias *alias = find_alias(name);
                    if (alias != NULL) {
                        printf("%s='%s'\n", alias->name, alias->value);
                    }
                } else {
                    *value = '\0';
                    value++;
                    // Check if the name is already an alias
                    Alias *alias = find_alias(name);
                    if (alias != NULL) {
                        // Update the value of an existing alias
                        strcpy(alias->value, value);
                    } else {
                        // Create a new alias
                        add_alias(name, value);
                    }
                }
            }
        }
    }
	else {
        // Execute other commands
        // Replace variables in the command before execution
        replace_variables(command);

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Child process
            execvp(command, args);
            perror("execvp");
            exit(1);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_command_status = WEXITSTATUS(status);
            }
        }
    }

       
	char *full_path = get_full_path(command);

    if (full_path == NULL) {
        fprintf(stderr, "%s: command not found: %s\n", program_name, command);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        // Child process
        char *args[] = {full_path, NULL};
        execvp(full_path, args);
        // Execvp will only return if an error occurs
        perror(full_path);
        _exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }

    free(full_path);
}

void start_shell(char *program_name) {
    char command[MAX_COMMAND_LENGTH];

    // Check if the program is running in interactive mode
    bool interactive_mode = isatty(fileno(stdin));

    while (true) {
        if (interactive_mode) {
            printf("Your Shell > ");
            fflush(stdout);
        }

        // Read the command from stdin
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\n");
            break;
        }

        // Remove the trailing newline character, if present
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        if (strcmp(command, "env") == 0) {
            print_environment(); // Print the current environment
        }
	// Remove any comments from the input
        char *comment_pos = strchr(input, '#');
        if (comment_pos != NULL) {
            *comment_pos = '\0';
        }
        // Tokenize the input into multiple commands
        int command_count = my_tokenize(input, commands, MAX_COMMAND_LENGTH);
        if (command_count > 0) {
            // Execute each command separately
            bool prev_command_success = true;
            for (int i = 0; i < command_count; i++) {
                // Check if the command contains &&
                char *and_token = strstr(commands[i], "&&");
                if (and_token != NULL) {
                    *and_token = '\0'; // Replace && with null terminator
                    if (prev_command_success) {
                       char *args[MAX_COMMAND_LENGTH];
                        int token_count = my_tokenize(commands[i], args, MAX_COMMAND_LENGTH);
                        if (token_count > 0) {
				replace_variables(args[0]); // Replace variables before executing the command

                         // Get the full path of the command
                    char *full_path = get_full_path(args[0]);
                    if (full_path != NULL) {
                        execute_command(full_path, program_name, args);
                        free(full_path);
                    } else {
                        fprintf(stderr, "%s: command not found\n", args[0]);
                    }
		prev_command_success = (WIFEXITED(status) && WEXITSTATUS(status) == 0);       
			}
                    }
                    and_token[0] = '&'; // Restore the && separator
                    and_token[1] = '&';
                } else {
                    // Check if the command contains ||
                    char *or_token = strstr(commands[i], "||");
                    if (or_token != NULL) {
                        *or_token = '\0'; // Replace || with null terminator
                        if (!prev_command_success) {
                            char *args[MAX_COMMAND_LENGTH];
                            int token_count = my_tokenize(commands[i], args, MAX_COMMAND_LENGTH);
                            if (token_count > 0) {
				    replace_variables(args[0]); // Replace variables before executing the command

                                 // Get the full path of the command
                    char *full_path = get_full_path(args[0]);
                    if (full_path != NULL) {
                        execute_command(full_path, program_name, args);
                        free(full_path);
                    } else {
                        fprintf(stderr, "%s: command not found\n", args[0]);
                    }
                                prev_command_success = (WIFEXITED(status) && WEXITSTATUS(status) == 0);
                            }
                        }
                        or_token[0] = '|'; // Restore the || separator
                        or_token[1] = '|';
                    } else {
                        // No && or || found, execute the command normally
                        char *args[MAX_COMMAND_LENGTH];
                        int token_count = my_tokenize(commands[i], args, MAX_COMMAND_LENGTH);
                        if (token_count > 0) {
				replace_variables(args[0]); // Replace variables before executing the command

                            execute_command(args[0], program_name, args)
                         prev_command_success = (WIFEXITED(status) && WEXITSTATUS(status) == 0);
                        }
                    }
                }
            }
        }
    }
    }
}
}
