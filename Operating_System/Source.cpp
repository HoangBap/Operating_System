#include"FAT32.h"
#include"Menu.h"
int main()
{
	/*cout << ConvertHexToDec("3FE0");
	string** Table = new string * [16];
	for (int i = 0; i < 16; i++)
	{
		Table[i] = new string[16];
	}
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			Table[i] = new string[16];
		}
	}*/
	auto MainMenu_page = MainMenu::instance();
	MainMenu_page->Print();
}