#include "task.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    TaskList list;
    int id1;
    int id2;

    task_list_init(&list);

    assert(task_list_add(&list, "Write tests", &id1) == 0);
    assert(task_list_add_with_meta(&list, "Ship feature", 5, "2026-12-01", &id2) == 0);
    assert(id1 == 1);
    assert(id2 == 2);
    assert(list.count == 2);
    assert(task_list_find_by_id(&list, id2)->priority == 5);
    assert(strcmp(task_list_find_by_id(&list, id2)->due_date, "2026-12-01") == 0);

    assert(task_list_mark_done(&list, id1) == 0);
    assert(task_list_find_by_id(&list, id1)->is_done == 1);

    assert(task_list_delete(&list, id1) == 0);
    assert(task_list_find_by_id(&list, id1) == NULL);
    assert(list.count == 1);

    assert(task_list_add(&list, "Third item", &id1) == 0);
    assert(id1 == 3);

    assert(strcmp(task_list_find_by_id(&list, 2)->description, "Ship feature") == 0);

    task_list_free(&list);
    return 0;
}
