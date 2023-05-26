/*
    Newtrodit: A console text editor
    Copyright (C) 2021-2023 anic17 Software

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

const char newtrodit_version[] = "0.6 rc-1";
const char newtrodit_build_date[] = "18/2/2023";
const char newtrodit_repository[] = "https://github.com/anic17/Newtrodit";
const char newtrodit_lcl_repository[] = "https://github.com/anic17/Newtrodit-LCL";
char manual_file[MAX_PATH] = "newtrodit.man";
char settings_file[MAX_PATH] = "newtrodit.config";
char syntax_dir[MAX_PATH] = "syntax";
char syntax_ext[MAX_PATH] = ".nwtrd-syntax";
const char newtrodit_commit[] = ""; // Example commit

#define DEFAULT_NL "\n"

const int MANUAL_BUFFER_X = 300;
const int MANUAL_BUFFER_Y = 600;

int TAB_WIDE = TAB_WIDE_;
int CURSIZE = CURSIZE_;
int LINECOUNT_WIDE = LINECOUNT_WIDE_;
int lineCount = true; // Is line count enabled?


// Boolean settings
int convertTabtoSpaces = true;
int convertNull = true;
int trimLongLines = false;
int cursorSizeInsert = true;
int wrapLine = false;
int autoIndent = true;
int fullPathTitle = true;
int useOldKeybindings = false; // Bool to use old keybinds (^X instead of ^Q, ^K instead of ^X)
int longPositionDisplay = false;
int fullCursorInfoDisplay = true;
int generalUtf8Preference = false;
int partialMouseSupport = true;     // Partial mouse support, only changes cursor position when mouse is clicked
int showMillisecondsInTime = false; // Show milliseconds in time insert function (F6)
int useLogFile = true;
int createNewLogFile = false; // Create new log files when logging is enabled
int openFileHandle = true; // Open a file handle
int RGB24bit = false;          // Use 24-bit RGB instead of 4-bit colors
int findInsensitive = false; // Find insensitive string
int matchWholeWord = false; // Match whole word when finding a string
int devMode = false;  // Bool to enable or disable the dev mode
int autoComplete = true; // Keyword autocompleting (if syntax highlighting is there)

int bpsPairHighlight = false; // Use BPS pair highlighting (bugged)

int bps_pair_colors[] = {0x5, 0xb, 0xd, 0x6, 0xc, 0xe, 0x8, 0xf, 0x9}; // Brackets, parenthesis and square brackets colors
char bps_chars_open[10][3] = {"(", "{", "["};                          // Allocate 10 chars for each char array to support new syntax
char bps_chars_close[10][3] = {")", "}", "]"};

// Interal global variables
int clearAllBuffer = true;
int allowAutomaticResizing = false; // Changing this value can cause issues with the editor functions

int c = 0; // Different error/debug codes

/*
    TODO: Add multiline comment support
    static int multiLineComment = false;
*/

int syntaxHighlighting = true;
int syntaxAfterDisplay = false; // Display syntax after display
int allocateNewBuffer = true;

int file_index = 0;
int open_files = 1;

int wrapSize = 100; // Default wrap size
int scrollRate = 3; // Default scroll rate (scroll 3 lines per mouse wheel). Needs to have partialMouseSupport enabled

#define BG_DEFAULT 0x07 // Background black, foreground (font) white
#define FG_DEFAULT 0x70 // Background white, foreground (font) black
#define FIND_HIGHLIGHT_COLOR 0xE0 // Background yellow, foreground black

int bg_color = BG_DEFAULT; // Background color (menu)
int fg_color = FG_DEFAULT; // Foreground color (text)

char **old_open_files;
int oldFilesIndex = 0;

char *run_macro, *last_known_exception;

#define DEFAULT_COMPILER "gcc"
#define DEFAULT_COMPILER_FLAGS "-Wall -O2 -pedantic -std=c99" // Assuming it's GCC and it's C99 compliant

// Syntax highlighting

#define DEFAULT_SYNTAX_LANG "C"
#define DEFAULT_SEPARATORS "?!:,()+-/*=~[];{}<> \t&|^."
#define DEFAULT_COMMENTS "//"

#define DEFAULT_SYNTAX_COLOR 0x7  // White
#define DEFAULT_COMMENT_COLOR 0x8 // Dark grey
#define DEFAULT_QUOTE_COLOR 0xe   // Yellow

#define DEFAULT_NUM_COLOR 0x2     // Dark green
#define DEFAULT_CAPITAL_COLOR 0xc // Red
#define DEFAULT_CAPITAL_MIN_LEN 3 // Highlight capital words that are 3 or more characters long
#define DEFAULT_LINECOUNT_COLOR 0x80
#define DEFAULT_LINECOUNT_HIGHLIGHT_COLOR 0xf0

#define SELECTION_COLOR 0x8

#define SEPARATORS DEFAULT_SEPARATORS

char syntax_separators[512] = SEPARATORS;
char syntax_filename[MAX_PATH] = "";

int default_color = DEFAULT_SYNTAX_COLOR;
int comment_color = DEFAULT_COMMENT_COLOR;
int quote_color = DEFAULT_QUOTE_COLOR;
int num_color = DEFAULT_NUM_COLOR;
int capital_color = DEFAULT_CAPITAL_COLOR;
int capital_min_len = DEFAULT_CAPITAL_MIN_LEN;
int linecount_color = DEFAULT_LINECOUNT_COLOR;
int linecount_highlight_color = DEFAULT_LINECOUNT_HIGHLIGHT_COLOR;
int capitalMinEnabled = true;
int singleQuotes = true;
int finishQuotes = true;
int linecountHighlightLine = true;
int autoLoadSyntaxRules = true;

typedef struct position {
    int x;
    int y;
} position_t;

typedef struct select_t {
    position_t start;
    position_t end;
    bool is_selected;
} select_t;

typedef struct keywords
{
    char *keyword;
    int color;
} keywords_t;

typedef struct comment
{
    char *keyword;
    int color;
} comment_t;

keywords_t keywords[] = {
    {"break", 5},
    {"continue", 5},
    {"return", 5},

    {"auto", 9},
    {"const", 9},
    {"volatile", 9},
    {"extern", 9},
    {"static", 9},

    {"inline", 9},
    {"restrict", 9},

    {"char", 9},
    {"int", 9},
    {"short", 9},
    {"float", 9},
    {"double", 9},
    {"long", 9},
    {"bool", 9},
    {"void", 9},

    {"double_t", 0xa},
    {"div_t", 0xa},
    {"float_t", 0xa},
    {"fpos_t", 0xa},
    {"max_align_t", 0xa},
    {"mbstate_t", 0xa},
    {"nullptr_t", 0xa},
    {"ptrdiff_t", 0xa},
    {"sig_atomic_t", 0xa},
    {"size_t", 0xa},
    {"time_t", 0xa},
    {"wchar_t", 0xa},
    {"wint_t", 0xa},
    {"FILE", 0xa},

    {"uint8_t", 0xa},
    {"uint16_t", 0xa},
    {"uint32_t", 0xa},
    {"uint64_t", 0xa},
    {"uint128_t", 0xa},

    {"int8_t", 0xa},
    {"int16_t", 0xa},
    {"int32_t", 0xa},
    {"int64_t", 0xa},
    {"int128_t", 0xa},

    {"struct", 6},
    {"enum", 6},
    {"union", 6},
    {"typedef", 6},
    {"unsigned", 9},
    {"signed", 9},
    {"sizeof", 9},
    {"register", 9},

    {"do", 6},
    {"if", 6},
    {"else", 6},
    {"while", 6},
    {"switch", 6},
    {"for", 6},
    {"case", 6},
    {"default", 6},
    {"goto", 6},

    {"#include", 0xb},
    {"#pragma", 0xb},
    {"#define", 0xb},
    {"#ifdef", 0xb},
    {"#undef", 0xb},

    {"#ifndef", 0xb},
    {"#endif", 0xb},
    {"#if", 0xb},
    {"#else", 0xb},
    {"#elif", 0xb},
    {"#error", 0xb},
    {"#warning", 0xb},
    {"#line", 0xb},
    {"defined", 0xb},
    {"not", 0xb},

    {"main", 0x6},
    {"WinMain", 0x6},

    // Macro constants
    {"NULL", 0x9},
    {"EOF", 0x9},
    {"WEOF", 0x9},
    {"FILENAME_MAX", 0x9},
    {"WEOF", 0x9},
    {"true", 0x9},
    {"false", 0x9},
    {"errno", 0x9},
    {"stdin", 0x9},
    {"stdout", 0x9},
    {"stderr", 0x9},
    {"__LINE__", 0x9},
    {"__TIME__", 0x9},
    {"__DATE__", 0x9},
    {"__FILE__", 0x9},

    {"__STDC__", 0x9},
    {"__STDC_VERSION__", 0x9},
    {"__STDC_HOSTED__", 0x9},
    {"__OBJC__", 0x9},
    {"__ASSEMBLER__", 0x9},

    {"__cplusplus", 0x9},

    {"WINAPI", 0x6},
    {"APIENTRY", 0x6},

    {"_stdcall", 0x6},
    {"_CRTIMP", 0x6},
    {"__CRTIMP_INLINE", 0x6},

    {"__cdecl", 0x6},
    {"__asm__", 0x6},
    {"__volatile__", 0x6},
    {"__attribute__", 0x6},
    {"__AW_SUFFIXED__", 0x6},
    {"__AW_EXTENDED__", 0x6},
    {"__NAME__", 0x6},

};

comment_t comments[] = {
    {"//", 0x8},
    {"/*", 0x8},
    {"*/", 0x8},
};

typedef struct theme
{
    int bg_color;
    int fg_color;
    int syntax_color;
    int comment_color;
    int quote_color;
    int num_color;
    int capital_color;
    int capital_min_len;
    int single_quotes;
    int capital_min_enabled;

    int linecount_color;
} theme_t;

theme_t themedark = {
    .bg_color = 0,
    .fg_color = 0x7,
    .syntax_color = 0x7,
    .comment_color = 0x8,
    .quote_color = 0xe,
    .num_color = 0x2,
    .capital_color = 0x1,
    .capital_min_len = 3,
    .single_quotes = false,
    .capital_min_enabled = true,
    .linecount_color = 0x07,
};

theme_t themelight = {
    .bg_color = 0x7,
    .fg_color = 0x0,
    .syntax_color = 0x7,
    .comment_color = 0x8,
    .quote_color = 0x6,
    .num_color = 0x2,
    .capital_color = 0x1,
    .capital_min_len = 3,
    .single_quotes = false,
    .capital_min_enabled = true,
    .linecount_color = 0x07,
};

#define BIT_ESC224 0x80000000
#define BIT_ESC0 0x40000000

/* These four functions are only used on Newtrodit-LCL */
#define ESC_BITMASK 0x80000000
#define CTRLCHAR_BITMASK 0x40000000
#define FKEYS_BITMASK 0x20000000
#define TILDE_BITMASK 0x10000000

enum CONTROL_CODES

#ifdef _WIN32
{
    BS = 8,
    TAB = 9,
    CTRLENTER = 10,
    ENTER = 13,
    CTRLBS = 127,
    ESC = 27,

    UP = 72,
    LEFT = 75,
    RIGHT = 77,
    DOWN = 80,

    HOME = 71,
    END = 79,

    CTRLHOME = 119,
    CTRLEND = 117,

    INS = 82,
    DEL = 83,

    CTRLA = 1,
    CTRLB = 2,
    CTRLC = 3,
    CTRLD = 4,
    CTRLE = 5,
    CTRLF = 6,
    CTRLG = 7,
    CTRLH = 8,
    CTRLI = 9,
    CTRLJ = 10,
    CTRLK = 11,
    CTRLL = 12,
    CTRLM = 13,
    CTRLN = 14,
    CTRLO = 15,
    CTRLP = 16,
    CTRLQ = 17,
    CTRLR = 18,
    CTRLS = 19,
    CTRLT = 20,
    CTRLU = 21,
    CTRLV = 22,
    CTRLW = 23,
    CTRLX = 24,
    CTRLY = 25,
    CTRLZ = 26,

    /* Different naming only happens in F11 and F12 */
    F1 = 59,
    F2 = 60,
    F3 = 61,
    F4 = 62,
    F5 = 63,
    F6 = 64,
    F7 = 65,
    F8 = 66,
    F9 = 67,
    F10 = 68,

    F11 = 133,
    F12 = 134,

    PGUP = 73,
    PGDW = 81,

    CTRLF1 = 94,
    CTRLF2 = 95,
    CTRLF3 = 96,
    CTRLF4 = 97,
    CTRLF5 = 98,
    CTRLF6 = 99,
    CTRLF7 = 100,
    CTRLF8 = 101,
    CTRLF9 = 102,
    CTRLF10 = 103,
    CTRLF11 = 137,
    CTRLF12 = 138,

    ALTF1 = 104,
    ALTF2 = 105,
    ALTF3 = 106,
    ALTF4 = 107,
    ALTF5 = 108,
    ALTF6 = 109,
    ALTF7 = 110,
    ALTF8 = 111,
    ALTF9 = 112,
    ALTF10 = 113,
    ALTF11 = 139,
    ALTF12 = 140,

    ALTINS = 162,
    ALTHOME = 151,
    ALTPGUP = 153,
    ALTDEL = 163,
    ALTEND = 159,
    ALTPGDW = 161,

    SHIFTF1 = 84,
    SHIFTF2 = 85,
    SHIFTF3 = 86,
    SHIFTF4 = 87,
    SHIFTF5 = 88,
    SHIFTF6 = 89,
    SHIFTF7 = 90,
    SHIFTF8 = 91,
    SHIFTF9 = 92,
    SHIFTF10 = 93,
    SHIFTF11 = 135,
    SHIFTF12 = 136,

    CTRLALTA = 30,
    CTRLALTB = 48,
    CTRLALTC = 46,
    CTRLALTD = 32,
    CTRLALTE = 18, // This one actually reports as 63 because on my locale it's the euro symbol.
    CTRLALTF = 33,
    CTRLALTG = 34,
    CTRLALTH = 35,
    CTRLALTI = 23,
    CTRLALTJ = 36,
    CTRLALTK = 37,
    CTRLALTL = 38,
    CTRLALTM = 50,
    CTRLALTN = 49,
    CTRLALTO = 24,
    CTRLALTP = 25,
    CTRLALTQ = 16,
    CTRLALTR = 19,
    CTRLALTS = 31,
    CTRLALTT = 20,
    CTRLALTU = 22,
    CTRLALTV = 47,
    CTRLALTW = 17,
    CTRLALTX = 45,
    CTRLALTY = 21,
    CTRLALTZ = 44,
};
#else
{
    BS = 127,
    TAB = 9,
    CTRLENTER = 10,
    ENTER = 13,
    CTRLBS = 8,

    ESC = 27,

    /*
     Used an algorithm in order to encode and fit up to 7 characters into a single int32_t
     Assuming characters are all lower than 0x80h (127d)

     The algorithm is works by shifting up to 4 characters 7 bits to the left, using 28 of 32 available bits.

     The last 3 bytes are guaranteed to be boolean data so each character fits in a single bit.
     The big endian bit is used to ensure that it is a control keybind starting with ESC.


     This way we can place 7 characters into 4 bytes, or an int32_t.

    */

    LEFT = 0xc0002200,
    UP = 0xc0002080,
    DOWN = 0xc0002100,
    RIGHT = 0xc0002180,

    INS = 0xc01f9900,
    DEL = 0xc01f9980,

    HOME = 0xc0002400,
    END = 0xc0002300,

    PGUP = 0xc01f9a80,
    PGDW = 0xc01f9b00,

    SHIFTTAB = 0xc0002d00,

    CTRLA = 1,
    CTRLB = 2,
    CTRLC = 3,
    CTRLD = 4,
    CTRLE = 5,
    CTRLF = 6,
    CTRLG = 7,
    CTRLH = 8,
    CTRLI = 9,
    CTRLJ = 10,
    CTRLK = 11,
    CTRLL = 12,
    CTRLM = 13,
    CTRLN = 14,
    CTRLO = 15,
    CTRLP = 16,
    CTRLQ = 17,
    CTRLR = 18,
    CTRLS = 19,
    CTRLT = 20,
    CTRLU = 21,
    CTRLV = 22,
    CTRLW = 23,
    CTRLX = 24,
    CTRLY = 25,
    CTRLZ = 26,

    /* Different naming is used between F1 and F4 (termios-related things) */
    F1 = 0xa0002800,
    F2 = 0xa0002880,
    F3 = 0xa0002900,
    F4 = 0xa0002980,

    F5 = 0xcfcd5880,
    F6 = 0xcfcdd880,
    F7 = 0xcfce1880,
    F8 = 0xcfce5880,
    F9 = 0xcfcc1900,
    F10 = 0xcfcc5900,
    F11 = 0xcfccd900,
    F12 = 0xcfcd1900,

    SHIFTF1 = 0xc64ed880,
    SHIFTF2 = 0xd64ed880,
    SHIFTF3 = 0xe64ed880,
    SHIFTF4 = 0xf64ed880,

    SHIFTF5 = 0xf76d5c70,
    SHIFTF6 = 0xf76ddc70,
    SHIFTF7 = 0xf76e1c70,
    SHIFTF8 = 0xf76e5c70,
    SHIFTF9 = 0xf76c1cf0,
    SHIFTF10 = 0xf76c5cf0,
    SHIFTF11 = 0xf76cdcf0,
    SHIFTF12 = 0xf76d1cf0,

    CTRLF1 = 0xc6aed880,
    CTRLF2 = 0xd6aed880,
    CTRLF3 = 0xe6aed880,
    CTRLF4 = 0xf6aed880,
    CTRLF5 = 0xd76d5c70,
    CTRLF6 = 0xd76ddc70,
    CTRLF7 = 0xd76e1c70,
    CTRLF8 = 0xd76e5c70,
    CTRLF9 = 0xd76c1cf0,
    CTRLF10 = 0xd76c5cf0,
    CTRLF11 = 0xd76cdcf0,
    CTRLF12 = 0xd76d1cf0,

    CTRLINS = 0xe6aed900,
    CTRLDEL = 0xe6aed980,
    CTRLHOME = 0xc6aed880,
    CTRLEND = 0xe6aed880,
    CTRLPGUP = 0xe6aeda80,
    CTRLPGDW = 0xe6aedb00,

    CTRLLEFT = 0xc6aed880,
    CTRLUP = 0xd6aed880,
    CTRLDOWN = 0xe6aed880,
    CTRLRIGHT = 0xf6aed880,

    SHIFTLEFT = 0xc64ed880,
    SHIFTUP = 0xd64ed880,
    SHIFTDOWN = 0xe64ed880,
    SHIFTRIGHT = 0xf64ed880,

    CTRLSHIFTLEFT = 0xc6ced880,
    CTRLSHIFTUP = 0xd6ced880,
    CTRLSHIFTDOWN = 0xe6ced880,
    CTRLSHIFTRIGHT = 0xf6ced880,

    ALTF1 = 0xc66ed880,
    ALTF2 = 0xd66ed880,
    ALTF3 = 0xe66ed880,
    ALTF4 = 0xf66ed880,
    ALTF5 = 0xf76d5c70,
    ALTF6 = 0xf76ddc70,
    ALTF7 = 0xf76e1c70,
    ALTF8 = 0xf76e5c70,
    ALTF9 = 0xf76c1cf0,
    ALTF10 = 0xf76c5cf0,
    ALTF11 = 0xf76cdcf0,
    ALTF12 = 0xf76d1cf0,

    CTRLALTF1 = 0xc6eed880,
    CTRLALTF2 = 0xd6eed880,
    CTRLALTF3 = 0xe6eed880,
    CTRLALTF4 = 0xf6eed880,
    CTRLALTF5 = 0xf76d5c70,
    CTRLALTF6 = 0xf76ddc70,
    CTRLALTF7 = 0xf76e1c70,
    CTRLALTF8 = 0xf76e5c70,
    CTRLALTF9 = 0xf76c1cf0,
    CTRLALTF10 = 0xf76c5cf0,
    CTRLALTF11 = 0xf76cdcf0,
    CTRLALTF12 = 0xf76d1cf0,

    CTRLSHIFTF1 = 0xc6ced880,
    CTRLSHIFTF2 = 0xd6ced880,
    CTRLSHIFTF3 = 0xe6ced880,
    CTRLSHIFTF4 = 0xf6ced880,
    CTRLSHIFTF5 = 0xf76d5c70,
    CTRLSHIFTF6 = 0xf76ddc70,
    CTRLSHIFTF7 = 0xf76e1c70,
    CTRLSHIFTF8 = 0xf76e5c70,
    CTRLSHIFTF9 = 0xf76c1cf0,
    CTRLSHIFTF10 = 0xf76c5cf0,
    CTRLSHIFTF11 = 0xf76cdcf0,
    CTRLSHIFTF12 = 0xf76d1cf0,

    ALTINS = 0xe66ed900,
    ALTHOME = 0xc66ed880,
    ALTPGUP = 0xe66eda80,
    ALTDEL = 0xe66ed980,
    ALTEND = 0xe66ed880,
    ALTPGDW = 0xe66edb00,

    ALTA = 0xa0184d80,
    ALTB = 0xa0188d80,
    ALTC = 0xa018cd80,
    ALTD = 0xa0190d80,
    ALTE = 0xa0194d80,
    ALTF = 0xa0198d80,
    ALTG = 0xa019cd80,
    ALTH = 0xa01a0d80,
    ALTI = 0xa01a4d80,
    ALTJ = 0xa01a8d80,
    ALTK = 0xa01acd80,
    ALTL = 0xa01b0d80,
    ALTM = 0xa01b4d80,
    ALTN = 0xa01b8d80,
    ALTO = 0xa01bcd80,
    ALTP = 0xa01c0d80,
    ALTQ = 0xa01c4d80,
    ALTR = 0xa01c8d80,
    ALTS = 0xa01ccd80,
    ALTT = 0xa01d0d80,
    ALTU = 0xa01d4d80,
    ALTV = 0xa01d8d80,
    ALTW = 0xa01dcd80,
    ALTX = 0xa01e0d80,
    ALTY = 0xa01e4d80,
    ALTZ = 0xa01e8d80,

    CTRLALTA = 0xa0004d80,
    CTRLALTB = 0xa0008d80,
    CTRLALTC = 0xa000cd80,
    CTRLALTD = 0xa0010d80,
    CTRLALTE = 0xa0014d80,
    CTRLALTF = 0xa0018d80,
    CTRLALTG = 0xa001cd80,
    CTRLALTH = 0xa0020d80,
    CTRLALTI = 0xa0024d80,
    CTRLALTJ = 0xa0028d80,
    CTRLALTK = 0xa002cd80,
    CTRLALTL = 0xa0030d80,
    CTRLALTM = 0xa0034d80,
    CTRLALTN = 0xa0038d80,
    CTRLALTO = 0xa003cd80,
    CTRLALTP = 0xa0040d80,
    CTRLALTQ = 0xa0044d80,
    CTRLALTR = 0xa0048d80,
    CTRLALTT = 0xa004cd80,
    CTRLALTS = 0xa0050d80,
    CTRLALTU = 0xa0054d80,
    CTRLALTV = 0xa0058d80,
    CTRLALTW = 0xa005cd80,
    CTRLALTX = 0xa0060d80,
    CTRLALTY = 0xa0064d80,
    CTRLALTZ = 0xa0068d80,
};
#endif

typedef struct file_t
{
    char *extensions; // Used separator is '|' because for some reason I couldn't get a 2D array working
    char *display_name;
    size_t extcount;
} file_t;

char* DEFAULT_LANGUAGE ="File";

static file_t FileLang[] = {
    {"adb|ads", "Ada", 2},                                                       // Ada
    {"awk", "AWK script", 1},                                                    // AWK script
    {"bas", "BASIC", 1},                                                         // BASIC
    {"bat|cmd|btm", "Batch", 3},                                                 // Batch file
    {"bin|exe|dll|sys|ocx|elf|out", "Binary file", 7},                           // Binary file
    {"c|h|inl", "C", 3},                                                         // C
    {"clj|cljs|cljc|edn", "Clojure", 4},                                         // Clojure
    {"config|conf|ini|cfg|cnf|cf", "Configuration file", 6},                     // Configuration files
    {"cpp|hpp|cxx|hxx|c++|h++|cc|hh", "C++", 8},                                 // C++ file
    {"cs|csx", "C#", 2},                                                         // C#
    {"css", "CSS style", 1},                                                     // CSS file
    {"csv", "CSV file", 1},                                                      // CSV file
    {"dart", "Dart", 1},                                                         // Dart
    {"docx|docm|pptx|pptm|xlsx|xlsm", "Microsoft Office XML", 6},                // Microsoft Office XML
    {"e", "Eiffel", 1},                                                          // Eiffel
    {"el|elc|eln", "Emacs Lisp", 3},                                             // Emacs lisp
    {"elm", "Elm", 1},                                                           // ELM
    {"erl|hrl", "Erlang", 2},                                                    // Erlang
    {"ex|exs", "Elixir", 2},                                                     // Elixir
    {"fs|fsi|fsx|fsscript", "F#", 4},                                            // F#
    {"git", "Git", 1},                                                           // Git (not sure if this is an actual file extension)
    {"go", "Golang", 1},                                                         // Golang
    {"hs|lhs", "Haskell", 1},                                                    // Haskell
    {"html|htm", "HTML file", 2},                                                // HTML
    {"java|class|jar|jmod", "Java", 4},                                          // Java
    {"jl", "Julia", 1},                                                          // Julia
    {"js|cjs|mjs", "JavaScript", 3},                                             // JavaScript
    {"json", "JSON file", 1},                                                    // JSON file
    {"lua", "Lua", 1},                                                           // Lua
    {"m|p|mex|mat|fig|mlx|mlapp|mltbx|mlappinstall|mlpkginstall", "MATLAB", 10}, // MATLAB
    {"md|markdown", "Markdown", 2},                                              // Markdown
    {"ml|mli", "OCaml", 2},                                                      // OCaml
    {"nb|wl", "Wolfram Language", 2},                                            // Wolfram Language (Mathematica)
    {"newtrodit|nwtrd", "Newtrodit script", 2},                                  // Newtrodit script
    {"nwtrd-syntax", "Newtrodit syntax rules", 1},                               // Newtrodit syntax highlighting
    {"odt|fodt|ods|fods|odp|fodp|odg|fodg|odf", "OpenDocument file", 9},         // OpenDocument
    {"pas|pp|inc", "Pascal", 3},                                                 // Pascal
    {"pdf", "Portable Document Format", 1},                                      // Portable Document Format
    {"php|phar|phtml|pht|phps", "PHP", 5},                                       // PHP
    {"pl|plx|pm|xs|t|pod|cgi", "Perl", 7},                                       // Perl
    {"ps1|psd1|psm1|ps1xml|pssc|psrc|cdxml", "PowerShell", 7},                   // PowerShell script
    {"py|pyi|pyc|pyd|pyo|pyw|pyz", "Python", 7},                                 // Python
    {"rb", "Ruby", 1},                                                           // Ruby
    {"rs|rlib", "Rust", 2},                                                      // Rust
    {"rtf", "Rich Text Format", 1},                                              // Rich Text Format
    {"s|asm|arm", "Assembly", 3},                                                // Assembly
    {"scala|sc", "Scala", 2},                                                    // Scala
    {"scm|ss", "Scheme", 2},                                                     // Scheme
    {"sd7|s7i", "Seed7", 2},                                                     // Seed7
    {"sh|bashrc", "Shell script", 1},                                            // Shell script
    {"sml", "Standard ML", 1},                                                   // Standard ML
    {"svg|svgz", "SVG", 2},                                                      // SVG
    {"tcl|tbc", "Tcl", 2},                                                       // Tcl
    {"tex", "LaTeX", 1},                                                         // LaTeX
    {"ts|tsx", "TypeScript", 2},                                                 // TypeScript
    {"txt|text", "Text file", 2},                                                // Text file
    {"vbs|vbe|wsf|wsc|hta|asp", "VBScript", 6},                                  // VBScript
    {"vim|vimrc", "Vim script", 2},                                              // Vim script
    {"vue", "Vue", 1},                                                           // Vue
    {"xhtml|xhtm|xht", "XHTML", 2},                                              // XHTML
    {"xml", "XML", 1},                                                           // XML
    {"yml|yaml", "YAML", 2},                                                     // YAML
};