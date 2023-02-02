#include"win.h"
#include<windows.h>

int color(int a)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);    //更改文字颜色 
	return 0;
}