#include "storage.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STORAGE_MAGIC 0x544D4442u

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t count;
} StorageHeader;

typedef struct {
    int32_t id;
    int32_t is_done;
    int32_t priority;
    char due_date[TASK_DUE_DATE_MAX];
    char description[TASK_DESCRIPTION_MAX];
} StorageRecord;

static void trim_line_end(char *line)
{
    size_t len;

    len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
        len--;
    }
}

int storage_is_dat_path(const char *path)
{
    const char *dot;

    if (path == NULL) {
        return 0;
    }

    dot = strrchr(path, '.');
    if (dot == NULL) {
        return 0;
    }

#ifdef _WIN32
    return _stricmp(dot, ".dat") == 0;
#else
    return strcmp(dot, ".dat") == 0;
#endif
}

static int load_txt(const char *path, TaskList *list)
{
    FILE *file;
    char line[1024];

    file = fopen(path, "r");
    if (file == NULL) {
        return STORAGE_OK;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Task task;
        char *first_delim;
        char *second_delim;
        char *third_delim;
        char *fourth_delim;
        int id;
        int is_done;

        trim_line_end(line);
        if (line[0] == '\0') {
            continue;
        }

        first_delim = strchr(line, '|');
        if (first_delim == NULL) {
            fclose(file);
            return STORAGE_ERR_PARSE;
        }
        *first_delim = '\0';

        second_delim = strchr(first_delim + 1, '|');
        if (second_delim == NULL) {
            fclose(file);
            return STORAGE_ERR_PARSE;
        }
        *second_delim = '\0';

        if (sscanf(line, "%d", &id) != 1 || sscanf(first_delim + 1, "%d", &is_done) != 1) {
            fclose(file);
            return STORAGE_ERR_PARSE;
        }

        task.id = id;
        task.is_done = is_done ? 1 : 0;
        task.priority = 3;
        task.due_date[0] = '\0';

        third_delim = strchr(second_delim + 1, '|');
        if (third_delim == NULL) {
            strncpy(task.description, second_delim + 1, TASK_DESCRIPTION_MAX - 1);
            task.description[TASK_DESCRIPTION_MAX - 1] = '\0';
        } else {
            int priority;
            *third_delim = '\0';
            if (sscanf(second_delim + 1, "%d", &priority) != 1) {
                fclose(file);
                return STORAGE_ERR_PARSE;
            }
            task.priority = (priority >= 1 && priority <= 5) ? priority : 3;

            fourth_delim = strchr(third_delim + 1, '|');
            if (fourth_delim == NULL) {
                fclose(file);
                return STORAGE_ERR_PARSE;
            }
            *fourth_delim = '\0';

            strncpy(task.due_date, third_delim + 1, TASK_DUE_DATE_MAX - 1);
            task.due_date[TASK_DUE_DATE_MAX - 1] = '\0';

            strncpy(task.description, fourth_delim + 1, TASK_DESCRIPTION_MAX - 1);
            task.description[TASK_DESCRIPTION_MAX - 1] = '\0';
        }

        if (task.description[0] == '\0') {
            fclose(file);
            return STORAGE_ERR_PARSE;
        }

        if (task_list_append_loaded(list, &task) != 0) {
            fclose(file);
            return STORAGE_ERR_MEMORY;
        }
    }

    if (ferror(file)) {
        fclose(file);
        return STORAGE_ERR_READ;
    }

    fclose(file);
    return STORAGE_OK;
}

static int save_txt(const char *path, const TaskList *list)
{
    FILE *file;
    size_t i;
    char temp_path[512];

    if (snprintf(temp_path, sizeof(temp_path), "%s.tmp", path) >= (int)sizeof(temp_path)) {
        return STORAGE_ERR_WRITE;
    }

    file = fopen(temp_path, "w");
    if (file == NULL) {
        return STORAGE_ERR_OPEN;
    }

    for (i = 0; i < list->count; i++) {
        if (fprintf(file, "%d|%d|%d|%s|%s\n",
                list->items[i].id,
                list->items[i].is_done,
                list->items[i].priority,
                list->items[i].due_date,
                list->items[i].description) < 0) {
            fclose(file);
            remove(temp_path);
            return STORAGE_ERR_WRITE;
        }
    }

    if (fclose(file) != 0) {
        remove(temp_path);
        return STORAGE_ERR_WRITE;
    }

    remove(path);
    if (rename(temp_path, path) != 0) {
        remove(temp_path);
        return STORAGE_ERR_RENAME;
    }

    return STORAGE_OK;
}

static int load_dat(const char *path, TaskList *list)
{
    FILE *file;
    StorageHeader header;
    uint32_t i;

    file = fopen(path, "rb");
    if (file == NULL) {
        return STORAGE_OK;
    }

    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return STORAGE_ERR_READ;
    }

    if (header.magic != STORAGE_MAGIC || header.version != 1u) {
        fclose(file);
        return STORAGE_ERR_FORMAT;
    }

    for (i = 0; i < header.count; i++) {
        StorageRecord record;
        Task task;

        if (fread(&record, sizeof(record), 1, file) != 1) {
            fclose(file);
            return STORAGE_ERR_READ;
        }

        task.id = (int)record.id;
        task.is_done = record.is_done ? 1 : 0;
        task.priority = (record.priority >= 1 && record.priority <= 5) ? (int)record.priority : 3;
        strncpy(task.due_date, record.due_date, TASK_DUE_DATE_MAX - 1);
        task.due_date[TASK_DUE_DATE_MAX - 1] = '\0';
        strncpy(task.description, record.description, TASK_DESCRIPTION_MAX - 1);
        task.description[TASK_DESCRIPTION_MAX - 1] = '\0';

        if (task.description[0] == '\0') {
            fclose(file);
            return STORAGE_ERR_PARSE;
        }

        if (task_list_append_loaded(list, &task) != 0) {
            fclose(file);
            return STORAGE_ERR_MEMORY;
        }
    }

    fclose(file);
    return STORAGE_OK;
}

static int save_dat(const char *path, const TaskList *list)
{
    FILE *file;
    StorageHeader header;
    size_t i;
    char temp_path[512];

    if (snprintf(temp_path, sizeof(temp_path), "%s.tmp", path) >= (int)sizeof(temp_path)) {
        return STORAGE_ERR_WRITE;
    }

    file = fopen(temp_path, "wb");
    if (file == NULL) {
        return STORAGE_ERR_OPEN;
    }

    header.magic = STORAGE_MAGIC;
    header.version = 1u;
    header.count = (uint32_t)list->count;
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        remove(temp_path);
        return STORAGE_ERR_WRITE;
    }

    for (i = 0; i < list->count; i++) {
        StorageRecord record;
        memset(&record, 0, sizeof(record));
        record.id = (int32_t)list->items[i].id;
        record.is_done = (int32_t)list->items[i].is_done;
        record.priority = (int32_t)list->items[i].priority;
        strncpy(record.due_date, list->items[i].due_date, TASK_DUE_DATE_MAX - 1);
        strncpy(record.description, list->items[i].description, TASK_DESCRIPTION_MAX - 1);

        if (fwrite(&record, sizeof(record), 1, file) != 1) {
            fclose(file);
            remove(temp_path);
            return STORAGE_ERR_WRITE;
        }
    }

    if (fclose(file) != 0) {
        remove(temp_path);
        return STORAGE_ERR_WRITE;
    }

    remove(path);
    if (rename(temp_path, path) != 0) {
        remove(temp_path);
        return STORAGE_ERR_RENAME;
    }

    return STORAGE_OK;
}

const char *storage_result_message(int code)
{
    switch (code) {
    case STORAGE_OK:
        return "ok";
    case STORAGE_ERR_OPEN:
        return "could not open storage file";
    case STORAGE_ERR_READ:
        return "could not read storage file";
    case STORAGE_ERR_WRITE:
        return "could not write storage file";
    case STORAGE_ERR_PARSE:
        return "invalid line in storage file";
    case STORAGE_ERR_MEMORY:
        return "out of memory";
    case STORAGE_ERR_RENAME:
        return "could not replace storage file";
    case STORAGE_ERR_FORMAT:
        return "unsupported binary storage format";
    default:
        return "unknown storage error";
    }
}

int storage_load_tasks(const char *path, TaskList *list)
{
    if (storage_is_dat_path(path)) {
        return load_dat(path, list);
    }
    return load_txt(path, list);
}

int storage_save_tasks(const char *path, const TaskList *list)
{
    if (storage_is_dat_path(path)) {
        return save_dat(path, list);
    }
    return save_txt(path, list);
}
