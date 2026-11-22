#ifndef DB_H
#define DB_H

int DatabaseInit(void);
void SaveSessionToDb(const char *player, int wpm, double accuracy, int errors, int duration, const char *difficulty);
void ShowLeaderboard(void);
void ShowHistory(void);

#endif