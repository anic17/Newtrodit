/*
    Newtrodit: A console text editor
    Copyright (C) 2021 anic17 Software

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

const char newtrodit_version[] = "0.5";
const char newtrodit_build_date[] = "9/12/2021";
char manual_file[_MAX_PATH] = "newtrodit.man";
char settings_file[_MAX_PATH] = "newtrodit.config";

int start_color;
const char newtrodit_commit[] = ""; // Example commit

char newlinestring[100] = "\n";

const int MANUAL_BUFFER_X = 300;
const int MANUAL_BUFFER_Y = 1000;

int TAB_WIDE = TAB_WIDE_;
int CURSIZE = CURSIZE_;
int LINECOUNT_WIDE = LINECOUNT_WIDE_;
int goto_len = 4;


int xpos = 0, ypos = 1, display_y = 1; // Cursor position

// Boolean settings
int convertTabtoSpaces = true;
int convertNull = true;
int trimLongLines = true;
int cursorSizeInsert = true;
int wrapLine = false;
int autoIndent = false;
int fullPathTitle = true;
int useOldKeybinds = false; // Bool to use old keybinds (^X instead of ^Q, ^K instead of ^X)
int longPosition = false;

// Interal global variables
int clearBufferScreen = false;
int isUntitled = true;
int isModified = false;
int isSaved = false;
int multiLineComment = false;
int syntaxHighlighting = false;

int horizontalScroll = 0;
int wrapSize = 100; // Default wrap size



#define BG_DEFAULT 0x07
#define FG_DEFAULT 0x70

int bg_color = BG_DEFAULT; // Background color (menu)
int fg_color = FG_DEFAULT; // Foreground color (text)

char filename_text[_MAX_PATH] = filename_text_;

char **str_save; // Buffer to save the text

char *run_macro;

// Syntax highlighting

#define DEFAULT_SYNTAX_COLOR 0x7
#define DEFAULT_COMMENT_COLOR 0x8
#define DEFAULT_QUOTE_COLOR 0xe

#define DEFAULT_NUM_COLOR 0x2
#define SEPARATORS  ",.()+-/*=~[];{}<> "

char syntax_separators[512] = SEPARATORS;
char syntax_filename[_MAX_PATH] = "";

int default_color = DEFAULT_SYNTAX_COLOR;
int comment_color = DEFAULT_COMMENT_COLOR;
int quote_color = DEFAULT_QUOTE_COLOR;
int num_color = DEFAULT_NUM_COLOR;

