/**********************************************************************************

	Basic Terminal Control Library
	Copyright 2022 Zackery Smith
	This library is released under the GPLv3 license

	This library has no dependencies other than the standard C runtime library

***********************************************************************************/
#ifndef TC_H
#define TC_H
#endif

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h> // For _getch() function
HANDLE hConsole = INVALID_HANDLE_VALUE, hAlternateScreen = INVALID_HANDLE_VALUE; // WinAPI structures for console
CONSOLE_SCREEN_BUFFER_INFO csbi;
CONSOLE_CURSOR_INFO cci;
#else

#endif
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define TC_NRM ""
#else
#define TC_NRM "\x1B[0m" /* Normalize color */
#endif

#define TC_RED "\x1B[1;31m" /* Red */
#define TC_GRN "\x1B[1;32m" /* Green */
#define TC_YEL "\x1B[1;33m" /* Yellow */
#define TC_BLU "\x1B[1;34m" /* Blue */
#define TC_MAG "\x1B[1;35m" /* Magenta */
#define TC_CYN "\x1B[1;36m" /* Cyan */
#define TC_WHT "\x1B[1;37m" /* White */

#define TC_B_NRM "\x1B[0m"	  /* Normalize Bright Color */
#define TC_B_RED "\x1B[0;31m" /* Bright Red */
#define TC_B_GRN "\x1B[0;32m" /* Bright Green */
#define TC_B_YEL "\x1B[0;33m" /* Bright Yellow */
#define TC_B_BLU "\x1B[0;34m" /* Bright Blue */
#define TC_B_MAG "\x1B[0;35m" /* Bright Magenta */
#define TC_B_CYN "\x1B[0;36m" /* Bright Cyan */
#define TC_B_WHT "\x1B[0;37m" /* Bright White */

#define TC_BG_NRM "\x1B[40m" /* Normalize Background Color */
#define TC_BG_RED "\x1B[41m" /* Background Red */
#define TC_BG_GRN "\x1B[42m" /* Background Green */
#define TC_BG_YEL "\x1B[43m" /* Background Yellow */
#define TC_BG_BLU "\x1B[44m" /* Background Blue */
#define TC_BG_MAG "\x1B[45m" /* Background Magenta*/
#define TC_BG_CYN "\x1B[46m" /* Background Cyan */
#define TC_BG_WHT "\x1B[47m" /* Background White */

// `asprintf` is usable on any POSIX-2008 compliant system (any modern Linux system)
// My compiler likes to complain about it.. Another way to preform this is with `vfprintf`
// Or maybe I'm just a bad programmer :I

#ifdef _WIN32

char *tc_color_id(uint8_t cid, int l)
{ // "l" flag is ignored, just to make it compatible with the POSIX version
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (cid / 16) << 4 | (cid % 16));
	return "";
}

#else

char *tc_color_id(uint8_t cid, int l)
{

	char *esc;
	if (!l)
	{
		asprintf(&esc, "\x1B[48;5;%dm", cid);
	}
	else
	{
		asprintf(&esc, "\x1B[38;5;%dm", cid);
	}
	return esc;
}

//////////////////////////////////////////////////////////////////
//    WARNING: WinAPI doesn't natively support 24-bit colors    //
//    So this function is not available on Windows systems      //
//////////////////////////////////////////////////////////////////
char *tc_rgb(int r, int g, int b, int l)
{
	char *esc;
	if (!l)
	{
		asprintf(&esc, "\x1B[48;2;%d;%d;%dm", r, g, b);
	}
	else
	{
		asprintf(&esc, "\x1B[38;2;%d;%d;%dm", r, g, b);
	}
	return esc;
}
#endif

//////////////////////////////////////
//   Additional formatting (ANSI)   //
//////////////////////////////////////

#define TC_BLD "\x1B[1m"  /* Bold */
#define TC_DIM "\x1B[2m"  /* Dim */
#define TC_ITAL "\x1B[3m" /* Standout (italics) */
#define TC_UNDR "\x1B[4m" /* Underline */
#define TC_BLNK "\x1B[5m" /* Blink */
#define TC_REV "\x1B[7m"  /* Reverse */
#define TC_INV "\x1B[8m"  /* Invisible */

//////////////////////////////////////

void tc_get_cols_rows(int *cols, int *rows);

//////////////////////////////
//   Common private modes   //
//////////////////////////////

#ifdef _WIN32

void tc_hide_cursor()
{
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cci);
}
void tc_show_cursor()
{
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	cci.bVisible = TRUE;
	SetConsoleCursorInfo(hConsole, &cci);
}

void tc_enter_alt_screen()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	hAlternateScreen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hAlternateScreen);
	SetStdHandle(STD_OUTPUT_HANDLE, hAlternateScreen);
}

void tc_exit_alt_screen()
{
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return;
	}
	SetConsoleActiveScreenBuffer(hConsole);
	CloseHandle(hAlternateScreen);
	SetStdHandle(STD_OUTPUT_HANDLE, hConsole);
}

#else

#define tc_hide_cursor() puts("\033[?25l")
#define tc_show_cursor() puts("\033[?25h")

/* These functions don't seem to be doing anything

	#define tc_save_screen() puts("\033[?47h")
	#define tc_restore_screen() puts("\033[?47l")

*/
#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")
#endif
//////////////////////////////

void tc_echo_off();
void tc_echo_on();

void tc_get_cols_rows(int *cols, int *rows)
{
#ifdef _WIN32

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	*cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	*cols = size.ws_col;
	*rows = size.ws_row;
#endif
}

#ifdef _WIN32
void tc_echo_off() // Not intended for user use
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD old_mode;
	GetConsoleMode(hConsole, &old_mode);
	SetConsoleMode(hConsole, old_mode & (~ENABLE_ECHO_INPUT));
}
void tc_echo_on()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD old_mode;
	GetConsoleMode(hConsole, &old_mode);
	SetConsoleMode(hConsole, old_mode | ENABLE_ECHO_INPUT);
}

void tc_canon_on()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD old_mode;
	GetConsoleMode(hConsole, &old_mode);
	SetConsoleMode(hConsole, old_mode | ENABLE_ECHO_INPUT);
}

void tc_canon_off()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD old_mode;
	GetConsoleMode(hConsole, &old_mode);
	SetConsoleMode(hConsole, old_mode & (~ENABLE_ECHO_INPUT));
}

void tc_clear_partial(int x, int y, int width, int height) // Clears a section of the screen
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	COORD homeCoords = {x, y};

	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	for (int i = 0; i < height; i++)
	{
		FillConsoleOutputCharacter(hConsole, ' ', width, homeCoords, &count);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, width, homeCoords, &count);

		homeCoords.Y++;
	}
	homeCoords.Y = y;
	SetConsoleCursorPosition(hConsole, homeCoords);
	return;
}

void tc_get_cursor(int *x, int *y)
{
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	*x = csbi.dwCursorPosition.X;
	*y = csbi.dwCursorPosition.Y;
}
void tc_set_cursor(int x, int y)
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(hConsole, pos);
}

void tc_move_cursor(int x, int y)
{
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int cur_x = csbi.dwCursorPosition.X + x;
	int cur_y = csbi.dwCursorPosition.Y + y;
	if (cur_x < 0)
	{
		cur_x = 0;
	}
	if (cur_y < 0)
	{
		cur_y = 0;
	}
	COORD pos = {cur_x, cur_y};
	SetConsoleCursorPosition(hConsole, pos);

}

void tc_clear_screen()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD written;
	DWORD bufSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD homeCoords = {0, 0}; // Home coordinates
	FillConsoleOutputCharacter(hConsole, ' ', bufSize, homeCoords, &written);
}

void tc_clear_entire_line()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	tc_clear_partial(0, csbi.dwCursorPosition.Y, csbi.dwSize.X, 1);
}

void tc_clear_line_till_cursor()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	tc_clear_partial(0, csbi.dwCursorPosition.Y, csbi.dwCursorPosition.X, 1);
}

void tc_clear_line_from_cursor()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	tc_clear_partial(csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y, csbi.dwSize.X, 1);
}

void tc_clear_from_top_to_cursor()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	tc_clear_partial(0, 0, csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
}

void tc_clear_from_cursor_to_bottom()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	tc_clear_partial(csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y, csbi.dwSize.X, csbi.dwSize.Y - csbi.dwCursorPosition.Y);
}

void tc_print(const char *s)
{
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s, strlen(s), NULL, NULL);
}

int tc_getch()
{
	int ch = _getch();
	if(ch == 0 || ch == 0xE0)
	{
		ch += 255;
	}
	return ch;
}

#else

#define tc_clear_entire_line() puts("\x1B[2K")
#define tc_clear_line_till_cursor() puts("\x1B[1K")
#define tc_clear_line_from_cursor() puts("\x1B[0K")

void tc_echo_off()
{
	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(1, TCSANOW, &term);
}

void tc_echo_on()
{
	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ECHO;
	tcsetattr(1, TCSANOW, &term);
}

void tc_canon_on()
{
	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ICANON;
	tcsetattr(1, TCSANOW, &term);
}

void tc_canon_off()
{
	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ICANON;
	tcsetattr(1, TCSANOW, &term);
}

void tc_get_cursor(int *X, int *Y)
{
	tc_echo_off();
	tc_canon_off();
	printf("\033[6n");
	scanf("\033[%d;%dR", X, Y);
}
#define tc_set_cursor(X, Y) printf("\033[%d;%dH", Y, X)
void tc_move_cursor(int X, int Y)
{
	if (X > 0)
	{
		printf("\033[%dC", X);
	}
	else if (X < 0)
	{
		printf("\033[%dD", (X * -1));
	}

	if (Y > 0)
	{
		printf("\033[%dB", Y);
	}
	else if (Y < 0)
	{
		printf("\033[%dA", (Y * -1));
	}
}

#define tc_clear_screen() puts("\x1B[2J")
#define tc_clear_from_top_to_cursor() puts("\x1B[1J")
#define tc_clear_from_cursor_to_bottom() puts("\x1B[0J")

void tc_clear_partial(int x, int y, int width, int height)
{
	char *buf = (char *)calloc(width + 1, 1);
	memset(buf, 32, width);
	tc_set_cursor(x, y);
	for (int i = 0; i < height; i++)
	{
		tc_set_cursor(x, y + i);
		fwrite(buf, width, 1, stdout);
	}
	free(buf);
}

void tc_print(const char *s)
{
	fprintf(stdout, "%s", s);
}

int tc_getch() // TODO: Implement this
{
	return 0;
}

#endif
