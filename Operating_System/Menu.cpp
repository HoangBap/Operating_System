#include<string>

#include"Menu.h"
#include"FAT32.h"
#include"Function.h"
using namespace std;

void printFAT32Menu(LPCWSTR  drive, BYTE sector[512])
{
    FAT32 origin;
    DirectoryFile* Root = new DirectoryFile();

    unsigned int* FAT = NULL;
    BYTE* FATsector = NULL;
    BYTE* RDET;
    ull beginRdet;
    ull rdetSize;
    readFAT32Info(drive, sector, origin, RDET, *Root, FAT, FATsector, beginRdet, rdetSize);

    system("cls");
    cout << "______________FAT32______________\n";
    cout << "1. Bootsector Information\n2. RDET Information\n3. Exit\n";
    cout << "Enter your choice (1->3): ";

    int choice;
    cin >> choice;
    while (true) {
        switch (choice)
        {
        case 1:
        {
            //    PrintInfo();
         //   DisplayFatInfo(BYTE * sector, ull readPoint, ull totalByteSector, unsigned int* FAT);
         //   displayFAT32BootSectorInfo();
            break;
        }
        case 2:
        {
            void PrintDir(LPCWSTR  drive, DirectoryFile inp, ull number, int level,
                unsigned int* FAT, FAT32 origin);
            break;
        }
        //Thoat khoi menu FAT32
        case 3:
            return;

        default:
        {
            cout << "Please choose your choice again...!" << endl;
            break;
        }
        }
    }
}

void printMainMenu()
{
    while (true) {
        system("cls");
        string USB;

        cout << "_______________________WELCOME TO  OUR PROGRAM________________________\n";
        cout << "Enter USB label name or press<q> to Exit: " << endl;
        cin >> USB;

        if (USB == "q")
            break;

        cout << "Program is running..." << endl;
        wstring convert(USB.begin(), USB.end());
        wstring path = L"\\\\.\\" + convert + L":";
        BYTE sector[512];
        readSector(path.c_str(), 0, sector);

        if (isFAT32(sector))
        {
            cout << "File system: FAT32" << endl;
            printFAT32Menu(path.c_str(), sector);
        }
        /*else
        {
        cout << "File system: NTFS" << end;;
        printNTFSMenu(path.c_str(), sector);
        }*/
    }

    cout << "Program is shutting down..." << endl;
}
