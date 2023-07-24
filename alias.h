Copy code
#ifndef ALIAS_H
#define ALIAS_H

#define MAX_COMMAND_LENGTH 100

// Struct to represent an alias
typedef struct Alias {
    char name[MAX_COMMAND_LENGTH];
    char value[MAX_COMMAND_LENGTH];
    struct Alias *next;
} Alias;

// Function to add an alias to the alias list
void add_alias(char *name, char *value);

// Function to print all aliases
void print_aliases();

// Function to find an alias by name
Alias *find_alias(char *name);

#endif /* ALIAS_H */
