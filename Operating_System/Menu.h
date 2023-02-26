#pragma once
#include<iostream>
using namespace std;

class MainMenu
{
private:
    inline static shared_ptr<MainMenu> _instance = NULL;
    MainMenu();
public:
    static shared_ptr<MainMenu> instance();
    void Print();
};

class FAT32_Menu
{
private:
    inline static shared_ptr<FAT32_Menu> _instance = NULL;
    FAT32_Menu();
public:
    static shared_ptr<FAT32_Menu> instance();
    void Print();
};

class NTFS_Menu
{
private:
    inline static shared_ptr<NTFS_Menu> _instance = NULL;
    NTFS_Menu();
public:
    static shared_ptr<NTFS_Menu> instance();
    void Print();
};

