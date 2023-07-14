/*
	Newtrodit: A console text editor
	Copyright (c) 2021-2023 anic17 Software

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
#else
#error "A newtrodit_core file has already been included"
#endif

#include <string.h> // All string functions
#include <stdlib.h> // Some standard functions (malloc, calloc)
#include <stdio.h>	// Standard I/O
#include <stdarg.h> // va_list
#include <errno.h>	// errno

#include <Windows.h> // Console access
#include <conio.h>	 // getch()
#include <direct.h>	 // _access()
#include <process.h> // _beginthread()

#include <fcntl.h>	// _setmode, _fileno
#include <ctype.h>	// isxdigit, etc.
#include <wchar.h>	// For UTF-8 support
#include <stdint.h> // uintptr_t
#include "../dialog.h"

/* =============================== SETTINGS ================================== */
#define _NEWTRODIT_OLD_SUPPORT 0				 // Toggle support for old versions of Windows (Windows XP and below)
#define _NEWTRODIT_EXPERIMENTAL_RESTORE_BUFFER 1 // Toggle support for restoring buffer on exit (currently experimental)
#define DEBUG_MODE 1

#define TAB_WIDE_ 2
#define CURSIZE_ 20
#define CURSIZE_INS 80
#define MIN_BUFSIZE 256
#define LINE_MAX 8192
#define MAX_TABS 48 // Maximum number of files opened at once
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#if 1
#define HORIZONTAL_SCROLL
#endif

#ifndef __bool_true_false_are_defined
#define _bool_true_false_are_defined
typedef int bool;

#define false 0
#define true 1
#endif

#ifndef SIGTRAP
#define SIGTRAP 5
#endif

#ifndef UNDO_STACK_SIZE
#define UNDO_STACK_SIZE 2048
#endif

const char *filename_text_ = "Untitled"; // Default filename
#define DEFAULT_BUFFER_X 640

#define MAKE_INT64(hi, lo) ((LONGLONG(DWORD(hi) & 0xffffffff) << 32) | LONGLONG(DWORD(lo) & 0xffffffff)) // https://stackoverflow.com/a/21022647/12613647

// Only used for debugging, not for release

#if DEBUG_MODE
#define DEBUG                                           \
	PrintBottomString("OK. File:%s Line:%d\n", __FILE__, __LINE__); \
	getch_n();
#else
#define DEBUG
#endif

#include "../globals.h"

enum ConsoleQueryList
{
	XWINDOW = 1,
	XBUFFER_SIZE = 2,
	YWINDOW = 4,
	YBUFFER_SIZE = 8,
	XCURSOR = 16,
	YCURSOR = 32,
	COLOR = 64,
	XMAX_WINDOW = 128,
	YMAX_WINDOW = 256,
	CURSOR_SIZE = 512,
	CURSOR_VISIBLE = 1024,
};

int GetConsoleInfo(int type);

#define XSIZE GetConsoleInfo(XWINDOW)
#define YSIZE GetConsoleInfo(YWINDOW)

#define YSCROLL GetConsoleInfo(YWINDOW) - 2

#define BOTTOM GetConsoleInfo(YWINDOW) - 1

#define DEFAULT_ALLOC_SIZE 512
#define MACRO_ALLOC_SIZE 2048

#define _xpos Tab_stack[file_index].xpos // For simplicity
#define _ypos Tab_stack[file_index].ypos

int BUFFER_X = DEFAULT_BUFFER_X;
int BUFFER_Y = 6400;

int BUFFER_INCREMENT = 150;			// How much to increment the buffer size by when it is full.
int BUFFER_INCREMENT_LOADFILE = 50; // When loading a file, how much to increment the buffer size by.
/* ============================ END OF SETTINGS ============================== */

#if !defined ENABLE_VIRTUAL_TERMINAL_PROCESSING || !defined DISABLE_NEWLINE_AUTO_RETURN
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

#ifdef _WIN32
const char PATHTOKENS[] = "\\/";
#else
const char PATHTOKENS[] = "/";
#endif

typedef struct Startup_info
{
	char *location; // Location of the file (full path)
	char *dir;		// Directory that is changed with 'cd' or 'chdir'
	int xsize;		// X size of the window
	int ysize;		// Y size of the window
	int xbuf;
	int ybuf;
	int color;
	char **argv;
	int argc;
	int manual_open;
	LPSTR console_buffer; // Console buffer contents
	bool save_buffer;
	char *log_file_name;
	bool using_log;
	struct cursor
	{

		int x;
		int y;
	} cursor;

} Startup_info; // Startup data when Newtrodit is started

typedef struct Undo_stack
{
	char *line;
	int line_count;
	int line_pos;
	size_t size;
	bool create_nl;
	bool delete_nl;
} Undo_stack;

typedef struct Syntax_info
{
	// File and file type
	char *syntax_file;
	char *syntax_lang;
	char *separators;

	// Colors
	int capital_color;
	int comment_color;
	int default_color;
	int num_color;
	int override_color;
	int quote_color;

	// Various size parameters

	size_t keyword_count;
	size_t comment_count;
	size_t enclosing_count;
	size_t capital_min; // Minimum length of a word to be highlighted as capital

	bool capital_enabled;
	bool multi_line_comment;
	bool single_quotes;
	bool finish_quotes;

	// Highlight pairs of brackets, parenthesis, and square brackets {}, (), [] (not supported as of 0.6)

	size_t bracket_pair_count;
	size_t parenthesis_pair_count;
	size_t square_bracket_pair_count;
	// Keyword info
	char **comments;
	char **keywords;
	char **enclosing_char; // Enclosing quote-like characters
	int *color;
} Syntax_info; // Syntax highlighting information

typedef struct Compiler
{
	char *path;
	char *flags;
	char *output;
} Compiler; // Compiler settings, used for macro building command lines

typedef struct File_info
{
	char *filename;
	char *fullpath;

	char *language; // Language (e.g: PowerShell, C, C++, etc.)

	bool is_readonly;
	int permissions;

	bool is_loaded;
	bool utf8;

	// File information
	int xpos;
	int ypos;
	int display_x;	// X scrolling
	int display_y;	// Y scrolling
	int last_dispy; // Used for line number highlighting
	int last_y;
	int last_pos_scroll; // Last X position before scrolling

	bool scrolled_x; // If the cursor is scrolled to the right
	bool scrolled_y; // If the file is scrolled vertically

	char **strsave;	  // Buffer to store the file
	int **tabcount;	  // Number of tabs in the line
	size_t *linesize; // Size of the line
	size_t linecount;
	size_t linecount_wide;

	long long size;
	size_t bufx;
	size_t bufy;

	char *newline;

	bool is_saved; // Replacing old variables 'isSaved', 'isModified', 'isUntitled'
	bool is_modified;
	bool is_untitled;
	Undo_stack *Ustack;
	Syntax_info Syntaxinfo;
	Compiler Compilerinfo;
	HANDLE hFile; // File handle
	FILETIME fwrite_time;
	FILETIME fread_time;
	select_t selection;
} File_info; // File information. This is used to store all the information about the file.

Startup_info SInf;
File_info Tab_stack[MAX_TABS];
DWORD dwStdoutMode, dwStdinMode;
HANDLE hOldBuf = INVALID_HANDLE_VALUE, hNewBuf = INVALID_HANDLE_VALUE;

bool used_tab_indexes[MAX_TABS];

/* =================================== TERM  ===================================== */
/* -------------------------------- TERM CURSOR ---------------------------------- */

/* Set cursor's X, Y position */
void gotoxy(int x, int y)
{
	COORD dwPos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwPos);
}

/* Change the visibility of the terminal cursor */
void DisplayCursor(bool disp)
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = disp;
	cursor.dwSize = CURSIZE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

/* ---------------------------- TERM SCREEN CONTROL ------------------------------ */

int VTSettings(bool enabled)
{
#ifdef _WIN32
	DWORD lmode; // Process ANSI escape sequences
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(hStdout, &lmode);
	if (enabled)
	{
		lmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING & ~DISABLE_NEWLINE_AUTO_RETURN;
	}
	else
	{

		lmode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	}
	return SetConsoleMode(hStdout, lmode);
#endif
}

/* Enter or quit alternate screen buffer (unfinished) */
int SetAltConsoleBuffer(bool enabled)
{
#if _NEWTRODIT_EXPERIMENTAL_RESTORE_BUFFER
	if (VTSettings(true))
	{
		fputs(enabled ? "\e[?1049h" : "\e[?1049l", stdout);
		VTSettings(false);
		return 1;
	}
#endif
	return 0;
}

/* Restore original screen buffer (if not already active) */
int RestoreConsoleBuffer()
{
	SetConsoleActiveScreenBuffer(hOldBuf);
	CloseHandle(hNewBuf);
	SetStdHandle(STD_OUTPUT_HANDLE, hOldBuf);
	fclose(stdout);
	fclose(stderr);
	return 0;
}

/* Set default FG color for text */
void SetColor(int color)
{
	if (color > 255) // RGB
	{
		// TODO: Add RGB support for Windows
	}
	else
	{
		HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsoleColor, color);
	}
}

/* Set color of a specific terminal cell (specified by X, and Y) */
int SetCharColor(size_t count, int color, int x, int y)
{
	if (x < 0 || y < 0)
	{
		return 0;
	}
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD writePos = {x, y};
	DWORD dwWritten = 0;
	FillConsoleOutputAttribute(hConsole, color, count, writePos, &dwWritten);
	return 1;
}

/* Clear entire screen (keeping terminal cursor in the same location) */
void ClearScreen()
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count, cellCount;
	COORD homeCoords = {0, 0};

	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		return;
	}
	cellCount = csbi.dwSize.X * csbi.dwSize.Y; // All the buffer, not only the visible part

	FillConsoleOutputCharacter(hStdOut, L' ', cellCount, homeCoords, &count);
	FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);

	SetConsoleCursorPosition(hStdOut, homeCoords);
}

/* Clear a section of the screen
 *
 * Think of it as a rectangle. All this function does is create a rectangle
 * of a specified width and height at a position on the screen. Anything in
 * the rectangle will be cleared are replaced by a empty cell.
 */
void ClearPartial(int x, int y, int width, int height) // Clears a section of the screen
{
	if (!width || !height)
	{
		return;
	}
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	COORD homeCoords = {x, y};

	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		return;
	}
	for (int i = 0; i < height; i++)
	{
		FillConsoleOutputCharacter(hStdOut, ' ', width, homeCoords, &count);
		FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, width, homeCoords, &count);

		homeCoords.Y++;
	}
	homeCoords.Y = y;
	SetConsoleCursorPosition(hStdOut, homeCoords);
	return;
}

/* Function for printing a string on the last row of the screen */

size_t strlen_n(const char *s) // Safe strlen() version
{
	if (!s)
	{
		return 0;
	}
	return strlen(s);
}

void PrintBottomString(char *str, ...)
{
	va_list args;
	va_start(args, str);
	int xs = XSIZE;
	char *printbuf = calloc(xs + 1, sizeof(char));
	vsnprintf(printbuf, xs + 1, str, args);
	size_t len = strlen_n(printbuf);
	ClearPartial(len, BOTTOM, xs - len, 1);
	gotoxy(0, BOTTOM);
	printf("%.*s", xs, printbuf); // Don't get out of the buffer
	free(printbuf);
	va_end(args);
	return;
}

/* Set size of the console window */
void SetConsoleSize(int xsize, int ysize, int xbuf, int ybuf)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD a = {xsize, ysize};
	SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = xsize - 2;
	rect.Right = ysize - 2;

	SetConsoleScreenBufferSize(handle, a);
	SetConsoleWindowInfo(handle, 1, &rect);
	SetConsoleScreenBufferSize(handle, a);
}

/* Convert a string hex to a decimal number
 *
 * Makes life a bit easier by removing repeditive calls
 */
int HexStrToDec(char *s)
{
	return strtol(s, NULL, 16);
}

int atoi_tf(char *s)
{
	if (!_stricmp(s, "true") || atoi(s) == 1)
	{
		return 1;
	}
	return 0;
}

int SetBoolValue(int *boolv, char *s)
{
	int retval = atoi_tf(s);
	*boolv = !!retval;
	return retval;
}

int IsNumberString(char *s)
{
	for (size_t i = 0; i < strlen_n(s); i++)
	{
		if (!isdigit(s[i]))
		{
			return 0;
		}
	}
	return 1;
}

char *Substring(size_t start, size_t count, const char *str)
{
	char *new_str = calloc(sizeof(char), count + 1);
	strncat(new_str, str + start, count);
	return new_str;
}

char *ParseHexString(char *hexstr)
{
	int hstd_return = 0, index = 0;
	char *hex_prefix = "0x";
	char *ptr_tokenize;
	size_t max_len = 2 * (strlen_n(hexstr) + 1);
	char *str = calloc(max_len, sizeof(char));
	if (!str)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
		return NULL;
	}
	char *delims = ", \t\n";
	if (!strpbrk(hexstr, "01234567890abcdefABCDEF")) // Make sure strings contains hex characters
	{
		return hexstr;
	}

	// Convert the string to a hex string
	ptr_tokenize = strtok(hexstr, delims);
	while (ptr_tokenize != NULL)
	{
		strlwr(ptr_tokenize);
		if (!strncmp(ptr_tokenize, hex_prefix, strlen_n(hex_prefix))) // Only parse if the strings starts with 0x
		{
			hstd_return = HexStrToDec(ptr_tokenize);
			if (hstd_return % 256) // Ignore null values
			{
				str[index] = HexStrToDec(ptr_tokenize);
				index++;
			}
		}
		ptr_tokenize = strtok(NULL, delims);
	}
	return hexstr;
}

/* =============================== END OF TERM  ================================== */

/* Get line length, ignoring linefeeds */
size_t NoLfLen(char *s)
{
	char *exclude = Tab_stack[file_index].newline;
	if (!strchr(exclude, '\n')) // Always exclude a newline (\n) even if it's not present
	{
		// strncat(exclude, "\n", 2);
	}
	size_t len = 0;
	while (*s)
	{
		if (!strchr(exclude, *s))
		{
			len++;
		}
		s++;
	}
	return len;
}

/* Count the number of chars in a string, chars to search set via delim.
 *
 * 	E.g. `TokCount(str, "!*")` will return the number of occurrences of `!`
 * and `*` in `s`
 */
int TokCount(char *s, char *delim)
{
	int count = 0;
	for (int i = 0; i < strlen_n(s); i++)
	{
		for (int j = 0; j < strlen_n(delim); j++)
		{
			if (s[i] == delim[j])
			{
				count++;
			}
		}
	}
	return count;
}

/* ? */
int TokLastPos(char *s, const char *token)
{

	int lastpos = -1;
	if (!s || !token)
	{
		return lastpos;
	}
	for (int i = 0; i < strlen_n(s); i++)
	{
		for (int j = 0; j < strlen_n(token); j++)
		{
			if (s[i] == token[j])
			{
				lastpos = i;
			}
		}
	}
	return lastpos;
}

/* ? */
char *StrLastTok(char *tok, const char *char_token)
{
	if (!tok | !char_token)
	{
		return tok;
	}
	int pos = TokLastPos(tok, char_token);
	return tok + pos + 1;
}

/* ? */
int TokBackPos(char *s, char *p, char *p2)
{
	/*
		p is the delimiter on the last position
		p2 is the delimiter for an index of + 1
	*/
	for (int i = strlen_n(s); i > 0; i--)
	{
		for (int k = 0; k < strlen_n(p2); k++)
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
		for (int j = 0; j < strlen_n(p); j++)
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

/* Base name (no path, no extension) */

char *GetBaseName(char *file)
{
	char *ptr = StrLastTok(file, PATHTOKENS), *pchar = strrchr(ptr, '.'); // Remove the path
	char *basename = calloc(MAX_PATH + 1, sizeof(char));
	if (!pchar)
	{
		return ptr;
	}
	memcpy(basename, ptr, pchar - ptr); // Copy until the last dot
	return basename;
}

/* Compare string and check for tokens */
int strcmptok(char *s, char *short_s, char *tok)
{
	if (!s || !short_s || !tok)
	{
		return -1;
	}
	if (!strncmp(s, short_s, strlen_n(short_s)))
	{
		for (size_t k = 0; k < strlen_n(tok); k++)
		{
			if (s[strlen_n(short_s)] == tok[k])
			{
				return 0;
			}
		}
	}
	return 1;
}

/* Print tab of set size? */
char *PrintTab(int tab_count)
{
	char *s = calloc(sizeof(char), (tab_count + 1));
	memset(s, 32, tab_count);
	return s;
}

/* ============================ STRING MANIPULATION ============================== */

/* Custom strtok() function */
char *strtok_n(char *str, char *tokens)
{
	if (!str || !tokens)
	{
		return NULL;
	}
	size_t span = strcspn(str, tokens);
	if (span == strlen_n(str))
	{
		return NULL;
	}
	return Substring(0, span, str);
}

/* Safe version of strcpy() and strncpy() */
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

/* ? */
size_t strrpbrk(char *s, char *find) // Reverse strpbrk, just like strrchr but for multiple characters
{
	size_t findlen = strlen_n(find);
	size_t slen = strlen_n(s);
	for (size_t i = slen; i > 0; i--)
	{
		for (size_t j = 0; j < findlen; j++)
		{
			if (s[i - 1] == find[j])
			{
				return i - 1;
			}
		}
	}
	return 0;
}

size_t strcmpcount(char *s1, char *s2)
{
	size_t l1 = strlen_n(s1);
	size_t l2 = strlen_n(s2);
	if (!l1 || !l2)
	{
		return 0;
	}
	size_t complen = l1 > l2 ? l2 : l1, cnt = 0;
	for (int i = 0; i < complen; i++)
	{
		if (s1[i] == s2[i])
		{
			cnt++;
		}
		else
		{
			return cnt;
		}
	}
	return cnt;
}

/* Find a needle in a haystack.
 *
 * Search a string for a sub-string
 */
int FindString(char *str, char *find)
{
	char *ptr = strstr(str, find);
	return (ptr) ? (ptr - str) : strlen_n(str);
}

/* ? */
// Remove the quotes on a string
char *RemoveQuotes(char *s)
{
	size_t len = strlen_n(s);
	if (s[0] == '\"' && s[len - 1] == '\"')
	{
		memmove(s, s + 1, len - 2);
		s[len] = '\0';
	}
	return s;
}

// Replace a string
char *ReplaceString(char *s, char *find, char *replace, int *occurenceCount)
{
	char *result;
	int c = 0;
	int replacelen = strlen_n(replace);
	int findlen = strlen_n(find);
	size_t sz = strlen_n(s) + replacelen + 1;
	// BUG: Assigning to 'char *' from incompatible type 'void *'
	result = malloc(sz);
	if (!result)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
		return s;
	}
	for (size_t i = 0; s[i] != '\0'; i++)
	{
		if (strncmp(s + i, find, findlen) == 0)
		{
			strcpy(result + c, replace);
			if (replacelen > findlen) // Allocate more memory in case it is needed
			{
				sz += replacelen - findlen;
				result = realloc(result, sz);
			}
			(*occurenceCount)++;
			c += replacelen;
			i += findlen - 1;
		}
		else
		{
			result[c++] = s[i];
		}
	}
	result[c] = '\0';
	return result;
}

/* Safely join two strings by allocating memory */
char *join(char *s1, const char *s2)
{
	if (!s2)
	{
		return s1;
	}
	size_t arr_size = strlen_n(s1) + strlen_n(s2) + 1;
	char *s = calloc(arr_size, sizeof(char));
	if (!s)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
		return NULL;
	}
	strncpy_n(s, s1, arr_size);
	strncat(s, s2, arr_size);

	return s;
}

/* Returns a string where all characters are lower case */
char *strlwr(char *s)
{
	for (int i = 0; i < strlen_n(s); i++)
	{
		s[i] = tolower(s[i]);
	}
	return s;
}

char *InsertStr(char *s1, const char *s2, size_t pos, bool allocstring, size_t maxsize)
{
	size_t l1 = strlen_n(s1), l2 = strlen_n(s2);

	char *new_str;
	if (allocstring)
	{
		new_str = calloc(l1 + l2 + 3, sizeof(char));
		memcpy(new_str, s1, strlen_n(s1));

		if (!new_str)
		{
			last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
			return NULL;
		}
	}
	else
	{
		new_str = s1;
	}
	if (l1 + l2 < maxsize && !allocstring)
	{
		memmove(new_str + pos + l2, new_str + pos, l1);
		memcpy(new_str + pos, s2, l2);
	} else {
		return NULL;
	}
	new_str[strlen_n(new_str)] = '\0';

	return new_str;
}

// Insert a single character at a specific index
char *InsertChar(char *str, char c, int pos, bool allocstring, size_t maxsz)
{
	char s2[2] = {c, '\0'};
	return InsertStr(str, s2, pos, allocstring, maxsz);
}

// Delete a string on a specified position
char *DeleteStr(char *str, int pos, size_t count)
{
	size_t len = strlen_n(str);
	if (pos + count > len)
	{
		return str;
	}
	memmove(str + pos, str + pos + count, len - (pos + count));
	str[len - count] = '\0';
	return str;
}

char *DeleteChar(char *str, int pos) // This function is limited to lines with a length of 2^31 - 1 characters
{
	size_t len = strlen_n(str);
	if (len > pos)
	{
		memmove(str + pos, str + pos + 1, len - pos);
		str[strlen_n(str)] = '\0';
	}
	return str;
}

char *InsertRow(char **arr, int startpos, size_t arrsize, char *arrvalue)
{
	for (int i = arrsize; i > startpos; i--)
	{
		arr[i] = arr[i - 1];
	}
	arr[startpos + 1] = arrvalue;
	return arr[startpos];
}

/* ? */
char *DeleteRow(char **arr, int startpos, size_t arrsize)
{
	for (int i = startpos; i < arrsize; i++)
	{
		arr[i] = arr[i + 1];
	}
	return arr[startpos];
}

/* ? */
char *InsertDeletedRow(File_info *tstack)
{
	int n = NoLfLen(tstack->strsave[tstack->ypos]);
	strncat(tstack->strsave[tstack->ypos - 1], tstack->strsave[tstack->ypos], strlen_n(tstack->strsave[tstack->ypos])); // Concatenate the next line
	memset(tstack->strsave[tstack->ypos] + n, 0, BUFFER_X - n);															// Empty the new line

	if (tstack->ypos != 1)
	{
		// Add the newline string only if the ypos isn't 1
		strncat(tstack->strsave[tstack->ypos - 1], tstack->newline, strlen_n(tstack->newline));
	}
	// Delete the old row, shifting other rows down
	DeleteRow(tstack->strsave, tstack->ypos, tstack->bufy - 1);

	// Decrease the yp pointer by one
	tstack->xpos = NoLfLen(tstack->strsave[tstack->ypos]);
	last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;

	return tstack->strsave[tstack->ypos];
}

/* ========================= END OF STRING MANIPULATION ========================== */

/* =================================== SYSTEM  =================================== */

/* Improved getch(), returns all codes in a single call */
int getch_n()
{
	int gc_n = 0;
	gc_n = getch();
	if (gc_n == 0)
	{
		gc_n = getch();
		gc_n |= BIT_ESC0;
	}
	else if (gc_n == 0xE0)
	{
		gc_n = getch();
		gc_n |= BIT_ESC224;
	}

	return gc_n;
}

/* Checks the physical state of a key (Pressed or not pressed) */
int CheckKey(int keycode)
{
	return GetKeyState(keycode) <= -127;
}

/* Get absolute path to a file on the drive */
char *FullPath(char *file)
{
	char *path = calloc(MAX_PATH, sizeof(char));
	GetFullPathName(file, MAX_PATH * sizeof(char), path, NULL);
	return path;
}

/* Return a string representing the current date and time */
char *GetTime(bool display_ms)
{
	char *time_buf = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	if (!time_buf)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;

		return NULL;
	}

	SYSTEMTIME lt;

	GetLocalTime(&lt);
	if (display_ms)
	{
		snprintf(time_buf, DEFAULT_ALLOC_SIZE, "%02d:%02d:%02d,%03d %02d/%02d/%04d", lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, lt.wDay, lt.wMonth, lt.wYear);
	}
	else
	{
		snprintf(time_buf, DEFAULT_ALLOC_SIZE, "%02d:%02d:%02d %02d/%02d/%04d", lt.wHour, lt.wMinute, lt.wSecond, lt.wDay, lt.wMonth, lt.wYear);
	}

	return time_buf;
}

/* Create a log file in the program directory (current working directory) */
int WriteLogFile(char *data, ...)
{
	va_list args;
	va_start(args, data);
	char *writebuf;

	if (useLogFile)
	{
		char *filename = SInf.log_file_name; // Get the log file name

		FILE *f = fopen(filename, "ab");
		if (!f)
		{
			return errno;
		}
		char *gettime = GetTime(true);
		writebuf = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(char));
		if (!writebuf)
		{
			last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
			PrintBottomString("%s", NEWTRODIT_ERROR_OUT_OF_MEMORY);
			free(gettime);
			return -1;
		}
		vsnprintf(writebuf, DEFAULT_ALLOC_SIZE, data, args);
		fprintf(f, "[%s] %s\n", gettime, writebuf);
		free(writebuf);
		free(gettime);
		fclose(f);
		return 0;
	}
	else
	{
		return -1; // No log file
	}
}

void ExitRoutine(int retval) // Cleanup routine
{
	if (SInf.location != NULL)
	{
		chdir(SInf.dir);
		free(SInf.location);
	}

	SetConsoleSize(SInf.xsize, SInf.ysize, SInf.xbuf, SInf.ybuf);
#if !_NEWTRODIT_OLD_SUPPORT
	SetAltConsoleBuffer(false);
#endif
	exit(retval);
}

/* Check if a file exists (will return 0 if file exists) */
int CheckFile(char *filename)
{
	return (((access(filename, 0)) != -1 && (access(filename, 6)) != -1)) ? 0 : 1;
}

/* ================================ END OF SYSTEM  =============================== */

char *itoa_n(int n)
{
	char *s = malloc(DEFAULT_ALLOC_SIZE);
	itoa(n, s, 10);

	return s;
}

char *lltoa_n(long long n) // https://stackoverflow.com/a/18858248/12613647
{

	static char buf[256] = {0};

	int i = 62, base = 10;
	int sign = (n < 0);
	if (sign)
		n = -n;

	if (n == 0)
		return "0";

	for (; n && i; --i, n /= base)
	{
		buf[i] = "0123456789abcdef"[n % base];
	}

	if (sign)
	{
		buf[i--] = '-';
	}
	return &buf[i + 1];
}

char *ProgInfo()
{
	char *info = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	char *lcl = " ";

#ifndef _WIN32
	lcl = "-LCL ";
#endif
	snprintf(info, DEFAULT_ALLOC_SIZE * sizeof(char), "Newtrodit%s%s [Built at %s %s]", lcl, newtrodit_version, newtrodit_build_date, __TIME__);
	return info;
}

/* Check if the Terminal is a valid size to use Newtrodit */
int ValidSize()
{
	int min_x = 60;
	int min_y = 6;
	if (XSIZE < min_x || min_y < 6) // Check for console size
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		WriteLogFile("%s", NEWTRODIT_ERROR_WINDOW_TOO_SMALL);
		last_known_exception = NEWTRODIT_ERROR_WINDOW_TOO_SMALL;
		SetConsoleSize(min_x + 5, min_y + 3, GetConsoleInfo(XBUFFER_SIZE), GetConsoleInfo(YBUFFER_SIZE));

		return 0;
	}
	return 1;
}

int YesNoPrompt()
{
	int chr = 0;
	while (tolower(chr) != 'y')
	{
		chr = getch_n();
		if (tolower(chr) == 'n' || chr == ESC)
		{
			return 0;
		}
	}
	return 1;
}

char *rot13(char *s)
{
	while (*s)
	{
		if (isalpha(*s))
		{
			tolower(*s) >= 'n' ? (*s -= 13) : (*s += 13);
		}
		s++;
	}
	return s;
}

int BufferLimit(File_info *tstack)
{
	if (tstack->xpos >= tstack->bufx || tstack->ypos >= tstack->bufy)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;
		PrintBottomString("%s", NEWTRODIT_ERROR_OUT_OF_MEMORY);
		getch_n();
		return 1;
	}
	return 0;
}

void *realloc_n(void *old, size_t old_sz, size_t new_sz)
{
	void *new = malloc(new_sz);
	if (!new)
	{
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;

		return old;
	}
	memcpy(new, old, old_sz);
	free(old);
	return new;
}

char *ErrorMessage(int err, const char *filename)
{
	switch (err)
	{
	case ENOMEM:
		last_known_exception = NEWTRODIT_ERROR_ALLOCATION_FAILED;
		return NEWTRODIT_ERROR_ALLOCATION_FAILED;
		break;
	case EACCES:
		last_known_exception = NEWTRODIT_FS_ACCESS_DENIED;
		return join(NEWTRODIT_FS_ACCESS_DENIED, filename);
		break;
	case ENOENT:
		last_known_exception = NEWTRODIT_FS_FILE_NOT_FOUND;

		return join(NEWTRODIT_FS_FILE_NOT_FOUND, filename);
		break;
	case EFBIG:
		last_known_exception = NEWTRODIT_FS_FILE_TOO_LARGE;

		return join(NEWTRODIT_FS_FILE_TOO_LARGE, filename);
		break;
	case EINVAL:
		last_known_exception = NEWTRODIT_FS_FILE_INVALID_NAME;

		return join(NEWTRODIT_FS_FILE_INVALID_NAME, filename);
		break;
	case EMFILE:
		last_known_exception = NEWTRODIT_ERROR_TOO_MANY_FILES_OPEN;
		return NEWTRODIT_ERROR_TOO_MANY_FILES_OPEN;
		break;
	case ENAMETOOLONG:

		last_known_exception = NEWTRODIT_FS_FILE_NAME_TOO_LONG;

		return join(NEWTRODIT_FS_FILE_NAME_TOO_LONG, filename);
		break;
	case ENOSPC:
		last_known_exception = NEWTRODIT_FS_DISK_FULL;

		return NEWTRODIT_FS_DISK_FULL;
		break;
	case EILSEQ:
		last_known_exception = NEWTRODIT_ERROR_INVALID_UNICODE_SEQUENCE;

		return NEWTRODIT_ERROR_INVALID_UNICODE_SEQUENCE;
		break;
	case EISDIR:
		last_known_exception = NEWTRODIT_FS_IS_A_DIRECTORY;
		return join(NEWTRODIT_FS_IS_A_DIRECTORY, filename);
	default:
		last_known_exception = NEWTRODIT_CRASH_UNKNOWN_EXCEPTION;

		return NEWTRODIT_ERROR_UNKNOWN;
		break;
	}
}

// Get error description from dwError, which should be a call to GetLastError()
LPSTR GetErrorDescription(DWORD dwError)
{
	LPSTR lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);
	lpMsgBuf[strcspn(lpMsgBuf, "\r\n")] = '\0';
	return lpMsgBuf;
}

void StartProcess(char *command_line)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pinf;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pinf, sizeof(pinf));
	if (!CreateProcess(NULL, command_line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pinf))
	{
		WriteLogFile("%s%s", NEWTRODIT_ERROR_FAILED_PROCESS_START, command_line);
		PrintBottomString("%s", NEWTRODIT_ERROR_FAILED_PROCESS_START);
		MessageBox(0, join(NEWTRODIT_ERROR_FAILED_PROCESS_START, GetErrorDescription(GetLastError())), "Error", MB_ICONERROR);
	}
	CloseHandle(pinf.hProcess);
	CloseHandle(pinf.hThread);
	return;
}

void SetTitle(char *s, ...)
{
	va_list args;
	va_start(args, s);
	char *title = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	vsnprintf(title, sizeof(char) * DEFAULT_ALLOC_SIZE, s, args);
	SetConsoleTitle(title);
	va_end(args);
	free(title);
}

char *get_path_directory(char *path, char *dest) // Not a WinAPI function
{
	memcpy(dest, path, strlen_n(path));

	int tmp_int = TokLastPos(path, PATHTOKENS);

	if (tmp_int != -1)
	{

		for (int i = 0; i < strlen_n(PATHTOKENS); i++)
		{
			if (dest[strlen_n(dest) - 1] != PATHTOKENS[i])
			{
				dest[strlen_n(dest)] = PATHTOKENS[i];
			}
		}

		memset(dest + tmp_int + 1, 0, MAX_PATH - tmp_int);

		return dest;
	}
	else
	{
		return NULL;
	}
}

void print_box_char()
{
	printf("\u2610");
}

char *GetLogFileName()
{
	SYSTEMTIME lt;

	GetLocalTime(&lt);

	char *buf = calloc(sizeof(char), MAX_PATH + 1);
	char *dirloc = calloc(sizeof(char), MAX_PATH + 1);
	GetModuleFileName(NULL, buf, MAX_PATH);

	get_path_directory(buf, dirloc);

	snprintf(buf, MAX_PATH, "%snewtrodit.log", dirloc);
	if (createNewLogFile)
	{
		remove(buf);
	}
	free(dirloc);

	return buf;
}

/*
 *   I've put some of the bigger functions here.
 * Makes scrolling through the code much easier
 */

int GetConsoleInfo(int type)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CONSOLE_CURSOR_INFO cci;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi) || !GetConsoleCursorInfo(hConsole, &cci))
	{
		return -1;
	}
	switch (type)
	{
	// Console width in cells
	case XWINDOW:
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;

	// Console height in cells
	case YWINDOW:
		return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	// Buffer width in cells
	case XBUFFER_SIZE:
		return csbi.dwSize.X;

	// Buffer height in cells
	case YBUFFER_SIZE:
		return csbi.dwSize.Y;

	// Cursor's X position
	case XCURSOR:
		return csbi.dwCursorPosition.X;

	// Cursor's Y position
	case YCURSOR:
		return csbi.dwCursorPosition.Y;

	// Current console color
	case COLOR:
		return csbi.wAttributes;

	// Console's number of columns -- May be wrong
	case XMAX_WINDOW:
		return csbi.dwMaximumWindowSize.X;

	// Console's number of rows -- May be wrong
	case YMAX_WINDOW:
		return csbi.dwMaximumWindowSize.Y;

	// Cursor's size - From 0 (invisible) to 100 (full cell)
	case CURSOR_SIZE:
		return cci.dwSize;

	// Returns true or false depending on cursor visibility
	case CURSOR_VISIBLE:
		return cci.bVisible;

	default:
		return 1;
	}
}

int AllocateBufferMemory(File_info *tstack)
{
	WriteLogFile("Allocating buffer memory");
	BUFFER_X = DEFAULT_BUFFER_X; // TODO: Remove BUFFER_X variable
	tstack->bufx = BUFFER_X;
	tstack->bufy = BUFFER_Y;

	tstack->strsave = calloc(sizeof(char *), tstack->bufy);
	tstack->tabcount = calloc(sizeof(int *), tstack->bufy);
	tstack->linesize = calloc(sizeof(size_t *), tstack->bufy);

	tstack->utf8 = generalUtf8Preference;
	for (int i = 0; i < tstack->bufy; i++)
	{
		tstack->strsave[i] = calloc(tstack->bufx, sizeof(char));
		tstack->tabcount[i] = calloc(1, sizeof(int));
		if (!tstack->strsave[i] || !tstack->tabcount[i])
		{
			last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;

			WriteLogFile("Failed to allocate buffer memory");

			return 0;
		}
	}
	tstack->Ustack = 0; // Initialize the undo stack
	tstack->newline = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	tstack->newline = strdup(DEFAULT_NL);

	tstack->Compilerinfo.path = calloc(MAX_PATH, sizeof(char));
	tstack->Compilerinfo.flags = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	tstack->Compilerinfo.output = calloc(MAX_PATH, sizeof(char));
	tstack->Compilerinfo.path = strdup(DEFAULT_COMPILER);
	tstack->Compilerinfo.flags = strdup(DEFAULT_COMPILER_FLAGS);

	tstack->Syntaxinfo.syntax_lang = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	tstack->Syntaxinfo.syntax_file = calloc(MAX_PATH, sizeof(char));
	tstack->Syntaxinfo.separators = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	tstack->Syntaxinfo.comments = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	tstack->Syntaxinfo.enclosing_char = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));

	tstack->Syntaxinfo.syntax_file = "Default syntax highlighting";
	tstack->Syntaxinfo.num_color = DEFAULT_NUM_COLOR;
	tstack->Syntaxinfo.capital_color = DEFAULT_CAPITAL_COLOR;
	tstack->Syntaxinfo.capital_min = DEFAULT_CAPITAL_MIN_LEN;
	tstack->Syntaxinfo.capital_enabled = capitalMinEnabled;
	tstack->Syntaxinfo.override_color = 0; // No override color
	tstack->Syntaxinfo.single_quotes = singleQuotes;
	tstack->Syntaxinfo.finish_quotes = finishQuotes;

	tstack->Syntaxinfo.quote_color = DEFAULT_QUOTE_COLOR;
	tstack->Syntaxinfo.default_color = DEFAULT_SYNTAX_COLOR;
	tstack->Syntaxinfo.comment_color = DEFAULT_COMMENT_COLOR;
	tstack->Syntaxinfo.keyword_count = sizeof(keywords) / sizeof(keywords[0]);
	tstack->Syntaxinfo.comment_count = sizeof(comments) / sizeof(comments[0]);
	tstack->Syntaxinfo.enclosing_count = sizeof(enclosing) / sizeof(enclosing[0]);

	tstack->Syntaxinfo.keywords = calloc(sizeof(keywords) / sizeof(keywords[0]), sizeof(char *));
	tstack->Syntaxinfo.comments = calloc(sizeof(comments) / sizeof(comments[0]), sizeof(char *));
	tstack->Syntaxinfo.color = calloc(sizeof(keywords) / sizeof(keywords[0]), sizeof(int));

	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
	{
		tstack->Syntaxinfo.keywords[i] = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
		tstack->Syntaxinfo.keywords[i] = keywords[i].keyword;
		tstack->Syntaxinfo.color[i] = keywords[i].color;
	}

	for (int i = 0; i < sizeof(comments) / sizeof(comments[0]); i++)
	{
		tstack->Syntaxinfo.comments[i] = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
		tstack->Syntaxinfo.comments[i] = comments[i].keyword;
	}
	for (int i = 0; i < sizeof(enclosing) / sizeof(enclosing[0]); i++)
	{
		tstack->Syntaxinfo.enclosing_char[i] = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
		tstack->Syntaxinfo.enclosing_char[i] = enclosing[i].keyword;
	}

	tstack->Syntaxinfo.multi_line_comment = false;
	tstack->Syntaxinfo.bracket_pair_count = 0;
	tstack->Syntaxinfo.parenthesis_pair_count = 0;
	tstack->Syntaxinfo.square_bracket_pair_count = 0;
	tstack->linecount_wide = LINECOUNT_WIDE;
	tstack->filename = calloc(MAX_PATH, sizeof(char));
	tstack->language = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	if (!tstack->filename || !tstack->language)
	{
		WriteLogFile("Failed to allocate buffer memory");
		last_known_exception = NEWTRODIT_ERROR_OUT_OF_MEMORY;

		return 0;
	}
	tstack->filename = strdup(filename_text_);
	tstack->is_untitled = true;
	tstack->is_modified = false;
	tstack->is_saved = false;
	tstack->is_loaded = false;
	tstack->xpos = 0;
	tstack->ypos = 1;
	tstack->linecount = 1; // Only 1 line
	tstack->display_x = tstack->xpos;
	tstack->display_y = tstack->ypos;
	tstack->scrolled_x = false;
	tstack->scrolled_y = false;
	tstack->last_pos_scroll = 0;
	tstack->last_dispy = 1;
	tstack->last_y = 1;
	tstack->is_readonly = false;
	tstack->hFile = INVALID_HANDLE_VALUE;
	// Initialize the file time structures
	tstack->fwrite_time.dwHighDateTime = 0;
	tstack->fwrite_time.dwLowDateTime = 0;
	tstack->fread_time.dwHighDateTime = 0;
	tstack->fread_time.dwLowDateTime = 0;
	tstack->language = calloc(DEFAULT_ALLOC_SIZE, sizeof(char));
	memcpy(tstack->language, DEFAULT_LANGUAGE, strlen_n(DEFAULT_LANGUAGE));
	memcpy(tstack->Syntaxinfo.separators, SEPARATORS, strlen_n(SEPARATORS));
	memcpy(tstack->Syntaxinfo.syntax_lang, DEFAULT_SYNTAX_LANG, strlen_n(DEFAULT_SYNTAX_LANG));

	tstack->selection.start.x = 0;
	tstack->selection.start.y = 0;
	tstack->selection.end.x = 0;
	tstack->selection.end.y = 0;

	tstack->selection.is_selected = false;
	used_tab_indexes[file_index] = true; // TODO: archaic code, should be removed

	WriteLogFile("Buffer memory successfully allocated");
	WriteLogFile("Allocated initial address of strsave[3]: %p", Tab_stack[file_index].strsave[3]);

	return 1;
}

int FreeBufferMemory(File_info *tstack)
{
	WriteLogFile("Freeing buffer memory for file %s", tstack->filename);

	for (int i = 0; i < tstack->bufy; i++)
	{
		free(tstack->strsave[i]);
		free(tstack->tabcount[i]);
	}

	free(tstack->strsave);
	free(tstack->tabcount);
	free(tstack->linesize);

	free(tstack->newline);

	free(tstack->Compilerinfo.path);
	free(tstack->Compilerinfo.flags);
	free(tstack->Compilerinfo.output);

	free(tstack->Syntaxinfo.syntax_lang);
	free(tstack->Syntaxinfo.syntax_file);
	free(tstack->Syntaxinfo.separators);
	free(tstack->Syntaxinfo.comments);

	for (int i = 0; i < tstack->Syntaxinfo.keyword_count; i++)
	{
		free(tstack->Syntaxinfo.keywords[i]);
	}

	for (int i = 0; i < tstack->Syntaxinfo.comment_count; i++)
	{
		free(tstack->Syntaxinfo.comments[i]);
	}

	free(tstack->Syntaxinfo.keywords);
	free(tstack->Syntaxinfo.color);
	free(tstack->Syntaxinfo.comments);

	free(tstack->filename);
	free(tstack->language);
	free(tstack->hFile);
	WriteLogFile("Buffer memory successfully freed");

	return 1;
}