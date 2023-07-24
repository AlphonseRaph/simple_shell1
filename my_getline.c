#include "my_getline.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

char* my_getline(void)
{
    static char buffer[BUFFER_SIZE];
    static int buffer_index = 0;
    static int bytes_read = 0;
    static int bytes_remaining = 0;

    char* line = NULL;
    int line_index = 0;
    int read_complete = 0;

    while (!read_complete) {
        // Check if there's more data in the buffer
        if (bytes_remaining <= 0) {
            bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
                // End of input or error occurred
                return NULL;
            }
            bytes_remaining = bytes_read;
            buffer_index = 0;
        }

        // Read character by character until newline or end of buffer
        while (buffer_index < bytes_read) {
            if (buffer[buffer_index] == '\n') {
                read_complete = 1;
                break;
            }
            buffer_index++;
        }

        // Allocate memory for the line and copy characters
        int new_line_length = buffer_index - line_index;
        line = realloc(line, line_index + new_line_length + 1);
        if (!line) {
            fprintf(stderr, "Memory allocation failed.\n");
            return NULL;
        }
        memcpy(line + line_index, buffer, new_line_length);
        line_index += new_line_length;

        // Move remaining characters to the beginning of the buffer
        int remaining_bytes = bytes_read - buffer_index;
        if (remaining_bytes > 0) {
            memmove(buffer, buffer + buffer_index, remaining_bytes);
        }
        buffer_index = 0;
        bytes_remaining = remaining_bytes;
    }

    // Null-terminate the line
    line[line_index] = '\0';

    return line;
}

