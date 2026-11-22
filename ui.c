#include "config.h"
#include "ui.h"
#include "db.h"
#include "engine.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static HFONT gFont = NULL, gFontSmall = NULL, gFontTitle = NULL, gFontMono = NULL;
static HBRUSH gBgBrush = NULL, gInputBrush = NULL;

static void SetFont(HWND h, HFONT f) {
    if (h && f) SendMessageA(h, WM_SETFONT, (WPARAM)f, TRUE);
}

static HWND MakeControl(const char *cls, const char *text, DWORD style,
                        int x, int y, int w, int h, int id) {
    return CreateWindowExA(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                           x, y, w, h, gMain, (HMENU)(INT_PTR)id, gInstance, NULL);
}

static void DrawCard(HDC dc, RECT r, const char *label) {
    HBRUSH b = CreateSolidBrush(C_WHITE);
    FillRect(dc, &r, b);
    DeleteObject(b);

    HPEN pen = CreatePen(PS_SOLID, 1, C_BORDER);
    HPEN oldPen = (HPEN)SelectObject(dc, pen);
    HBRUSH hollow = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, hollow);
    Rectangle(dc, r.left, r.top, r.right, r.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, C_MUTED);
    RECT lr = {r.left + 14, r.top + 8, r.right - 10, r.top + 26};
    DrawTextA(dc, label, -1, &lr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        gMain = hwnd;
        gBgBrush = CreateSolidBrush(C_BG);
        gInputBrush = CreateSolidBrush(C_WHITE);

        gFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        gFontSmall = CreateFontA(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        gFontTitle = CreateFontA(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        gFontMono = CreateFontA(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                FIXED_PITCH | FF_MODERN, "Consolas");

        HWND title = MakeControl("STATIC", APP_NAME, SS_LEFT, 28, 16, 500, 36, -1);
        SetFont(title, gFontTitle);

        HWND subtitle = MakeControl("STATIC", "Build accuracy, consistency, and speed with focused practice.",
                                  SS_LEFT, 30, 54, 620, 22, -1);
        SetFont(subtitle, gFontSmall);

        HWND nameLabel = MakeControl("STATIC", "PLAYER NAME", SS_LEFT, 30, 96, 110, 20, -1);
        SetFont(nameLabel, gFontSmall);
        gName = MakeControl("EDIT", "Player", WS_BORDER | ES_AUTOHSCROLL, 30, 118, 175, 30, ID_NAME);
        SetFont(gName, gFont);

        HWND durLabel = MakeControl("STATIC", "TEST DURATION", SS_LEFT, 218, 96, 120, 20, -1);
        SetFont(durLabel, gFontSmall);
        gDuration = MakeControl("COMBOBOX", "", CBS_DROPDOWNLIST | WS_VSCROLL, 218, 118, 130, 150, ID_DURATION);
        SendMessageA(gDuration, CB_ADDSTRING, 0, (LPARAM)"30 seconds");
        SendMessageA(gDuration, CB_ADDSTRING, 0, (LPARAM)"60 seconds");
        SendMessageA(gDuration, CB_ADDSTRING, 0, (LPARAM)"120 seconds");
        SendMessageA(gDuration, CB_ADDSTRING, 0, (LPARAM)"Free practice");
        SendMessageA(gDuration, CB_SETCURSEL, 1, 0);
        SetFont(gDuration, gFont);

        HWND levelLabel = MakeControl("STATIC", "DIFFICULTY", SS_LEFT, 360, 96, 100, 20, -1);
        SetFont(levelLabel, gFontSmall);
        gLevel = MakeControl("COMBOBOX", "", CBS_DROPDOWNLIST | WS_VSCROLL, 360, 118, 135, 150, ID_LEVEL);
        SendMessageA(gLevel, CB_ADDSTRING, 0, (LPARAM)"Beginner");
        SendMessageA(gLevel, CB_ADDSTRING, 0, (LPARAM)"Intermediate");
        SendMessageA(gLevel, CB_ADDSTRING, 0, (LPARAM)"Advanced");
        SendMessageA(gLevel, CB_SETCURSEL, 0, 0);
        SetFont(gLevel, gFont);

        HWND start = MakeControl("BUTTON", "START", BS_PUSHBUTTON, 508, 115, 92, 36, ID_START);
        SetFont(start, gFont);
        gFinishBtn = MakeControl("BUTTON", "FINISH", BS_PUSHBUTTON, 608, 115, 92, 36, ID_FINISH);
        SetFont(gFinishBtn, gFont);
        EnableWindow(gFinishBtn, FALSE);
        HWND reset = MakeControl("BUTTON", "RESET", BS_PUSHBUTTON, 708, 115, 82, 36, ID_RESET);
        SetFont(reset, gFont);

        RECT cards[4] = {
            {30, 172, 200, 236},
            {220, 172, 390, 236},
            {410, 172, 580, 236},
            {600, 172, 770, 236}
        };
        gWpm      = MakeControl("STATIC", "0",      SS_CENTER, cards[0].left + 8, cards[0].top + 28, cards[0].right - cards[0].left - 16, 30, ID_WPM);
        gAccuracy = MakeControl("STATIC", "100.0%", SS_CENTER, cards[1].left + 8, cards[1].top + 28, cards[1].right - cards[1].left - 16, 30, ID_ACCURACY);
        gTime     = MakeControl("STATIC", "01:00",  SS_CENTER, cards[2].left + 8, cards[2].top + 28, cards[2].right - cards[2].left - 16, 30, ID_TIME);
        gErrors   = MakeControl("STATIC", "0",      SS_CENTER, cards[3].left + 8, cards[3].top + 28, cards[3].right - cards[3].left - 16, 30, ID_ERRORS);
        SetFont(gWpm, gFontTitle);
        SetFont(gAccuracy, gFontTitle);
        SetFont(gTime, gFontTitle);
        SetFont(gErrors, gFontTitle);

        HWND passageLabel = MakeControl("STATIC", "TYPE THE PASSAGE BELOW", SS_LEFT, 30, 250, 300, 22, -1);
        SetFont(passageLabel, gFont);
        gPassage = MakeControl("EDIT", "", ES_MULTILINE | ES_READONLY | WS_BORDER | WS_VSCROLL, 30, 278, 760, 112, ID_PASSAGE);
        SetFont(gPassage, gFontMono);

        HWND inputLabel = MakeControl("STATIC", "YOUR TYPING", SS_LEFT, 30, 405, 250, 22, -1);
        SetFont(inputLabel, gFont);
        gInput = MakeControl("EDIT", "", ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER, 30, 433, 760, 115, ID_INPUT);
        SetFont(gInput, gFontMono);

        gStatus = MakeControl("STATIC", "Choose your settings, then press Start.", SS_LEFT, 30, 560, 540, 25, ID_STATUS);
        SetFont(gStatus, gFontSmall);

        gLeaderBtn  = MakeControl("BUTTON", "LEADERBOARD", BS_PUSHBUTTON, 580, 555, 105, 34, ID_LEADER);
        gHistoryBtn = MakeControl("BUTTON", "HISTORY",     BS_PUSHBUTTON, 695, 555, 95, 34, ID_HISTORY);
        SetFont(gLeaderBtn, gFontSmall);
        SetFont(gHistoryBtn, gFontSmall);

        srand((unsigned)time(NULL));
        ChoosePassage();
        gDbReady = DatabaseInit();
        if (!gDbReady) {
            EnableWindow(gLeaderBtn, FALSE);
            EnableWindow(gHistoryBtn, FALSE);
        }
        return 0;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);
        int code = HIWORD(wp);

        if (id == ID_START && code == BN_CLICKED) StartTest();
        else if (id == ID_FINISH && code == BN_CLICKED) FinishTest();
        else if (id == ID_RESET && code == BN_CLICKED) ResetTest();
        else if (id == ID_LEADER && code == BN_CLICKED) ShowLeaderboard();
        else if (id == ID_HISTORY && code == BN_CLICKED) ShowHistory();
        else if (id == ID_INPUT && code == EN_CHANGE && !gFinished) {
            char typed[2048];
            GetControlText(gInput, typed, sizeof(typed));

            if (!gHasStarted && typed[0] != '\0') {
                gHasStarted = 1;
                gStartTick = GetTickCount();
                if (gDurationSeconds > 0) {
                    gRemaining = gDurationSeconds;
                    gRunning = 1;
                    SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
                    SetStatus("Test in progress. Keep a steady rhythm.");
                } else {
                    SetStatus("Free practice in progress. Click Finish when ready.");
                }
            }

            UpdateStats();

            if ((gRunning || (gDurationSeconds == 0 && gHasStarted)) &&
                gCurrentPassage[0] != '\0' &&
                strlen(typed) >= strlen(gCurrentPassage)) {
                FinishTest();
            }
        }
        break;
    }

    case WM_TIMER:
        if (wp == TIMER_ID && gRunning) {
            int elapsed = ComputeElapsedSeconds();
            gRemaining = gDurationSeconds - elapsed;
            if (gRemaining <= 0) {
                gRemaining = 0;
                gElapsedSeconds = gDurationSeconds;
                FinishTest();
            } else {
                UpdateStats();
            }
        }
        return 0;

    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)wp;
        SetTextColor(dc, C_TEXT);
        SetBkColor(dc, C_WHITE);
        return (LRESULT)gInputBrush;
    }

    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wp;
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, C_TEXT);
        return (LRESULT)gBgBrush;
    }

    case WM_ERASEBKGND: {
        RECT r;
        GetClientRect(hwnd, &r);
        FillRect((HDC)wp, &r, gBgBrush);
        return 1;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT cards[4] = {
            {30, 172, 200, 236},
            {220, 172, 390, 236},
            {410, 172, 580, 236},
            {600, 172, 770, 236}
        };
        const char *labels[4] = {"WORDS / MIN", "ACCURACY", "TIME LEFT", "ERRORS"};
        for (int i = 0; i < 4; i++) DrawCard(dc, cards[i], labels[i]);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        if (gDb) { sqlite3_close(gDb); gDb = NULL; }
        if (gFont) DeleteObject(gFont);
        if (gFontSmall) DeleteObject(gFontSmall);
        if (gFontTitle) DeleteObject(gFontTitle);
        if (gFontMono) DeleteObject(gFontMono);
        if (gBgBrush) DeleteObject(gBgBrush);
        if (gInputBrush) DeleteObject(gInputBrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}