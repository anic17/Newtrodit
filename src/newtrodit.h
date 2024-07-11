#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>
#include <locale.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <utf8/utf8.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

const int DEFAULT_ALLOC_SIZE = 512;

#if !defined ENABLE_VIRTUAL_TERMINAL_PROCESSING || !defined DISABLE_NEWLINE_AUTO_RETURN
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

enum file_flags_modifier
{
	IS_MODIFIED = 1,
	IS_SAVED = 2,
	IS_UNTITLED = 4,
	IS_READONLY = 8,
};

const char newtrodit_version[] = "1.0";
const char newtrodit_date[] = "2024/06/10";

typedef struct Line
{
	char *str; // Buffer that contains the line
	char *render;

	size_t bufx;		// Allocated line size
	size_t render_bufx; // Allocated render size

	size_t len;	 // Line Length
	size_t rlen; // Rendered length

	size_t ulen; // UTF-8 length
} Line;

typedef struct Position
{
	int x;
	int y;
} Position;

typedef struct Select
{
	Position start;
	Position end;
	bool is_selected;
} Select;

typedef struct File
{
	char *filename;
	char *fullpath;
	char *language; // Language (e.g: PowerShell, C, C++, etc.)

	// File information
	size_t xpos, ypos;
	size_t uxpos; // UTF-8 x position

	size_t display_start;
	size_t display_end;

	Line **line; // Store each line file
	size_t alloc_lines;
	size_t linecount;
	long long size;

	size_t linenumber_wide;
	size_t linenumber_padding;

	char *newline;
	unsigned int file_flags;

	time_t fwrite_time;
	time_t fread_time;
	Select selection;
} File; // File information. This is used to store all the information about the file.

typedef struct Editor
{
	int xsize, ysize; // Window size
	int open_files;	  // Number of files open
	int file_index;	  // Current file index

	char *log_file_name;
	bool useLogFile;
	bool lineNumbers;
	bool dirty;
	char *status_msg;

} Editor; // Editor information

typedef struct Color
{
	int r, g, b;
	bool background;
} Color;

int end_editor();
int init_editor();

#ifdef _WIN32
#include "win32/graphics_win32.c"
#else
#include "linux/graphics_linux.c"
#endif

#define _xpos file[ed.file_index]->xpos
#define _ypos file[ed.file_index]->ypos
#define _uxpos file[ed.file_index]->uxpos

size_t utf8len_n(char *s)
{
	if (!s)
		return 0;
	return utf8len(s);
}
size_t utf8nlen_n(char *s, size_t n)
{
	if (!s)
		return 0;
	return utf8nlen(s, n);
}

size_t strlen_n(char *s)
{
	if (!s)
		return 0;
	return strlen(s);
}

int vt_settings(bool enabled)
{
#ifdef _WIN32
	DWORD lmode; // Process ANSI escape sequences

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
	return 0;
}

/* Safe version of strcpy() and strncpy() */
char *utf8ncpy_n(char *dest, const char *src, size_t count)
{
	// Better version that utf8ncpy() because it always null terminates strings

	if (count)
	{
		memset(dest, 0, count);
		utf8ncat(dest, src, count);
		return dest;
	}
	return NULL;
}

size_t strrpbrk(char *s, char *find) // Reverse strpbrk, just like strrchr but for multiple characters
{
	size_t findlen = utf8len_n(find);
	size_t slen = utf8len_n(s);
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

int valid_file_name(char *filename)
{
	return utf8pbrk(filename, "*?\"<>|\x1b") == NULL;
}

char *remove_quotes(char *s)
{
	size_t len = utf8len_n(s);
	if (s[0] == L'\"' && s[len - 1] == L'\"')
	{
		memmove(s, s + 1, len - 2);
		s[len] = L'\0';
	}
	return s;
}