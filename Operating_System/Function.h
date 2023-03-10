#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#define uin32 uint32_t
using namespace std;

string intToHex(uin32 value);
uin32 hexToInt(string value);
uin32 byteToInt(BYTE value);
char hexToChar(string value);
void printToASCCIIandHEX(BYTE sector[], uin32 begin, uin32 n);
void trimWstring(wstring& input);

//Hàm lấy thông tin trong bootsector với FAT32
uin32 readPlace(BYTE sector[512], string addr, uin32 sizeByte);
void readPlaceForString(BYTE sector[512], string addr, uin32 sizeByte, string& byteString);

//Hàm đọc sector
void readSector(LPCWSTR, int, BYTE sector[512]);
//Hàm đọc sector với dung lượng lớn hơn, số byte đọc lớn hơn
void readSectorByByte(LPCWSTR, uin32, BYTE*&, uin32);

//Kiểm tra file system là loại nào (FAT32 hay NTFS)
bool isNTFS(BYTE sector[512]);
