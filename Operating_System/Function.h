#pragma once
#define ull uint32_t
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

string IntToHex(ull value);
ull HexToInt(string value);
ull ByteToInt(BYTE value);

int ReadSector(LPCWSTR, int, BYTE sector[512]);
ull readPlace(BYTE sector[512], string& byteString, string addr, ull sizeByte);

bool isFAT32(BYTE sector[512]);
bool isNTFS(BYTE sector[512]);