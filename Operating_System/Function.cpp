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

//char hexToChar(string value)
//{
//
//    char res = 0;
//    for (int i = 0; i < value.length(); i++)
//    {
//        res *= 16;
//        if (value[i] >= '0' && value[i] <= '9')
//        {
//            res += (value[i] - '0');
//        }
//        else if ((tolower(value[i])) >= 'a' && (tolower(value[i]) <= 'f'))
//        {
//            res += (tolower(value[i]) - 'a' + 10);
//        }
//    }
//
//    return res;
//}

char hexToChar(string value)
{

    char res = 0;
    for (int i = 0; i < value.length(); i++)
    {
        res *= 16;
        switch (value[i])
        {
        case '0':res += 0; break;
        case '1':res += 1; break;
        case '2':res += 2; break;
        case '3':res += 3; break;
        case '4':res += 4; break;
        case '5':res += 5; break;
        case '6':res += 6; break;
        case '7':res += 7; break;
        case '8':res += 8; break;
        case '9':res += 9; break;
        case 'a':res += 10; break;
        case 'b':res += 11; break;
        case 'c':res += 12; break;
        case 'd':res += 13; break;
        case 'e':res += 14; break;
        case 'f':res += 15; break;
        default:
            res += 0;
        }
    }

    return res;
}

void printToASCCIIandHEX(BYTE sector[], uin32 begin, uin32 n)
{
    uin32 temp = begin + n;
    int count = 0;
    while (temp > 0)
    {
        count++; 
        temp /= 10;
    }

    cout << setw(count + 3) << " ";
    for (int i = 0; i < 16; i++)
        cout << setw(2) << intToHex(i) << " ";
    
    cout << endl;

    for (uin32 i = 0; i < n; i++)
    {
        if (0 == i % 16)
        {
            if (i != 0)
            {
                for (uin32 j = i - 16; j < i; j++)
                {
                    BYTE c = sector[j];
                    printf("%c", char(c) != '\n' ? (c == 0 ? '.' : c) : '.');

                }
            }
            cout << endl;
            cout << setw(count + 2) << setfill('0') << intToHex(i + begin) << " ";
        }

        BYTE b = sector[i];
        cout << setw(2) << fixed << intToHex(byteToInt(b)) << " ";
    }
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

void readPlaceForString(BYTE sector[512], string addr, uin32 sizeByte, string& byteString)
{
    uin32 beginOffset = hexToInt(addr);
    byteString = "";
    for (uin32 i = sizeByte - 1; i >= 0; i--)
    {
        BYTE b = sector[beginOffset + i];
        byteString += intToHex(byteToInt(b));
        if (i == 0)
            break;
    }
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



