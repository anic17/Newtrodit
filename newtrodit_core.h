#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctype.h>
#include <stdbool.h>

const char newtrodit_version[20] = "0.1";

int gotoxy(int cursorx, int cursory)
{
    HANDLE hConsoleHandle;
    hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD dwPos;
    dwPos.X = cursorx;
    dwPos.Y = cursory;

    SetConsoleCursorPosition(hConsoleHandle,dwPos);
}

int SetColor(int color_hex)
{
    int h_1 = color_hex/16;
    int h_2 = color_hex%16;
    HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsoleColor, h_1<<4 | h_2);
    return 0;
}

void Alert()
{
  printf("\a");
}

void ClearScreen()
{
  HANDLE					 hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD					  count;
  DWORD					  cellCount;
  COORD					  homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
	hStdOut,
	(TCHAR) ' ',
	cellCount,
	homeCoords,
	&count
	)) return;


  if (!FillConsoleOutputAttribute(
	hStdOut,
	csbi.wAttributes,
	cellCount,
	homeCoords,
	&count
	)) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
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
