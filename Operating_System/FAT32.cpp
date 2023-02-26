#include"FAT32.h"

int ConvertHexToDec(string HexValue)
{
	int res = 0;
	for (size_t i = 0; i < HexValue.size(); i++)
	{
		int value;
		if (HexValue[i] >= '0' && HexValue[i] <= '9') value = HexValue[i] - '0';
		else value = HexValue[i] - 'A' + 10;
		res += value * pow(16, HexValue.size() - i - 1);
	}
	return res;
}

string Read(string** Table, int x, int y, int NumberOfBytes)
{
	string res = "", temp;
	for (int i = 0; i < NumberOfBytes; i++)
	{
		res = Table[0][x * 16 + y + i] + res;
	}
	return res;
}
//FAT32_File::FAT32_File(string Input)
//{
//	string** Table = new string * [16];
//	for (int i = 0; i < 16; i++)
//	{
//		Table[i] = new string[16];
//	}
//}