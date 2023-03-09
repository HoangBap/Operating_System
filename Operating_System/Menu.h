#pragma once
#include"Function.h"
using namespace std;

void printMainMenu();
void printFAT32Menu(LPCWSTR  drive, BYTE sector[512]);
void printNTFSMenu(LPCWSTR drive, BYTE sector[512]);