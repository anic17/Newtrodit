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
#include "newtrodit_buf.h"
#include "dialog.h"

#define XSIZE GetConsoleXDimension()
#define YSIZE GetConsoleYDimension()

#define BOTTOM GetConsoleYDimension() - 1

#define ALT VK_MENU
#define LALT VK_LMENU
#define RALT VK_RMENU

#define SHIFT VK_SHIFT
#define LSHIFT VK_LSHIFT
#define RSHIFT VK_RSHIFT

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008

#define SHIFT_RIGHT 1
#define SHIFT_LEFT 0

const char newtrodit_version[] = "0.3";
const char newtrodit_build_date[] = "26/7/2021";
const char manual_file[] = "newtrodit.man";
const int BUFFER_X = 6144;
const int BUFFER_Y = 2048;
const int TAB_WIDE = 4;
const int CURSIZE = 20;

char filename_text[FILENAME_MAX] = "Untitled";
char settings_file[FILENAME_MAX] = "newtrodit.config";

char *GetDir()
{
  char *curr_dir_buf = _getcwd(NULL, 0);
}

void StartProcess(char *command_line)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  CreateProcess(NULL, command_line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
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

int gotoxy(int cursorx, int cursory)
{
  HANDLE hConsoleHandle;
  hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

  COORD dwPos;
  dwPos.X = cursorx;
  dwPos.Y = cursory;

  SetConsoleCursorPosition(hConsoleHandle, dwPos);
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
}

void ClearScreen()
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

  if (!FillConsoleOutputCharacter(
          hStdOut,
          (TCHAR)' ',
          cellCount,
          homeCoords,
          &count))
    return;

  if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count))
  {
    return;
  }
  SetConsoleCursorPosition(hStdOut, homeCoords);
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

void ClearLine(int line_clear)
{
  gotoxy(0, line_clear);
  int console_size_clearline = GetConsoleXDimension() - 1;
  for (int i = 0; i < console_size_clearline; i++)
  {
    putchar(' ');
  }
}

void PrintBottomString(char *bottom_string)
{
  gotoxy(0, GetConsoleYDimension() - 1);
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
}

void SetConsoleSize(int xs, int ys)
{
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD a;
  a.X = xs;
  a.Y = 300;

  SMALL_RECT rect;
  rect.Top = 0;
  rect.Left = 0;
  rect.Bottom = xs - 1;
  rect.Right = ys - 1;

  SetConsoleScreenBufferSize(handle, a);

  SetConsoleWindowInfo(handle, 1, &rect);
}

char *shift_right(unsigned char insert_char, char *str_shift, int startpos)
{

  char sr_temp;
  size_t len_shift_right = strlen(str_shift);

  sr_temp = str_shift[len_shift_right - 1];                                     // Save last character because it'll be trimmed
  memmove(str_shift + 1 + startpos, str_shift + startpos, len_shift_right - 1); // Move every character by one to the right
  str_shift[startpos] = insert_char;                                            // Put the initial character
  return str_shift;
}

char *shift_left(char *str_shift, int startpos)
{
  char sl_temp;
  size_t len_shift_left = strlen(str_shift);
  sl_temp = str_shift[0];                                                      // Save first character because it'll be trimmed
  memmove(str_shift - 1 + startpos, str_shift + startpos, len_shift_left - 1); // Move every character by one to the left                                                                      // Restore first character
  str_shift[len_shift_left] = '\0';                                            // Put a null character at the end
  str_shift[0] = sl_temp;                                                      // Put the initial character back
  return str_shift;
}

int nolflen(char *len_str)
{
  unsigned int n = 0, i = 0;
  while (len_str[i] != NULL)
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
  for (int i = 0; i < tab_size; ++i)
  {
    putchar(' ');
  }
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
  int position_found = 0;
  size_t find_len = strlen(str);
  if (find_len > 0)
  {
    char *str_find = strstr(str, find);
    if (str_find != NULL)
    {
      position_found = str_find - str;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -2;
  }
  return position_found;
}

char *tokback(char *s, char p)
{
  for (int i = strlen(s); i > 0; i--)
  {
    if (s[i] == p)
    {
      s[i] = '\0';
      return s;
    }
  }
  return s;
}

int tokback_pos(char *s, char p)
{
  for (int i = strlen(s); i > 0; i--)
  {
    if (s[i] == p)
    {
      return i;
    }
  }
  return -1;
}

void BS()
{
  printf("\b \b");
}


char* join(char* s1, char* s2)
{
  char* s = (char*)malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(s, s1);
  strcat(s, s2);
  return s;
}