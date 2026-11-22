#include "config.h"
#include "ui.h"
#include <string.h>

#ifndef ICC_STANDARD_CLASSES
#define ICC_STANDARD_CLASSES 0x00004000
#endif

typedef BOOL (WINAPI *PFN_InitCommonControlsEx)(const INITCOMMONCONTROLSEX *);

/* Global Variable Definitions */
HINSTANCE gInstance = NULL;
HWND gMain = NULL, gName = NULL, gDuration = NULL, gLevel = NULL, gPassage = NULL, gInput = NULL;
HWND gFinishBtn = NULL, gLeaderBtn = NULL, gHistoryBtn = NULL;
HWND gStatus = NULL, gWpm = NULL, gAccuracy = NULL, gTime = NULL, gErrors = NULL;

sqlite3 *gDb = NULL;
int gDbReady = 0;
char gCurrentPassage[1024];
char gPlayer[64] = "Player";
int gDurationSeconds = 60;
int gRemaining = 60;
int gElapsedSeconds = 0;
int gRunning = 0;
int gFinished = 0;
int gHasStarted = 0;
int gLastWpm = 0;
double gLastAccuracy = 0.0;
int gLastErrors = 0;
DWORD gStartTick = 0;

/* Shared Utility Definitions */
void SetStatus(const char *s) {
    if (gStatus && s) SetWindowTextA(gStatus, s);
}

void CopySafe(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    strncpy(dst, src, cap - 1);
    dst[cap - 1] = '\0';
}

int GetControlText(HWND h, char *out, int cap) {
    if (!h || !out || cap <= 0) return 0;
    out[0] = '\0';
    GetWindowTextA(h, out, cap);
    return (int)strlen(out);
}

int GetDurationSelection(void) {
    int i = (int)SendMessageA(gDuration, CB_GETCURSEL, 0, 0);
    if (i == 0) return 30;
    if (i == 2) return 120;
    if (i == 3) return 0;
    return 60;
}

const char *GetLevelSelection(void) {
    static const char *levels[] = {"Beginner", "Intermediate", "Advanced"};
    int i = (int)SendMessageA(gLevel, CB_GETCURSEL, 0, 0);
    if (i < 0 || i > 2) i = 0;
    return levels[i];
}

static void InitCommonControlsSafely(void) {
    HMODULE hComCtl = LoadLibraryA("comctl32.dll");
    if (hComCtl) {
        PFN_InitCommonControlsEx pfnInit =
            (PFN_InitCommonControlsEx)GetProcAddress(hComCtl, "InitCommonControlsEx");
        if (pfnInit) {
            INITCOMMONCONTROLSEX icc;
            ZeroMemory(&icc, sizeof(icc));
            icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icc.dwICC = ICC_STANDARD_CLASSES;
            pfnInit(&icc);
        }
        FreeLibrary(hComCtl);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    (void)hPrev;
    (void)cmd;
    gInstance = hInst;

    InitCommonControlsSafely();

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "TypingSpeedTestProWindow";
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassA(&wc)) return 1;

    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, APP_NAME,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 840, 660,
                                NULL, NULL, hInst, NULL);
    if (!hwnd) return 1;

    gMain = hwnd;
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}