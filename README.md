# Typing Speed Test Pro

A modular Windows desktop typing assessment and performance tracking application developed in C (MinGW-w64 GCC) using the Win32 API (`user32`, `gdi32`, `comctl32`) and embedded SQLite3 (`typing_test.db`), authored by **Shamiul Islam Shanto** (`https://github.com/himi-07`).

---

## Project Summary (.....)

- **Project Title:** (Typing Speed Test Pro)
- **Developer:** (Shamiul Islam Shanto)
- **GitHub Profile:** (https://github.com/himi-07)
- **Repository Link:** (https://github.com/himi-07/typing-speed-test-pro)
- **Language & Compiler:** (C / MinGW-w64 GCC)
- **GUI & Graphics:** (Win32 API + GDI Custom Card Rendering)
- **Database Engine:** (SQLite3 with Write-Ahead Logging / WAL mode)
- **Passage Bank:** (30 passages across Beginner, Intermediate, and Advanced levels)
- **Test Modes:** (30s, 60s, 120s, and Free Practice)

---

## Console Build & Running Links (.....)

### 1. Build via Console (GUI Executable)
- **Compile Command:** (`gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mwindows -o typing_test.exe`)
- **Run Executable:** (`.\typing_test.exe`)

### 2. Build & Run with Console Output Attached (For Debugging / Logs)
- **Compile with Console Subsystem:** (`gcc -Wall -Wextra -g main.c db.c engine.c ui.c -I. -L. -lsqlite3 -lcomctl32 -lgdi32 -luser32 -mconsole -o typing_test_console.exe`)
- **Run Console Build:** (`.\typing_test_console.exe`)

### 3. Push to GitHub via Console
- **Initialize & Stage:** (`git init && git add config.h db.h db.c engine.h engine.c ui.h ui.c main.c README.md .gitignore`)
- **Commit & Push:** (`git commit -m "feat: add modular typing speed test pro with sqlite3" && git branch -M main && git remote add origin https://github.com/himi-07/typing-speed-test-pro.git && git push -u origin main`)

---

## Project File Structure (.....)

- **Configuration & Shared State:** (`config.h`)
- **Database & Leaderboard/History Queries:** (`db.h`, `db.c`)
- **Typing Engine, Passages & Metrics:** (`engine.h`, `engine.c`)
- **Win32 UI, GDI Cards & Event Loop:** (`ui.h`, `ui.c`, `main.c`)
- **VS Code Build & Debug Config:** (`.vscode/tasks.json`, `.vscode/launch.json`, `.vscode/c_cpp_properties.json`)
- **Git Ignore Rules:** (`.gitignore`)

---

## Key Features (.....)

- **Live Telemetry:** (Real-time Words Per Minute / WPM, Accuracy %, countdown/elapsed clock, and character error tracking)
- **Automatic Timer Start:** (Starts on the first character typed and finishes on timeout, full passage completion, or `FINISH`)
- **Persistent SQLite3 Storage:** (Saves sessions in `typing_test.db` table `sessions` with player name, WPM, accuracy, errors, duration, difficulty, and timestamp)
- **Leaderboard & History:** (Displays Top 10 players ranked by best WPM with average accuracy, plus recent test sessions)

