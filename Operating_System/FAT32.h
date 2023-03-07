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
	ull clusterBeginData;
	unsigned int* _FAT;
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

void displayBootSectorInfo(FAT32 origin);
void printFileTextContent(BYTE sector[], ull begin, ull n);
void displayDirFile(DirectoryFile input, int level);
void printRDET(LPCWSTR  drive, DirectoryFile inp, ull number, int level, unsigned int* FAT, FAT32 origin);

void readMainEntry(Entry c, DirectoryFile& res);
wstring readAdditionEntry(Entry c);
string fileCategory(BYTE value);

void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet);
void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir);

void FATOverView(LPCWSTR, unsigned int*&, FAT32&, BYTE*&);
void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
void viewDirectory(DirectoryFile*& Dir, ull& numberFile, const Entry* listEntry, const ull& countEntry);
void SDETView(LPCWSTR  drive, const FAT32& input, DirectoryFile& Dir, unsigned int*& fat);
