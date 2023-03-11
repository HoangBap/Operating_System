#pragma once
#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>
#include<algorithm>

#include"Function.h"
using namespace std;

struct FAT32
{
	uin32 bytePerSector;
	uin32 sectorPerCluster;
	uin32 sectorBootsector;
	uin32 numFAT;
	uin32 volumeSize;
	uin32 sizeFAT;
	uin32 clusterBeginOfRDET;
	uin32 bootSectorSaveSector;
	uin32 additionalInfoSector;
};

struct Entry
{
	BYTE status;
	BYTE type;
	bool additionEntry;
	BYTE information[32];
};

struct DirectoryFile
{
	string type;
	wstring name;
	string extension;
	uin32 beginCluster;
	uin32 fileSize;
	Entry* currEntry;
	vector<uin32> listSector;
	uin32 numberEntry;

	//next section
	uin32 numberFile;
	DirectoryFile* childFiles;

	//prev sector
	DirectoryFile* fatherFiles;

};

bool isFAT32(BYTE sector[512]);

uin32 getSizeFAT32(DirectoryFile input);

//Hiển thị thông tin của bootsector FAT32 (số sector của bootsector, số sector cho mỗi cluster, ...)
void displayFAT32BootSectorInfo(FAT32 origin); 

//Hiển thị cây thư mục
void printDirectory(LPCWSTR  drive, DirectoryFile inp, uin32 number, unsigned int* FAT, FAT32 origin, bool flag);
//In ra nội dung file nếu file đó là file text .txt
void printFileTextContent(BYTE sector[], uin32 begin, uin32 n);
//Hiển thị thông tin của file (Tên, loại, cluster bắt đầu, kích thước file, chiếm các sector nào)
void displayDirFile(DirectoryFile input, int level);

//Đọc entry chính
void readMainEntry(Entry c, DirectoryFile& res);
//Đọc entry phụ (nếu có)
wstring readAdditionEntry(Entry c);
void entrySplitView(BYTE*& sector, uin32 sizeSector, DirectoryFile& Dir);

//Xác định thuộc tính, trạng thái file/thư mục
string fileCategory(BYTE value);

//Đọc dữ liệu của bộ nhớ có file system là FAT32
void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, uin32& beginRdet, uin32& sizeRdet);

//Tạo bảng FAT để sau này RDET có thể tra bảng FAT
void FATOverView(LPCWSTR, unsigned int*&, FAT32&, BYTE*&);
//Tạo bảng thư mục gốc RDET
void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, uin32& readPoint, uin32& totalByteSector);

//Đọc entry của các tập tin/thư mục (của RDET hoặc SDET)
void viewDirectory(DirectoryFile*& Dir, uin32& numberFile, const Entry* listEntry, const uin32& countEntry);
//Tạo bảng thư mục con SDET
void SDETView(LPCWSTR  drive, const FAT32& input, DirectoryFile& Dir, unsigned int*& fat);
