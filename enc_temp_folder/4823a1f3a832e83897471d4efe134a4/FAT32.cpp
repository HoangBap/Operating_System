#include<iostream>
#include<sstream>
#include<iomanip>
#include<algorithm>

#include"Function.h"
#include"FAT32.h"
using namespace std;

void displayBootSectorInfo(FAT32 origin)
{
	cout << "____________FAT32 Information____________" << endl;
	cout << "Bytes per sector: " << origin.bytePerSector << " bytes" << endl;
	cout << "Sectors per cluster (Sc): " << origin.sectorPerCluster << " sectors" << endl;
	cout << "Sectors boot sector (Sb) : " << origin.sectorBootsector << " sectors" << endl;
	cout << "Number of FAT (nF): " << origin.numFAT << endl;
	cout << "Size of one FAT (Sf): " << origin.sizeFAT << " sectors" << endl;
	cout << "Volume size (Sv): " << origin.volumeSize << " sectors " << endl;
	cout << "RDET&Data cluster begin: cluster " << origin.clusterBeginOfRDET << endl;
	cout << "Boot sector save sector : sector " << origin.bootSectorSaveSector << endl;
	cout << "------------------------------------------" << endl;

	system("pause");
}

void printFileTextContent(BYTE sector[], ull begin, ull n)
{
	cout << "\033[96m" << "-------------------------------------------------" << endl;
	cout << "\033[96m" << "		FILE TEXT CONTENT" << endl;
	cout << "\033[96m" << "-------------------------------------------------" << endl;
	ull temp = begin;

	for (ull i = 0; i < n; i++)
	{
		BYTE b = sector[i];
		char character = isascii(b) ? b : '.';
		cout << "\033[0m" << character;
	}

	cout << endl << "\033[96m"  << "---------------- END OF FILE ------------------" << "\033[0m" << endl;
}

void displayDirFile(DirectoryFile input, int level)
{
	int spaceing = 4 * level;
	wcout << spaceW(spaceing) << "\033[0m" << "- File name: " << input.name;

	if (input.fileSize != 0)
	{
		cout << "\033[0m" << "." << input.extension;
	}
	cout << "\033[0m" << endl;

	cout << space(spaceing) << "\033[0m" << "- Type: " << input.type << endl;
	cout << space(spaceing) << "\033[0m" << "- Begin cluster: " << input.beginCluster << endl;
	cout << space(spaceing) << "\033[0m" << "- File size: " << input.fileSize << " bytes" << endl;
	cout << space(spaceing) << "\033[0m" << "- List sector: ";

	if (input.listSector.size() > 0)
		cout << "\033[0m" << input.listSector.at(0) << ", ... ," << input.listSector.at(input.listSector.size() - 1) << endl;
	cout << space(spaceing) << "\033[0m" << endl;
}

void printRDET(LPCWSTR  drive, DirectoryFile inp, ull number, int level, unsigned int* FAT, FAT32 origin)
{
	if (inp.type.find("Folder") != string::npos)
	{
		for (ull i = 0; i < number; i++)
		{
			DirectoryFile c = inp.childFiles[i];
			displayDirFile(c, level);
			if (c.name == L"." || c.name == L"..")
				continue;

			printRDET(drive, c, c.numberFile, level + 1, FAT, origin);
		}
	}

	else
	{
		string temp = inp.extension;
		transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

		if (temp.find("txt") == string::npos && temp.find("sql") == string::npos)
		{
			cout << "\033[96m" << "------------------------" << endl;
			cout << "\033[96m" << "Please use an appropriate program to read this file" << endl;
			cout << endl << "-----------------------" << endl;
			return;
		}

		vector<ull> clusterContain;
		clusterContain.push_back(inp.beginCluster);
		unsigned int field = FAT[clusterContain.at(clusterContain.size() - 1)];

		while (field != hexToInt("0fffffff"))
		{
			clusterContain.push_back(field);
			if (clusterContain[clusterContain.size() - 1] > (origin.numFAT * origin.bytePerSector / 4))
				break;
			field = FAT[clusterContain[clusterContain.size() - 1]];
		}

		ull CurrentSector = origin.sectorBootsector + origin.numFAT * origin.sizeFAT + inp.beginCluster * origin.sectorPerCluster - origin.clusterBeginOfRDET * origin.sectorPerCluster;
		ull readPoint = CurrentSector * origin.bytePerSector;
		ull totalByteSector = clusterContain.size() * origin.sectorPerCluster * origin.bytePerSector;
		BYTE* sector = NULL;

		readSectorByByte(drive, readPoint, sector, totalByteSector);
		printFileTextContent(sector, readPoint, totalByteSector);
		cout << endl;
	}
}

void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir)
{
	Dir.numberEntry = 0;
	ull i = 0;
	while (true)
	{
		BYTE c = sector[i];
		string status = intToHex(byteToInt(c));
		//Check Entry status (00h: blank entry, e5h: deleted entry)
		if (status.find("e5") == string::npos && status.find("00") == string::npos)
			Dir.numberEntry++;

		//File/Folder information is stored in this entry
		if (status.find("00") != string::npos)
			break;

		//Size of bytes per Entry: 32 bytes
		i += 32;
	}

	Dir.currEntry = new Entry[Dir.numberEntry];

	ull count = 0;
	i = 0;
	while (count < Dir.numberEntry)
	{
		BYTE c = sector[i];
		string status = intToHex(byteToInt(c));
		if (status.find("e5") == string::npos && status.find("00") == string::npos)
		{
			Dir.currEntry[count].status = c;
			Dir.currEntry[count].additionEntry = byteToInt(sector[i + 11]) != 15 ? false : true;
			Dir.currEntry[count].type = sector[i + 11];

			for (int l = 0; l < 32; l++)
			{
				Dir.currEntry[count].information[l] = sector[i + l];
			}
			count++;
		}
		i += 32;
	}
	//DisplayListEntry(Dir.currEntry, Dir.numberEntry);
}

string fileCategory(BYTE value)
{
	unsigned short type = 0;
	string attribute = "";

	while (value > 0)
	{
		if (value % 2 == 1)
		{
			switch (type)
			{
			case 0: attribute += "Read Only "; break;
			case 1: attribute += "Hidden "; break;
			case 2: attribute += "System "; break;
			case 3: attribute += "Vollable "; break;
			case 4: attribute += "Folder "; break;
			case 5: attribute += "Archive "; break;
			}
		}
		type++;
		value >>= 1;
	}
	return attribute;
}

void readMainEntry(Entry c, DirectoryFile& res)
{
	res.numberFile = 0;
	res.childFiles = NULL;
	for (int i = 0; i < 8; i++)  // get file name;
		res.name += char(c.information[i]);

	for (int i = 0; i < 3; i++)  // get extension name;
		res.extension += char(c.information[i + 8]);

	res.type = fileCategory(c.type);
	res.beginCluster = 0;
	res.beginCluster += (int)c.information[1 * 16 + 4];
	res.beginCluster <<= 8;
	res.beginCluster += (int)c.information[1 * 16 + 10];

	if (res.type == "Folder")
	{
		res.fileSize = 0;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			res.fileSize <<= 8;
			res.fileSize += c.information[31 - i];
		}
	}
	trimWstring(res.name);
}

wstring readAdditionEntry(Entry c)
{
	wstring res = L"";

	int offset = 1;
	for (int i = 0; i < 5; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];
		if (number == 0 || number == hexToInt("ff"))
		{
			break;
		}
		wchar_t element = wchar_t(number);
		res += element;
	}

	offset = hexToInt("e");
	for (int i = 0; i < 6; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];
		if (number == 0 || number == hexToInt("ff"))
		{
			break;
		}
		wchar_t element = wchar_t(number);
		res += element;
	}

	offset = hexToInt("1c");
	for (int i = 0; i < 2; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];
		if (number == 0 || number == hexToInt("ff"))
		{
			break;
		}
		wchar_t element = wchar_t(number);
		res += element;
	}
	return res;
}

void FATOverView(LPCWSTR  drive, unsigned int*& FAT, FAT32& origin, BYTE*& sector)
{
	ull readPoint = origin.bytePerSector * origin.sectorBootsector;
	ull totalFATByte = origin.numFAT * origin.bytePerSector;

	//Create a FAT table for future use
	FAT = new unsigned int[totalFATByte / 4];
	readSectorByByte(drive, readPoint, sector, totalFATByte);

	for (ull i = 0; i < totalFATByte / 4; i++)
	{
		unsigned int a = 0;
		for (int j = 3; j >= 0; j--)
		{
			a <<= 8; // a = a * 2^(8)
			unsigned int temp = (unsigned int) sector[4 * i + j];
			a += temp;
		}
		FAT[i] = a;
	}

}

void viewDirectory(DirectoryFile*& Dir, ull& numberFile, const Entry* listEntry, const ull& countEntry)
{
	ull count = 0;
	numberFile = 0;

	while (count < countEntry)
	{
		if (!listEntry[count].additionEntry)
			numberFile++;
		count++;
	}
	
	if (Dir != NULL)
		delete[] Dir;
	Dir = new DirectoryFile[numberFile];

	ull DirCnt = 0;
	count = countEntry - 1;
	ull lastState = count;

	wstring tempNameFile = L"";
	while (countEntry > count)
	{
		Entry c = listEntry[count];
		if (!c.additionEntry)
		{
			if (lastState - count > 1)
			{
				Dir[DirCnt - 1].name = tempNameFile;
				tempNameFile = L"";
			}
			readMainEntry(c, Dir[DirCnt]);
			DirCnt++;
			lastState = count;
		}
		else
		{
			tempNameFile += readAdditionEntry(c);
		}
		count--;
	}
	if (tempNameFile.length() != 0)
	{
		Dir[DirCnt - 1].name = tempNameFile;
	}
}

void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector)
{
	vector<ull> clusterContain;
	clusterContain.push_back(origin.clusterBeginOfRDET);

	unsigned int field = FAT[clusterContain[clusterContain.size() - 1]];
	while (field != hexToInt("0fffffff"))
	{
		clusterContain.push_back(field);
		field = FAT[clusterContain[clusterContain.size() - 1]];
	}

	readPoint = (origin.sectorBootsector + origin.numFAT * origin.sizeFAT) * origin.bytePerSector;
	totalByteSector = clusterContain.size() * origin.sectorPerCluster * origin.bytePerSector;
	delete[] sector;

	readSectorByByte(drive, readPoint, sector, totalByteSector);
	entrySplitView(sector, totalByteSector, Dir);
}

void SDETView(LPCWSTR  drive, const FAT32& input, DirectoryFile& Dir, unsigned int*& fat)
{
	ull cnt = Dir.numberFile - 1;
	while (true)
	{
		if (cnt > Dir.numberFile) break;
		DirectoryFile& c = Dir.childFiles[cnt];
		c.fatherFiles = &Dir;
		if (c.name == L"." || c.name == L"..")
		{
			cnt--;
			if (cnt > Dir.numberFile) break;
			continue;
		}

		vector<ull> clusterContain;

		clusterContain.push_back(c.beginCluster);
		unsigned int field = fat[clusterContain.at(clusterContain.size() - 1)];
		while (field != hexToInt("0fffffff"))
		{
			clusterContain.push_back(field);
			if (clusterContain[clusterContain.size() - 1] > (input.numFAT * input.bytePerSector / 4))
				break;
			field = fat[clusterContain[clusterContain.size() - 1]];
		}

		ull CurrentSector = input.sectorBootsector + input.numFAT * input.sizeFAT + c.beginCluster * input.sectorPerCluster - input.clusterBeginOfRDET * input.sectorPerCluster;
		ull readPoint = CurrentSector * input.bytePerSector;
		ull totalByteSector = clusterContain.size() * input.sectorPerCluster * input.bytePerSector;
		BYTE* sector = NULL;
		for (ull i = 0; i < clusterContain.size(); i++)
		{
			for (ull k = 0; k < input.sectorPerCluster; k++)
				c.listSector.push_back(CurrentSector + i * input.sectorPerCluster + k);
		}

		readSectorByByte(drive, readPoint, sector, totalByteSector);

		if (c.type.find("Folder") != string::npos)
		{
			entrySplitView(sector, totalByteSector, c);
			viewDirectory(c.childFiles, c.numberFile, c.currEntry, c.numberEntry);
			SDETView(drive, input, c, fat);

		}
		else
		{
			//printFileContent(sector, readPoint, totalByteSector);
		}
		//if (cnt == 0) break;
		cnt--;
	}


}

void readFAT32Info(LPCWSTR  drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet)
{
	RDET = NULL;

	Dir.type = "Folder";
	Dir.name = wstring(drive);
	Dir.extension = "RDET";
	Dir.beginCluster = 2;
	Dir.fileSize = 0;
	Dir.childFiles = NULL;
	Dir.numberFile = 0;
	Dir.fatherFiles = NULL;

	origin.bytePerSector = readPlace(sector, "b", 2);
	origin.sectorPerCluster = readPlace(sector, "d", 1);
	origin.sectorBootsector = readPlace(sector, "e", 2);
	origin.numFAT = readPlace(sector, "10", 1);
	origin.volumeSize = readPlace(sector, "20", 4);
	origin.sizeFAT = readPlace(sector, "24", 4);
	origin.clusterBeginOfRDET = readPlace(sector, "2c", 4);
	origin.bootSectorSaveSector = readPlace(sector, "32", 2);
	origin.additionalInfoSector = readPlace(sector, "30", 2);

	FATOverView(drive, FAT, origin, FATsector);
	RDETOverView(drive, RDET, origin, Dir, FAT, beginRdet, sizeRdet);
	viewDirectory(Dir.childFiles, Dir.numberFile, Dir.currEntry, Dir.numberEntry);
	SDETView(drive, origin, Dir, FAT);
}


