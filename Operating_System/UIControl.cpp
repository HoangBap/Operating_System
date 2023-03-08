#include"UIControl.h"

BOOL setxy(int x, int y)
{
	COORD c = { x,y };
	return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawRect(int x, int y, int width, int height, int curPosX, int curPosY)
{
	setxy(x, y); 
	cout << "\033[96m" << char(201);

	for (int i = 1; i < width; i++)
		cout << "\033[96m" << char(205);

	cout << "\033[96m" << char(187);
	setxy(x, height + y); 
	cout << char(200);

	for (int i = 1; i < width; i++)
		cout << "\033[96m" << char(205);

	cout << "\033[96m" << char(188);
	for (int i = y + 1; i < height + y; i++)
	{
		setxy(x, i); cout << "\033[96m" << char(186);
		setxy(x + width, i); cout << "\033[96m" << char(186);
	}

	setxy(curPosX, curPosY);
}

string space(int input)
{
	string character = "";
	for (int i = 0; i < input; i++)
		character += " ";

	return character;
}

wstring spaceW(int input)
{
	wstring character = L"";
	for (int i = 0; i < input; i++)
		character += L" ";

	return character;
}