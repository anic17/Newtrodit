/*
  Copyright 2020 anic17 Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of 
  this software and associated documentation files (the "Software"), to deal in 
  the Software without restriction, including without limitation the rights to 
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
  the Software, and to permit persons to whom the Software is furnished to do so, 
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all 
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctype.h>
#include <stdbool.h>
#include <direct.h>
#include <tchar.h>
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
#define MAX_FILENAME 8192

const char newtrodit_version[20] = "0.3";
const char newtrodit_build_date[20] = "30/06/2021";
const char manual_file[] = "newtrodit.man";
const int BUFFER_X = 6144;
const int BUFFER_Y = 2048;

char filename_text[MAX_FILENAME] = "Untitled";
char settings_file[MAX_FILENAME] = "newtrodit.config";

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

int PrintBottomString(char *bottom_string)
{
  gotoxy(0, GetConsoleYDimension() - 1);
  printf("%s", bottom_string);
  return 0;
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

void NewtroditBSOD(int errcode)
{
  printf("Newtrodit ran into a problem and it crashed: 0x%x", errcode);
}