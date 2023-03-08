#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

BOOL setxy(int x, int y);
void drawRect(int x, int y, int width, int height, int curPosX, int curPosY);
string space(int inp);
wstring spaceW(int inp);
