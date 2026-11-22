#ifndef CONFIG_H
#define CONFIG_H

#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#include <windows.h>
#include <commctrl.h>
#include <sqlite3.h>

#define APP_NAME "Typing Speed Test Pro"
#define DB_NAME  "typing_test.db"

/* Control IDs */
#define ID_NAME       101
#define ID_DURATION   102
#define ID_LEVEL      103
#define ID_PASSAGE    104
#define ID_INPUT      105
#define ID_START      106
#define ID_FINISH     107
#define ID_RESET      108
#define ID_LEADER     109
#define ID_HISTORY    110
#define ID_STATUS     111
#define ID_WPM        112
#define ID_ACCURACY   113
#define ID_TIME       114
#define ID_ERRORS     115

/* Timer */
#define TIMER_ID       1
#define TIMER_INTERVAL 200

/* Theme Colors */
#define C_BG     RGB(245, 247, 250)
#define C_WHITE  RGB(255, 255, 255)
#define C_TEXT   RGB(31, 41, 55)
#define C_MUTED  RGB(107, 114, 128)
#define C_BORDER RGB(226, 232, 240)

/* Shared UI Handles */
extern HINSTANCE gInstance;
extern HWND gMain, gName, gDuration, gLevel, gPassage, gInput;
extern HWND gFinishBtn, gLeaderBtn, gHistoryBtn;
extern HWND gStatus, gWpm, gAccuracy, gTime, gErrors;

/* Shared DB & Game State */
extern sqlite3 *gDb;
extern int gDbReady;
extern char gCurrentPassage[1024];
extern char gPlayer[64];
extern int gDurationSeconds;
extern int gRemaining;
extern int gElapsedSeconds;
extern int gRunning;
extern int gFinished;
extern int gHasStarted;
extern int gLastWpm;
extern double gLastAccuracy;
extern int gLastErrors;
extern DWORD gStartTick;

/* Utility Helpers */
void SetStatus(const char *s);
void CopySafe(char *dst, size_t cap, const char *src);
int GetControlText(HWND h, char *out, int cap);
int GetDurationSelection(void);
const char *GetLevelSelection(void);

#endif