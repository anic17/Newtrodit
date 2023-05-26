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

// Errors

char NEWTRODIT_ERROR_UNKNOWN[] = "Unknown error.";
char NEWTRODIT_ERROR_CLIPBOARD_COPY[] = "Cannot copy string to clipboard.";
char NEWTRODIT_ERROR_INVALID_XPOS[] = "Invalid X position for cursor.";
char NEWTRODIT_ERROR_INVALID_YPOS[] = "Invalid Y position for cursor.";
char NEWTRODIT_ERROR_MANUAL_INVALID_LINE[] = "Invalid line (maximum line number is %d)"; // I could use NEWTRODIT_ERROR_INVALID_YPOS but I prefer more specific errors (or messages)
char NEWTRODIT_ERROR_INVALID_POS_RESET[] = "Invalid position for cursor detected. Resetting coordinates.";
char NEWTRODIT_ERROR_WINDOW_TOO_SMALL[] = "Console window is too small. Please resize it.";
char NEWTRODIT_ERROR_MANUAL_TOO_BIG[] = "Manual file is too big.";
char NEWTRODIT_ERROR_MISSING_MANUAL[] = "Manual file is missing: ";
char NEWTRODIT_ERROR_INVALID_MANUAL[] = "Invalid manual file: ";
char NEWTRODIT_ERROR_LOADING_MANUAL[] = "Error loading manual file. This might be caused due to an outdated OS version ";

char NEWTRODIT_ERROR_INVALID_INBOUND[] = "Invalid inbound key combination: ";
char NEWTRODIT_ERROR_RELOAD_SETTINGS[] = "Cannot reload settings. Settings file doesn't exist or it's corrupt.";
char NEWTRODIT_ERROR_OUT_OF_MEMORY[] = "Out of memory.";
char NEWTRODIT_ERROR_ALLOCATION_FAILED[] = "Memory allocation failed.";
char NEWTRODIT_ERROR_INVALID_SYNTAX[] = "Invalid syntax highlighting file: ";
char NEWTRODIT_ERROR_SYNTAX_RULES[] = "No syntax highlighting rules found.";
char NEWTRODIT_ERROR_FAILED_PROCESS_START[] = "Failed to start the process: ";
char NEWTRODIT_ERROR_FAILED_CLOSE_FILE[] = "Failed to close the file '%s'";

char NEWTRODIT_ERROR_INVALID_MACRO[] = "Invalid macro: ";
char NEWTRODIT_ERROR_INVALID_UNICODE_SEQUENCE[] = "Invalid Unicode byte sequence.";
char NEWTRODIT_ERROR_TOO_MANY_FILES_OPEN[] = "Too many files open.";
char NEWTRODIT_ERROR_CANNOT_OPEN_DEVICE[] = "Cannot open device file: ";
char NEWTRODIT_ERROR_CONSOLE_HANDLE[] = "Cannot get console handle.";
char NEWTRODIT_ERROR_UNKNOWN_COMMAND[] = "Unknown command: '%s'";
char NEWTRODIT_LICENSE_INVALID_LICENSE[] = "Detected an invalid or outdated license file.";
char NEWTRODIT_LICENSE_MISSING_LICENSE[] = "License file is missing: ";
char NEWTRODIT_ERROR_INVALID_FILE_INDEX[] = "Invalid file index. Switching to the first file.";

// Internal errors
char NEWTRODIT_INTERNAL_EXPECTED_NULL[] = "Internal error: expected a non-null string";

// Crashes

char NEWTRODIT_CRASH_INVALID_SETTINGS[] = "Invalid settings file.";
char NEWTRODIT_CRASH_UNKNOWN_EXCEPTION[] = "Unknown exception.";


// Warnings
char NEWTRODIT_WARNING_SYNTAX_TOO_BIG[] = "Warning: Syntax highlighting file is too big. Using the first keywords.";
char NEWTRODIT_WARNING_READONLY_FILE[] = "Warning: File has read-only attribute.";

// Command-line arguments
char NEWTRODIT_ERROR_MISSING_ARGUMENT[] = "Missing argument. See 'newtrodit --help'";
char NEWTRODIT_ERROR_INVALID_COLOR[] = "Invalid color. Color range is between 0 and F.";

// File IO error/info dialogs
char NEWTRODIT_FS_FILE_TOO_LARGE[] = "File too large: ";
char NEWTRODIT_FS_FILE_OPEN_ERR[] = "Failed to open the file: %s";
char NEWTRODIT_FS_FILE_SAVE_ERR[] = "Cannot save the file.";
char NEWTRODIT_FS_FILE_INVALID_NAME[] = "Invalid file name: ";
char NEWTRODIT_FS_FILE_NAME_TOO_LONG[] = "File name too long: ";
char NEWTRODIT_FS_FILE_NOT_FOUND[] = "File not found: ";
char NEWTRODIT_FS_DIR_NOT_FOUND[] = "Directory not found: %s";

char NEWTRODIT_FS_FILE_RENAME[] = "Cannot rename the file.";
char NEWTRODIT_FS_FILE_DELETE[] = "Failed to delete the file: ";
char NEWTRODIT_FS_SAME_FILE[] = "Cannot open the same file.";
char NEWTRODIT_FS_FOUND_FILES[] = "Showing %d of %d files matching the search pattern.";
char NEWTRODIT_FS_READONLY_SAVE[] = "Cannot write to a read-only file.";
char NEWTRODIT_FS_IS_A_DIRECTORY[] = "Is a directory: ";
char NEWTRODIT_FS_NOT_A_DIRECTORY[] = "Not a directory: %s";


char NEWTRODIT_FS_DISK_FULL[] = "Disk is full.";
char NEWTRODIT_FS_ACCESS_DENIED[] = "Access is denied: ";

// Prompts

char NEWTRODIT_PROMPT_FIND_STRING[] = "String to find: ";
char NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE[] = "String to find (case insensitive): ";
char NEWTRODIT_PROMPT_REPLACE_STRING[] = "String to replace: ";
char NEWTRODIT_PROMPT_FOPEN[] = "File to open: ";
char NEWTRODIT_PROMPT_ALREADY_OPEN_TAB[] = "'%s' is already opened in another tab. Do you want to switch to it? (y/n)";

char NEWTRODIT_PROMPT_GOTO_LINE[] = "Line number: ";
char NEWTRODIT_PROMPT_GOTO_COLUMN[] = "Column number: ";
char NEWTRODIT_PROMPT_RENAME_FILE[] = "New file name: ";
char NEWTRODIT_PROMPT_LOCATE_FILE[] = "File to locate: ";

char NEWTRODIT_PROMPT_OVERWRITE[] = "File already exists. Overwrite? (y/n)";
char NEWTRODIT_PROMPT_QUIT[] = "Are you sure you want to quit Newtrodit? (y/n)";
char NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE[] = "File has been modified. Save changes? (y/n)";
char NEWTRODIT_PROMPT_NEW_FILE[] = "Are you sure you want to create a new file? (y/n)";
char NEWTRODIT_PROMPT_CLOSE_FILE[] = "Are you sure you want to close the file? (y/n)";
char NEWTRODIT_PROMPT_RELOAD_FILE[] = "Are you sure you want to reload the file? (y/n)";
char NEWTRODIT_PROMPT_RELOAD_SETTINGS[] = "Are you sure you want to reload the settings? (y/n)";
char NEWTRODIT_PROMPT_FILE_CREATING[] = " doesn't exist. Do you want to create it? (y/n)"; // Must be added using join() later
char NEWTRODIT_PROMPT_REOPEN_FILE[] = "Do you want to reopen the file? (y/n)";
char NEWTRODIT_PROMPT_MODIFIED_FILE_DISK[] = "File has been modified on disk. Do you want to reload it? (y/n)";
char NEWTRODIT_PROMPT_SAVE_FILE[] = "File to save: ";
char NEWTRODIT_PROMPT_SAVE_FILE_AS[] = "Save file as: ";
char NEWTRODIT_PROMPT_CREATE_MACRO[] = "Macro command line: ";
char NEWTRODIT_PROMPT_SYNTAX_FILE[] = "Syntax highligthing file: ";
char NEWTRODIT_PROMPT_COMMAND_PALETTE[] = "Command palette: ";


char NEWTRODIT_PROMPT_FIRST_FILE_COMPARE[] = "First file to compare: ";
char NEWTRODIT_PROMPT_SECOND_FILE_COMPARE[] = "Second file to compare: ";

// Informational dialogs
char NEWTRODIT_NO_ERROR[] = "No error.";
char NEWTRODIT_FILE_SAVED[] = "File saved successfully.";
char NEWTRODIT_FILE_RELOADED[] = "File reloaded successfully.";
char NEWTRODIT_NEW_FILE_CREATED[] = "New file created successfully.";
char NEWTRODIT_FILE_CLOSED[] = "File closed successfully.";
char NEWTRODIT_FILE_RENAMED[] = "File renamed successfully to: ";
char NEWTRODIT_FILE_WROTE_BUFFER[] = "Wrote the buffer content to: ";
char NEWTRODIT_SETTINGS_RELOADED[] = "Settings reloaded successfully.";
char NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED[] = "Syntax highlighting rules for %s language successfully loaded.";
char NEWTRODIT_SYNTAX_HIGHLIGHTING_FAILED[] = "Failed to load syntax highlighting rules.";
char NEWTRODIT_LOCATE_END_REACHED[] = "End of directory reached.";


char NEWTRODIT_FIND_STRING_NOT_FOUND[] = "String not found: ";
char NEWTRODIT_FIND_CASE_SENSITIVE[] = "Case sensitive search: ";
char NEWTRODIT_FIND_MATCH_WHOLE_WORD[] = "Match whole word: ";
char NEWTRODIT_FIND_NO_MORE_MATCHES[] = "No more matches.";

char NEWTRODIT_DEV_TOOLS[] = "Developer tools: ";
char NEWTRODIT_TAB_CONVERSION[] = "Tab conversion: ";
char NEWTRODIT_NULL_CONVERSION[] = "Null character to spaces conversion: ";
char NEWTRODIT_LINE_COUNT[] = "Line count: ";
char NEWTRODIT_SYNTAX_HIGHLIGHTING[] = "Syntax highlighting: ";
char NEWTRODIT_MACRO_SET[] = "Macro set: ";
char NEWTRODIT_OLD_KEYBINDINGS[] = "Old keybindings: ";
char NEWTRODIT_AUTO_SYNTAX_LOAD[] = "Automatically load syntax highlighting rules for supported languages: ";
char NEWTRODIT_FILE_COMPARE_DIFF[] = "Difference found in byte number ";
char NEWTRODIT_FILE_COMPARE_NODIFF[] = "No difference found between files.";
char NEWTRODIT_LOADING_COMPARING_FILES[] = "Comparing files '%s' and '%s'. This may take a while.";
char NEWTRODIT_CURRENT_FILE_SWITCHED[] = "Editing now: ";
char NEWTRODIT_MOUSE[] = "Mouse: ";

char NEWTRODIT_SHOWING_PREVIOUS_FILE[] = "Showing previous file";
char NEWTRODIT_SHOWING_NEXT_FILE[] = "Showing next file";
char NEWTRODIT_SWITCHING_FILE[] = "Switching to file";
char NEWTRODIT_INFO_NO_FILES_TO_SWITCH[] = "No files to switch to.";

// Other dialogs

char NEWTRODIT_DIALOG_BOTTOM_HELP[] = "For help, press F1 | ";
char NEWTRODIT_DIALOG_MANUAL[] = "^X Close help | A-F4 Quit Newtrodit";
char NEWTRODIT_DIALOG_MANUAL_TITLE[] = " Newtrodit help";
char NEWTRODIT_DIALOG_BOTTOM_LOCATE[] = " | Space Next page | ^B Go back | ^D Go to dir | ^X Quit";
char NEWTRODIT_DIALOG_LOCATE_POS[] = "Showing %d-%d out of %d files ";


char NEWTRODIT_DIALOG_ENABLED[] = "Enabled";
char NEWTRODIT_DIALOG_DISABLED[] = "Disabled";
char NEWTRODIT_FUNCTION_ABORTED[] = "Function aborted.";

// Constants used internally

char NEWTRODIT_MANUAL_MAGIC_NUMBER[] = "$NEWTRODIT_MANUAL";

char NEWTRODIT_SYNTAX_CAPITAL[] = "$CAPITAL";
char NEWTRODIT_SYNTAX_CAPITAL_COLOR[] = "$CAPITAL_COLOR";
char NEWTRODIT_SYNTAX_CAPITAL_MIN[] = "$CAPITAL_MIN";
char NEWTRODIT_SYNTAX_COMMENT[] = "$COMMENT";
char NEWTRODIT_SYNTAX_COMMENT_COLOR[] = "$COMMENT_COLOR";
char NEWTRODIT_SYNTAX_DEFAULT_COLOR[] = "$DEFAULT_COLOR";
char NEWTRODIT_SYNTAX_LANGUAGE[] = "$LANGUAGE";
char NEWTRODIT_SYNTAX_MAGIC_NUMBER[] = "$NEWTRODIT_SYNTAX";
char NEWTRODIT_SYNTAX_NUMBER_COLOR[] = "$NUMBER_COLOR";
char NEWTRODIT_SYNTAX_QUOTE_COLOR[] = "$QUOTE_COLOR";
char NEWTRODIT_SYNTAX_SEPARATORS[] = "$SEPARATORS";
char NEWTRODIT_SYNTAX_SINGLE_QUOTES[] = "$SINGLE_QUOTES";
char NEWTRODIT_SYNTAX_FINISH_QUOTES[] = "$FINISH_QUOTES";


char NEWTRODIT_MACRO_CURRENT_FILE[] = "$FILE";
char NEWTRODIT_MACRO_FULL_PATH[] = "$PATH";
char NEWTRODIT_MACRO_CURRENT_DIR[] = "$DIR";
char NEWTRODIT_MACRO_CURRENT_EXTENSION[] = "$EXT";
char NEWTRODIT_MACRO_CURRENT_BASENAME[] = "$BASENAME";
char NEWTRODIT_MACRO_CURRENT_DRIVE[] = "$DRIVE";