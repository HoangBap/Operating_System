#pragma once
#include"Function.h"
using namespace std;

//class FAT32_Menu
//{
//private:
//    inline static shared_ptr<FAT32_Menu> _instance = NULL;
//    FAT32_Menu();
//public:
//    static shared_ptr<FAT32_Menu> instance();
//    void Print();
//};
//
//class NTFS_Menu
//{
//private:
//    inline static shared_ptr<NTFS_Menu> _instance = NULL;
//    NTFS_Menu();
//public:
//    static shared_ptr<NTFS_Menu> instance();
//    void Print();
//};

void printMainMenu();
void printFAT32Menu(LPCWSTR  drive, BYTE sector[512]);