#ifndef STORAGE_H
#define STORAGE_H

#include "task.h"

enum {
    STORAGE_OK = 0,
    STORAGE_ERR_OPEN = 1,
    STORAGE_ERR_READ = 2,
    STORAGE_ERR_WRITE = 3,
    STORAGE_ERR_PARSE = 4,
    STORAGE_ERR_MEMORY = 5,
    STORAGE_ERR_RENAME = 6,
    STORAGE_ERR_FORMAT = 7
};

int storage_load_tasks(const char *path, TaskList *list);
int storage_save_tasks(const char *path, const TaskList *list);
int storage_is_dat_path(const char *path);
const char *storage_result_message(int code);

#endif
