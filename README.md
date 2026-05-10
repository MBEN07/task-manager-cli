# Task Manager CLI in C

A lightweight command-line task manager built in C.

It supports:
- Adding tasks
- Listing tasks
- Marking tasks as done
- Deleting tasks
- Clearing all tasks (with confirmation)
- Persistent file storage
- Optional .dat binary storage backend
- Priority levels (1 to 5)
- Due dates (YYYY-MM-DD)
- Colored list output

## Project Structure

```
task-manager-cli/
├── src/
│   ├── main.c
│   ├── task.c
│   ├── storage.c
│   └── utils.c
├── include/
│   ├── task.h
│   ├── storage.h
│   └── utils.h
├── data/
│   └── tasks.txt
├── build/
├── tests/
│   └── test_tasks.c
├── Makefile
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Features

- Modular C architecture with clear separation of responsibilities
- Monotonic task IDs (never reused)
- Storage auto-detection from extension (`.txt` or `.dat`)
- Text format with metadata: `id|is_done|priority|due_date|description`
- Safe save strategy using temporary file replacement
- Basic unit tests for core task behavior
- GitHub Actions CI for build and tests

## Build and Run

### Option 1: Makefile

Requirements: `gcc`, `make`

```bash
make
./build/task_manager list
```

Use binary backend:

```bash
./build/task_manager --storage data/tasks.dat list
```

### Option 2: CMake

Requirements: `cmake`, C compiler (MSVC, gcc, or clang)

```bash
cmake -S . -B build
cmake --build build
```

Run (from repository root):

```bash
./build/task_manager list
```

### Quick Windows (PowerShell) helper scripts

Run the setup script to install CMake (uses winget):

```powershell
.\scripts\setup-windows.ps1
```

Build and run the menu with the helper scripts:

```powershell
.\scripts\build.ps1
.\scripts\run-menu.ps1
```

## Usage

```bash
./build/task_manager add "Learn file handling"
./build/task_manager add --priority 5 --due 2026-12-01 "Finish portfolio README"
./build/task_manager add "Write modular code"
./build/task_manager list
./build/task_manager list --no-color
./build/task_manager done 1
./build/task_manager delete 2
./build/task_manager clear --yes
./build/task_manager help
```

## Command Reference

- `[--storage <path>] add [--priority 1-5] [--due YYYY-MM-DD] <description>`: Adds a task
- `[--storage <path>] list [--no-color]`: Shows all tasks
- `done <id>`: Marks a task as completed
- `delete <id>`: Deletes a task
- `clear --yes`: Removes all tasks
- `help`: Shows usage

## Storage Notes

Tasks are stored in `data/tasks.txt` using one line per task:

```
<id>|<is_done>|<priority>|<due_date>|<description>
```

Example:

```
1|0|4|2026-06-01|Read chapter 1
2|1|2||Push project to GitHub
```

If the file path ends with `.dat`, the app stores tasks in a compact binary format automatically.

## Testing

With Makefile:

```bash
make test
```

With CMake:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Design Notes

- `task.c`: Task list operations and ID management
- `storage.c`: File load/save and storage error mapping
- `utils.c`: Input helpers and command usage text
- `main.c`: CLI command parsing and orchestration

The storage layer is intentionally isolated so a future `.dat` backend can be added with minimal changes to command logic.

CI workflow:
- `.github/workflows/ci.yml` builds with CMake and runs tests on every push and pull request.

## Roadmap

Potential upgrades:
- JSON or SQLite storage backend
- Interactive mode and filtering (`list --done`, `list --priority 5`)
