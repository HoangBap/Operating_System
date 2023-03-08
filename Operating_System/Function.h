#pragma once
#define ull uint32_t
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

#include"FAT32.h"
using namespace std;

string intToHex(uin32 value);
uin32 hexToInt(string value);
uin32 byteToInt(BYTE value);
void trimWstring(wstring& input);

uin32 readPlace(BYTE sector[512], string addr, uin32 sizeByte);
void readSector(LPCWSTR, int, BYTE sector[512]);
void readSectorByByte(LPCWSTR, uin32, BYTE*&, uin32);

bool isNTFS(BYTE sector[512]);
bool isFAT32(BYTE sector[512]);
