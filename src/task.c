#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_RESET "\x1b[0m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RED "\x1b[31m"

static int ensure_capacity(TaskList *list, size_t required)
{
    Task *resized;
    size_t new_capacity;

    if (required <= list->capacity) {
        return 0;
    }

    new_capacity = (list->capacity == 0) ? 8 : list->capacity * 2;
    while (new_capacity < required) {
        new_capacity *= 2;
    }

    resized = (Task *)realloc(list->items, new_capacity * sizeof(Task));
    if (resized == NULL) {
        return -1;
    }

    list->items = resized;
    list->capacity = new_capacity;
    return 0;
}

void task_list_init(TaskList *list)
{
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->next_id = 1;
}

void task_list_free(TaskList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->next_id = 1;
}

int task_list_add(TaskList *list, const char *description, int *out_id)
{
    return task_list_add_with_meta(list, description, 3, "", out_id);
}

int task_list_add_with_meta(TaskList *list, const char *description, int priority,
    const char *due_date, int *out_id)
{
    Task *task;

    if (description == NULL || description[0] == '\0') {
        return -1;
    }

    if (ensure_capacity(list, list->count + 1) != 0) {
        return -1;
    }

    task = &list->items[list->count];
    task->id = list->next_id;
    task->is_done = 0;
    task->priority = (priority >= 1 && priority <= 5) ? priority : 3;
    strncpy(task->due_date, due_date == NULL ? "" : due_date, TASK_DUE_DATE_MAX - 1);
    task->due_date[TASK_DUE_DATE_MAX - 1] = '\0';
    strncpy(task->description, description, TASK_DESCRIPTION_MAX - 1);
    task->description[TASK_DESCRIPTION_MAX - 1] = '\0';

    list->count += 1;
    list->next_id += 1;

    if (out_id != NULL) {
        *out_id = task->id;
    }

    return 0;
}

int task_list_mark_done(TaskList *list, int id)
{
    size_t i;

    for (i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            list->items[i].is_done = 1;
            return 0;
        }
    }

    return -1;
}

int task_list_delete(TaskList *list, int id)
{
    size_t i;

    for (i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            if (i < list->count - 1) {
                memmove(&list->items[i], &list->items[i + 1],
                    (list->count - i - 1) * sizeof(Task));
            }
            list->count -= 1;
            return 0;
        }
    }

    return -1;
}

const Task *task_list_find_by_id(const TaskList *list, int id)
{
    size_t i;

    for (i = 0; i < list->count; i++) {
        if (list->items[i].id == id) {
            return &list->items[i];
        }
    }

    return NULL;
}

int task_list_append_loaded(TaskList *list, const Task *task)
{
    int candidate_next_id;
    Task normalized;

    if (task == NULL || task->description[0] == '\0') {
        return -1;
    }

    if (ensure_capacity(list, list->count + 1) != 0) {
        return -1;
    }

    normalized = *task;
    if (normalized.priority < 1 || normalized.priority > 5) {
        normalized.priority = 3;
    }
    normalized.due_date[TASK_DUE_DATE_MAX - 1] = '\0';
    normalized.description[TASK_DESCRIPTION_MAX - 1] = '\0';

    list->items[list->count] = normalized;
    list->count += 1;

    candidate_next_id = task->id + 1;
    if (candidate_next_id > list->next_id) {
        list->next_id = candidate_next_id;
    }

    return 0;
}

void task_list_print(const TaskList *list)
{
    task_list_print_colored(list, 0);
}

void task_list_print_colored(const TaskList *list, int enable_color)
{
    size_t i;
    const char *color;
    char due_fragment[32];

    if (list->count == 0) {
        printf("No tasks found.\\n");
        return;
    }

    for (i = 0; i < list->count; i++) {
        if (!enable_color) {
            color = "";
        } else if (list->items[i].is_done) {
            color = ANSI_GREEN;
        } else if (list->items[i].priority >= 4) {
            color = ANSI_RED;
        } else if (list->items[i].priority == 3) {
            color = ANSI_YELLOW;
        } else {
            color = "";
        }

        if (list->items[i].due_date[0] != '\0') {
            snprintf(due_fragment, sizeof(due_fragment), " [due %s]", list->items[i].due_date);
        } else {
            due_fragment[0] = '\0';
        }

        printf("%s%d. [%c] (P%d)%s %s%s\\n",
            color,
            list->items[i].id,
            list->items[i].is_done ? 'x' : ' ',
            list->items[i].priority,
            due_fragment,
            list->items[i].description,
            enable_color ? ANSI_RESET : "");
    }
}
