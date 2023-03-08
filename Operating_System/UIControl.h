#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

//Xác định toạ độ x, y trên console 
BOOL setxy(int x, int y);
//Hàm vẽ bảng
void drawRect(int x, int y, int width, int height, int curPosX, int curPosY);
//Hàm tạo khoảng trống
string space(int inp);
wstring spaceW(int inp);
