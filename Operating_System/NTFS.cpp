#include"NTFS.h"
#include"UIControl.h"

void readMFTEntryHeader(BYTE*& sector, MFTEntryHeader& header)
{
	header.entrySign = "";
	for (int i = 0; i < 4; i++)
	{
		BYTE b = sector[0 + i];

		header.entrySign += isascii(b) ? b : '.';
	}

	string temp;
	header.updateSequenceOffset = readPlace(sector, "4", 2);
	header.numberFixupElement = readPlace(sector, "6", 2);
	readPlaceForString(sector, "8", 8, header.logFileSquenceNumber);
	header.sequenceNumber = readPlace(sector, "10", 2);
	header.referenceCount = readPlace(sector, "12", 2);
	header.firstAttributeOffset = readPlace(sector, "14", 2);
	header.flag = readPlace(sector, "16", 2);

	header.MFTusedBytes = readPlace(sector, "18", 4);
	header.MFTEntrySize = readPlace(sector, "1c", 4);
	readPlaceForString(sector, "20", 8, header.ExtensionRecord);
	readPlaceForString(sector, "28", 2, header.nextAttribute);
	header.idOfThisRecord = readPlace(sector, "2c", 4);
	//DisplayMFTEntryHeader(header);
}

int readNTFSSectorByByte(LPCWSTR  drive, uin32 readPoint, BYTE*& sector, uin32 totalByteSector)
{

	sector = new BYTE[totalByteSector];
	LARGE_INTEGER scale;
	scale.QuadPart = readPoint;
	//cout << endl << scale.LowPart << "-" << scale.HighPart << endl;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		printf("CreateFile: %u\n", GetLastError());
		return 1;
	}
	SetFilePointer(device, scale.LowPart, &scale.HighPart, FILE_BEGIN);//Set a Point to Read


	//SetFilePointer(device, numberSector*sectorSize, NULL, FILE_CURRENT);//Set a Point to Read
	if (!ReadFile(device, sector, totalByteSector, &bytesRead, NULL))
	{
		printf("ReadFile: %u\n", GetLastError());
	}

}

string entryAttributeType(uin32 value)
{
	unsigned short type = 0;
	string res = "";

	while (value > 0)
	{

		if (value % 2 == 1)
		{
			switch (type)
			{
			case 0: res += "Compressed "; break;
			case 14: res += "Encrypted "; break;
			case 15: res += "Sparse "; break;


			}
		}
		type++;
		value >>= 1;
	}


	return res;
}

void readMFTEntryAttribute(BYTE* sector, MFTEntryAttribute& attrib)
{

	MFTEntryAttributeHeader& header = attrib.header;
	//header
	string temp;
	readPlaceForString(sector, "0", 4, header.attributeType);
	header.attributeSize = readPlace(sector, "4", 4);

	header.nonResidentFlag = readPlace(sector, "8", 1);
	header.nameLength = readPlace(sector, "9", 1);
	readPlaceForString(sector, "a", 2, header.nameOffset);
	readPlaceForString(sector, "c", 2, header.flags);
	header.flags = entryAttributeType(hexToInt(header.flags));
	header.AttributeID = readPlace(sector, "e", 2);
	header.AttributeLength = readPlace(sector, "10", 4);
	readPlaceForString(sector, "14", 2, header.AttributeOffset);
	//DisplayMFTEntryAttributeHeader(header);
}

void readMFTStandardInfo(BYTE* sector, StandardInfomationAttribute& standard)
{
	readPlaceForString(sector, "0", 8, standard.createTime);
	readPlaceForString(sector, "8", 8, standard.lastUpdatedTime);
	readPlaceForString(sector, "10", 8, standard.lastUpdatedTimeDiscription);
	readPlaceForString(sector, "18", 8, standard.lastSeenContentTime);
	readPlaceForString(sector, "20", 4, standard.flags);
	readPlaceForString(sector, "2c", 4, standard.classID);
	readPlaceForString(sector, "30", 4, standard.ownerID);
	readPlaceForString(sector, "34", 4, standard.securityID);
	standard.flags = attributeType(hexToInt(standard.flags));
}

string attributeType(uin32 value)
{
	unsigned short type = 0;
	string res = "";

	while (value > 0)
	{

		if (value % 2 == 1)
		{
			switch (type)
			{
			case 0: res += "Read Only "; break;
			case 1: res += "Hidden "; break;
			case 2: res += "System "; break;
			case 5: res += "Achive "; break;
			case 6: res += "Device "; break;
			case 7: res += "Normal "; break;
			case 8: res += "Temporary "; break;
			case 9: res += "Sparse "; break;
			case 10: res += "Resparese "; break;
			case 11: res += "Compressed "; break;
			case 12: res += "Offline "; break;
			case 13: res += "Not Content "; break;
			case 14: res += "Encrypted "; break;
			case 28: res += "Directory "; break;
			case 29: res += "Index View "; break;

			}
		}
		type++;
		value >>= 1;
	}


	return res;
}

void readMFTFileName(BYTE* sector, FilenameAtrtibute& standard)
{
	readPlaceForString(sector, "38", 4, standard.flags);
	standard.flags = attributeType(hexToInt(standard.flags));
	string temp;
	standard.FatherMFTEntry = readPlace(sector, "0", 8);
	standard.nameLength = readPlace(sector, "40", 1);
	readPlaceForString(sector, "41", 1, standard.nameType);
	readPlaceForString(sector, "42", standard.nameLength * 2, standard.filename);
	string name = "";
	for (uin32 i = standard.nameLength * 2 - 1; i > 0; i--)
	{
		string temping = standard.filename.substr(2 * i, 2);
		if (temping.find("00") == string::npos)
		{
			name += char(hexToInt(temping));
		}
	}
	standard.filename = name;
	switch (hexToInt(standard.nameType))
	{
	case 0: 
		standard.nameType = "POSIX"; 
		break;

	case 1: 
		standard.nameType = "Win32"; 
		break;

	case 2: 
		standard.nameType = "DOS"; 
		break;

	case 3: 
		standard.nameType = "Win32 & DOS";
		break;

	default:
		break;
	}
}

void readMFTData(BYTE* sector, DataAttribute& standard, uin32 size, uin32 VCN, uin32 fullSize, string beginAttributeOffset)
{
	if (standard.haveContentInEntry == true)
	{
		string readDatatemp;
		uin32 beginOffset = hexToInt(beginAttributeOffset);
		//printToASCCIIandHEX(sector, 0, size);
		readPlaceForString(sector, "0", size, readDatatemp);

		string name = "";
		for (uin32 i = readDatatemp.length() - 2; i >= 0; i -= 2)
		{
			if (i > readDatatemp.length()) 
				break;

			string temping = readDatatemp.substr(i, 2);
			if (temping.find("00") == string::npos)
			{
				name += char(hexToInt(temping));
			}
		}
		standard.data = name;

	}
	else
	{
		//printToASCCIIandHEX(sector, 0, fullSize);
		string as = intToHex(VCN);

		uin32 tempsize = as.length() % 2 == 0 ? as.length() / 2 : as.length() / 2 + 1;
		uin32 RundataOffset = readPlace(sector, "20", 2);
		standard.contentSize = readPlace(sector + RundataOffset, "0", 1);
		standard.clusterCount = readPlace(sector + RundataOffset, "1", tempsize);
		standard.FirstCluster = readPlace(sector + RundataOffset, intToHex(1 + tempsize), fullSize - RundataOffset - 1 - tempsize);
		standard.data = "";

	}
}

MFTEntry readMFTEntry(LPCWSTR  drive, NTFS origin, uin32 beginEntryId, bool& flag)
{

	uin32 beginMFT = origin.MFTClusterBegin * origin.sectorPerCluster * origin.bytePerSector + beginEntryId * origin.MFTEntrySize;
	uin32 MFTEntrySize = origin.MFTEntrySize;
	BYTE* sector = NULL;
	MFTEntry temp;
	ReadNTFSSectorByByte(drive, beginMFT, sector, MFTEntrySize);
	if (sector[0] == 0)
	{
		flag = false;
		return temp;
	}
	readMFTEntryHeader(sector, temp.header);

	uin32 paddingOffset = temp.header.firstAttributeOffset;
	BYTE* sectorCurr = sector + paddingOffset;
	uin32 remainSize = temp.header.MFTusedBytes - 56;
	temp.sizeAttribute = 0;
	while (remainSize > 8)
	{
		MFTEntryAttribute temp1;
		temp1.typeAttribute = 0;
		temp1.StandardInfo = NULL;

		readMFTEntryAttribute(sectorCurr, temp1);
		temp1.typeAttribute = hexToInt(temp1.header.attributeType);
		BYTE* readType = sectorCurr + hexToInt(temp1.header.AttributeOffset);
		if (temp1.typeAttribute == 16)
		{
			temp1.StandardInfo = new StandardInfomationAttribute();
			readMFTStandardInfo(readType, *(temp1.StandardInfo));

		}
		else
		{
			if (temp1.typeAttribute == 48)
			{
				temp1.Filename = new FilenameAtrtibute();
				readMFTFileName(readType, *(temp1.Filename));


			}
			else
			{
				if (temp1.header.attributeType.find("80") != string::npos)
				{
					string l;
					//printToASCCIIandHEX(sectorCurr, 0, 32);

					uin32 firstVCN = readPlace(sectorCurr, "10", 8);
					uin32 lastVCN = readPlace(sectorCurr, "18", 8);
					uin32 dataRunOffset = readPlace(sectorCurr, "1a", 2);
					uin32 dataSizeRemain = temp1.header.attributeSize - dataRunOffset;
					temp1.data = new DataAttribute();
					if (temp1.header.AttributeLength == unsigned int(0))
					{
						temp1.data->haveContentInEntry = false;
						readMFTData(sectorCurr + dataRunOffset, *(temp1.data), temp1.header.AttributeLength, lastVCN - firstVCN + 1, dataSizeRemain, temp1.header.AttributeOffset);


					}
					else
					{
						temp1.data->haveContentInEntry = true;
						readMFTData(readType, *(temp1.data), temp1.header.AttributeLength, lastVCN - firstVCN + 1, dataSizeRemain, temp1.header.AttributeOffset);
						temp1.data->contentSize = 2;

					}

					///readMFTFileName(readType, *(temp1.Filename));

				}
			}

		}

		temp.attribute.push_back(temp1);
		temp.sizeAttribute++;
		paddingOffset += temp1.header.attributeSize;
		sectorCurr = sector + paddingOffset;
		remainSize -= temp1.header.attributeSize;
	}

	return temp;

}

bool AddNTFSFileToTree(NTFSDirectoryFile& root, NTFSDirectoryFile& inp)
{
	if (root.idFile == inp.parrentId)
	{
		root.childFiles.push_back(inp);
		return true;
	}
	for (uin32 i = 0; i < root.childFiles.size(); i++)
	{
		if (AddNTFSFileToTree(root.childFiles[i], inp))
		{
			return true;
		}

	}
	return false;
}

void readNTFSInfo(LPCWSTR  drive, BYTE sector[512], NTFS& origin,
	MFT& fileMFT, NTFSDirectoryFile& root)
{

	origin.bytePerSectorString = "";
	origin.sectorPerClusterString = "";
	origin.DiskType = "";
	origin.sectorPerTrackString = "";
	origin.numberHeadString = "";
	origin.DiskSectorBeginString = "";
	origin.sectorDiskLogicString = "";
	origin.MFTClusterBeginString = "";
	origin.MFTBackupClusterBeginSring = "";
	origin.MFTEntrySizeString = "";
	origin.clusterOfIndexBufferString = "";
	origin.VolumnSerialNumber = "";


	origin.bytePerSector = readPlace(sector, "b", 2);
	origin.sectorPerCluster = readPlace(sector, "d", 1);
	readPlaceForString(sector, "15", 1, origin.DiskType);
	origin.sectorPerTrack = readPlace(sector, "18", 2);
	origin.numberHead = readPlace(sector, "1a", 2);
	origin.DiskSectorBegin = readPlace(sector, "1c", 4);
	origin.sectorDiskLogic = readPlace(sector, "28", 8);
	origin.MFTClusterBegin = readPlace(sector, "30", 8);
	origin.MFTBackupClusterBegin = readPlace(sector, "38", 8);
	string temp = "";
	readPlaceForString(sector, "40", 1, temp);
	int value = hexToChar(temp);
	origin.MFTEntrySize = (uin32)pow(2, abs(value));
	origin.clusterOfIndexBuffer = readPlace(sector, "44", 1);
	readPlace(sector, "48", 8);

	//read MFT
	uin32 count = 36;
	bool flag = true;
	vector<NTFSDirectoryFile>& list = root.childFiles;
	fileMFT.numberMFTEntry = 0;

	while (true)
	{


		MFTEntry entryAdd = readMFTEntry(drive, origin, count, flag);
		if (flag == false) break;
		fileMFT.arrayMFTEntry.push_back(entryAdd);
		fileMFT.numberMFTEntry++;

		count++;
	}

	for (uin32 k = 0; k < fileMFT.numberMFTEntry; k++)
	{
		NTFSDirectoryFile file;
		//	cout << "-------------- [Entry " << 36+k<< "]-----------------" << endl;
		//	DisplayMFTEntryHeader(fileMFT.arrayMFTEntry[k].header);
		file.idFile = fileMFT.arrayMFTEntry[k].header.idOfThisRecord;
		file.data = NULL;
		file.allowPrintData = false;

		MFTEntry temp = fileMFT.arrayMFTEntry[k];
		for (uin32 i = 0; i < temp.sizeAttribute; i++)
		{
			//	cout << endl << "------------- Header " << i << " ---------------" << endl;
			//	DisplayMFTEntryAttributeHeader(temp.attribute[i].header);

			if (temp.attribute[i].typeAttribute == 16)
			{
				//		DisplayMFTStandardInfo(*(temp.attribute[i].StandardInfo));
			}
			if (temp.attribute[i].typeAttribute == 48)
			{
				file.parrentId = temp.attribute[i].Filename->FatherMFTEntry;
				file.fileName = temp.attribute[i].Filename->filename;
				file.fileType = temp.attribute[i].Filename->flags;
				file.nameType = temp.attribute[i].Filename->nameType;

				if (k == 0)
				{
					root.idFile = temp.attribute[i].Filename->FatherMFTEntry;
				}

				if (temp.attribute[i].Filename->filename.find("txt") != string::npos ||
					temp.attribute[i].Filename->filename.find("sql") != string::npos)
				{
					file.allowPrintData = true;
				}
				//	DisplayMFTFileName(*(temp.attribute[i].Filename));
			}
			if (temp.attribute[i].header.attributeType.find("80") != string::npos)
			{
				file.data = (temp.attribute[i].data);
				if (file.data->haveContentInEntry)
				{
					file.data->FirstCluster = origin.MFTClusterBegin * origin.sectorPerCluster + (36 + k) * 2;
					file.data->contentSize = 2;
				}
				//DisplayMFTData(*(temp.attribute[i].data));
				//if (!temp.attribute[i].data->haveContentInEntry )
				//{
				//	if (allowPrintdata)
				//	{
				//	//	printFileNTFSData(drive, temp.attribute[i].data->clusterCount, temp.attribute[i].data->FirstCluster, origin);

				//	}
				//	else
				//	{
				//		cout << "\033[95m" << "------------------------" << endl;
				//		cout << "\033[95m" << "Use another program to read this file" << endl;
				//		cout << endl << "-----------------------" << "\033[0m" << endl;
				//	}
				//}
			}
		}
		if (!AddNTFSFileToTree(root, file))
		{
			root.childFiles.push_back(file);
		}



	}



	/*	FatOverView(drive, FAT, origin, FATsector);
			RDetOverView(drive, RDET, origin, Dir, FAT, beginRdet, sizeRdet);
			DirectoryView(Dir.childFiles, Dir.numberFile, Dir.currEntry, Dir.numberEntry);
			SDETView(drive, origin, Dir, FAT);*/

}

void displayBPBInfo(NTFS origin)
{
	system("cls");
	drawRect(1, 1, 62, 13, 1, 1);

	setxy(2, 2);
	cout << setw(20) << " " << "Partition Information" << endl;

	setxy(2, 4);
	cout << "\033[00m" << setw(8) << " " << "- Bytes per sector: " << origin.bytePerSector << " bytes" << endl;
	
	setxy(2, 5);
	cout << "\033[00m" << setw(8) << " " << "- Sectors per cluster (Sc): " << origin.sectorPerCluster << " sectors" << endl;
	
	setxy(2, 6);
	cout << "\033[00m" << setw(8) << " " << "- Sector per track : " << origin.sectorPerTrack << " sectors" << endl;

	setxy(2, 7);
	cout << "\033[00m" << setw(8) << " " << "- Number of heads : " << origin.numberHead << endl;

	setxy(2, 8);
	cout << "\033[00m" << setw(8) << " " << "- Logic Disk sector begin: " << origin.DiskSectorBegin << " sectors" << endl;

	setxy(2, 9);
	cout << "\033[00m" << setw(8) << " " << "- Logic Disk sector: " << origin.sectorDiskLogic << " sectors " << endl;

	setxy(2, 10);
	cout << "\033[00m" << setw(8) << " " << "- MFT cluster begin: cluster " << origin.MFTClusterBegin << endl;

	setxy(2, 11);
	cout << "\033[00m" << setw(8) << " " << "- MFT entry size: " << origin.MFTEntrySize << endl;

	setxy(2, 12);
	cout << "\033[00m" << setw(8) << " " << "- Index Buffer cluster: " << origin.clusterOfIndexBuffer << endl;

	setxy(2, 16);
	system("pause");
}

int ReadNTFSSectorByByte(LPCWSTR  drive, uin32 readPoint, BYTE*& sector, uin32 totalByteSector)
{

	sector = new BYTE[totalByteSector];
	LARGE_INTEGER scale;
	scale.QuadPart = readPoint;
	//cout << endl << scale.LowPart << "-" << scale.HighPart << endl;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		printf("CreateFile: %u\n", GetLastError());
		return 1;
	}
	SetFilePointer(device, scale.LowPart, &scale.HighPart, FILE_BEGIN);//Set a Point to Read


	//SetFilePointer(device, numberSector*sectorSize, NULL, FILE_CURRENT);//Set a Point to Read
	if (!ReadFile(device, sector, totalByteSector, &bytesRead, NULL))
	{
		printf("ReadFile: %u\n", GetLastError());
	}

}

void printFileNTFSData(LPCWSTR  drive, uin32 clusterSize, uin32 clusterBegin, NTFS origin)
{
	uin32 begin = clusterBegin * origin.sectorPerCluster * origin.bytePerSector;
	uin32 totalSector = clusterSize * origin.sectorPerCluster;

	BYTE* sector;
	ReadNTFSSectorByByte(drive, begin, sector, totalSector * origin.bytePerSector);
	//	printFileContent(sector, 0, totalSector * origin.bytePerSector);

}

uin32 getSizeNTFS(NTFSDirectoryFile input)
{
	uin32 total = 0;
	if (input.type.find("Folder") == string::npos)
		return input.fileSize;

	for (int i = 0; i < input.numberFile - 2; i++) {
		total += getSizeNTFS(input.childFiles[i]);
	}
	return total;
}

void displayNTFSDirFileInfo(NTFSDirectoryFile input, int numberFolder)
{
	drawRect(1, 1 + numberFolder * DISTANCE, WIDTH, HEIGHT, numberFolder * DISTANCE, numberFolder * DISTANCE);

	setxy(3, 2 + numberFolder * DISTANCE);
	wcout << numberFolder << ": " << "\033[96m" << input.name;

	setxy(10, 4 + numberFolder * DISTANCE);
	cout << "\033[0m" << "- Type: " << input.type;

	setxy(10, 5 + numberFolder * DISTANCE);
	cout << "\033[0m" << "- Begin cluster: " << input.beginCluster;

	setxy(10, 6 + numberFolder * DISTANCE);
	cout << "\033[0m" << "- File size: " << input.fileSize << " bytes";

	setxy(10, 7 + numberFolder * DISTANCE);
	cout << "\033[0m" << "- List sector: ";

	if (input.listSector.size() > 0)
		cout << "\033[0m" << input.listSector.at(0) << ", ... ," << input.listSector.at(input.listSector.size() - 1);
}

//void display(LPCWSTR  drive, NTFSDirectoryFile input, uin32 number, NTFS origin, bool flag)
//{
//	system("cls");
//
//	NTFSDirectoryFile folder;
//	if (!flag)
//		cout << setw(20) << " " << "ROOT DIRECTORY";
//	else
//		cout << setw(20) << " " << "DIRECTORY";
//
//	uin32 fileSize;
//	//Print files and folders in RDET
//
//	for (uin32 i = number - 1; i >= 0; i--)
//	{
//		fileSize = 0;
//		folder = input.childFiles[i];
//		//Folder size
//		if (folder.numberFile != 0) {
//			for (int j = 0; j < folder.numberFile - 2; j++) {
//				fileSize += getSizeNTFS(folder.childFiles[j]);
//			}
//			folder.fileSize = fileSize;
//		}
//
//		displayDirFile(folder, number - i - 1);
//		if (i == 0)
//			break;
//	}
//}

void displayNTFSDirectoryFile(LPCWSTR  drive, NTFSDirectoryFile temp, NTFS origin, int level)
{
	string padding = "";
	for (int i = 0; i < level; i++)
	{
		padding += "     ";
	}
	cout << endl;
	cout << padding << "File name: " << temp.fileName << endl;
	cout << padding << "- File type: " << temp.fileType << endl;
	cout << padding << "- File name type: " << temp.nameType << endl;

	if (temp.data == NULL)
	{
		return;
	}
	cout << padding << "- File sector: ";
	if (temp.data->haveContentInEntry)
	{

		cout << temp.data->FirstCluster << ", ... , " << temp.data->FirstCluster + temp.data->contentSize - 1;


	}
	else
	{

		cout << temp.data->FirstCluster * origin.sectorPerCluster << ", ... , " << temp.data->FirstCluster * origin.sectorPerCluster + (temp.data->clusterCount * origin.sectorPerCluster - 1);

	}
	cout << endl;
	cout << padding << "- File data: " << endl;

	if (!temp.data->haveContentInEntry)
	{
		if (temp.allowPrintData)
		{
			printFileNTFSData(drive, temp.data->clusterCount, temp.data->FirstCluster, origin);

		}
		else
		{
			cout << padding << "\033[95m" << "------------------------" << endl;
			cout << padding << "\033[95m" << "Use another program to read this file" << endl;
			cout << padding << "--------- EOF ------------" << "\033[0m" << endl;
		}
	}
	else
	{
		cout << padding << "\033[95m" << "------------------------" << endl;
		cout << padding << "\033[95m" << "     FILE CONTENT" << endl;
		cout << padding << "\033[95m" << "------------------------" << endl;
		cout << padding << "\033[95m" << temp.data->data << endl;
		cout << padding << "--------- EOF ------------" << "\033[0m" << endl;
	}
	cout << endl;
}

void displayNTFSFileTree(LPCWSTR  drive, NTFSDirectoryFile root, NTFS origin, int level)
{
	for (uin32 i = 0; i < root.childFiles.size(); i++)
	{
		NTFSDirectoryFile& temp = root.childFiles[i];
		displayNTFSDirectoryFile(drive, temp, origin, level);
		displayNTFSFileTree(drive, root.childFiles[i], origin, level + 1);
	}
}