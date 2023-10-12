#ifndef TOOLS_W_H
#define TOOLS_W_H

#include <windows.h>

void proceed();
WCHAR* getCurrentDateWCHAR();
WCHAR* getCurrentTimeWCHAR();
int choice(WCHAR* sentence1, int x, WCHAR* sentence2, int y);
int choiceBTW(WCHAR* sentence1, int x, WCHAR* sentence2, int y);

#endif