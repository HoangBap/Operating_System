#include<iostream>
#include<sstream>
#include"Function.h"
#include"FAT32.h"
using namespace std;


void FATOverView(LPCWSTR  drive, unsigned int*& FAT, FAT32& origin, BYTE*& sector)
{
	//cout << endl;	
	//cout << "----------FAT-----------" << endl; 
	ull readPoint = origin.bytePerSector * origin.sectorBootsector;
	ull totalFATByte = origin.numFAT * origin.bytePerSector;
	//sector = NULL;

	FAT = new unsigned int[totalFATByte / 4];
	readSectorByByte(drive, readPoint, sector, totalFATByte);
	for (ull i = 0; i < totalFATByte / 4; i++)
	{
		unsigned int a = 0;
		for (int j = 3; j >= 0; j--)
		{
			a <<= 8;
			unsigned int temp = (unsigned int)sector[4 * i + j];
			a += temp;
		}

		FAT[i] = a;
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

	readPoint = origin.sectorBootsector + origin.numFAT * origin.sizeFAT * origin.bytePerSector;
	totalByteSector = clusterContain.size() * origin.sectorPerCluster * origin.bytePerSector;
	delete[] sector;

	readSectorByByte(drive, readPoint, sector, totalByteSector);
	entrySplitView(sector, totalByteSector, Dir);
}

void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir)
{
	Dir.numberEntry = 0;
	ull i = 0;
	while (true)
	{
		BYTE c = sector[i];
		string status = intToHex(byteToInt(c));
		if (status.find("e5") == string::npos && status.find("00") == string::npos)
		{

			Dir.numberEntry++;
		}

		if (status.find("00") != string::npos)
		{
			break;
		}
		i += 32;
	}

	Dir.currEntry = new Entry[Dir.numberEntry];
	ull cnt = 0;
	i = 0;
	while (cnt < Dir.numberEntry)
	{
		BYTE c = sector[i];
		string status = intToHex(byteToInt(c));
		if (status.find("e5") == string::npos && status.find("00") == string::npos)
		{

			Dir.currEntry[cnt].status = c;
			Dir.currEntry[cnt].additionEntry = byteToInt(sector[i + 11]) != 15 ? false : true;
			Dir.currEntry[cnt].type = sector[i + 11];



			for (int l = 0; l < 32; l++)
			{
				Dir.currEntry[cnt].information[l] = sector[i + l];
			}
			cnt++;
		}
		i += 32;
	}
	//DisplayListEntry(Dir.currEntry, Dir.numberEntry);

}



string FileCategory(BYTE value)
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
			case 3: res += "Vollable "; break;
			case 4: res += "Folder "; break;
			case 5: res += "Achive "; break;

			}
		}
		type++;
		value >>= 1;
	}

	return res;
}

void readMainEntry(Entry c, DirectoryFile& res)
{

	res.numberFile = 0;
	res.childFiles = NULL;

	for (int i = 0; i < 8; i++)  // get file name;
	{

		res.name += char(c.information[i]);
	}

	for (int i = 0; i < 3; i++)  // get extension name;
	{
		res.extension += char(c.information[i + 8]);
	}

	res.type = FileCategory(c.type);
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

void viewDirectory(DirectoryFile*& Dir, ull& numberFile, const Entry* listEntry, const ull& countEntry)
{
	ull cnt = 0;
	numberFile = 0;

	while (cnt < countEntry)
	{
		if (!listEntry[cnt].additionEntry)
		{
			numberFile++;
		}
		cnt++;
	}

	Dir = new DirectoryFile[numberFile];
	ull DirCnt = 0;
	cnt = countEntry - 1;
	ull lastState = cnt;
	wstring tempNameFile = L"";
	while (countEntry > cnt)
	{
		Entry c = listEntry[cnt];
		if (!c.additionEntry)
		{
			if (lastState - cnt > 1)
			{
				Dir[DirCnt - 1].name = tempNameFile;
				tempNameFile = L"";
			}
			readMainEntry(c, Dir[DirCnt]);

			DirCnt++;

			lastState = cnt;
		}
		else
		{
			tempNameFile += readAdditionEntry(c);
		}
		cnt--;
	}
	if (tempNameFile.length() != 0)
	{
		Dir[DirCnt - 1].name = tempNameFile;
	}
	//DisplayDirFile(Dir,numberFile);
}

void directoryView(DirectoryFile*& Dir, ull& numberFile, const Entry* listEntry, const ull& countEntry)
{
	ull cnt = 0;
	numberFile = 0;

	while (cnt < countEntry)
	{
		if (!listEntry[cnt].additionEntry)
		{
			numberFile++;
		}
		cnt++;
	}

	Dir = new DirectoryFile[numberFile];
	ull DirCnt = 0;
	cnt = countEntry - 1;
	ull lastState = cnt;
	wstring tempNameFile = L"";
	while (countEntry > cnt)
	{
		Entry c = listEntry[cnt];
		if (!c.additionEntry)
		{
			if (lastState - cnt > 1)
			{
				Dir[DirCnt - 1].name = tempNameFile;
				tempNameFile = L"";
			}
			readMainEntry(c, Dir[DirCnt]);

			DirCnt++;

			lastState = cnt;
		}
		else
		{
			tempNameFile += readAdditionEntry(c);
		}
		cnt--;
	}
	if (tempNameFile.length() != 0)
	{
		Dir[DirCnt - 1].name = tempNameFile;
	}
	//DisplayDirFile(Dir,numberFile);
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

		ull CurrentSector = input.bytePerSector + input.numFAT * input.sizeFAT
			+ c.beginCluster * input.sectorPerCluster - input.clusterBeginOfRDET * input.sectorPerCluster;
		ull readPoint = CurrentSector * input.bytePerSector;
		ull totalByteSector = clusterContain.size() * input.sectorPerCluster * input.bytePerSector;
		BYTE* sector = NULL;
		for (ull i = 0; i < clusterContain.size(); i++)
		{
			for (ull k = 0; k < input.sectorPerCluster; k++)
			{
				c.listSector.push_back(CurrentSector + i * input.sectorPerCluster + k);

			}

		}

		readSectorByByte(drive, readPoint, sector, totalByteSector);

		if (c.type.find("Folder") != string::npos)
		{
			entrySplitView(sector, totalByteSector, c);
			directoryView(c.childFiles, c.numberFile, c.currEntry, c.numberEntry);
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

void readFAT32Info(LPCWSTR  drive, BYTE sector[512], FAT32& origin,
	BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet)
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
	directoryView(Dir.childFiles, Dir.numberFile, Dir.currEntry, Dir.numberEntry);
	SDETView(drive, origin, Dir, FAT);
}


