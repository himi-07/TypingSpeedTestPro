#include "config.h"
#include "engine.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *level;
    const char *text;
} Passage;

static const Passage passages[] = {
    /* ==================== BEGINNER ==================== */
    {"Beginner", "A good typing habit begins with a relaxed posture and a steady rhythm. Keep your eyes on the screen and let your fingers learn the position of each key. Accuracy is more important than speed when you are learning."},
    {"Beginner", "Small steps can lead to meaningful progress. Practice for a few minutes every day, review your results, and focus on the keys that slow you down. Consistent effort helps build confidence and control."},
    {"Beginner", "Sit straight with your feet flat on the floor and your wrists lifted slightly above the desk. Light, even keystrokes reduce fatigue and help you type smoothly without rushing through words."},
    {"Beginner", "Start each session slowly so your hands can find the home row without looking down. Speed grows naturally once your fingers reach the right letters on the first try."},
    {"Beginner", "Reading each sentence calmly before your fingers move helps prevent extra typos. Smooth timing beats frantic bursts of speed every single time."},
    {"Beginner", "Keep your shoulders loose and avoid pressing keys harder than needed. A calm pace makes practice enjoyable and helps your muscle memory settle in."},
    {"Beginner", "Short daily drills work much better than one long session once a week. Even ten focused minutes can sharpen your accuracy over time."},
    {"Beginner", "Notice which letters trip you up during a test and give them extra care. Slowing down right after a mistake keeps your rhythm intact."},
    {"Beginner", "A clean desk and a comfortable chair make it easier to stay focused. Good habits around your workspace support steady daily improvement."},
    {"Beginner", "Do not worry if your speed feels low at first. Every fast typist started by learning where the keys live one letter at a time."},

    /* ==================== INTERMEDIATE ==================== */
    {"Intermediate", "Technology makes it easier to communicate, learn, and solve everyday problems. However, useful digital skills still require patience and regular practice. A clear process helps people turn new ideas into reliable results."},
    {"Intermediate", "A productive workspace is not only about having modern tools. It also depends on planning, attention, and the ability to focus on one task at a time. Good habits make complex work feel more manageable."},
    {"Intermediate", "Clear writing saves time for everyone who reads your notes, messages, or code comments. Short sentences and accurate spelling make technical ideas easier to share and review."},
    {"Intermediate", "Debugging becomes much faster when you test one change at a time and observe what happens. Careful observation turns confusing errors into predictable steps toward a working fix."},
    {"Intermediate", "Good documentation explains why a decision was made, not just what the code does. Future readers, including your future self, will thank you for clear notes."},
    {"Intermediate", "Version control lets you experiment freely while keeping a safe history of changes. Small, meaningful commits make it easier to track progress and roll back mistakes."},
    {"Intermediate", "Learning a new tool takes less time when you connect it to concepts you already know. Compare patterns, test small examples, and build from working parts."},
    {"Intermediate", "Steady pacing during a typing test keeps punctuation and capital letters accurate. Rushing at the end of a line often causes unnecessary errors."},
    {"Intermediate", "Organizing files and naming variables clearly reduces mental clutter during long projects. Simple structure prevents small messes from growing into headaches."},
    {"Intermediate", "Reviewing your past scores helps you spot whether you trade accuracy for speed. Aim for high accuracy first, then raise your pace smoothly."},

    /* ==================== ADVANCED ==================== */
    {"Advanced", "Software engineering combines logical thinking with careful communication. Developers break large problems into smaller tasks, test assumptions, and improve their solutions through feedback. Reliable software is usually the result of many thoughtful decisions."},
    {"Advanced", "In a connected world, information moves quickly across networks and devices. Security, accessibility, and responsible design must be considered from the beginning. Strong systems balance performance with the needs of the people who use them."},
    {"Advanced", "Modular architecture keeps codebases readable as features grow beyond a single file. Separating data storage, core logic, and user interface state makes testing and refactoring far less risky."},
    {"Advanced", "Performance optimization works best when guided by real measurements rather than guesses. Profiling bottlenecks, minimizing unnecessary allocations, and handling edge cases keep applications responsive under load."},
    {"Advanced", "Explicit state transitions prevent subtle bugs in event-driven applications. When timers, user input, and background tasks share state, clear invariants keep behavior predictable."},
    {"Advanced", "Defensive input validation and bound checks protect programs against unexpected data. Checking sizes, null pointers, and return codes early avoids silent corruption later."},
    {"Advanced", "Maintainable systems favor clarity and composability over clever shortcuts. Code that reads plainly makes onboarding, code reviews, and long-term maintenance far smoother."},
    {"Advanced", "Data integrity depends on clean schemas, proper error handling, and safe resource cleanup. Closing handles, freeing memory, and verifying writes keep applications dependable."},
    {"Advanced", "Complex workflows become easier to reason about when each component has a single responsibility. Clear boundaries let you upgrade one layer without breaking unrelated features."},
    {"Advanced", "Sustained technical growth comes from building real projects, inspecting failures, and refining your craft. Treating every bug as a clue turns routine practice into deep mastery."}
};
#define PASSAGE_COUNT (int)(sizeof(passages) / sizeof(passages[0]))

void ChoosePassage(void) {
    const char *level = GetLevelSelection();
    int candidates[PASSAGE_COUNT];
    int count = 0;

    for (int i = 0; i < PASSAGE_COUNT; ++i) {
        if (strcmp(passages[i].level, level) == 0) candidates[count++] = i;
    }

    if (count == 0) {
        CopySafe(gCurrentPassage, sizeof(gCurrentPassage), passages[0].text);
    } else {
        CopySafe(gCurrentPassage, sizeof(gCurrentPassage), passages[candidates[rand() % count]].text);
    }

    if (gPassage) SetWindowTextA(gPassage, gCurrentPassage);
}

static void CalculateStats(const char *typed, int elapsedSeconds, int *wpm, double *accuracy, int *errors) {
    int typedLen = (int)strlen(typed);
    int passageLen = (int)strlen(gCurrentPassage);
    int correct = 0, wrong = 0;

    for (int i = 0; i < typedLen; ++i) {
        if (i < passageLen && typed[i] == gCurrentPassage[i]) correct++;
        else wrong++;
    }

    if (elapsedSeconds < 1) elapsedSeconds = 1;
    *wpm = (int)((correct / 5.0) / (elapsedSeconds / 60.0) + 0.5);
    *accuracy = typedLen > 0 ? (100.0 * correct / typedLen) : 100.0;
    *errors = wrong;
}

int ComputeElapsedSeconds(void) {
    if (!gHasStarted) return 0;
    if (gFinished) return gElapsedSeconds > 0 ? gElapsedSeconds : 1;
    int sec = (int)((GetTickCount() - gStartTick) / 1000);
    return sec < 0 ? 0 : sec;
}

void UpdateStats(void) {
    char typed[2048];
    GetControlText(gInput, typed, sizeof(typed));

    int elapsed = ComputeElapsedSeconds();
    int wpm = 0, errors = 0;
    double accuracy = 100.0;
    CalculateStats(typed, elapsed, &wpm, &accuracy, &errors);

    char buf[64];
    snprintf(buf, sizeof(buf), "%d", wpm);
    SetWindowTextA(gWpm, buf);

    snprintf(buf, sizeof(buf), "%.1f%%", accuracy);
    SetWindowTextA(gAccuracy, buf);

    snprintf(buf, sizeof(buf), "%d", errors);
    SetWindowTextA(gErrors, buf);

    if (gDurationSeconds == 0) {
        snprintf(buf, sizeof(buf), "%02d:%02d", elapsed / 60, elapsed % 60);
    } else {
        int displayTime = gRunning ? gRemaining : (gFinished ? 0 : gDurationSeconds);
        if (displayTime < 0) displayTime = 0;
        snprintf(buf, sizeof(buf), "%02d:%02d", displayTime / 60, displayTime % 60);
    }
    SetWindowTextA(gTime, buf);
}

static void SyncPlayerName(void) {
    char name[64];
    if (GetControlText(gName, name, sizeof(name)) > 0) {
        CopySafe(gPlayer, sizeof(gPlayer), name);
    }
}

void FinishTest(void) {
    if ((!gRunning && !gHasStarted) || gFinished) return;

    KillTimer(gMain, TIMER_ID);
    gElapsedSeconds = ComputeElapsedSeconds();
    if (gElapsedSeconds < 1) gElapsedSeconds = 1;
    gRunning = 0;
    gFinished = 1;

    SyncPlayerName();
    char typed[2048];
    GetControlText(gInput, typed, sizeof(typed));
    CalculateStats(typed, gElapsedSeconds, &gLastWpm, &gLastAccuracy, &gLastErrors);

    int savedDuration = gDurationSeconds > 0 ? gDurationSeconds : gElapsedSeconds;
    SaveSessionToDb(gPlayer, gLastWpm, gLastAccuracy, gLastErrors, savedDuration, GetLevelSelection());

    UpdateStats();
    EnableWindow(gInput, FALSE);
    EnableWindow(gFinishBtn, FALSE);

    char msg[256];
    snprintf(msg, sizeof(msg),
             "Test complete, %s!\n\nSpeed: %d WPM\nAccuracy: %.1f%%\nErrors: %d",
             gPlayer, gLastWpm, gLastAccuracy, gLastErrors);
    MessageBoxA(gMain, msg, "Your typing result", MB_OK | MB_ICONINFORMATION);
}

void StartTest(void) {
    KillTimer(gMain, TIMER_ID);
    SyncPlayerName();

    gDurationSeconds = GetDurationSelection();
    gRemaining = gDurationSeconds;
    gElapsedSeconds = 0;
    gRunning = 0;
    gFinished = 0;
    gHasStarted = 0;
    gLastWpm = 0;
    gLastErrors = 0;
    gLastAccuracy = 0.0;

    ChoosePassage();
    SetWindowTextA(gInput, "");
    EnableWindow(gInput, TRUE);
    EnableWindow(gFinishBtn, TRUE);
    SetFocus(gInput);

    SetStatus(gDurationSeconds == 0
        ? "Free practice ready. Type the passage and click Finish when done."
        : "Ready. The timer starts when you type your first character.");
    UpdateStats();
}

void ResetTest(void) {
    KillTimer(gMain, TIMER_ID);
    gRunning = 0;
    gFinished = 0;
    gHasStarted = 0;
    gElapsedSeconds = 0;
    gDurationSeconds = GetDurationSelection();
    gRemaining = gDurationSeconds;

    SetWindowTextA(gInput, "");
    EnableWindow(gInput, TRUE);
    EnableWindow(gFinishBtn, FALSE);
    ChoosePassage();
    SetStatus("Test reset. Press Start to begin.");
    UpdateStats();
}