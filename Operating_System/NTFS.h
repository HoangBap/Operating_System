#pragma once
#pragma once
#include<iostream>
#include<string>
#include"Function.h"
using namespace std;

struct NTFS
{
	string bytePerSectorString = "";
	string sectorPerClusterString = "";
	string DiskType = "";
	string sectorPerTrackString = "";
	string numberHeadString = "";
	string DiskSectorBeginString = "";
	string sectorDiskLogicString = "";
	string MFTClusterBeginString = "";
	string MFTBackupClusterBeginSring = "";
	string MFTEntrySizeString = "";
	string clusterOfIndexBufferString = "";
	string VolumnSerialNumber = "";

	uin32 bytePerSector;
	uin32 sectorPerCluster;
	uin32 sectorPerTrack;
	uin32 numberHead;
	uin32 DiskSectorBegin;
	uin32 sectorDiskLogic;
	uin32 MFTClusterBegin;
	uin32 MFTBackupClusterBegin;
	uin32 MFTEntrySize;
	uin32 clusterOfIndexBuffer;
};
struct MFTEntryAttributeHeader
{
	string attributeType;
	uin32 attributeSize;
	uin32 nonResidentFlag;
	uin32 nameLength;
	string nameOffset;
	string flags;
	uin32 AttributeID;
	uin32 AttributeLength;
	string AttributeOffset;



};
struct StandardInfomationAttribute
{
	string createTime = "";
	string lastUpdatedTime = "";
	string lastUpdatedTimeDiscription = "";
	string lastSeenContentTime = "";
	string flags;
	string classID;
	string ownerID;
	string securityID;

};
struct FilenameAtrtibute
{
	uin32 FatherMFTEntry;
	string flags;
	uin32 nameLength;
	string nameType;
	string filename;
	bool allowPrintData;
};
struct DataAttribute
{
	bool haveContentInEntry;
	uin32 contentSize;
	uin32 clusterCount;
	uin32 FirstCluster;
	string data;


};
struct MFTEntryAttribute
{
	MFTEntryAttributeHeader header;
	char typeAttribute;
	StandardInfomationAttribute* StandardInfo;
	FilenameAtrtibute* Filename;
	DataAttribute* data;
	//donotthing
};
struct MFTEntryHeader
{
	string entrySign = "";
	uin32 updateSequenceOffset;
	uin32 numberFixupElement;
	string logFileSquenceNumber;
	uin32 sequenceNumber;
	uin32 referenceCount;
	uin32 firstAttributeOffset;
	uin32 flag;
	uin32 MFTusedBytes;
	uin32 MFTEntrySize;
	string ExtensionRecord;
	string nextAttribute;
	uin32 idOfThisRecord;
};
struct MFTEntry
{
	MFTEntryHeader header;
	uin32 sizeAttribute;
	vector<MFTEntryAttribute> attribute;
};
struct MFT
{
	uin32 numberMFTEntry;
	vector<MFTEntry> arrayMFTEntry;
};
struct NTFSDirectoryFile
{
	uin32 idFile;
	uin32 parrentId;
	string fileName;
	string fileType;
	string nameType;
	bool allowPrintData;
	DataAttribute* data;

	// connection
	vector<NTFSDirectoryFile> chillFile;

};

bool isNTFS(BYTE sector[512]);

void readMFTEntryHeader(BYTE*& sector, MFTEntryHeader& header);

int readNTFSSectorByByte(LPCWSTR  drive, uin32 readPoint, BYTE*& sector, uin32 totalByteSector);

void readMFTEntryAttribute(BYTE* sector, MFTEntryAttribute& attribute);

string entryAttributeType(uin32 value);

void readMFTStandardInfo(BYTE* sector, StandardInfomationAttribute& standard);

string attributeType(uin32 value);

void readMFTFileName(BYTE* sector, FilenameAtrtibute& standard);

void readMFTData(BYTE* sector, DataAttribute& standard, uin32 size, uin32 VCN, uin32 fullSize, string beginAttributeOffset);

MFTEntry readMFTEntry(LPCWSTR  drive, NTFS origin, uin32 beginEntryId, bool& flag);

bool AddNTFSFileToTree(NTFSDirectoryFile& root, NTFSDirectoryFile& inp);

void readNTFSInfo(LPCWSTR  drive, BYTE sector[512], NTFS& origin, MFT& fileMFT, NTFSDirectoryFile& root);

void displayBPBInfo(NTFS origin);

void displayNTFSFileTree(LPCWSTR  drive, NTFSDirectoryFile root, NTFS origin, int level);

int ReadNTFSSectorByByte(LPCWSTR  drive, uin32 readPoint, BYTE*& sector, uin32 totalByteSector);

void printFileNTFSData(LPCWSTR  drive, uin32 clusterSize, uin32 clusterBegin, NTFS origin);

void displayNTFSDirectoryFile(LPCWSTR  drive, NTFSDirectoryFile temp, NTFS origin, int level);