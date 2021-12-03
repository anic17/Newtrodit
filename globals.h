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
const char newtrodit_build_date[] = "2/12/2021";
char manual_file[_MAX_PATH] = "newtrodit.man";
char settings_file[_MAX_PATH] = "newtrodit.config";

int start_color;
const char newtrodit_commit[] = ""; // Example commit

char newlinestring[100] = "\n";

const int MANUAL_BUFFER_X = 300;
const int MANUAL_BUFFER_Y = 2000;

int TAB_WIDE = TAB_WIDE_;
int CURSIZE = CURSIZE_;
int LINECOUNT_WIDE = LINECOUNT_WIDE_;
int goto_len = 4;

int xpos = 0, ypos = 1, display_y = 1; // Cursor position

// Boolean
int convertTabtoSpaces = true;
int convertNull = true;
int trimLongLines = true;
int cursorSizeInsert = true;
int wrapLine = false;
int wrapSize = 100; // Default wrap size
int autoIndent = false;
int fullPathTitle = true;
int useOldKeybinds = false; // Bool to use old keybinds (^X instead of ^Q, ^K instead of ^X)
int clearBufferScreen = false;

int isUntitled = true;
int isModified = false;
int isSaved = false;

int syntaxHighlighting = false;

#define BG_DEFAULT 0x07
#define FG_DEFAULT 0x70

int bg_color = BG_DEFAULT;
int fg_color = FG_DEFAULT;

char filename_text[_MAX_PATH] = filename_text_;

char **str_save; // Buffer to save the text

char *run_macro;
