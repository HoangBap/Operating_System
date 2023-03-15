#pragma once
#include<iostream>
#include<string>

#include"UIControl.h"
#include"Function.h"

#define HEIGHT 8
#define WIDTH 50
#define DISTANCE 10

using namespace std;

struct NTFS
{
	string DiskType = "";
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
	uin32 fileSize;
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
	uin32 fileSize = 0;
	string fileName;
	string fileType;
	string nameType;
	bool allowPrintData;
	DataAttribute* data;

	// connection
	vector<NTFSDirectoryFile> childFiles;
};

bool isNTFS(BYTE sector[512]);

//_____________________Hàm xử lý MFT
void readMFTEntryHeader(BYTE*& sector, MFTEntryHeader& header);
void readMFTEntryAttribute(BYTE* sector, MFTEntryAttribute& attribute);
//Gán thuộc tích cho tập tin/thư mục
void readMFTStandardInfo(BYTE* sector, StandardInfomationAttribute& standard);
//Đọc tên file của MFT
void readMFTFileName(BYTE* sector, FilenameAtrtibute& standard);
//Đọc dữ liệu của MFT
void readMFTData(BYTE* sector, DataAttribute& standard, uin32 size, uin32 VCN, uin32 fullSize, string beginAttributeOffset);
//Đọc Entry của MFT
MFTEntry readMFTEntry(LPCWSTR  drive, NTFS origin, uin32 beginEntryId, bool& flag);
string entryAttributeType(uin32 value);
//Convert giá trị thập phân thành nhị phân và sau đó trả về thuộc tính của tập tin/thư mục
string attributeType(uin32 value);
//______________________________________________________________

bool AddNTFSFileToTree(NTFSDirectoryFile& root, NTFSDirectoryFile& inp);

void readFileNTFSData(LPCWSTR  drive, uin32 clusterSize, uin32 clusterBegin, NTFS origin);
uin32 getNTFSSize(NTFSDirectoryFile input, NTFS origin);

//In nội dung file text
void printNTFSFileTextContent(BYTE sector[], uin32 begin, uin32 n);
//Đọc nội dung dữ liệu của file
void readFileNTFSData(LPCWSTR  drive, uin32 clusterSize, uin32 clusterBegin, NTFS origin);
//Hiển thị các thông tin cơ bản của tập tin/thư mục
void displayNTFSDirFileInfo(NTFSDirectoryFile input, NTFS origin, uin32 numberFolder);
//In cây thư mục
void printNTFSDirectory(LPCWSTR  drive, NTFSDirectoryFile root, NTFS origin, uin32 numberFile, bool flag);

//Hiển thị thông tin cơ bản của phân vùng
void displayBPBInfo(NTFS origin);
//Đọc các thông tin cơ bản của phân vùng 
void readNTFSInfo(LPCWSTR  drive, BYTE sector[512], NTFS& origin, MFT& fileMFT, NTFSDirectoryFile& root);
