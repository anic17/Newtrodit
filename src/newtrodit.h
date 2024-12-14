#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "../include/utf8/utf8.h"
#include "../include/wcwidth.h"

#ifdef _WIN32
#include <conio.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#include "dialog.h"
#include "globals.h"

const int DEFAULT_ALLOC_SIZE = 512;
const size_t LINE_SIZE = 64;
const size_t DEFAULT_ALLOC_LINES = 10;

const size_t LINE_Y_INCREASE = 5;

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

enum unicode_encodings
{
	ENCODING_UTF8 = 1,
	ENCODING_UTF16LE = 2,
	ENCODING_UTF16BE = 3,
	ENCODING_UTF8BOM = 4,
	ENCODING_UTF32LE = 5,
	ENCODING_UTF32BE = 6,
};

const char newtrodit_version[] = "1.0";
const char newtrodit_date[] = "2024/09/05";

typedef struct Line
{
	char *str; // Buffer that contains the line
	char *render;

	size_t bufx;		// Allocated line size
	size_t render_bufx; // Allocated render size

	size_t len;	 // Line Length
	size_t rlen; // Rendered length
	size_t rnlen; // Rendered length without padding

	size_t ulen; // UTF-8 length
} Line;

typedef struct Position
{
	size_t x;
	size_t y;
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
	size_t uxpos;  // UTF-8 X position
	size_t uwxpos; // UTF-8 width position

	Line **line; // Store each line file
	size_t alloc_lines;
	size_t linecount;
	long long size;

	size_t linenumber_wide;
	size_t linenumber_padding;

	char *newline;
	unsigned int file_flags;

	unsigned int encoding;
	size_t encoding_bom_len;
	bool post_load_rendering;

	time_t fwrite_time;
	time_t fread_time;
	Select selection;
	Position begin_display;
	Position scroll_pos;
} File; // File information. This is used to store all the information about the file.

typedef struct Editor
{
	size_t xsize, ysize; // Window size
	int open_files;		 // Number of files open
	int file_index;		 // Current file index

	char *log_file_name;
	char *status_msg;
	bool useLogFile;
	bool lineNumbers;
	bool dirty;
	bool displayStatusOnce;

} Editor; // Editor information

typedef struct Color
{
	int r, g, b;
	bool background;
} Color;

Editor ed;
File **file;

int end_editor();
int init_editor();
Line *create_line(File *tstack, size_t ypos);
int codepoint_width(const char *utf8_char, utf8_int32_t val);

#ifdef _WIN32
#include "win32/graphics_win32.c"
const char PATHDELIMS[] = "\\/"; // Set the path delimiters for Windows and Linux
#else
#include "linux/graphics_linux.c"
const char PATHDELIMS[] = "/";
#endif

#define _xpos file[ed.file_index]->xpos
#define _ypos file[ed.file_index]->ypos
#define _uxpos file[ed.file_index]->uxpos
#define _uwxpos file[ed.file_index]->uwxpos

size_t utf8len_n(const char *s)
{
	if (!s)
		return 0;
	return utf8len(s);
}

size_t utf8nlen_n(const char *s, size_t n)
{
	if (!s)
		return 0;
	return utf8nlen(s, n);
}

size_t utf8len_null(char *s, size_t max_bytes) // Get the number of UTF-8 characters in a non-null terminated string with a known byte length
{
    if (!s)
        return 0;
    char *ptr = s;
    utf8_int32_t cp=0;
    size_t ulen = 0;
    while (*ptr != '\0' && ulen < max_bytes)
    {
        ptr = utf8codepoint(ptr, &cp); // Get a pointer to the next codepoint
        ulen++; // Increase the Unicode character counter
    }
    return ulen;
}

size_t strlen_n(const char *s)
{
	if (!s)
		return 0;
	return strlen(s);
}

int vt_settings(bool enabled)
{
#ifdef _WIN32
	DWORD lmode; // Process ANSI escape sequences

	if (!GetConsoleMode(hStdout, &lmode))
		return 0;
	if (enabled)
		lmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING & ~DISABLE_NEWLINE_AUTO_RETURN;
	else
		lmode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;

	return SetConsoleMode(hStdout, lmode);
#endif
	return 0;
}

/* Safe version of strcpy() and strncpy() */
char *strncpy_n(char *dest, const char *src, size_t count)
{
	// Better version that str8ncpy() because it always null terminates strings

	if (count)
	{
		memset(dest, 0, count);
		strncat(dest, src, count);
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
				return i - 1;
		}
	}
	return 0;
}

void *realloc_n(void *old, size_t old_sz, size_t new_sz)
{
	void *new = malloc(new_sz);
	if (!new)
		return NULL;
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
	size_t len = strlen_n(s);
	if (s[0] == '\"' && s[len - 1] == '\"')
	{
		memmove(s, s + 1, len - 2);
		s[len] = '\0';
	}
	return s;
}

int trim_message(char *msg, size_t max_len)
{
	size_t ulen = utf8len(msg);
	if (ulen > max_len)
	{
		msg[max_len] = '\0';
		return 1;
	}
	return 0;
}

int set_status_msg(bool display_once, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	ed.status_msg = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(utf8_int32_t));
	if (!ed.status_msg)
		return 0;

	size_t copyamount = DEFAULT_ALLOC_SIZE * sizeof(utf8_int32_t);
	vsnprintf(ed.status_msg, copyamount, msg, args);
	trim_message(ed.status_msg, ed.xsize - (fullCursorInfoDisplay ? 45 : 20));

	ed.dirty = true;
	ed.displayStatusOnce = display_once;
	return 1;
}

int clear_status_msg()
{
	if (ed.status_msg != NULL)
		free(ed.status_msg);

	ed.status_msg = NULL;
	ed.dirty = true;
	return 1;
}

int last_token_position(char *s, const char *token)
{

	int lastpos = -1;
	if (!s || !token)
		return lastpos;

	for (size_t i = 0; i < strlen_n(s); i++)
	{
		for (size_t j = 0; j < strlen_n(token); j++)
		{
			if (s[i] == token[j])
				lastpos = i;
		}
	}
	return lastpos;
}

char *last_token(char *tok, const char *char_token)
{
	if (!tok || !char_token)
		return tok;

	int pos = last_token_position(tok, char_token);
	return tok + pos + 1;
}

int allocate_buffer(File **tstack)
{
	*tstack = calloc(1, sizeof(File));
	(*tstack)->file_flags = IS_UNTITLED;

	(*tstack)->filename = calloc(MAX_PATH * sizeof(utf8_int32_t) + 1, sizeof(char));
	memcpy((*tstack)->filename, default_filename, MAX_PATH);

	(*tstack)->fwrite_time = time(NULL);
	(*tstack)->fread_time = time(NULL);
	(*tstack)->language = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(char));
	memcpy((*tstack)->language, default_language, utf8len_n(default_language));
	(*tstack)->linenumber_wide = 3;
	(*tstack)->linenumber_padding = 1;
	(*tstack)->linecount = 0;
	(*tstack)->xpos = 0;
	(*tstack)->ypos = 1;
	(*tstack)->size = 0;
	(*tstack)->encoding = ENCODING_UTF8;
	(*tstack)->encoding_bom_len = 0;
	(*tstack)->post_load_rendering = true;

	(*tstack)->line = calloc(DEFAULT_ALLOC_LINES, sizeof(Line *));

	for (size_t i = 0; i <= DEFAULT_ALLOC_LINES; i++)
		create_line(*tstack, i);

	(*tstack)->line[0]->str = " Illegal row number. Report this issue to the GitHub repository."; // If for some reason line 0 is accessed

	(*tstack)->newline = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(char));
	memcpy((*tstack)->newline, default_newline, utf8len_n(default_newline));

	return 1;
}

int free_buffer(File **tstack)
{

	for (size_t i = 1; i < (*tstack)->alloc_lines; i++)
	{
		free((*tstack)->line[i]->render);
		free((*tstack)->line[i]->str);
		free((*tstack)->line[i]);
	}

	// free((*tstack)->line);

	free((*tstack)->newline);
	free((*tstack)->filename);
	free((*tstack)->language);

	/*     free((*tstack)->compilerinfo.path);
		free((*tstack)->compilerinfo.flags);
		free((*tstack)->compilerinfo.output);

		free((*tstack)->syntaxinfo.syntax_lang);
		free((*tstack)->syntaxinfo.syntax_file);
		free((*tstack)->syntaxinfo.separators);
		free((*tstack)->syntaxinfo.comments);

		for (int i = 0; i < (*tstack)->syntaxinfo.keyword_count; i++)
		{
			free((*tstack)->syntaxinfo.keywords[i]);
		}

		for (int i = 0; i < (*tstack)->syntaxinfo.comment_count; i++)
		{
			free((*tstack)->syntaxinfo.comments[i]);
		}

		free((*tstack)->syntaxinfo.keywords);
		free((*tstack)->syntaxinfo.color);
		free((*tstack)->syntaxinfo.comments); */
	return 1;
}
