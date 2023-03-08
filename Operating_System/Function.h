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

//Hàm lấy thông tin trong bootsector với FAT32
uin32 readPlace(BYTE sector[512], string addr, uin32 sizeByte);

//Hàm đọc sector
void readSector(LPCWSTR, int, BYTE sector[512]);
//Hàm đọc sector với dung lượng lớn hơn, số byte đọc lớn hơn
void readSectorByByte(LPCWSTR, uin32, BYTE*&, uin32);

//Lấy size của tập tin/thư mục
uin32 getSize(DirectoryFile input);

//Kiểm tra file system là loại nào (FAT32 hay NTFS)
bool isNTFS(BYTE sector[512]);
bool isFAT32(BYTE sector[512]);
