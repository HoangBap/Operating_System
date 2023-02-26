#pragma once
#include<iostream>
#include<string>
using namespace std;

class Object
{
public:

};

class FAT32_File
{
public:
	int Sc, Sb, Sv, Sf, Sr, Nf;
	int BytesPerSector;
	FAT32_File(string);
	int FirstSectorOfFAT1();
	int FirstSectorOfRDET();
	int FirstSectorOfData();
};

int ConvertHexToDec(string);

string Read(string** Table, int x, int y, int NumberOfBytes);