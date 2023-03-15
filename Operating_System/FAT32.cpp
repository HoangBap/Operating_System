#include"UIControl.h"
#include"Function.h"
#include"FAT32.h"

using namespace std;

bool isFAT32(BYTE sector[512]) {
	string FATType = "";

	for (int i = 0; i < 8; i++)
	{
		BYTE b = sector[0x52 + i];
		FATType += isascii(b) ? b : '.';
	}

	//File system is FAT32
	if (FATType.find("FAT32") != string::npos)
		return true;

	return false;
}

void displayFAT32BootSectorInfo(FAT32 origin)
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
	cout << "\033[00m" << setw(8) << " " << "- Sectors boot sector (Sb) : " << origin.sectorBootsector << " sectors" << endl;
	
	setxy(2, 7);
	cout << "\033[00m" << setw(8) << " " << "- Number of FAT (nF): " << origin.numFAT << endl;
	
	setxy(2, 8);
	cout << "\033[00m" << setw(8) << " " << "- Size of one FAT (Sf): " << origin.sizeFAT << " sectors" << endl;
	
	setxy(2, 9);
	cout << "\033[00m" << setw(8) << " " << "- Volume size (Sv): " << origin.volumeSize << " sectors " << endl;
	
	setxy(2, 10);
	cout << "\033[00m" << setw(8) << " " << "- RDET&Data cluster begin: cluster " << origin.clusterBeginOfRDET << endl;
	
	setxy(2, 11);
	cout << "\033[00m" << setw(8) << " " << "- Boot sector save sector : sector " << origin.bootSectorSaveSector << endl;

	setxy(2, 15);
	system("pause");
}

void printFileTextContent(BYTE sector[], uin32 begin, uin32 n)
{
	system("cls");
	cout << "\033[96m" << setw(20) << " " << "FILE TEXT CONTENT" << endl << endl;
	uin32 temp = begin;

	for (uin32 i = 0; i < n; i++)
	{
		BYTE b = sector[i];
		char character = isascii(b) ? b : '.';
		cout << "\033[0m" << character;
	}

	cout << endl << endl << "\033[96m" << setw(20) << " " << "END OF FILE" << endl;
}

void displayDirFile(DirectoryFile input, uin32 numberFolder)
{
	drawRect(1, 1 + numberFolder*DISTANCE, WIDTH, HEIGHT, numberFolder*DISTANCE, numberFolder*DISTANCE);

	setxy(3, 2 + numberFolder*DISTANCE);
	wcout << numberFolder << ": " << "\033[96m" << input.name;
	if (input.type.find("Folder") == string::npos && input.name.find(L".") == string::npos) {
		transform(input.extension.begin(), input.extension.end(), input.extension.begin(), ::tolower);
		cout << "\033[96m" << "." << input.extension;
	}

	setxy(10, 4 + numberFolder*DISTANCE);
	cout << "\033[0m" << "- Type: " << input.type;

	setxy(10, 5 + numberFolder*DISTANCE);
	cout <<  "\033[0m" << "- Begin cluster: " << input.beginCluster;

	setxy(10, 6 + numberFolder*DISTANCE);
	cout << "\033[0m" << "- File size: " << input.fileSize << " bytes";

	setxy(10, 7 + numberFolder*DISTANCE);
	cout << "\033[0m" << "- List sector: ";
	if (input.listSector.size() > 0)
		cout << "\033[0m" << input.listSector.at(0) << ", ... ," << input.listSector.at(input.listSector.size() - 1) << endl;
}

uin32 getSizeFAT32(DirectoryFile input) {
	uin32 total = 0;
	if (input.type.find("Folder") == string::npos)
		return input.fileSize;

	for (uin32 i = 0; i < input.numberFile - 2; i++) {
		total += getSizeFAT32(input.childFiles[i]);
	}
	return total;
}

void printDirectory(LPCWSTR  drive, DirectoryFile input, uin32 number, unsigned int* FAT, FAT32 origin, bool flag)
{
	system("cls");

	DirectoryFile folder;
	if (!flag)
		cout << setw(20) << " " << "ROOT DIRECTORY";
	else
		cout << setw(20) << " " << "DIRECTORY";

	uin32 fileSize;
	//Print files and folders in RDET
		
	for (uin32 i = number - 1; i >= 0; i--)
	{
		fileSize = 0;
		folder = input.childFiles[i];

		//Folder size
		if (folder.numberFile != 0) {
			for (int j = 0; j < folder.numberFile - 2; j++) {
				fileSize += getSizeFAT32(folder.childFiles[j]);
			}
			folder.fileSize = fileSize;
		}
		
		displayDirFile(folder, number - i - 1);
		if (i == 0) 
			break;
	}

	//Cho người dùng muốn truy xuất thông tin của tập tin/thư mục trong cây thư mục
	setxy(0, number * DISTANCE);
	uin32 choice;
	cout << "- Enter the number corresponding to the file to see more information" << endl;
	cout << "- Enter the unrelated key to QUIT: " << endl;
	cout << "  Choice: ";
	cin >> choice;
	if (choice > input.numberFile)
		return;

	choice = number - 1 - choice;
	//Nếu người dùng muốn truy xuất thông tin của thư mục thì sẽ in ra cây thư mục con bên trong thư mục đó
	if(input.childFiles[choice].type.find("Folder") != string::npos)
		printDirectory(drive, input.childFiles[choice], input.childFiles[choice].numberFile, FAT, origin, 1);

	//Nếu người dùng muốn truy xuất nội dung thông tin của tập tin
	else
	{
		string temp = input.childFiles[choice].extension;
		transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

		//Nếu không phải file text 
		if (temp.find("txt") == string::npos)
		{
			system("cls");
			cout << "\033[96m" << endl << "Please use an approriate program to read this file" << endl;
			return;
		}

		vector<uin32> clusterContain;

		clusterContain.push_back(input.beginCluster);
		unsigned int field = FAT[clusterContain.at(clusterContain.size() - 1)];
		while (field != hexToInt("0fffffff"))
		{
			clusterContain.push_back(field);
			if (clusterContain[clusterContain.size() - 1] > (origin.numFAT * origin.bytePerSector / 4))
				break;
			field = FAT[clusterContain[clusterContain.size() - 1]];
		}


		uin32 CurrentSector = origin.sectorBootsector + origin.numFAT * origin.sizeFAT + input.childFiles[choice].beginCluster * origin.sectorPerCluster - origin.clusterBeginOfRDET * origin.sectorPerCluster;
		uin32 readPoint = CurrentSector * origin.bytePerSector;
		uin32 totalByteSector = clusterContain.size() * origin.sectorPerCluster * origin.bytePerSector;
		BYTE* sector = NULL;

		readSectorByByte(drive, readPoint, sector, totalByteSector);
		printFileTextContent(sector, readPoint, totalByteSector);
		cout << endl;
	}
}

void entrySplitView(BYTE*& sector, uin32 sizeSector, DirectoryFile& Dir)
{
	Dir.numberEntry = 0;
	uin32 i = 0;
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
	uin32 count = 0;
	i = 0;

	//Store information of each entry (Each entry has 32 bytes)
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
				Dir.currEntry[count].information[l] = sector[i + l];
			
			count++;
		}
		i += 32;
	}
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
	//Read begin cluster 
	res.beginCluster += (int)c.information[1 * 16 + 4];//Low-word Part (Offset 14)
	res.beginCluster <<= 8;
	res.beginCluster += (int)c.information[1 * 16 + 10];//High-word Part (Offset 1A)

	
	if (res.type == "Folder")
		res.fileSize = 0;
	//Field size (Offset 1C: 4 bytes)
	else
		for (int i = 0; i < 4; i++)
		{
			res.fileSize <<= 8;
			res.fileSize += c.information[31 - i];
		}

	trimWstring(res.name);
}

wstring readAdditionEntry(Entry c)
{
	wstring string = L"";

	int offset = 1;
	for (int i = 0; i < 5; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];

		if (number == 0 || number == hexToInt("ff"))
			break;
		
		wchar_t element = wchar_t(number);
		string += element;
	}

	offset = hexToInt("e");
	for (int i = 0; i < 6; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];

		if (number == 0 || number == hexToInt("ff"))
			break;
		
		wchar_t element = wchar_t(number);
		string += element;
	}

	offset = hexToInt("1c");
	for (int i = 0; i < 2; i++)
	{
		unsigned short number = 0 + (unsigned short)c.information[offset + 2 * i + 1];
		number <<= 8;
		number = 0 + (unsigned short)c.information[offset + 2 * i];

		if (number == 0 || number == hexToInt("ff"))
			break;

		wchar_t element = wchar_t(number);
		string += element;
	}
	return string;
}

void FATOverView(LPCWSTR  drive, unsigned int*& FAT, FAT32& origin, BYTE*& sector)
{
	uin32 readPoint = origin.bytePerSector * origin.sectorBootsector;
	uin32 totalFATByte = origin.numFAT * origin.bytePerSector;

	//Create a FAT table for future use
	FAT = new unsigned int[totalFATByte / 4];
	readSectorByByte(drive, readPoint, sector, totalFATByte);

	//Một phần tử trong bảng FAT32 chiếm 4 byte
	for (uin32 i = 0; i < totalFATByte / 4; i++)
	{
		unsigned int a = 0;
		for (int j = 3; j >= 0; j--)
		{
			a <<= 8; // a = a * 2^(8)
			unsigned int temp = (unsigned int)sector[4 * i + j];
			a += temp;
		}
		FAT[i] = a; //Giá trị trong phần tử i của bảng FAT
	}
}

void viewDirectory(DirectoryFile*& Dir, uin32& numberFile, const Entry* listEntry, const uin32& countEntry)
{
	uin32 count = 0;
	numberFile = 0;

	//Đếm số tập tin/thư mục 
	while (count < countEntry)
	{
		if (!listEntry[count].additionEntry)
			numberFile++;
		count++;
	}
	
	if (Dir != NULL)
		delete[] Dir;
	Dir = new DirectoryFile[numberFile];

	uin32 DirCnt = 0;
	count = countEntry - 1;
	uin32 lastState = count;
	wstring tempNameFile = L"";

	while (countEntry > count)
	{
		Entry c = listEntry[count];
		//Nếu như chỉ có entry chính 
		if (!c.additionEntry)
		{
			if (lastState - count > 1)
			{
				Dir[DirCnt - 1].name = tempNameFile;
				tempNameFile = L"";
			}
			//Đọc entry chính 
			readMainEntry(c, Dir[DirCnt]);
			DirCnt++;
			lastState = count;
		}
		//Nếu có entry phụ
		else
			tempNameFile += readAdditionEntry(c);
		count--;
	}
	//Ghép entry phụ và chính với nhau để tạo được tên của tập tin/thư mục (nếu có)
	if (tempNameFile.length() != 0)
		Dir[DirCnt - 1].name = tempNameFile;
}

void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, uin32& readPoint, uin32& totalByteSector)
{
	vector<uin32> clusterContain;
	clusterContain.push_back(origin.clusterBeginOfRDET);
	unsigned int field = FAT[clusterContain[clusterContain.size() - 1]];
	
	while (field != hexToInt("0fffffff"))
	{
		clusterContain.push_back(field);
		field = FAT[clusterContain[clusterContain.size() - 1]];
	}

	readPoint = (origin.sectorBootsector + origin.numFAT * origin.sizeFAT) * origin.bytePerSector;
	totalByteSector = clusterContain.size() * origin.sectorPerCluster * origin.bytePerSector;

	readSectorByByte(drive, readPoint, sector, totalByteSector);
	entrySplitView(sector, totalByteSector, Dir);
}

void SDETView(LPCWSTR  drive, const FAT32& input, DirectoryFile& Dir, unsigned int*& FAT)
{
	uin32 count = Dir.numberFile - 1;
	while (true)
	{
		//Because count does not have negative number so when it will get back to
		if (count > Dir.numberFile) 
			break;

		DirectoryFile& c = Dir.childFiles[count];
		c.fatherFiles = &Dir;

		if (c.name == L"." || c.name == L"..")
		{
			count--;
			if (count > Dir.numberFile) 
				break;

			continue;
		}

		vector<uin32> clusterContain;
		clusterContain.push_back(c.beginCluster);
		unsigned int field = FAT[clusterContain.at(clusterContain.size() - 1)];

		while (field != hexToInt("0fffffff"))
		{
			clusterContain.push_back(field);
			if (clusterContain[clusterContain.size() - 1] > (input.numFAT * input.bytePerSector / 4))
				break;

			field = FAT[clusterContain[clusterContain.size() - 1]];
		}

		uin32 CurrentSector = input.sectorBootsector + input.numFAT * input.sizeFAT + c.beginCluster * input.sectorPerCluster - input.clusterBeginOfRDET * input.sectorPerCluster;
		uin32 readPoint = CurrentSector * input.bytePerSector;
		uin32 totalByteSector = clusterContain.size() * input.sectorPerCluster * input.bytePerSector;
		BYTE* sector = NULL;

		for (uin32 i = 0; i < clusterContain.size(); i++)
		{
			for (uin32 k = 0; k < input.sectorPerCluster; k++)
				c.listSector.push_back(CurrentSector + i * input.sectorPerCluster + k);
		}

		readSectorByByte(drive, readPoint, sector, totalByteSector);

		if (c.type.find("Folder") != string::npos)
		{
			entrySplitView(sector, totalByteSector, c);
			viewDirectory(c.childFiles, c.numberFile, c.currEntry, c.numberEntry);
			SDETView(drive, input, c, FAT);
		}
		count--;
	}
}

void readFAT32Info(LPCWSTR  drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, uin32& beginRdet, uin32& sizeRdet)
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

	//Lấy các thông tin của phân vùng trong Bootsector
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


