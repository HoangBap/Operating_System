#pragma once
#include<iostream>
#include<string>
#include"Function.h"
#define ull uint32_t
using namespace std;

struct FAT32
{
	ull bytePerSector;
	ull sectorPerCluster;
	ull sectorBootsector;
	ull numFAT;
	ull volumeSize;
	ull sizeFAT;
	ull clusterBeginOfRDET;
	ull bootSectorSaveSector;
	ull additionalInfoSector;
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
	ull beginCluster;
	ull fileSize;
	Entry* currEntry;
	vector<ull> listSector;
	ull numberEntry;

	//next section
	ull numberFile;
	DirectoryFile* childFiles;

	//prev sector
	DirectoryFile* fatherFiles;

};

//Hiển thị thông tin của bootsector FAT32 (số sector của bootsector, số sector cho mỗi cluster, ...)
void displayBootSectorInfo(FAT32 origin); 

//Hiển thị cây thư mục
void printRDET(LPCWSTR  drive, DirectoryFile inp, ull number, int level, unsigned int* FAT, FAT32 origin);
//In ra nội dung file nếu file đó là file text .txt
void printFileTextContent(BYTE sector[], ull begin, ull n);
//Hiển thị thông tin của file (Tên, loại, cluster bắt đầu, kích thước file, chiếm các sector nào)
void displayDirFile(DirectoryFile input, int level);

//Đọc entry chính
void readMainEntry(Entry c, DirectoryFile& res);
//Đọc entry phụ (nếu có)
wstring readAdditionEntry(Entry c);
void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir);

//Xác định thuộc tính, trạng thái file/thư mục
string fileCategory(BYTE value);

//Đọc dữ liệu của bộ nhớ có file system là FAT32
void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet);

//Tạo bảng FAT để sau này RDET có thể tra bảng FAT
void FATOverView(LPCWSTR, unsigned int*&, FAT32&, BYTE*&);
//
void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
void viewDirectory(DirectoryFile*& Dir, ull& numberFile, const Entry* listEntry, const ull& countEntry);
void SDETView(LPCWSTR  drive, const FAT32& input, DirectoryFile& Dir, unsigned int*& fat);
