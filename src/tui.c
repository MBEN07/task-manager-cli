#ifdef HAVE_NCURSES
#include <curses.h>
#else
#error "tui.c should only be compiled when ncurses is available"
#endif

#include "tui.h"
#include "storage.h"
#include "task.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>

static void draw_tasks(WINDOW *win, const TaskList *list, int highlight, int offset)
{
    int y = 1;
    int i;
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Tasks ");
    for (i = offset; i < (int)list->count && y < LINES - 3; i++, y++) {
        if (i == highlight) {
            wattron(win, A_REVERSE);
        }
        mvwprintw(win, y, 2, "%d. [%c] P%d %s", list->items[i].id,
                  list->items[i].is_done ? 'x' : ' ', list->items[i].priority,
                  list->items[i].description);
        if (i == highlight) {
            wattroff(win, A_REVERSE);
        }
    }
    wrefresh(win);
}

static char *prompt_input(const char *prompt)
{
    int max = 256;
    char *buf = malloc(max);
    if (!buf) return NULL;
    echo();
    curs_set(1);
    mvprintw(LINES - 2, 0, "%s", prompt);
    clrtoeol();
    move(LINES - 1, 0);
    getnstr(buf, max - 1);
    noecho();
    curs_set(0);
    return buf;
}

int run_tui(const char *storage_path)
{
    TaskList list;
    int ch;
    int highlight = 0;
    int offset = 0;
    WINDOW *win;

    task_list_init(&list);
    if (storage_load_tasks(storage_path, &list) != STORAGE_OK) {
        // proceed with empty list but warn
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    win = newwin(LINES - 2, COLS, 0, 0);

    while (1) {
        draw_tasks(win, &list, highlight, offset);
        mvprintw(LINES - 2, 0, "Commands: Up/Down move, a=add, x=done, d=delete, s=save, q=quit");
        clrtoeol();
        refresh();

        ch = getch();
        if (ch == KEY_UP) {
            if (highlight > 0) highlight--; if (highlight < offset) offset = highlight;
        } else if (ch == KEY_DOWN) {
            if (highlight < (int)list.count - 1) highlight++; if (highlight >= offset + LINES - 3) offset = highlight - (LINES - 4);
        } else if (ch == 'a') {
            char *desc = prompt_input("Description: ");
            if (desc && desc[0] != '\0') {
                normalize_description(desc);
                task_list_add(&list, desc, NULL);
                free(desc);
            } else {
                free(desc);
            }
        } else if (ch == 'x') {
            if (list.count > 0) task_list_mark_done(&list, list.items[highlight].id);
        } else if (ch == 'd') {
            if (list.count > 0) task_list_delete(&list, list.items[highlight].id);
            if (highlight >= (int)list.count) highlight = (int)list.count - 1;
        } else if (ch == 's') {
            storage_save_tasks(storage_path, &list);
        } else if (ch == 'q') {
            break;
        }
    }

    storage_save_tasks(storage_path, &list);
    delwin(win);
    endwin();
    task_list_free(&list);
    return 0;
}
