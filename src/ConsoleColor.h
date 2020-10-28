#pragma once

#include <windows.h>

inline const char* Color(int color = 7, const char* Message = "")
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	return Message;
}

