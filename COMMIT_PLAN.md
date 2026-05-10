# Suggested Commit Plan

Use these commits in order for a clean project history.

## 1) Initial CLI Scaffold

Title:
- feat: scaffold modular C task manager project

Includes:
- Base project structure
- Core headers and source files
- Makefile and CMake setup
- Basic README and .gitignore

## 2) Storage Backend Expansion

Title:
- feat(storage): add auto-selected .txt and .dat persistence backends

Includes:
- Extension-based storage selection
- Backward-compatible txt parsing
- Binary .dat format with header/version validation
- Shared error messages for format/IO failures

## 3) Task Metadata and Colored Output

Title:
- feat(tasks): add priority, due dates, and colorized list rendering

Includes:
- Task model fields: priority and due_date
- Add command flags: --priority and --due
- List command flag: --no-color
- Default metadata handling and normalization

## 4) CI and Tests

Title:
- ci: add GitHub Actions CMake build and test workflow

Includes:
- .github/workflows/ci.yml
- Extended unit test coverage for metadata behavior

## 5) Docs Refresh

Title:
- docs: update README for storage formats, metadata flags, and CI

Includes:
- New command examples
- Storage format details for txt and dat
- Build/test guidance and roadmap updates

## Optional Squash Strategy

If you prefer fewer commits:
1. feat: implement task manager CLI with txt/dat storage and metadata
2. ci: add GitHub Actions workflow
3. docs: refresh README and usage examples
