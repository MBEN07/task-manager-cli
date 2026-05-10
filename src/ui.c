#include "ui.h"
#include "storage.h"
#include "task.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void trim_newline(char *s)
{
    size_t n;
    if (s == NULL) return;
    n = strlen(s);
    if (n == 0) return;
    if (s[n - 1] == '\n') s[n - 1] = '\0';
    if (n > 1 && s[n - 2] == '\r') s[n - 2] = '\0';
}

int run_menu(const char *storage_path)
{
    TaskList tasks;
    char buf[512];
    int rc;

    task_list_init(&tasks);
    rc = storage_load_tasks(storage_path, &tasks);
    if (rc != STORAGE_OK) {
        fprintf(stderr, "Error loading tasks: %s\n", storage_result_message(rc));
        return 1;
    }

    while (1) {
        printf("\nSimple Task Manager - Menu\n");
        printf("1) List tasks\n");
        printf("2) Add task\n");
        printf("3) Mark done\n");
        printf("4) Delete task\n");
        printf("5) Clear all tasks\n");
        printf("6) Save and exit\n");
        printf("7) Exit without saving\n");
        printf("Choose an option: ");

        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        trim_newline(buf);

        if (strcmp(buf, "1") == 0) {
            task_list_print_colored(&tasks, 1);
            continue;
        }

        if (strcmp(buf, "2") == 0) {
            char desc[256];
            char pri[8];
            char due[16];
            int priority = 3;

            printf("Description: ");
            if (fgets(desc, sizeof(desc), stdin) == NULL) break;
            trim_newline(desc);
            normalize_description(desc);
            if (str_is_blank(desc)) {
                printf("Empty description. Aborted.\n");
                continue;
            }

            printf("Priority (1-5, default 3): ");
            if (fgets(pri, sizeof(pri), stdin) != NULL) {
                trim_newline(pri);
                if (pri[0] != '\0') {
                    if (parse_priority(pri, &priority) != 0) {
                        printf("Invalid priority, using default 3.\n");
                        priority = 3;
                    }
                }
            }

            printf("Due date (YYYY-MM-DD, optional): ");
            if (fgets(due, sizeof(due), stdin) != NULL) {
                trim_newline(due);
                if (!is_valid_due_date(due)) {
                    if (due[0] != '\0') {
                        printf("Invalid due date format, ignoring.\n");
                        due[0] = '\0';
                    }
                }
            }

            if (task_list_add_with_meta(&tasks, desc, priority, due, NULL) != 0) {
                printf("Failed to add task.\n");
            } else {
                if (storage_save_tasks(storage_path, &tasks) != STORAGE_OK) {
                    printf("Warning: could not save after adding.\n");
                }
                printf("Added.\n");
            }
            continue;
        }

        if (strcmp(buf, "3") == 0) {
            char idbuf[16];
            int id;
            printf("Task id to mark done: ");
            if (fgets(idbuf, sizeof(idbuf), stdin) == NULL) break;
            trim_newline(idbuf);
            if (parse_int(idbuf, &id) != 0) {
                printf("Invalid id.\n");
                continue;
            }
            if (task_list_mark_done(&tasks, id) != 0) {
                printf("Task not found.\n");
            } else {
                storage_save_tasks(storage_path, &tasks);
                printf("Marked done.\n");
            }
            continue;
        }

        if (strcmp(buf, "4") == 0) {
            char idbuf[16];
            int id;
            printf("Task id to delete: ");
            if (fgets(idbuf, sizeof(idbuf), stdin) == NULL) break;
            trim_newline(idbuf);
            if (parse_int(idbuf, &id) != 0) {
                printf("Invalid id.\n");
                continue;
            }
            if (task_list_delete(&tasks, id) != 0) {
                printf("Task not found.\n");
            } else {
                storage_save_tasks(storage_path, &tasks);
                printf("Deleted.\n");
            }
            continue;
        }

        if (strcmp(buf, "5") == 0) {
            char confirm[8];
            printf("Are you sure? Type YES to confirm: ");
            if (fgets(confirm, sizeof(confirm), stdin) == NULL) break;
            trim_newline(confirm);
            if (strcmp(confirm, "YES") == 0) {
                tasks.count = 0;
                storage_save_tasks(storage_path, &tasks);
                printf("Cleared.\n");
            } else {
                printf("Aborted.\n");
            }
            continue;
        }

        if (strcmp(buf, "6") == 0) {
            if (storage_save_tasks(storage_path, &tasks) != STORAGE_OK) {
                printf("Error saving tasks.\n");
                task_list_free(&tasks);
                return 1;
            }
            task_list_free(&tasks);
            printf("Saved. Bye.\n");
            return 0;
        }

        if (strcmp(buf, "7") == 0) {
            task_list_free(&tasks);
            printf("Exit without saving. Bye.\n");
            return 0;
        }

        printf("Unknown choice.\n");
    }

    task_list_free(&tasks);
    return 0;
}
