CC=gcc
CFLAGS=-Wall -Wextra -Iinclude
SRC=src/main.c src/ui.c src/task.c src/storage.c src/utils.c
OUT=build/task_manager
TEST_OUT=build/test_tasks

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: tests/test_tasks.c src/task.c
	mkdir -p build
	$(CC) $(CFLAGS) tests/test_tasks.c src/task.c -o $(TEST_OUT)
	$(TEST_OUT)

clean:
	rm -f $(OUT) $(TEST_OUT)
