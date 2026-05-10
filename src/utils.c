#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int parse_int(const char *text, int *out_value)
{
    int value;
    int sign;
    size_t i;

    if (text == NULL || text[0] == '\0') {
        return -1;
    }

    value = 0;
    sign = 1;
    i = 0;

    if (text[0] == '-') {
        sign = -1;
        i = 1;
    }

    if (text[i] == '\0') {
        return -1;
    }

    for (; text[i] != '\0'; i++) {
        if (!isdigit((unsigned char)text[i])) {
            return -1;
        }
        value = (value * 10) + (text[i] - '0');
    }

    *out_value = value * sign;
    return 0;
}

int parse_priority(const char *text, int *out_priority)
{
    int value;

    if (parse_int(text, &value) != 0) {
        return -1;
    }
    if (value < 1 || value > 5) {
        return -1;
    }

    *out_priority = value;
    return 0;
}

int is_valid_due_date(const char *text)
{
    size_t i;

    if (text == NULL || text[0] == '\0') {
        return 1;
    }

    if (strlen(text) != 10) {
        return 0;
    }

    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (text[i] != '-') {
                return 0;
            }
        } else if (!isdigit((unsigned char)text[i])) {
            return 0;
        }
    }

    return 1;
}

int str_is_blank(const char *text)
{
    size_t i;

    if (text == NULL) {
        return 1;
    }

    for (i = 0; text[i] != '\0'; i++) {
        if (!isspace((unsigned char)text[i])) {
            return 0;
        }
    }

    return 1;
}

void normalize_description(char *text)
{
    size_t read_index;
    size_t write_index;
    int last_was_space;

    if (text == NULL) {
        return;
    }

    read_index = 0;
    while (text[read_index] != '\0' && isspace((unsigned char)text[read_index])) {
        read_index++;
    }

    write_index = 0;
    last_was_space = 0;

    for (; text[read_index] != '\0'; read_index++) {
        if (text[read_index] == '\r' || text[read_index] == '\n' || text[read_index] == '\t') {
            text[read_index] = ' ';
        }

        if (isspace((unsigned char)text[read_index])) {
            if (!last_was_space) {
                text[write_index++] = ' ';
                last_was_space = 1;
            }
        } else {
            text[write_index++] = text[read_index];
            last_was_space = 0;
        }
    }

    if (write_index > 0 && text[write_index - 1] == ' ') {
        write_index--;
    }

    text[write_index] = '\0';
}

int join_args(char *dest, size_t dest_size, int start_index, int argc, char *argv[])
{
    int i;
    size_t used;
    int written;

    if (dest == NULL || dest_size == 0 || start_index >= argc) {
        return -1;
    }

    dest[0] = '\0';
    used = 0;

    for (i = start_index; i < argc; i++) {
        if (i > start_index) {
            if (used + 1 >= dest_size) {
                return -1;
            }
            dest[used++] = ' ';
            dest[used] = '\0';
        }

        written = snprintf(dest + used, dest_size - used, "%s", argv[i]);
        if (written < 0 || (size_t)written >= dest_size - used) {
            return -1;
        }

        used += (size_t)written;
    }

    return 0;
}

void print_usage(const char *program_name)
{
    printf("Simple CLI Task Manager\\n\\n");
    printf("Usage:\\n");
    printf("  %s [--storage <path>] add [--priority 1-5] [--due YYYY-MM-DD] <task description>\\n", program_name);
    printf("  %s [--storage <path>] list [--no-color]\\n", program_name);
    printf("  %s done <id>\\n", program_name);
    printf("  %s delete <id>\\n", program_name);
    printf("  %s clear --yes\\n", program_name);
    printf("  %s help\\n", program_name);
    printf("\\nStorage format is auto-selected by extension (.txt or .dat).\\n");
}
