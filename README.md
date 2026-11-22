**Typing Speed Test Pro**

A modular Windows desktop typing assessment and performance tracking application written in C (MinGW-w64 GCC) using the Win32 API (user32, gdi32, comctl32) and embedded SQLite3 (typing_test.db), developed by Luhana Rashid Himi (https://github.com/himi-07).

**Repository & Project Links**
GitHub Profile: https://github.com/himi-07

GitHub Repository: https://github.com/himi-07/TypingSpeedTestPro

README URL: https://github.com/himi-07/TypingSpeedTestPro/blob/main/README.md

**Key Features**


Live Performance Metrics: Real-time calculation of Words Per Minute (WPM), character Accuracy (%), countdown/elapsed clock, and character Errors.

Test Durations & Difficulty Tiers: Supports 30s, 60s, 120s, and Free Practice modes across 30 passages split into Beginner, Intermediate, and Advanced levels.

Automatic Timer Start: Starts timing on the very first character typed and finishes on timeout, full passage completion, or manual FINISH.

Persistent SQLite3 Storage: Stores completed sessions in typing_test.db (sessions table) using Write-Ahead Logging (WAL) mode and prepared statements.

Leaderboard & History Dialogs: View Top 10 players ranked by highest WPM with average accuracy, plus recent test session logs.

**Project Structure**

TypingSpeedTestPro/

├── .vscode/

│   ├── tasks.json

│   ├── launch.json

│   └── c_cpp_properties.json

├── config.h       # Constants, control IDs, theme colors, shared state & helpers

├── db.h           # Database & reporting declarations

├── db.c           # SQLite3 initialization, saving sessions, Leaderboard & History

├── engine.h       # Typing engine declarations

├── engine.c       # 30 passages, WPM/accuracy calculations & test lifecycle

├── ui.h           # UI & WindowProc declarations

├── ui.c           # Win32 controls, GDI card painting & event handling

├── main.c         # Common controls init, window registration & message loop

├── README.md      # Project documentation

└── .gitignore     # Excludes build artifacts, local DB files & editor configs



**Console Build & Run Commands**

1. Build & Run (Standard GUI Executable)

gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mwindows -o typing_test.exe
.\typing_test.exe


2. Build & Run with Console Output Attached (Debugging)

gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mconsole -o typing_test_console.exe
.\typing_test_console.exe


**Push to GitHub via Console**

git init

git add .

git commit -m "Add Typing Speed Test Pro project"

git branch -M main

git remote add origin https://github.com/himi-07/TypingSpeedTestPro.git

git push -u origin main
If the remote repository already has commits on main, pull and rebase first:

git pull --rebase origin main
git push -u origin main
