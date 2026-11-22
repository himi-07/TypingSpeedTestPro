# Typing Speed Test Pro

A modular Windows desktop typing assessment and performance tracking application written in **C** (MinGW-w64 GCC) using the **Win32 API** (`user32`, `gdi32`, `comctl32`) and embedded **SQLite3** (`typing_test.db`), developed by **Luhana Rashid Himi**.

## Project Overview

**Typing Speed Test Pro** is an offline Windows GUI application designed to help learners, students, and developers build touch-typing speed, accuracy, and consistency without relying on web browsers or internet access. It combines an event-driven Win32 user interface with custom GDI card rendering, real-time typing telemetry, a 30-passage bank across three difficulty levels, and local SQLite3 persistence.

## Repository & Project Links

* **GitHub Profile:** https://github.com/himi-07
* **GitHub Repository:** https://github.com/himi-07/TypingSpeedTestPro
* **README URL:** https://github.com/himi-07/TypingSpeedTestPro/blob/main/README.md

## Key Features

* **Real-Time Telemetry Cards:** Live calculation and display of Words Per Minute (**WPM**), character **Accuracy (%)**, countdown or elapsed **Time**, and **Errors**.
* **Test Durations & Difficulty Tiers:** Choose from **30 seconds**, **60 seconds**, **120 seconds**, or **Free Practice** across **30 passages** evenly divided into **Beginner**, **Intermediate**, and **Advanced** tiers.
* **Keystroke-Triggered Timer:** The countdown or elapsed timer starts automatically on the very first character typed and finishes on timeout, full passage completion, or clicking `FINISH`.
* **Persistent SQLite3 Storage:** Automatically creates and updates `typing_test.db` in Write-Ahead Logging (**WAL**) mode using prepared statements (`sqlite3_prepare_v2`).
* **Leaderboard & Session History:** View the **Top 10** players ranked by best WPM alongside average accuracy, or inspect the **12 most recent** test sessions.
* **Modular C Architecture:** Separates configuration, database queries, game logic, UI rendering, and the Win32 message loop into dedicated header and source files.

## Project Structure

```text
TypingSpeedTestPro/
├── .vscode/
│   ├── tasks.json           # VS Code build task (Ctrl+Shift+B)
│   ├── launch.json          # VS Code debugger launch configuration (F5)
│   └── c_cpp_properties.json# IntelliSense compiler & include paths
├── config.h                 # Control IDs, timer/color constants, extern state & shared helpers
├── db.h                     # Database initialization, save & reporting declarations
├── db.c                     # SQLite3 WAL setup, sessions insert, Leaderboard & History queries
├── engine.h                 # Game engine function declarations
├── engine.c                 # 30 passages, WPM/accuracy math, elapsed clock & test lifecycle
├── ui.h                     # WindowProc declaration
├── ui.c                     # Win32 controls, custom GDI card drawing & WM_* event handling
├── main.c                   # Global state definitions, common controls init & GetMessageA loop
├── README.md                # Project documentation
└── .gitignore               # Ignores compiled binaries, SQLite DB files & local editor configs
```

## How Scoring & Timing Work

* **Character Comparison:** Each typed character at index `i` is compared against `gCurrentPassage[i]`. Matching characters increment `correct`; mismatches or characters beyond the passage length increment `errors` (`wrong`).
* **Words Per Minute (WPM):** Uses the standard 5-character word unit over elapsed time (clamped to at least `1` second):
  $$\text{WPM} = \text{round}\left(\frac{\text{correct} / 5.0}{\text{elapsedSeconds} / 60.0}\right)$$
* **Accuracy Percentage:** Computed across all typed characters (`typedLen > 0`, defaulting to `100.0%` before typing begins):
  $$\text{Accuracy (\%)} = \frac{100.0 \times \text{correct}}{\text{typedLen}}$$

## SQLite3 Database Schema

On startup (`DatabaseInit()`), the application opens `typing_test.db`, enables WAL mode (`PRAGMA journal_mode=WAL;`), and creates the `sessions` table if it does not already exist:

```sql
CREATE TABLE IF NOT EXISTS sessions (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    player     TEXT NOT NULL,
    wpm        INTEGER NOT NULL,
    accuracy   REAL NOT NULL,
    errors     INTEGER NOT NULL,
    duration   INTEGER NOT NULL,
    difficulty TEXT NOT NULL,
    created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
);
```

## Console Build & Run Commands

### 1. Build & Run (Standard GUI Executable)

```powershell
gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mwindows -o typing_test.exe
.\typing_test.exe
```

### 2. Build & Run with Console Output Attached (Debugging)

```powershell
gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mconsole -o typing_test_console.exe
.\typing_test_console.exe
```

## Push to GitHub via Console

```powershell
git init
git add .
git commit -m "Add Typing Speed Test Pro project"
git branch -M main
git remote add origin https://github.com/himi-07/TypingSpeedTestPro.git
git push -u origin main
```

If the remote repository already has commits on `main`, pull and rebase first:

```powershell
git pull --rebase origin main
git push -u origin main
```
