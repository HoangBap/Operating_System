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

string intToHex(ull value);
ull hexToInt(string value);
ull byteToInt(BYTE value);
void trimWstring(wstring& input);

int readSector(LPCWSTR, int, BYTE sector[512]);
ull readPlace(BYTE sector[512], string addr, ull sizeByte);
void readSectorByByte(LPCWSTR, ull, BYTE*&, ull);

bool isNTFS(BYTE sector[512]);

bool isFAT32(BYTE sector[512]);
void RDETOverView(LPCWSTR  drive, BYTE*& sector, const FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
void displayFAT32(LPCWSTR drive, BYTE sector[512]);