#include "storage.h"
#include "task.h"
#include "utils.h"
#include "ui.h"
#ifdef HAVE_NCURSES
#include "tui.h"
#endif

#include <stdio.h>
#include <string.h>

#define DEFAULT_TASK_FILE "data/tasks.txt"

static int save_with_report(const char *path, const TaskList *tasks)
{
    int result;

    result = storage_save_tasks(path, tasks);
    if (result != STORAGE_OK) {
        fprintf(stderr, "Error: %s (%s)\n", storage_result_message(result), path);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    TaskList tasks;
    int load_result;
    const char *storage_path;
    int argi;
    const char *command;

    task_list_init(&tasks);
    storage_path = DEFAULT_TASK_FILE;

    argi = 1;
    if (argc > 3 && strcmp(argv[argi], "--storage") == 0) {
        storage_path = argv[argi + 1];
        argi += 2;
    }

    if (argc <= argi) {
        print_usage(argv[0]);
        task_list_free(&tasks);
        return 0;
    }

    command = argv[argi];

    load_result = storage_load_tasks(storage_path, &tasks);
    if (load_result != STORAGE_OK) {
        fprintf(stderr, "Error: %s (%s)\n", storage_result_message(load_result), storage_path);
        task_list_free(&tasks);
        return 1;
    }

    if (strcmp(command, "help") == 0) {
        print_usage(argv[0]);
        task_list_free(&tasks);
        return 0;
    }

    if (strcmp(command, "list") == 0) {
        int color_enabled;

        color_enabled = 1;
        if (argc > argi + 1 && strcmp(argv[argi + 1], "--no-color") == 0) {
            color_enabled = 0;
        }

        task_list_print_colored(&tasks, color_enabled);
        task_list_free(&tasks);
        return 0;
    }

    if (strcmp(command, "menu") == 0) {
        task_list_free(&tasks);
        return run_menu(storage_path);
    }

#ifdef HAVE_NCURSES
    if (strcmp(command, "tui") == 0) {
        task_list_free(&tasks);
        return run_tui(storage_path);
    }
#else
    if (strcmp(command, "tui") == 0) {
        fprintf(stderr, "TUI not available: ncurses not found at build time.\n");
        task_list_free(&tasks);
        return 1;
    }
#endif

    if (strcmp(command, "add") == 0) {
        char description[TASK_DESCRIPTION_MAX * 2];
        int new_id;
        int priority;
        char due_date[TASK_DUE_DATE_MAX];
        int desc_start;

        priority = 3;
        due_date[0] = '\0';
        desc_start = argi + 1;

        while (desc_start + 1 < argc) {
            if (strcmp(argv[desc_start], "--priority") == 0) {
                if (parse_priority(argv[desc_start + 1], &priority) != 0) {
                    fprintf(stderr, "Error: --priority must be between 1 and 5.\n");
                    task_list_free(&tasks);
                    return 1;
                }
                desc_start += 2;
                continue;
            }

            if (strcmp(argv[desc_start], "--due") == 0) {
                if (!is_valid_due_date(argv[desc_start + 1])) {
                    fprintf(stderr, "Error: --due must use YYYY-MM-DD.\n");
                    task_list_free(&tasks);
                    return 1;
                }
                strncpy(due_date, argv[desc_start + 1], TASK_DUE_DATE_MAX - 1);
                due_date[TASK_DUE_DATE_MAX - 1] = '\0';
                desc_start += 2;
                continue;
            }

            break;
        }

        if (join_args(description, sizeof(description), desc_start, argc, argv) != 0) {
            fprintf(stderr, "Error: provide a task description.\n");
            task_list_free(&tasks);
            return 1;
        }

        normalize_description(description);
        if (str_is_blank(description)) {
            fprintf(stderr, "Error: description cannot be empty.\n");
            task_list_free(&tasks);
            return 1;
        }

        if (task_list_add_with_meta(&tasks, description, priority, due_date, &new_id) != 0) {
            fprintf(stderr, "Error: could not add task.\n");
            task_list_free(&tasks);
            return 1;
        }

        if (save_with_report(storage_path, &tasks) != 0) {
            task_list_free(&tasks);
            return 1;
        }

        printf("Added task %d.\n", new_id);
        task_list_free(&tasks);
        return 0;
    }

    if (strcmp(command, "done") == 0) {
        int id;

        if (argc < argi + 2 || parse_int(argv[argi + 1], &id) != 0 || id <= 0) {
            fprintf(stderr, "Error: done requires a positive numeric id.\n");
            task_list_free(&tasks);
            return 1;
        }

        if (task_list_mark_done(&tasks, id) != 0) {
            fprintf(stderr, "Error: task %d not found.\n", id);
            task_list_free(&tasks);
            return 1;
        }

        if (save_with_report(storage_path, &tasks) != 0) {
            task_list_free(&tasks);
            return 1;
        }

        printf("Marked task %d as done.\n", id);
        task_list_free(&tasks);
        return 0;
    }

    if (strcmp(command, "delete") == 0) {
        int id;

        if (argc < argi + 2 || parse_int(argv[argi + 1], &id) != 0 || id <= 0) {
            fprintf(stderr, "Error: delete requires a positive numeric id.\n");
            task_list_free(&tasks);
            return 1;
        }

        if (task_list_delete(&tasks, id) != 0) {
            fprintf(stderr, "Error: task %d not found.\n", id);
            task_list_free(&tasks);
            return 1;
        }

        if (save_with_report(storage_path, &tasks) != 0) {
            task_list_free(&tasks);
            return 1;
        }

        printf("Deleted task %d.\n", id);
        task_list_free(&tasks);
        return 0;
    }

    if (strcmp(command, "clear") == 0) {
        if (argc < argi + 2 || strcmp(argv[argi + 1], "--yes") != 0) {
            fprintf(stderr, "Error: clear requires confirmation flag --yes.\n");
            task_list_free(&tasks);
            return 1;
        }

        tasks.count = 0;

        if (save_with_report(storage_path, &tasks) != 0) {
            task_list_free(&tasks);
            return 1;
        }

        printf("All tasks cleared.\n");
        task_list_free(&tasks);
        return 0;
    }

    fprintf(stderr, "Error: unknown command '%s'.\n", command);
    print_usage(argv[0]);
    task_list_free(&tasks);
    return 1;
}
