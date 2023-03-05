#pragma once
#define ull uint32_t

#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

struct FAT32
{
	ull bytePerSector;
	ull sectorPerCluster;
	ull sectorBootsector;
	ull numFAT;
	ull volumeSize;
	ull sizeFAT;
	ull clusterBeginRDET;
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

bool runFAT32();

int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[512]);
ull hexaToInt(string value);
ull byteToInt(BYTE value);
string intToHexa(ull value);
ull readPlace(BYTE sector[512], string addr, ull sizeByte);
void readSectorByByte(LPCWSTR  drive, ull readPoint, BYTE*& sector, ull totalByteSector);
void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir);
void RDetOverView(LPCWSTR  drive, BYTE*& sector, const FAT32& input, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
void FATOverView(LPCWSTR  drive, unsigned int*& FAT, const FAT32& input, BYTE*& sector);
void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet);
void displayFAT32(LPCWSTR drive, BYTE sector[512]);
bool isFAT32(BYTE sector[512]);

