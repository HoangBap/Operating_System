#include"Function.h"
string intToHex(uin32 value)
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

uin32 byteToInt(BYTE value)
{
    uin32 res = uin32((unsigned char)(value));
    return res;

}

uin32 hexToInt(string value)
{
    uin32 res = 0;
    for (int i = 0; i < value.length(); i++)
    {
        res *= 16;
        if (value[i] >= '0' && value[i] <= '9')
        {
            res += (uin32)(value[i] - '0');
        }
        else if ((tolower(value[i])) >= 'a' && (tolower(value[i]) <= 'f'))
        {
            res += (uin32)(tolower(value[i]) - 'a' + 10);
        }
    }
    return res;
}

void trimWstring(wstring& input)
{
    for (int i = input.length() - 1; i > 0; i--)
    {
        if (input[i] != L' ')
            break;
       
        else
        {
            input[i] = L'\0';
            input.resize(input.length() - 1);
        }

    }
}

uin32 readPlace(BYTE sector[512], string addr, uin32 sizeByte)
{
    uin32 res = 0;
    uin32 beginOffset = hexToInt(addr);
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

void readSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,  // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        cout << "CreateFile: " << GetLastError();
        return;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
        cout << "ReadFile: \n" << GetLastError();
}

void readSectorByByte(LPCWSTR  drive, uin32 readPoint, BYTE*& sector, uin32 totalByteSector)
{
    //Clear out the data stored in sector
    if (sector != NULL)
        delete[] sector;

    sector = new BYTE[totalByteSector];
    
    LARGE_INTEGER scale;
    scale.QuadPart = readPoint;
 
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,  // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        cout << "CreateFile: %u\n" << GetLastError();
        return;
    }

    SetFilePointer(device, scale.LowPart, &scale.HighPart, FILE_BEGIN);

    if (!ReadFile(device, sector, totalByteSector, &bytesRead, NULL))
        cout << "ReadFile: \n" << GetLastError();
}

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