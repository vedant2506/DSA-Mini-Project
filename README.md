# Student Task Scheduler (C++)

A console-based task manager for college assignments, journals, practical submissions, exams, and project deadlines.

## Features
- Add tasks with title, subject, deadline, and priority.
- View all tasks sorted by deadline.
- View pending tasks ordered by priority and then deadline.
- Mark tasks as completed.
- Delete tasks.
- Save tasks automatically to `tasks.txt` and reload them on the next run.
- Coloured terminal headings and menu using standard ANSI escape sequences.

## Requirements
- A C++ compiler with C++11 support or newer.
- Windows: recent MinGW-w64 g++ or Visual Studio Developer Command Prompt (the included build script uses `g++`).
- Ubuntu: `g++` (install with `sudo apt install g++` if needed).

No third-party libraries are required.

## Build and run on Windows
1. Install a C++ compiler such as MinGW-w64 and ensure `g++` is on PATH.
2. Open this folder in File Explorer.
3. Double-click `build_windows.bat`.
4. If compilation succeeds, it creates `StudentTaskScheduler.exe` and runs it.

Alternatively, in Command Prompt from this folder:
```bat
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp TaskManager.cpp UI.cpp -o StudentTaskScheduler.exe
StudentTaskScheduler.exe
```

If colours do not display in an older terminal, use Windows Terminal or a recent terminal in VS Code.

## Build and run on Ubuntu
Open a terminal in this folder and run:
```bash
chmod +x build_ubuntu.sh
./build_ubuntu.sh
```

Or compile manually:
```bash
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp TaskManager.cpp UI.cpp -o StudentTaskScheduler
./StudentTaskScheduler
```

## Data storage
The program creates `tasks.txt` in the current working directory. Keep it in the project folder and run the program from that folder to reuse saved tasks.

## Deadline format
Enter dates as `YYYY-MM-DD`, for example `2026-10-19`. This format sorts chronologically as text and is validated for real calendar dates.

## DSA concepts demonstrated
- **Hash table (`std::unordered_map`)**: maps each task ID to its position in the task list, making ID lookup fast on average. The index is rebuilt after tasks are loaded, added, or deleted.
- **Binary Search Tree (BST)**: tasks are inserted into a BST ordered by deadline (then ID). In-order traversal displays tasks in chronological order.
- `std::vector`: stores the task records.
- `std::sort`: orders the urgent-task view by priority and deadline.
- File handling: saves tasks to `tasks.txt` and reloads them on the next run.

The BST is built when the user selects “View All Tasks”; this keeps it synchronized with the current task list and makes the tree traversal easy to demonstrate in a viva.
