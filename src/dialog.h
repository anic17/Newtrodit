/*
 * Newtrodit: A console text editor
 * Copyright (C) 2021-2025 anic17 Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 *
 */

#ifndef DIALOG_H
#define DIALOG_H

// --- IMPORTANT: All strings are now DECLARED using 'extern' ---
// This tells the compiler that the string exists, but the storage
// is allocated in dialog.c (or another single .c file).

// Errors
extern char NEWTRODIT_ERROR_UNKNOWN[];
extern char NEWTRODIT_ERROR_CLIPBOARD_COPY[];
extern char NEWTRODIT_ERROR_CLIPBOARD_PASTE[];

extern char NEWTRODIT_ERROR_INVALID_XPOS[];
extern char NEWTRODIT_ERROR_INVALID_YPOS[];
extern char NEWTRODIT_ERROR_MANUAL_INVALID_LINE[];
extern char NEWTRODIT_ERROR_INVALID_POS_RESET[];
extern char NEWTRODIT_ERROR_WINDOW_TOO_SMALL[];
extern char NEWTRODIT_ERROR_MANUAL_TOO_BIG[];
extern char NEWTRODIT_ERROR_MISSING_MANUAL[];
extern char NEWTRODIT_ERROR_INVALID_MANUAL[];
extern char NEWTRODIT_ERROR_LOADING_MANUAL[];

extern char NEWTRODIT_ERROR_INVALID_INBOUND[];
extern char NEWTRODIT_ERROR_RELOAD_SETTINGS[];
extern char NEWTRODIT_ERROR_OUT_OF_MEMORY[];
extern char NEWTRODIT_ERROR_ALLOCATION_FAILED[];
extern char NEWTRODIT_ERROR_INVALID_SYNTAX[];
extern char NEWTRODIT_ERROR_SYNTAX_RULES[];
extern char NEWTRODIT_ERROR_FAILED_PROCESS_START[];
extern char NEWTRODIT_ERROR_FAILED_CLOSE_FILE[];
extern char NEWTRODIT_ERROR_FAILED_CONSOLE_ATTRIB[];

extern char NEWTRODIT_ERROR_INVALID_MACRO[];

extern char NEWTRODIT_ERROR_INVALID_UNICODE_SEQUENCE[];
extern char NEWTRODIT_ERROR_INVALID_UNICODE_POSITION[];
extern char NEWTRODIT_ERROR_UNSUPPORTED_ENCODING[];


extern char NEWTRODIT_ERROR_TOO_MANY_FILES_OPEN[];
extern char NEWTRODIT_ERROR_CANNOT_OPEN_DEVICE[];
extern char NEWTRODIT_ERROR_CONSOLE_HANDLE[];
extern char NEWTRODIT_ERROR_UNKNOWN_COMMAND[];
extern char NEWTRODIT_ERROR_INVALID_FILE_INDEX[];
extern char NEWTRODIT_ERROR_NEW_FILE[];
extern char NEWTRODIT_ERROR_REDIRECTED_TTY[];


// Internal errors
extern char NEWTRODIT_INTERNAL_EXPECTED_NULL[];

// Crashes
extern char NEWTRODIT_CRASH_INVALID_SETTINGS[];
extern char NEWTRODIT_CRASH_UNKNOWN_EXCEPTION[];


// Warnings
extern char NEWTRODIT_WARNING_SYNTAX_TOO_BIG[];
extern char NEWTRODIT_WARNING_READONLY_FILE[];

// Command-line arguments
extern char NEWTRODIT_ERROR_MISSING_ARGUMENT[];
extern char NEWTRODIT_ERROR_INVALID_COLOR[];

// File IO error/info dialogs
extern char NEWTRODIT_FS_FILE_TOO_LARGE[];
extern char NEWTRODIT_FS_FILE_OPEN_ERR[];
extern char NEWTRODIT_FS_FILE_SAVE_ERR[];
extern char NEWTRODIT_FS_FILE_INVALID_NAME[];
extern char NEWTRODIT_FS_FILE_NAME_TOO_LONG[];
extern char NEWTRODIT_FS_FILE_NOT_FOUND[];
extern char NEWTRODIT_FS_DIR_NOT_FOUND[];

extern char NEWTRODIT_FS_FILE_RENAME[];
extern char NEWTRODIT_FS_FILE_DELETE[];
extern char NEWTRODIT_FS_SAME_FILE[];
extern char NEWTRODIT_FS_FOUND_FILES[];
extern char NEWTRODIT_FS_READONLY_SAVE[];
extern char NEWTRODIT_FS_IS_A_DIRECTORY[];
extern char NEWTRODIT_FS_NOT_A_DIRECTORY[];
extern char NEWTRODIT_FS_NO_FILES_FOUND[];

extern char NEWTRODIT_FS_DISK_FULL[];
extern char NEWTRODIT_FS_ACCESS_DENIED[];

// Prompts
extern char NEWTRODIT_PROMPT_FIND_STRING[];
extern char NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE[];
extern char NEWTRODIT_PROMPT_REPLACE_STRING[];
extern char NEWTRODIT_PROMPT_FOPEN[];
extern char NEWTRODIT_PROMPT_ALREADY_OPEN_TAB[];

extern char NEWTRODIT_PROMPT_GOTO_LINE[];
extern char NEWTRODIT_PROMPT_GOTO_COLUMN[];
extern char NEWTRODIT_PROMPT_RENAME_FILE[];
extern char NEWTRODIT_PROMPT_LOCATE_FILE[];

extern char NEWTRODIT_PROMPT_OVERWRITE[];
extern char NEWTRODIT_PROMPT_QUIT[];
extern char NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE[];
extern char NEWTRODIT_PROMPT_NEW_FILE[];
extern char NEWTRODIT_PROMPT_CLOSE_FILE[];
extern char NEWTRODIT_PROMPT_RELOAD_FILE[];
extern char NEWTRODIT_PROMPT_RELOAD_SETTINGS[];
extern char NEWTRODIT_PROMPT_FILE_CREATING[];
extern char NEWTRODIT_PROMPT_REOPEN_FILE[];
extern char NEWTRODIT_PROMPT_MODIFIED_FILE_DISK[];
extern char NEWTRODIT_PROMPT_SAVE_FILE[];
extern char NEWTRODIT_PROMPT_SAVE_FILE_AS[];
extern char NEWTRODIT_PROMPT_CREATE_MACRO[];
extern char NEWTRODIT_PROMPT_SYNTAX_FILE[];
extern char NEWTRODIT_PROMPT_COMMAND_PALETTE[];


extern char NEWTRODIT_PROMPT_FIRST_FILE_COMPARE[];
extern char NEWTRODIT_PROMPT_SECOND_FILE_COMPARE[];

// Informational dialogs
extern char NEWTRODIT_NO_ERROR[];
extern char NEWTRODIT_FILE_SAVED[];
extern char NEWTRODIT_FILE_OPENED[];
extern char NEWTRODIT_FILE_RELOADED[];
extern char NEWTRODIT_NEW_FILE_CREATED[];
extern char NEWTRODIT_FILE_CLOSED[];
extern char NEWTRODIT_FILE_RENAMED[];
extern char NEWTRODIT_FILE_WROTE_BUFFER[];
extern char NEWTRODIT_SETTINGS_RELOADED[];
extern char NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED[];
extern char NEWTRODIT_SYNTAX_HIGHLIGHTING_FAILED[];
extern char NEWTRODIT_LOCATE_END_REACHED[];


extern char NEWTRODIT_FIND_STRING_NOT_FOUND[];
extern char NEWTRODIT_FIND_CASE_SENSITIVE[];
extern char NEWTRODIT_FIND_MATCH_WHOLE_WORD[];
extern char NEWTRODIT_FIND_NO_MORE_MATCHES[];

extern char NEWTRODIT_DEV_TOOLS[];
extern char NEWTRODIT_TAB_CONVERSION[];
extern char NEWTRODIT_NULL_CONVERSION[];
extern char NEWTRODIT_LINE_COUNT[];
extern char NEWTRODIT_SYNTAX_HIGHLIGHTING[];
extern char NEWTRODIT_MACRO_SET[];
extern char NEWTRODIT_OLD_KEYBINDINGS[];
extern char NEWTRODIT_AUTO_SYNTAX_LOAD[];
extern char NEWTRODIT_FILE_COMPARE_DIFF[];
extern char NEWTRODIT_FILE_COMPARE_NODIFF[];
extern char NEWTRODIT_LOADING_COMPARING_FILES[];
extern char NEWTRODIT_CURRENT_FILE_SWITCHED[];
extern char NEWTRODIT_MOUSE[];

extern char NEWTRODIT_SHOWING_PREVIOUS_FILE[];
extern char NEWTRODIT_SHOWING_NEXT_FILE[];
extern char NEWTRODIT_SWITCHING_FILE[];
extern char NEWTRODIT_INFO_NO_FILES_TO_SWITCH[];
extern char NEWTRODIT_CLIPBOARD_COPIED[];

// Other dialogs

extern char NEWTRODIT_DIALOG_BOTTOM_HELP[];
extern char NEWTRODIT_DIALOG_MANUAL[];
extern char NEWTRODIT_DIALOG_MANUAL_TITLE[];
extern char NEWTRODIT_DIALOG_BOTTOM_LOCATE[];
extern char NEWTRODIT_DIALOG_LOCATE_POS[];


extern char NEWTRODIT_DIALOG_ENABLED[];
extern char NEWTRODIT_DIALOG_DISABLED[];
extern char NEWTRODIT_FUNCTION_ABORTED[];

// Constants used internally

extern char NEWTRODIT_MANUAL_MAGIC_NUMBER[];

extern char NEWTRODIT_SYNTAX_CAPITAL[];
extern char NEWTRODIT_SYNTAX_CAPITAL_COLOR[];
extern char NEWTRODIT_SYNTAX_CAPITAL_MIN[];
extern char NEWTRODIT_SYNTAX_COMMENT[];
extern char NEWTRODIT_SYNTAX_COMMENT_COLOR[];
extern char NEWTRODIT_SYNTAX_DEFAULT_COLOR[];
extern char NEWTRODIT_SYNTAX_LANGUAGE[];
extern char NEWTRODIT_SYNTAX_MAGIC_NUMBER[];
extern char NEWTRODIT_SYNTAX_NUMBER_COLOR[];
extern char NEWTRODIT_SYNTAX_QUOTE_COLOR[];
extern char NEWTRODIT_SYNTAX_SEPARATORS[];
extern char NEWTRODIT_SYNTAX_SINGLE_QUOTES[];
extern char NEWTRODIT_SYNTAX_FINISH_QUOTES[];


extern char NEWTRODIT_MACRO_CURRENT_FILE[];
extern char NEWTRODIT_MACRO_FULL_PATH[];
extern char NEWTRODIT_MACRO_CURRENT_DIR[];
extern char NEWTRODIT_MACRO_CURRENT_EXTENSION[];
extern char NEWTRODIT_MACRO_CURRENT_BASENAME[];
extern char NEWTRODIT_MACRO_CURRENT_DRIVE[];
extern char NEWTRODIT_SYNTAX_ENCLOSING[];

#endif // DIALOG_H