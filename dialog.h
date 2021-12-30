/*
  Newtrodit: A console text editor
  Copyright (C) 2021  anic17 Software

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

// Errors

char NEWTRODIT_ERROR_CLIPBOARD_COPY[] = "Cannot copy string to clipboard.";
char NEWTRODIT_ERROR_INVALID_XPOS[] = "Invalid X position for cursor.";
char NEWTRODIT_ERROR_INVALID_YPOS[] = "Invalid Y position for cursor.";
char NEWTRODIT_ERROR_WINDOW_TOO_SMALL[] = "Console window is too small. Please resize it.";
char NEWTRODIT_ERROR_MANUAL_TOO_BIG[] = "Manual file is too big.";
char NEWTRODIT_ERROR_MISSING_MANUAL[] = "Manual file is missing: ";
char NEWTRODIT_ERROR_INVALID_MANUAL[] = "Invalid manual file: ";

char NEWTRODIT_ERROR_INVALID_INBOUND[] = "Invalid inbound key combination: ";
char NEWTRODIT_ERROR_RELOAD_SETTINGS[] = "Cannot reload settings. Settings file doesn't exist or it's corrupt.";
char NEWTRODIT_ERROR_OUT_OF_MEMORY[] = "Out of memory.";
char NEWTRODIT_ERROR_INVALID_SYNTAX[] = "Invalid syntax highlighting file: ";
char NEWTRODIT_ERROR_SYNTAX_RULES[] = "No syntax highlighting rules found.";

char NEWTRODIT_ERROR_INVALID_MACRO[] = "Invalid macro: ";

// Command-line arguments
char NEWTRODIT_ERROR_MISSING_ARGUMENT[] = "Missing argument. See 'newtrodit --help'";
char NEWTRODIT_ERROR_INVALID_COLOR[] = "Invalid color. Color range is between 0 and F.";


// File IO error dialogs
char NEWTRODIT_FS_FILE_TOO_LARGE[] = "File too large: ";
char NEWTRODIT_FS_FILE_OPEN_ERR[] = "Cannot open the file.";
char NEWTRODIT_FS_FILE_SAVE_ERR[] = "Cannot save the file.";
char NEWTRODIT_FS_FILE_INVALID_NAME[] = "Invalid file name: ";
char NEWTRODIT_FS_FILE_NAME_TOO_LONG[] = "File name too long: ";
char NEWTRODIT_FS_FILE_NOT_FOUND[] = "File not found: ";
char NEWTRODIT_FS_FILE_RENAME[] = "Cannot rename the file.";
char NEWTRODIT_FS_FILE_DELETE[] = "Cannot delete the file.";

char NEWTRODIT_FS_DISK_FULL[] = "Disk is full.";
char NEWTRODIT_FS_ACCESS_DENIED[] = "Access denied.";



// Prompts

char NEWTRODIT_PROMPT_FIND_STRING[] = "String to find: ";
char NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE[] = "String to find (case insensitive): ";
char NEWTRODIT_PROMPT_REPLACE_STRING[] = "String to replace: ";
char NEWTRODIT_PROMPT_FOPEN[] = "File to open: ";
char NEWTRODIT_PROMPT_GOTO_LINE[] = "Line number: ";
char NEWTRODIT_PROMPT_RENAME_FILE[] = "New file name: ";
char NEWTRODIT_PROMPT_LOCATE_FILE[] = "File to locate: ";


char NEWTRODIT_PROMPT_OVERWRITE[] = "File already exists. Overwrite? (y/n)";
char NEWTRODIT_PROMPT_QUIT[] = "Are you sure you want to quit? (y/n)";
char NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE[] = "File has been modified. Save changes? (y/n)";
char NEWTRODIT_PROMPT_NEW_FILE[] = "Are you sure you want to create a new file? (y/n)";
char NEWTRODIT_PROMPT_CLOSE_FILE[] = "Are you sure you want to close the file? (y/n)";
char NEWTRODIT_PROMPT_RELOAD_FILE[] = "Are you sure you want to reload the file? (y/n)";
char NEWTRODIT_PROMPT_RELOAD_SETTINGS[] = "Are you sure you want to reload the settings? (y/n)";
char NEWTRODIT_PROMPT_FILE_CREATING[] = " doesn't exist. Do you want to create it? (y/n)"; // Must be added using join() later
char NEWTRODIT_PROMPT_SAVE_FILE[] = "File to save: ";
char NEWTRODIT_PROMPT_SAVE_FILE_AS[] = "Save file as: ";
char NEWTRODIT_PROMPT_CREATE_MACRO[] = "Macro command line: ";
char NEWTRODIT_PROMPT_SYNTAX_FILE[] = "Syntax highligthing file: ";

// Informational dialogs
char NEWTRODIT_FILE_SAVED[] = "File saved successfully.";
char NEWTRODIT_FILE_RELOADED[] = "File reloaded successfully.";
char NEWTRODIT_NEW_FILE_CREATED[] = "New file created successfully.";
char NEWTRODIT_FILE_CLOSED[] = "File closed successfully.";
char NEWTRODIT_FILE_RENAMED[] = "File renamed successfully to: ";
char NEWTRODIT_SETTINGS_RELOADED[] = "Settings reloaded successfully.";
char NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED[] = "Syntax highlighting rules have been successfully loaded.";


char NEWTRODIT_FIND_STRING_NOT_FOUND[] = "String not found: ";
char NEWTRODIT_DEV_TOOLS[] = "Developer tools: ";
char NEWTRODIT_TAB_CONVERSION[] = "Tab conversion: ";
char NEWTRODIT_NULL_CONVERSION[] = "Null character to spaces conversion: ";
char NEWTRODIT_LINE_COUNT[] = "Line count: ";
char NEWTRODIT_SYNTAX_HIGHLIGHTING[] = "Syntax highlighting: ";
char NEWTRODIT_MACRO_SET[] = "Macro set: ";
char NEWTRODIT_OLD_KEYBINDS[] = "Old keybindings: ";


// Other dialogs

char NEWTRODIT_DIALOG_BOTTOM_HELP[] = "For help, press F1 | ";
char NEWTRODIT_DIALOG_MANUAL[] = "^X  Close help | A-F4 Quit Newtrodit";
char NEWTRODIT_DIALOG_MANUAL_TITLE[] = " Newtrodit help";


char NEWTRODIT_DIALOG_ENABLED[] = "Enabled";
char NEWTRODIT_DIALOG_DISABLED[] = "Disabled";
char NEWTRODIT_FUNCTION_ABORTED[] = "Function aborted.";

char NEWTRODIT_MANUAL_MAGIC_NUMBER[] = "$NEWTRODIT_MANUAL";

char NEWTRODIT_SYNTAX_MAGIC_NUMBER[] = "$NEWTRODIT_SYNTAX";
char NEWTRODIT_SYNTAX_COMMENT[] = "$COMMENT";
char NEWTRODIT_SYNTAX_LANGUAGE[] = "$LANGUAGE";
char NEWTRODIT_SYNTAX_DEFAULT_COLOR[] = "$DEFAULT_COLOR";
char NEWTRODIT_SYNTAX_COMMENT_COLOR[] = "$COMMENT_COLOR";
char NEWTRODIT_SYNTAX_QUOTE_COLOR[] = "$QUOTE_COLOR";
char NEWTRODIT_SYNTAX_NUMBER_COLOR[] = "$NUMBER_COLOR";
char NEWTRODIT_SYNTAX_SEPARATORS[] = "$SEPARATORS";



char NEWTRODIT_MACRO_CURRENT_FILE[] = "$FILE";
char NEWTRODIT_MACRO_CURRENT_DIR[] = "$DIR";