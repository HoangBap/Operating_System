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

string space(int input)
{
    string character = "";
    for (int i = 0; i < input; i++)
        character += " ";
  
    return character;
}
wstring spaceW(int input)
{
    wstring character = L"";
    for (int i = 0; i < input; i++)
        character += L" ";
    
    return character;
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
            break;
       
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

    else
        cout << "Success!\n";
}

void readSectorByByte(LPCWSTR  drive, ull readPoint, BYTE*& sector, ull totalByteSector)
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