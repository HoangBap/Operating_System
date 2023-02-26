#include "Menu.h"

MainMenu::MainMenu()
{

}

shared_ptr<MainMenu> MainMenu::instance()
{
    if (_instance == NULL) {
        auto temp = new MainMenu();
        _instance = shared_ptr<MainMenu>(temp);
    }
    return _instance;
}

void MainMenu::Print()
{
    system("cls");
    cout << "______________MENU______________\n";
    cout << "1. FAT32\n2. NFTS\n3. Exit\n";
    cout << "Enter your choice (1->3): ";
    int choice;
    cin >> choice;
    if (choice == 1)
    {
        auto FAT32_Menu_page = FAT32_Menu::instance();
        FAT32_Menu_page->Print();
    }
    else if (choice == 2)
    {
        auto NTFS_Menu_page = NTFS_Menu::instance();
        NTFS_Menu_page->Print();
    }
    else
    {
        return;
    }
}

FAT32_Menu::FAT32_Menu()
{

}

shared_ptr<FAT32_Menu> FAT32_Menu::instance()
{
    if (_instance == NULL) {
        auto temp = new FAT32_Menu();
        _instance = shared_ptr<FAT32_Menu>(temp);
    }
    return _instance;
}

void FAT32_Menu::Print()
{
    system("cls");
    cout << "______________MENU______________\n";
    cout << "1. Read boot sector\n2. Print RDET\n3. Exit\n";
    cout << "Enter your choice (1->3): ";
    int choice;
    cin >> choice;
    if (choice == 1);
    else if (choice == 2);
    else
    {
        auto MainMenu_page = MainMenu::instance();
        MainMenu_page->Print();
    }
}

NTFS_Menu::NTFS_Menu()
{

}

shared_ptr<NTFS_Menu> NTFS_Menu::instance()
{
    if (_instance == NULL) {
        auto temp = new NTFS_Menu();
        _instance = shared_ptr<NTFS_Menu>(temp);
    }
    return _instance;
}

void NTFS_Menu::Print()
{
    system("cls");
    cout << "______________MENU______________\n";
    cout << "1. Read boot sector\n2. Print RDET\n3. Exit\n";
    cout << "Enter your choice (1->3): ";
    int choice;
    cin >> choice;
    if (choice == 1);
    else if (choice == 2);
    else
    {
        auto MainMenu_page = MainMenu::instance();
        MainMenu_page->Print();
    }
}