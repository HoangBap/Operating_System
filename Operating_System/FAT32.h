//#pragma once
//#define ull uint32_t
//
//#include <windows.h>
//#include <string>
//#include <iostream>
//#include <vector>
//using namespace std;
//
//struct FAT32
//{
//	ull bytePerSector;
//	ull sectorPerCluster;
//	ull sectorBootsector;
//	ull numFAT;
//	ull volumeSize;
//	ull sizeFAT;
//	ull clusterBeginRDET;
//	ull bootSectorSaveSector;
//	ull additionalInfoSector;
//};
//
//struct Entry
//{
//	BYTE status;
//	BYTE type;
//	bool additionEntry;
//	BYTE information[32];
//};
//
//struct DirectoryFile
//{
//	string type;
//	wstring name;
//	string extension;
//	ull beginCluster;
//	ull fileSize;
//	Entry* currEntry;
//	vector<ull> listSector;
//	ull numberEntry;
//
//	//next section
//	ull numberFile;
//	DirectoryFile* childFiles;
//
//	//prev sector
//	DirectoryFile* fatherFiles;
//
//};
//
//bool runFAT32();
//
//int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[512]);
//ull hexaToInt(string value);
//ull byteToInt(BYTE value);
//string intToHexa(ull value);
//ull readPlace(BYTE sector[512], string addr, ull sizeByte);
//void readSectorByByte(LPCWSTR  drive, ull readPoint, BYTE*& sector, ull totalByteSector);
//void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir);
//void RDetOverView(LPCWSTR  drive, BYTE*& sector, const FAT32& input, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector);
//void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet);
//void displayFAT32(LPCWSTR drive, BYTE sector[512]);
//bool isFAT32(BYTE sector[512]);
//
#pragma once
#include<iostream>
#include<string>
#include"Function.h"
#define ull uint32_t
using namespace std;

class Object
{
public:

};

class FAT32
{
private:
	ull _bytePerSector;
	ull _sectorPerCluster;
	ull _sectorBootsector;
	ull _numFAT;
	ull _volumeSize;
	ull _sizeFAT;
	ull _clusterBeginRDET;
	ull _bootSectorSaveSector;
	ull _additionalInfoSector;
	unsigned int* _FAT;

public:
	ull bytePerSector() { return _bytePerSector; }
	ull sectorPerCluster() { return _sectorPerCluster; }
	ull sectorBootsector() { return _sectorBootsector; }
	ull numFAT() { return _numFAT; }
	ull volumeSize() { return _volumeSize; }
	ull sizeFAT() { return _sizeFAT; }
	ull clusterBeginRDET() { return _clusterBeginRDET; }
	ull bootSectorSaveSector() { return _bootSectorSaveSector; }
	ull additionalInfoSector() { return _additionalInfoSector; }

	void setBytePerSector(ull value) { _bytePerSector = value; }
	void setSectorPerCluster(ull value) { _sectorPerCluster = value; }
	void setSectorBootSector(ull value) { _sectorBootsector = value; }
	void setNumFAT(ull value) { _numFAT = value; }
	void setVolumeSize(ull value) { _volumeSize = value; }
	void setSizeFAT(ull value) { _sizeFAT = value; }
	void setClusterBeginRDET(ull value) { _clusterBeginRDET = value; }
	void setBootSectorSaveSector(ull value) { _bootSectorSaveSector = value; }
	void setAdditionalInfoSector(ull value) { _additionalInfoSector = value; }

public:
	void printFAT32Info();
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

int ConvertHexToDec(string);
string Read(string** Table, int x, int y, int NumberOfBytes);