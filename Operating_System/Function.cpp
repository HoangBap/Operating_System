#include"Function.h"
string IntToHex(ull value)
{
    stringstream ss;
    ss << hex << value;
    string res(ss.str());
    if (res.length() != 2)
    {
        res = "0" + res;
    }

    return res;
}

ull ByteToInt(BYTE value)
{
    ull res = ull((unsigned char)(value));
    return res;

}

ull HexToInt(string value)
{
    ull res = 0;
    for (int i = 0; i < value.length(); i++)
    {
        res *= 16;
        if (value[i] >= '0' && value[i] <= '9')
        {
            res += (ull)(value[i] - '0');
        }
        else if ((tolower(value[i])) >= 'a' && (tolower(value[i]) <= 'f'))
        {
            res += (ull)(tolower(value[i]) - 'a' + 10);
        }
    }
    return res;
}

ull readPlace(BYTE sector[512], string addr, ull sizeByte)
{
    ull res = 0;
    ull beginOffset = HexToInt(addr);
    string byteString = "";

    for (sizeByte -= 1; sizeByte >= 0; sizeByte--)
    {
        BYTE b = sector[beginOffset + sizeByte];
        byteString += IntToHex(ByteToInt(b));
        if (sizeByte == 0)
            break;
    }

    res = HexToInt(byteString);
    return res;
}

int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
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

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Success!\n");
    }
}

void readSectorByByte(LPCWSTR  drive, ull readPoint, BYTE*& sector, ull totalByteSector)
{
    if (sector != NULL)
        delete[] sector;

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
        return;
    }
    SetFilePointer(device, scale.LowPart, &scale.HighPart, FILE_BEGIN);//Set a Point to Read

    //SetFilePointer(device, numberSector*sectorSize, NULL, FILE_CURRENT);//Set a Point to Read
    if (!ReadFile(device, sector, totalByteSector, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
}

bool isFAT32(BYTE sector[512]) {
    string FATType = "";

    for (int i = 0; i < 8; i++)
    {
        BYTE b = sector[0x52 + i];
        FATType += isascii(b) ? b : '.';
    }

    if (FATType.find("FAT32") != string::npos)
        return true;

    return false;
}

void FATOverView(LPCWSTR  drive, unsigned int*& FAT, FAT32& origin, BYTE*& sector)
{
    //cout << endl;	
    //cout << "----------FAT-----------" << endl; 
    ull readPoint = origin.bytePerSector() * origin.sectorBootsector();
    ull totalFATByte = origin.numFAT() * origin.bytePerSector();
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

void entrySplitView(BYTE*& sector, ull sizeSector, DirectoryFile& Dir)
{
    Dir.numberEntry = 0;
    ull i = 0;
    while (true)
    {
        BYTE c = sector[i];
        string status = IntToHex(ByteToInt(c));

        if (status.find("e5") == string::npos && status.find("00") == string::npos)
            Dir.numberEntry++;

        if (status.find("00") != string::npos)
            break;

        i += 32;
    }

    Dir.currEntry = new Entry[Dir.numberEntry];
    ull cnt = 0;
    i = 0;
    while (cnt < Dir.numberEntry)
    {
        BYTE c = sector[i];
        string status = IntToHex(ByteToInt(c));
        if (status.find("e5") == string::npos && status.find("00") == string::npos)
        {

            Dir.currEntry[cnt].status = c;
            Dir.currEntry[cnt].additionEntry = ByteToInt(sector[i + 11]) != 15 ? false : true;
            Dir.currEntry[cnt].type = sector[i + 11];

            for (int l = 0; l < 32; l++)
            {
                Dir.currEntry[cnt].information[l] = sector[i + l];
            }
            cnt++;
        }
        i += 32;
    }
}

void RDETOverView(LPCWSTR  drive, BYTE*& sector, FAT32& origin, DirectoryFile& Dir, unsigned int* FAT, ull& readPoint, ull& totalByteSector)
{
    vector<ull> clusterContain;
    clusterContain.push_back(origin.clusterBeginRDET());

    unsigned int field = FAT[clusterContain[clusterContain.size() - 1]];

    while (field != HexToInt("0fffffff"))
    {
        clusterContain.push_back(field);
        field = FAT[clusterContain[clusterContain.size() - 1]];
    }

    readPoint = origin.sectorBootsector() + origin.numFAT() * origin.sizeFAT() * origin.bytePerSector();
    totalByteSector = clusterContain.size() * origin.sectorPerCluster() * origin.bytePerSector();
    delete[] sector;

    readSectorByByte(drive, readPoint, sector, totalByteSector);
    entrySplitView(sector, totalByteSector, Dir);
}

void readFAT32Info(LPCWSTR drive, BYTE sector[512], FAT32& origin, BYTE*& RDET, DirectoryFile& Dir, unsigned int*& FAT, BYTE*& FATsector, ull& beginRdet, ull& sizeRdet)
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

    origin.setBytePerSector(readPlace(sector, "B", 2));
    origin.setSectorPerCluster(readPlace(sector, "D", 1));
    origin.setSectorBootSector(readPlace(sector, "E", 2));
    origin.setNumFAT(readPlace(sector, "10", 1));
    origin.setVolumeSize(readPlace(sector, "20", 4));
    origin.setSizeFAT(readPlace(sector, "24", 4));
    origin.setClusterBeginRDET(readPlace(sector, "2C", 4));
    origin.setBootSectorSaveSector(readPlace(sector, "32", 2));
    origin.setAdditionalInfoSector(readPlace(sector, "30", 2));

    FATOverView(drive, FAT, origin, FATsector);
    RDETOverView(drive, RDET, origin, Dir, FAT, beginRdet, sizeRdet);
    //DirectoryView(Dir.childFiles, Dir.numberFile, Dir.currEntry, Dir.numberEntry);
    //SDETView(drive, origin, Dir, FAT);
}

void displayFAT32(LPCWSTR drive, BYTE sector[512])
{
    FAT32 origin;
    DirectoryFile* Root = new DirectoryFile();

    unsigned int* FAT = NULL;

    BYTE* FATsector = NULL;
    BYTE* RDET;

    ull beginRDET;
    ull sizeRDET;

    readFAT32Info(drive, sector, origin, RDET, *Root, FAT, FATsector, beginRDET, sizeRDET);

    /*   DisplayBootSectorInfo(origin);

       cout << endl;
       cout << "-------Directory--------" << endl;
       PrintDir(drive, *Root, Root->numberFile, 0, FAT, origin);*/

}
