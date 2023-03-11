#include<string>

#include"UIcontrol.h"
#include"Menu.h"
#include"Function.h"
#include"FAT32.h"
#include"NTFS.h"
using namespace std;

void printNTFSMenu(LPCWSTR  drive, BYTE sector[512])
{
    NTFS origin;
    NTFSDirectoryFile* root = new NTFSDirectoryFile();
    MFT fileMFT;

    readNTFSInfo(drive, sector, origin, fileMFT, *root);

    string choice;
    while (true) {
        //Vẽ bảng cho nó đẹp
        system("cls");
        drawRect(1, 1, 62, 9, 1, 1);

        setxy(2, 2);
        cout << setw(20) << " " << "NTFS File System" << endl;

        setxy(2, 4);
        cout << "\033[00m" << setw(8) << " " << "1. Partition Boot Sector Information";

        setxy(2, 5);
        cout << "\033[00m" << setw(8) << " " << "2. Root Directory Information";

        setxy(2, 6);
        cout << "\033[00m" << setw(8) << " " << "3. Back to main menu";

        setxy(2, 8);
        cout << "\033[00m" << setw(8) << " " << "Enter your choice (1->3): ";
        getline(cin, choice);

        //Xét lựa chọn
        switch (choice[0])
        {
        case '1': //Hiển thị thông tin của một phân vùng người dùng đã chọn
        {
            //Hàm hiển thị thông tin phân vùng
            displayBPBInfo(origin);
            system("pause");
            break;
        }
        case '2': //Hiển thị cây thư mục
        {
            system("cls");
            //Hiển thị thông tin các tập tin/thư mục trong cây thư mục 
            printNTFSDirectory(drive, *root, origin, root->numberFile, 0);
            system("pause");
            break;
        }
        //Thoat khoi menu FAT32
        case '3':
            return;

        default:
            break;
        }
    }
}


void printFAT32Menu(LPCWSTR  drive, BYTE sector[512])
{
    FAT32 origin;
    DirectoryFile* root = new DirectoryFile();

    unsigned int* FAT = NULL;
    BYTE* FATsector = NULL;
    BYTE* RDET;
    uin32 beginRdet;
    uin32 rdetSize;
    readFAT32Info(drive, sector, origin, RDET, *root, FAT, FATsector, beginRdet, rdetSize);

    int choice;
    while (true) {
        //Vẽ bảng cho nó đẹp
        system("cls");
        drawRect(1, 1, 62, 9, 1, 1);

        setxy(2, 2);
        cout << setw(20) << " " << "FAT32 File System" << endl;

        setxy(2, 4);
        cout << "\033[00m" << setw(8) << " " << "1. Bootsector Information";
        
        setxy(2, 5);
        cout << "\033[00m" << setw(8) << " " << "2. RDET Information";
        
        setxy(2, 6);
        cout << "\033[00m" << setw(8) << " " << "3. Back to main menu";
        
        setxy(2, 8);
        cout << "\033[00m" << setw(8) << " " << "Enter your choice (1->3): ";
        cin >> choice;

        //Xét lựa chọn
        switch (choice)
        {
        case 1: //Hiển thị thông tin của một phân vùng người dùng đã chọn
        {
            //Hàm hiển thị thông tin phân vùng
            displayFAT32BootSectorInfo(origin);
            break;
        }
        case 2: //Hiển thị cây thư mục
        {
            system("cls");
            //Hiển thị thông tin các tập tin/thư mục trong cây thư mục 
            printDirectory(drive, *root, root->numberFile, FAT, origin, 0);
            system("pause");
            break;
        }
        //Thoat khoi menu FAT32
        case 3:
            return;

        default:
            break;
        }
    }
}

void printMainMenu()
{
    while (true) {
        system("cls");
        string USB;
        
        drawRect(1, 1, 62, 9, 1, 1);

        setxy(2, 3);
        cout << setw(20) << " " << "WELCOME TO OUR PROGRAM" << endl;
       
        setxy(2, 6);
        cout << "\033[00m" << setw(8) << " " << "Enter USB label name or type \"q\" to Quit : ";
        cin >> USB;

        if (USB == "q")
            break;

        wstring convert(USB.begin(), USB.end());
        wstring path = L"\\\\.\\" + convert + L":";
        BYTE sector[512];
        readSector(path.c_str(), 0, sector);

        if (isFAT32(sector))
            printFAT32Menu(path.c_str(), sector);

        else if (isNTFS(sector))
            printNTFSMenu(path.c_str(), sector);

        else
            cout << "Sorry our program can only read FAT32 or NTFS :'(" << endl;
    }
    
    system("cls");
    cout << "Program is shutting down....." << endl;
}
