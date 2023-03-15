#include"NTFS.h"
#include"UIControl.h"

bool isNTFS(BYTE sector[512])
{
	//printToASCCIIandHEX(sector, 0, 512);
	string NTFSType = "";
	int beginOffset = hexToInt("3");

	for (int i = 0; i < 8; i++)
	{
		BYTE b = sector[beginOffset + i];
		NTFSType += isascii(b) ? b : '.';
	}

	if (NTFSType.find("NTFS") != string::npos)
		return true;

	return false;
}

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
			case 0: 
				res += "Compressed "; 
				break;

			case 14:
				res += "Encrypted "; 
				break;

			case 15: 
				res += "Sparse "; 
				break;
			default:
				break;
			}
		}
		type++;
		value >>= 1;
	}
	return res;
}
void readMFTEntryAttribute(BYTE* sector, MFTEntryAttribute& attribute)
{
	MFTEntryAttributeHeader& header = attribute.header;
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
			case 0: 
				res += "Read Only "; 
				break;

			case 1:
				res += "Hidden "; 
				break;

			case 2: 
				res += "System "; 
				break;

			case 5: 
				res += "Archive "; 
				break;

			case 6:
				res += "Device "; 
				break;

			case 7: 
				res += "Normal "; 
				break;

			case 8: 
				res += "Temporary "; 
				break;

			case 9: 
				res += "Sparse "; 
				break;

			case 10: 
				res += "Resparese "; 
				break;

			case 11: 
				res += "Compressed "; 
				break;

			case 12: 
				res += "Offline "; 
				break;

			case 13: 
				res += "Not Content "; 
				break;

			case 14: 
				res += "Encrypted "; 
				break;

			case 28: 
				res += "Folder "; 
				break;

			case 29: 
				res += "Index View "; 
				break;

			default: 
				break;

			}
		}
		type++;
		value >>= 1;
	}

	return res;
}
void readMFTFileName(BYTE* sector, FilenameAtrtibute& standard)
{
	standard.FatherMFTEntry = readPlace(sector, "0", 8);
	standard.flags = attributeType(hexToInt(standard.flags));
	readPlaceForString(sector, "38", 4, standard.flags);
	standard.nameLength = readPlace(sector, "40", 1);
	readPlaceForString(sector, "41", 1, standard.nameType);
	readPlaceForString(sector, "42", standard.nameLength * 2, standard.filename);
	string name = "";
	for (uin32 i = standard.nameLength * 2 - 1; i > 0; i--)
	{
		string temping = standard.filename.substr(2 * i, 2);
		if (temping.find("00") == string::npos)
			name += char(hexToInt(temping));
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
	readSectorByByte(drive, beginMFT, sector, MFTEntrySize);
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
			return true;
	}

	return false;
}

void displayNTFSDirFileInfo(NTFSDirectoryFile folder, NTFS origin, uin32 numberFile)
{
	drawRect(1, 1 + numberFile * DISTANCE, WIDTH, HEIGHT + 1, numberFile * DISTANCE, numberFile * DISTANCE);

	setxy(3, 2 + numberFile * DISTANCE);
	cout << numberFile + 1 << ": " << "\033[96m" << folder.fileName;

	setxy(10, 4 + numberFile * DISTANCE);
	cout << "\033[0m" << "- File type: " << folder.fileType;

	setxy(10, 5 + numberFile * DISTANCE);
	cout << "\033[0m" << "- File name type: " << folder.nameType;

	setxy(10, 6 + numberFile * DISTANCE);
	cout << "\033[0m" << "- File size: " << folder.fileSize << " bytes";

	if (folder.data == NULL)
		return;

	setxy(10, 7 + numberFile * DISTANCE);
	if (folder.data->haveContentInEntry) 
		cout << "\033[0m" << "- List sector: " << folder.data->FirstCluster << ", ... , " << folder.data->FirstCluster + folder.data->contentSize - 1;
	
	else 
		cout << "\033[0m" << "- List sector: " << folder.data->FirstCluster * origin.sectorPerCluster << ", ... , " << folder.data->FirstCluster * origin.sectorPerCluster + (folder.data->clusterCount * origin.sectorPerCluster - 1);
	
}

uin32 getNTFSSize(NTFSDirectoryFile input, NTFS origin) {
	uin32 total = 0;
	if (input.fileType.find("Folder") == string::npos) {
		if (input.data->haveContentInEntry)
			return input.data->data.size();

		else
			return input.data->clusterCount * origin.sectorPerCluster * origin.bytePerSector;
	}
	else {
		for (uin32 i = 0; i < input.childFiles.size(); i++) {
			total += getNTFSSize(input.childFiles[i], origin);
		}
	}
	return total;
}

void printNTFSDirectory(LPCWSTR  drive, NTFSDirectoryFile root, NTFS origin, uin32 numberFile, bool flag)
{
	system("cls");


	uin32 choice;
	NTFSDirectoryFile folder;
	if (!flag) {
		cout << setw(20) << " " << "NTFS ROOT DIRECTORY";
		for (uin32 i = numberFile - 1; i >= 0; i--)
		{
			folder = root.childFiles[i];
			//Nếu tập tin là tập tin
			if (folder.fileType.find("Folder") == string::npos)
				folder.fileSize = getNTFSSize(folder, origin);

			//Nếu tập tin là thư mục
			for (uin32 j = 0; j < folder.childFiles.size(); j++)
				folder.fileSize = getNTFSSize(folder.childFiles[j], origin);

			displayNTFSDirFileInfo(folder, origin, numberFile - i - 1);
			if (i == 0)
				break;
		}

		setxy(0, numberFile * DISTANCE + 2);
		cout << " - Enter the number corresponding to the file to see more information" << endl <<
			" - Enter the unrelated key to QUIT: " << endl << "  Choice: ";
		cin >> choice;
		choice -= 1;
		if (choice > numberFile)
			return;

		choice = numberFile - 1 - choice;
	} 

	else {
		cout << setw(20) << " " << "DIRECTORY";
		for (uin32 i = 0; i < numberFile; i++)
		{
			folder = root.childFiles[i];
			//Nếu tập tin là tập tin
			if (folder.fileType.find("Folder") == string::npos)
				folder.fileSize = getNTFSSize(folder, origin);

			//Nếu tập tin là thư mục
			for (uin32 j = 0; j < folder.childFiles.size(); j++)
				folder.fileSize = getNTFSSize(folder.childFiles[j], origin);

			displayNTFSDirFileInfo(folder, origin, i);
		}

		setxy(0, numberFile * DISTANCE + 2);
		cout << " - Enter the number corresponding to the file to see more information" << endl <<
			" - Enter the unrelated key to QUIT: " << endl << "  Choice: ";
		cin >> choice;
		choice -= 1;
		if (choice > numberFile)
			return;
	}

	if (root.childFiles[choice].fileType.find("Folder") != string::npos)
		printNTFSDirectory(drive, root.childFiles[choice], origin, root.childFiles[choice].childFiles.size(), true);

	else if (!root.childFiles[choice].data->haveContentInEntry)
	{
		system("cls");
		cout << "\033[96m" << endl << "Please use an approriate program to read this file" << endl;
		return;
	}
	//In file text
	else {
		system("cls");
		cout << "\033[96m" << setw(20) << " " << "FILE TEXT CONTENT" << endl << endl;
		cout << "\033[00m" << root.childFiles[choice].data->data;
		cout << endl << endl << "\033[96m" << setw(20) << " " << "END OF FILE" << endl;
	}
	cout << endl;
}

void displayBPBInfo(NTFS origin)
{
	system("cls");
	drawRect(1, 1, 62, 14, 1, 1);

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

	setxy(2, 13);
	cout << "\033[00m" << setw(8) << " " << "- VolumeSerialNumber: " << origin.VolumnSerialNumber << endl;

	setxy(2, 16);
	system("pause");
}
void readNTFSInfo(LPCWSTR  drive, BYTE sector[512], NTFS& origin, MFT& fileMFT, NTFSDirectoryFile& root)
{
	origin.bytePerSector = readPlace(sector, "b", 2);
	origin.sectorPerCluster = readPlace(sector, "d", 1);
	readPlaceForString(sector, "15", 1, origin.DiskType);
	origin.sectorPerTrack = readPlace(sector, "18", 2);
	origin.numberHead = readPlace(sector, "1a", 2);
	origin.DiskSectorBegin = readPlace(sector, "1c", 4);
	origin.sectorDiskLogic = readPlace(sector, "28", 8);
	origin.MFTClusterBegin = readPlace(sector, "30", 8);
	origin.MFTBackupClusterBegin = readPlace(sector, "38", 8);
	string MFTEntrySizeString;
	readPlaceForString(sector, "40", 1, MFTEntrySizeString);
	origin.MFTEntrySize = (uin32)pow(2, abs(hexToChar(MFTEntrySizeString)));

	string clusterOfIndexBufferString;
	readPlaceForString(sector, "44", 1, clusterOfIndexBufferString);
	origin.clusterOfIndexBuffer = hexToChar(clusterOfIndexBufferString);
	readPlaceForString(sector, "48", 8, origin.VolumnSerialNumber);
	transform(origin.VolumnSerialNumber.begin(), origin.VolumnSerialNumber.end(), origin.VolumnSerialNumber.begin(), ::toupper);

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

		file.idFile = fileMFT.arrayMFTEntry[k].header.idOfThisRecord;
		file.data = NULL;
		file.allowPrintData = false;
		
		MFTEntry temp = fileMFT.arrayMFTEntry[k];
		for (uin32 i = 0; i < temp.sizeAttribute; i++)
		{
			if (temp.attribute[i].typeAttribute == 48)
			{
				file.parrentId = temp.attribute[i].Filename->FatherMFTEntry;
				file.fileName = temp.attribute[i].Filename->filename;

				//flag ban đầu mang giá trị hexa nên cần biến đổi thành giá trị thập phân để phân định thuộc tính thông qua hàm attributeType 
				file.fileType = attributeType(hexToInt(temp.attribute[i].Filename->flags));
				file.nameType = temp.attribute[i].Filename->nameType;

				if (k == 0)
					root.idFile = temp.attribute[i].Filename->FatherMFTEntry;

				if (temp.attribute[i].Filename->filename.find("txt") != string::npos)
					file.allowPrintData = true;
			}

			if (temp.attribute[i].header.attributeType.find("80") != string::npos)
			{
				file.data = (temp.attribute[i].data);
				if (file.data->haveContentInEntry)
				{
					file.data->FirstCluster = origin.MFTClusterBegin * origin.sectorPerCluster + (36 + k) * 2;
					file.data->contentSize = 2;
				}
			}
		}
		if (!AddNTFSFileToTree(root, file))
		{
			root.childFiles.push_back(file);
		}
	}
}