#include "alias.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Alias *alias_list = NULL;

void add_alias(char *name, char *value) {
    Alias *new_alias = (Alias *)malloc(sizeof(Alias));
    if (new_alias == NULL) {
        perror("malloc");
        return;
    }
    strcpy(new_alias->name, name);
    strcpy(new_alias->value, value);
    new_alias->next = alias_list;
    alias_list = new_alias;
}

void print_aliases() {
    Alias *current = alias_list;
    while (current != NULL) {
        printf("%s='%s'\n", current->name, current->value);
        current = current->next;
    }
}

Alias *find_alias(char *name) {
    Alias *current = alias_list;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

