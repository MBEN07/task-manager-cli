#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

int parse_int(const char *text, int *out_value);
int str_is_blank(const char *text);
void normalize_description(char *text);
void print_usage(const char *program_name);
int join_args(char *dest, size_t dest_size, int start_index, int argc, char *argv[]);
int parse_priority(const char *text, int *out_priority);
int is_valid_due_date(const char *text);

#endif
