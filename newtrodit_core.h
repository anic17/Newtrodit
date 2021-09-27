/*
	Newtrodit: A console text editor
  Copyright (C) 2021  anic17 Software

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>

*/

#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctype.h>
#include <direct.h>
#include <tchar.h>
#include <wchar.h>
#include "dialog.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#define XSIZE GetConsoleXDimension()
#define YSIZE GetConsoleYDimension()

#define BOTTOM GetConsoleYDimension() - 1

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008

#define BUFFER_X 1024
#define BUFFER_Y 1024

const char newtrodit_version[] = "0.4";
const char newtrodit_build_date[] = "27/9/2021";
char manual_file[_MAX_PATH] = "newtrodit.man";
char settings_file[_MAX_PATH] = "newtrodit.config";

const char newtrodit_commit[] = ""; // Example commit

char newlinestring[100] = "\n";

const int MANUAL_BUFFER_X = 300;
const int MANUAL_BUFFER_Y = 300;



const int TAB_WIDE_ = 7; // Standard length of the tab key is 8 characters. This has to be 7 because an index of 1 is always added.
int TAB_WIDE = 7;

int convertTabtoSpaces = true;

const int CURSIZE_ = 20;
int CURSIZE = 20;

int bg_color = 0x07;
int fg_color = 0x70;

const int LINECOUNT_WIDE_ = 3; // To backup original value
int LINECOUNT_WIDE = 3;		   // Not const

const char reset_color[] = "\e[0m";

const char filename_text_[FILENAME_MAX] = "Untitled"; // To backup original value
char filename_text[FILENAME_MAX] = "Untitled";


void StartProcess(char *command_line)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pinf;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pinf, sizeof(pinf));
	CreateProcess(NULL, command_line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pinf);
	return;
}

int CheckKey(int keycode)
{
	if (GetKeyState(keycode) <= -127)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *strlasttok(const char *path, int chartok)
{
	char *bs = strrchr(path, chartok);
	if (!bs)
	{
		return strdup(path);
	}
	else
	{
		return strdup(bs + 1);
	}
}

void gotoxy(int cursorx, int cursory) // Change cursor position to x,y
{
	HANDLE hConsoleHandle;
	hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD dwPos;
	dwPos.X = cursorx;
	dwPos.Y = cursory;

	SetConsoleCursorPosition(hConsoleHandle, dwPos);
	return;
}

int SetColor(int color_hex)
{
	HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleColor, (color_hex / 16) << 4 | (color_hex % 16));
	return 0;
}

int GetColor()
{
	WORD consolecolor;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		consolecolor = csbi.wAttributes;
	}

	return consolecolor;
}

void Alert()
{
	printf("\a");
	return;
}

void ClearScreen() // Clears the screen
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = {0, 0};

	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		return;
	}
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count))
	{
		return;
	}

	if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count))
	{
		return;
	}
	SetConsoleCursorPosition(hStdOut, homeCoords);
	return;
}

int GetConsoleXDimension()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	return columns;
}

int GetConsoleYDimension()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int rows;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return rows;
}

void ClearLine(int line_clear) // Clears the specified line
{
	gotoxy(0, line_clear);
	int console_size_clearline = GetConsoleXDimension() - 1;


	for (int i = 0; i < console_size_clearline/2; i++) // Doubled the speed by printing 2 spaces instead of 1
	{
		fputs("  ", stdout);
	}
	if(console_size_clearline % 2 != 0) 
	{
		putchar(' ');
	}

	return;
}

void PrintBottomString(char *bottom_string)
{
	ClearLine(BOTTOM);
	gotoxy(0, BOTTOM);
	printf("%s", bottom_string);
	return;
}

void DisplayCursor(int disp)
{
	CONSOLE_CURSOR_INFO info;
	if (disp == 0)
	{
		info.bVisible = FALSE;
	}
	else
	{
		info.bVisible = TRUE;
	}

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return;
}

void SetConsoleSize(int xs, int ys)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD a;
	a.X = xs;
	a.Y = ys;

	SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = xs - 1;
	rect.Right = ys - 1;

	SetConsoleScreenBufferSize(handle, a);

	SetConsoleWindowInfo(handle, 1, &rect);
	return;
}

size_t nolflen(char *len_str) // Line length without line feed
{
	unsigned int n = 0, i = 0;
	while (len_str[i] != '\0')
	{
		if (len_str[i] != 0x0A && len_str[n] != 0x0D)
		{
			n++;
		}

		i++;
	}
	return n;
}

void PrintTab(int tab_size)
{
	for (int i = 0; i < tab_size; i++)
	{
		putchar(' ');
	}
	return;
}

int CheckFile(char *filename)
{
	if ((_access(filename, 0)) != -1 && (_access(filename, 6)) != -1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int FindString(char *str, char *find)

{
	int n = strlen(find);
	for (int i = 0; i < strlen(str); i++)
	{
		if (!strncmp(str + i, find, n))
		{
			return i;
		}
	}
	return -1;
}

int tokback_pos(char *s, char *p, char *p2)
{
	/* 
    	p is the delimiter on the last position
    	p2 is the delimiter for an index of + 1
	*/
	for (int i = strlen(s); i > 0; i--)
	{
		for (int k = 0; k < strlen(p2); k++)
		{

			if (s[i] == p2[k])
			{
				while (s[i] == p2[k])
				{
					i--;
				}
				return i + 2;
			}
		}
		for (int j = 0; j < strlen(p); j++)
		{

			if (s[i] == p[j])
			{
				while (s[i] == p[j])
				{
					i--;
				}
				return i + 1;
			}
		}
	}
	return 0;
}

void BS()
{
	printf("\b \b");
}

char *join(const char *s1, const char *s2)
{
	char *s = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(s, s1);
	strcat(s, s2);

	return s;
}

char *itoa_n(int n)
{
	char *s = (char *)malloc(32);
	itoa(n, s, 10);
	return s;
}

char *str_lwr(char *s)
{
	char *s2 = (char *)malloc(strlen(s) + 1);
	for (int i = 0; i < strlen(s); i++)
	{
		s2[i] = tolower(s[i]);
	}
	s2[strlen(s)] = '\0';
	return s2;
}

char *GetTime()
{
	char *time_buf = (char *)malloc(128);
	SYSTEMTIME lt;

	GetLocalTime(&lt);

	sprintf(time_buf, "%02d:%02d %02d/%02d/%04d", lt.wHour, lt.wMinute, lt.wDay, lt.wMonth, lt.wYear);
	return time_buf;
}

char *insert_char(char *str, char c, int pos)
{
	char *new_str = (char *)malloc(strlen(str) + 2);

	strcpy(new_str, str);
	memmove(new_str + pos + 1, str + pos, strlen(str) - (pos - 1));
	new_str[strlen(new_str)] = '\0';
	new_str[pos] = c;
	return new_str;
}

char *delete_char(char *str, int pos)
{
	char *new_str = (char *)malloc(strlen(str));
	int i;
	for (i = 0; i < pos; i++)
	{
		new_str[i] = str[i];
	}
	for (i = pos; i < strlen(str) - 1; i++)
	{
		new_str[i] = str[i + 1];
	}
	new_str[i] = '\0';
	return new_str;
}

char *delete_char_left(char *str, int pos)
{
	char *new_str = (char *)malloc(strlen(str) + 1);

	strcpy(new_str, str);
	memmove(new_str + pos, str + pos + 1, strlen(str) - pos);
	new_str[strlen(new_str)] = '\0';
	return new_str;
}

char *strncpy_n(char *dest, const char *src, size_t count)
{
	// Better version that strncpy() because it always null terminates strings

	if (count)
	{
		*dest = '\0';
		strncat(dest, src, count);
		return dest;
	}
	return NULL;
}

char *EmptyString(char *s)
{
	size_t n = strlen(s);

	for (int i = 0; i < n; i++)
	{
		s[i] = '\0';
	}
	return s;
}

char *ProgInfo()
{

	char *info = (char *)malloc(1024);
	info = join(join(join(join(join(join("Newtrodit ", newtrodit_version), " [Built at "), (char *)__DATE__), " "), (char *)__TIME__), "]");
	return info;
}

int ValidSize()
{
	if (XSIZE < 60 || YSIZE < 6) // Check for console size
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}
	else
	{
		return 1;
	}
}

void MakePause()
{
	int mk_pause;
	mk_pause = getch();
	if (mk_pause == 0 || mk_pause == 0xE0)
	{
		mk_pause = getch();
	}
	return;
}

int tokcount(char *s, char *delim)
{
	int count = 0;
	for (int i = 0; i < strlen(s); i++)
	{
		for (int j = 0; j < strlen(delim); j++)
		{
			if (s[i] == delim[j])
			{
				count++;
			}
		}
	}
	return count;
}

int YesNoPrompt()
{
	int chr;
	while (1)
	{
		chr = getch();

		if (tolower(chr) == 'y' && chr != 0 && chr != 0xE0)
		{
			return 1;
		}
		else if (tolower(chr) == 'n' && chr != 0 && chr != 0xE0)
		{
			return 0;
		}
	}
}