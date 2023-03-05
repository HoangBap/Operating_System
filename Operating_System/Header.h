#pragma once
#include<iostream>
#include<string>
#include"Function.h"
#define ull uint32_t
using namespace std;

class Object
{
public:

};

class FAT32_File
{
private:
	ull bytePerSector;
	ull Sc;
	ull Sb;
	ull Nf;
	ull Sv;
	ull Sf;
	ull RDETClusterBegin;
	ull BootSectorSaveSector;
	ull AdditionalInfoSector;
public:
	FAT32_File(BYTE sector[512]);
	void PrintFAT32Info();
};

int ConvertHexToDec(string);

string Read(string** Table, int x, int y, int NumberOfBytes);