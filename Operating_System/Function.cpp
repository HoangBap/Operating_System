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

bool IsFAT32(BYTE sector[512])
{

    string FatType = "";
    int beginOffet = HexToInt("52");
    for (int i = 0; i < 8; i++)
    {
        BYTE b = sector[beginOffet + i];

        FatType += isascii(b) ? b : '.';
    }

    if (FatType.find("FAT32") != string::npos)
    {
        return true;
    }
    return false;
}