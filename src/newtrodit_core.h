#ifndef NEWTRODIT_CORE_H
#define NEWTRODIT_CORE_H
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
#include "include/wcwidth.h"
#include "utf8/utf8.h"

#ifdef _WIN32
#include <conio.h>
#endif


extern const int DEFAULT_ALLOC_SIZE;
extern const size_t LINE_SIZE;
extern const size_t DEFAULT_ALLOC_LINES;

extern const size_t LINE_Y_INCREASE;

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

    size_t bufx;        // Allocated line size
    size_t render_bufx; // Allocated render size

    size_t len;   // Line Length
    size_t rlen;  // Rendered length
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
    int open_files;      // Number of files open
    int file_index;      // Current file index

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

#ifdef _WIN32
#include "win32/graphics_win32.h"
const char PATHDELIMS[] = "\\/"; // Set the path delimiters for Windows and Linux
#else
#include "linux/graphics_linux.h	"
const char PATHDELIMS[] = "/";
#endif

#define _xpos file[ed.file_index]->xpos
#define _ypos file[ed.file_index]->ypos
#define _uxpos file[ed.file_index]->uxpos
#define _uwxpos file[ed.file_index]->uwxpos

size_t utf8len_n(const char *s);
size_t utf8nlen_n(const char *s, size_t max_upos);
size_t utf8len_null(const char *s, size_t max_bytes); // Get the number of UTF-8 characters in a non-null terminated string with a known byte length
size_t strlen_n(const char *s);
int vt_settings(bool enabled);
char *strncpy_n(char *dest, const char *src, size_t count);
size_t strrpbrk(const char *s, const char *find); // Reverse strpbrk, just like strrchr but for multiple characters
void *realloc_n(void *old, size_t old_sz, size_t new_sz);
int valid_file_name(const char *filename);
char *remove_quotes(char *s);
int trim_message(char *msg, size_t max_len);
int set_status_msg(bool display_once, char *msg, ...);
int clear_status_msg();
int last_token_position(const char *s, const char *token);
char *last_token(char *tok, const char *char_token);

#endif // NEWTRODIT_CORE_H