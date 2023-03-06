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

string IntToHex(ull value);
ull HexToInt(string value);
ull ByteToInt(BYTE value);

int ReadSector(LPCWSTR, int, BYTE sector[512]);
ull readPlace(BYTE sector[512], string addr, ull sizeByte);
void readSectorByByte(LPCWSTR, ull, BYTE*&, ull);

bool isNTFS(BYTE sector[512]);

bool isFAT32(BYTE sector[512]);
void FATOverView(LPCWSTR, unsigned int*&, FAT32&, BYTE*&);
void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet);
void RDETOverView(LPCWSTR  drive, BYTE*& sector, const FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
void displayFAT32(LPCWSTR drive, BYTE sector[512]);