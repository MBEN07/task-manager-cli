cmake_minimum_required(VERSION 3.16)
project(task_manager_c C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(tasklib
    src/task.c
    src/storage.c
    src/utils.c
)

target_include_directories(tasklib PUBLIC include)

if (MSVC)
    target_compile_options(tasklib PRIVATE /W4)
else()
    target_compile_options(tasklib PRIVATE -Wall -Wextra)
endif()

add_executable(task_manager src/main.c src/ui.c)
target_link_libraries(task_manager PRIVATE tasklib)

find_package(Curses)
if(CURSES_FOUND)
    message(STATUS "Found curses: ${CURSES_LIBRARIES}")
    target_compile_definitions(task_manager PRIVATE HAVE_NCURSES)
    target_sources(task_manager PRIVATE src/tui.c)
    target_link_libraries(task_manager PRIVATE ${CURSES_LIBRARIES})
endif()

enable_testing()
add_executable(test_tasks tests/test_tasks.c)
target_include_directories(test_tasks PRIVATE include)
target_link_libraries(test_tasks PRIVATE tasklib)
add_test(NAME test_tasks COMMAND test_tasks)
