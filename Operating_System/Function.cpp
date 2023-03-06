#include"Function.h"
string intToHex(ull value)
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

ull byteToInt(BYTE value)
{
    ull res = ull((unsigned char)(value));
    return res;

}

ull hexToInt(string value)
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

void trimWstring(wstring& input)
{
    for (int i = input.length() - 1; i > 0; i--)
    {
        if (input[i] != L' ')
        {
            break;
        }
        else
        {
            input[i] = L'\0';
            input.resize(input.length() - 1);
        }

    }
}

ull readPlace(BYTE sector[512], string addr, ull sizeByte)
{
    ull res = 0;
    ull beginOffset = hexToInt(addr);
    string byteString = "";

    for (sizeByte -= 1; sizeByte >= 0; sizeByte--)
    {
        BYTE b = sector[beginOffset + sizeByte];
        byteString += intToHex(byteToInt(b));
        if (sizeByte == 0)
            break;
    }

    res = hexToInt(byteString);
    return res;
}

int readSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
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
