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
#pragma once

#ifndef _NEWTRODIT_CORE_H_
#define _NEWTRODIT_CORE_H_
#endif

#define _NEWTRODIT_SYNTAX_H 1 // Toggle syntax highlighting

#include <string.h>
#include <stdlib.h>

#include <Windows.h>
#include <conio.h>

#include <ctype.h>
#include <direct.h> // _access()
#include <wchar.h>

#include "dialog.h"

#define _NEWTRODIT_OLD_SUPPORT 0 // Toggle support for old versions of Windows (Windows 98 and below)

#define TAB_WIDE_ 8 // Standard length of the tab key is 8 characters. This has to be 7 because an index of 1 is always added.
#define CURSIZE_ 20
#define LINECOUNT_WIDE_ 4 // To backup original value
#define MIN_BUFSIZE 100
#define LINE_MAX 8192

#ifndef __bool_true_false_are_defined
#define _bool_true_false_are_defined
typedef short bool;

#define false (bool)0
#define true (bool)1
#endif

#ifndef UNDO_STACK_SIZE
#define UNDO_STACK_SIZE 256
#endif

#ifndef _MAX_PATH
#warning "_MAX_PATH not defined. Using 260 as a value"
#define _MAX_PATH 260
#endif
#define filename_text_ "Untitled"

#include "globals.h"

#define XSIZE GetConsoleXDimension()
#define YSIZE GetConsoleYDimension()

#define BOTTOM GetConsoleYDimension() - 1
#define TOP 0

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008

#define PATHTOKENS "\\/"

#ifndef isblank
#define isblank(c) ((c) == ' ' || (c) == '\t')
#endif

int BUFFER_X = 640;
int BUFFER_Y = 5600;

int BUFFER_INCREMENT = 150; // How much to increment the buffer size by when it is full.

typedef struct
{
	char *str;
	int len;
	size_t size;
} Line;

void ExitRoutine(int retval) // Cleanup routine
{
	exit(retval);
}

void StartProcess(char *command_line)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pinf;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pinf, sizeof(pinf));
	CreateProcess(NULL, command_line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pinf);
	CloseHandle(pinf.hProcess);
	CloseHandle(pinf.hThread);
	return;
}

int CheckKey(int keycode)
{
	return GetKeyState(keycode) <= -127;
}

int toklastpos(char *s, char *token)
{
	int lastpos = -1;
	for (int i = 0; i < strlen(s); i++)
	{
		for (int j = 0; j < strlen(token); j++)
		{
			if (s[i] == token[j])
			{
				lastpos = i;
			}
		}
	}
	return lastpos;
}

char *strlasttok(char *tok, char *char_token)
{
	int pos;
	if ((pos = toklastpos(tok, char_token)) == -1)
	{
		return strdup(tok);
	}
	else
	{
		return strdup(tok + pos + 1);
	}
}

void gotoxy(int x, int y) // Change cursor position to x,y
{
	COORD dwPos = {x, y};

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwPos);
	return;
}

int GetConsoleXCursorPos()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition.X;
}

int GetConsoleYCursorPos()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition.Y;
}

void SetColor(int color_hex)
{
	HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleColor, (color_hex / 16) << 4 | (color_hex % 16));
}

int GetColor()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		return csbi.wAttributes;
	}
	return 0;
}

void Alert(void)
{
	putchar('\a');
}

int GetConsoleXDimension()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int GetConsoleYDimension()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void ClearPartial(int x, int y, int width, int height) // Clears a section of the screen
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = {x, y};

	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		return;
	}
	cellCount = width;

	for (int i = 0; i < height; i++)
	{
		FillConsoleOutputCharacter(hStdOut, ' ', cellCount, homeCoords, &count);
		FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);

		homeCoords.Y++;
	}
	homeCoords.Y -= height;
	SetConsoleCursorPosition(hStdOut, homeCoords);
	return;
}

void PrintBottomString(char *bottom_string)
{
	int xs = XSIZE;
	ClearPartial(0, BOTTOM, xs, 1);
	printf("%.*s", xs, bottom_string); // Don't get out of the buffer
	return;
}

void DisplayCursor(bool disp)
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = disp;
	cursor.dwSize = CURSIZE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
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

char *strncpy_n(char *dest, const char *src, size_t count)
{
	// Better version that strncpy() because it always null terminates strings

	if (count)
	{
		memset(dest, 0, count);
		strncat(dest, src, count);
		return dest;
	}
	return NULL;
}

size_t nolflen(char *s) // Line length without line feed
{
	char *exclude = newlinestring;
	if (!strchr(exclude, '\n'))
	{
		strncat(exclude, "\n", 1);
	}
	int len = 0;
	while (*s)
	{
		if (strchr(exclude, *s))
		{
			s++;
		}
		else
		{
			len++;
			s++;
		}
	}
	return len;
}

char *PrintTab(int tab_count)
{
	char *s = malloc(sizeof(char *) * tab_count + 1);
	memset(s, 32, tab_count);

	return s;
}

int CheckFile(char *filename) // Will return 0 if file exists
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

int FindString(char *str, char *find) // Not the best algorithm but it works
{
	char *ptr = strstr(str, find);

	if (!ptr)
	{
		return -1;
	}
	else
	{
		return ptr - str;
	}
}

char* RemoveQuotes(char* dest, char* src)
{
	if (src[0] == '\"' && src[strlen(src) - 1] == '\"')
	{
		strncpy_n(dest, src + 1, strlen(src) - 2);
	}
	else
	{
		strncpy_n(dest, src, strlen(src));
	}
	return dest;
}

char *ReplaceString(char *s, char *find, char *replace, int *occurenceCount)
{
	char *result;
	int c = 0;
	int replacelen = strlen(replace);
	int findlen = strlen(find);
	result = malloc(strlen(s) + 1 + BUFFER_X);
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (strncmp(&s[i], find, findlen) == 0)
		{
			strcpy(&result[c], replace);
			++(*occurenceCount);
			c += replacelen;
			i += findlen - 1;
		}
		else
		{
			result[c++] = s[i];
		}
	}
	result[c] = '\0';

	if (!strcmp(result, s))
	{
		return NULL;
	}
	return result;
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

char *join(const char *s1, const char *s2)
{
	size_t arr_size = strlen(s1) + strlen(s2) + 1;
	char *s = (char *)malloc(arr_size);
	strncpy_n(s, s1, arr_size);
	strncat(s, s2, arr_size);

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
	char *time_buf = (char *)malloc(256);
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

char *insert_row(char **arr, int startpos, size_t arrsize, char *arrvalue)
{
	for (int i = arrsize; i > startpos; i--)
	{
		arr[i] = arr[i - 1];
	}
	arr[startpos + 1] = arrvalue;
	return arr[startpos];
}

char *delete_row(char **arr, int startpos, size_t arrsize)
{
	for (int i = startpos; i < arrsize; i++)
	{
		arr[i] = arr[i + 1];
	}
	return arr[startpos];
}

char *insert_deleted_row(char **str, int *xp, int *yp, size_t num)
{
	size_t n = nolflen(str[ypos - 1]);
	memset(str[*yp - 1] + n, 0, num - n);							// Empty the new line
	strncat(str[*yp - 1], str[ypos] + *xp, strlen(str[*yp]) - *xp); // Concatenate the next line
	delete_row(str, *yp, num);										// Delete the old row, shifting other rows down

	// Decrease the yp pointer by one
	(*yp)--;

	*xp = n;
	return str[*yp];
}

char *EmptyString(char *s)
{
	size_t n = strlen(s);

	memset(s, 0, n);
	return s;
}

char *ProgInfo()
{

	char *info = (char *)malloc(1024);
	snprintf(info, 1024, "Newtrodit %s [Built at %s %s]", newtrodit_version, newtrodit_build_date, __TIME__);
	return info;
}

int ValidSize()
{
	if (XSIZE < 60 || YSIZE < 6) // Check for console size
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}
	return;
}

void MakePause()
{
	int mp;
	mp = getch();
	if (mp == 0 || mp == 0xE0)
	{
		mp = getch();
	}
	return;
}

int getch_n()
{
	int gc_n;
	gc_n = getch();
	if (gc_n == 0 || gc_n == 0xE0)
	{
		gc_n += 255;
		gc_n += getch();
	}
	return gc_n;
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
		else if (chr != 0 && chr != 0xE0 && (tolower(chr) == 'n' || chr == 27))
		{
			return 0;
		}
	}
}

char *rot13(char *s)
{
	for (int i = 0; i < strlen(s); i++)
	{
		if (isalpha(s[i]))
		{
			if (tolower(s[i]) >= 110)
			{
				s[i] -= 13;
			}
			else
			{
				s[i] += 13;
			}
		}
	}
	return s;
}

int BufferLimit()
{
	if (ypos >= BUFFER_Y || xpos >= BUFFER_X)
	{
		PrintBottomString(NEWTRODIT_ERROR_OUT_OF_MEMORY);
		MakePause();
		return 1;
	}
	return 0;
}

void *realloc_n(void *old, size_t old_sz, size_t new_sz)
{
	void *new = malloc(new_sz);
	if (!new)
	{
		return NULL;
	}
	memcpy(new, old, old_sz);
	free(old);
	return new;
}

int LocateFiles(int show_dir, char *file)
{
	const int max_files = 1024;
	int n = 0;
	HANDLE hFindFiles;
	WIN32_FIND_DATAA FindFileData; // Unicode is not supported in Newtrodit

	if ((hFindFiles = FindFirstFileA("*", &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		while (FindNextFileA(hFindFiles, &FindFileData))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{

				if (!show_dir)
				{
					continue;
				}
			}
			printf("%s\n", FindFileData.cFileName);
			if (FindString(FindFileData.cFileName, file) >= 0)
			{
				if (n >= YSIZE - 5)
				{
					return n;
				}
				n++;
			}
		}
		if (n == 0)
		{
			PrintBottomString(join(NEWTRODIT_FS_FILE_NOT_FOUND, file));
			return 0;
		}
	}
	FindClose(hFindFiles);
}

char *RemoveTab(char *s) // Replace all tabs with 8 spaces and return another string
{
	char *new_s = (char *)malloc(strlen(s) + 1);
	for (int i = 0; i < strlen(s); i++)
	{
		if (s[i] == '\t')
		{
			for (int j = 0; j < TAB_WIDE; j++)
			{
				new_s[i + j] = ' ';
				i++;
			}
		}
		else
		{
			new_s[i] = s[i];
		}
	}
	new_s[strlen(s) + TAB_WIDE] = '\0';
	printf("%s", new_s);

	return new_s;
}

int hexstrtodec(char *s) // Just to avoid repetitive calls
{
	return strtol(s, NULL, 16);
}