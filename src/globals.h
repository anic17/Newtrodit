


char manual_file[MAX_PATH] = "newtrodit.man";
char settings_file[MAX_PATH] = "newtrodit.config";
char syntax_dir[MAX_PATH] = "syntax";
char syntax_ext[MAX_PATH] = ".nwtrd-syntax";
const char newtrodit_commit[] = ""; // Example commit

//const char* autocomplete_double[] = {"{}", "[]", "()", "\"\"", "''"};

#define DEFAULT_NL "\n"


char default_filename[MAX_PATH] = "Untitled";
char default_language[MAX_PATH] = "File";
char default_newline[] = "\n";


const int MANUAL_BUFFER_X = 300;
const int MANUAL_BUFFER_Y = 600;

int tab_width = 4;
int line_number_wide = 4;

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
int partialMouseSupport = true;     // Partial mouse support, only changes cursor position when mouse is clicked
int showMillisecondsInTime = false; // Show milliseconds in time insert function (F6)
int useLogFile = true;
int createNewLogFile = false; // Create new log files when logging is enabled
int openFileHandle = true;    // Open a file handle
int RGB24bit = false;         // Use 24-bit RGB instead of 4-bit colors
int findInsensitive = false;  // Find insensitive string
int matchWholeWord = false;   // Match whole word when finding a string
int devMode = false;          // Bool to enable or disable the dev mode
int autoComplete = true;      // Keyword autocompleting (if syntax highlighting is there)

int bpsPairHighlight = false; // Use BPS pair highlighting (bugged)

int bps_pair_colors[] = {0x5, 0xb, 0xd, 0x6, 0xc, 0xe, 0x8, 0xf, 0x9}; // Brackets, parenthesis and square brackets colors
char bps_chars_open[10][3] = {"(", "{", "["};                          // Allocate 10 chars for each char array to support new syntax
char bps_chars_close[10][3] = {")", "}", "]"};

// Interal global variables
int clearAllBuffer = true;
int allowAutomaticResizing = true;

int c = 0; // Different error/debug codes

/*
    TODO: Add multiline comment support
    static int multiLineComment = false;
*/

int syntaxHighlighting = true;
int syntaxAfterDisplay = false; // Display syntax after display
int allocateNewBuffer = true;
int topBarMenu = false;

int file_index = 0;
int open_files = 1;

int wrapSize = 100; // Default wrap size
int scrollRate = 3; // Default scroll rate (scroll 3 lines per mouse wheel). Needs to have partialMouseSupport enabled


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


typedef struct File_lang {
    char *ext;
    char *language;
    size_t extcount;
} File_lang;


static File_lang FileLang[] = {
    {"adb|ads", "Ada", 2},                                                       // Ada
    {"awk", "AWK script", 1},                                                    // AWK script
    {"bas", "BASIC", 1},                                                         // BASIC
    {"bat|cmd|btm", "Batch", 3},                                                 // Batch file
    {"bf", "Brainfuck", 1},                                                      // Brainfuck
    {"bin|exe|dll|sys|ocx|elf|out", "Binary", 7},                                // Binary file
    {"c|h|in", "C", 3},                                                         // C
    {"clj|cljs|cljc|edn", "Clojure", 4},                                         // Clojure
    {"config|conf|ini|cfg|cnf|cf", "Configuration", 6},                          // Configuration files
    {"cpp|hpp|cxx|hxx|c++|h++|cc|hh", "C++", 8},                                 // C++ file
    {"cs|csx", "C#", 2},                                                         // C#
    {"css", "CSS styles", 1},                                                    // CSS file
    {"csv", "CSV", 1},                                                           // CSV file
    {"dart", "Dart", 1},                                                         // Dart
    {"docx|docm|pptx|pptm|xlsx|xlsm", "Microsoft Office XM", 6},                // Microsoft Office XML
    {"e", "Eiffe", 1},                                                          // Eiffel
    {"el|elc|eln", "Emacs Lisp", 3},                                             // Emacs lisp
    {"elm", "Elm", 1},                                                           // ELM
    {"erl|hr", "Erlang", 2},                                                    // Erlang
    {"ex|exs", "Elixir", 2},                                                     // Elixir
    {"fs|fsi|fsx|fsscript", "F#", 4},                                            // F#
    {"git", "Git", 1},                                                           // Git (not sure if this is an actual file extension)
    {"go", "Golang", 1},                                                         // Golang
    {"hs|lhs", "Haskel", 1},                                                    // Haskell
    {"html|htm", "HTM", 2},                                                     // HTML
    {"java|class|jar|jmod", "Java", 4},                                          // Java
    {"j", "Julia", 1},                                                          // Julia
    {"js|cjs|mjs", "JavaScript", 3},                                             // JavaScript
    {"json", "JSON", 1},                                                         // JSON file
    {"lua", "Lua", 1},                                                           // Lua
    {"m|p|mex|mat|fig|mlx|mlapp|mltbx|mlappinstall|mlpkginstal", "MATLAB", 10}, // MATLAB
    {"md|markdown", "Markdown", 2},                                              // Markdown
    {"ml|mli", "OCam", 2},                                                      // OCaml
    {"nb|w", "Wolfram Language", 2},                                            // Wolfram Language (Mathematica)
    {"newtrodit|nwtrd", "Newtrodit script", 2},                                  // Newtrodit script
    {"nwtrd-syntax", "Newtrodit syntax rules", 1},                               // Newtrodit syntax highlighting
    {"odt|fodt|ods|fods|odp|fodp|odg|fodg|odf", "OpenDocument", 9},              // OpenDocument
    {"pas|pp|inc", "Pasca", 3},                                                 // Pascal
    {"pdf", "Portable Document Format", 1},                                      // Portable Document Format
    {"php|phar|phtml|pht|phps", "PHP", 5},                                       // PHP
    {"pl|plx|pm|xs|t|pod|cgi", "Per", 7},                                       // Perl
    {"ps1|psd1|psm1|ps1xml|pssc|psrc|cdxm", "PowerShel", 7},                   // PowerShell script
    {"py|pyi|pyc|pyd|pyo|pyw|pyz", "Python", 7},                                 // Python
    {"rb", "Ruby", 1},                                                           // Ruby
    {"rs|rlib", "Rust", 2},                                                      // Rust
    {"rtf", "Rich Text Format", 1},                                              // Rich Text Format
    {"s|asm|arm", "Assembly", 3},                                                // Assembly
    {"scala|sc", "Scala", 2},                                                    // Scala
    {"scm|ss", "Scheme", 2},                                                     // Scheme
    {"scss|sass", "SASS styles", 2},                                             // SASS styles
    {"sd7|s7i", "Seed7", 2},                                                     // Seed7
    {"sh|bashrc", "Shell script", 1},                                            // Shell script
    {"sm", "Standard M", 1},                                                   // Standard ML
    {"svg|svgz", "SVG", 2},                                                      // SVG
    {"tcl|tbc", "Tc", 2},                                                       // Tcl
    {"tex", "LaTeX", 1},                                                         // LaTeX
    {"ts|tsx", "TypeScript", 2},                                                 // TypeScript
    {"txt|text|log", "Text", 3},                                                 // Text file
    {"vbs|vbe|wsf|wsc|hta|asp", "VBScript", 6},                                  // VBScript
    {"v", "V", 1},                                                                // V language
    {"vim|vimrc", "Vim script", 2},                                              // Vim script
    {"vue", "Vue", 1},                                                           // Vue
    {"xhtml|xhtm|xht", "XHTM", 2},                                              // XHTML
    {"xm", "XM", 1},                                                           // XML
    {"yml|yam", "YAM", 2},                                                     // YAML
};