#ifndef TASK_H
#define TASK_H

#include <stddef.h>

#define TASK_DESCRIPTION_MAX 256
#define TASK_DUE_DATE_MAX 11

typedef struct {
    int id;
    char description[TASK_DESCRIPTION_MAX];
    int is_done;
    int priority;
    char due_date[TASK_DUE_DATE_MAX];
} Task;

typedef struct {
    Task *items;
    size_t count;
    size_t capacity;
    int next_id;
} TaskList;

void task_list_init(TaskList *list);
void task_list_free(TaskList *list);
int task_list_add(TaskList *list, const char *description, int *out_id);
int task_list_add_with_meta(TaskList *list, const char *description, int priority,
    const char *due_date, int *out_id);
int task_list_mark_done(TaskList *list, int id);
int task_list_delete(TaskList *list, int id);
const Task *task_list_find_by_id(const TaskList *list, int id);
int task_list_append_loaded(TaskList *list, const Task *task);
void task_list_print(const TaskList *list);
void task_list_print_colored(const TaskList *list, int enable_color);

#endif
