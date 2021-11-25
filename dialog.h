// Errors and warnings

char NEWTRODIT_ERROR_BUFFER_OVERFLOW[] = "Internal buffer overflow.";
char NEWTRODIT_ERROR_CALLOC_FAILED[] = "Memory allocation failed. Program cannot start.\nerrno: ";
char NEWTRODIT_ERROR_CLIPBOARD_COPY[] = "Cannot copy string to clipboard.";
char NEWTRODIT_ERROR_CON_BUFFER_LIMIT[] = "Buffer arrived to limit.";
char NEWTRODIT_ERROR_GOTO_BUFFER_SIZE[] = "Buffer too small for the specified line.";
char NEWTRODIT_ERROR_INVALID_XPOS[] = "Invalid X position for cursor.";
char NEWTRODIT_ERROR_INVALID_YPOS[] = "Invalid Y position for cursor.";
char NEWTRODIT_ERROR_WINDOW_TOO_SMALL[] = "Console window is too small. Please resize it.";
char NEWTRODIT_WARNING_UNEXPECTED_ERRORS[] = "Warning: Unexpected errors may happen.";
char NEWTRODIT_ERROR_MANUAL_TOO_BIG[] = "Manual file is too big.";
char NEWTRODIT_ERROR_MISSING_MANUAL[] = "Manual file is missing: ";
char NEWTRODIT_ERROR_INVALID_MANUAL[] = "Invalid manual file: ";

char NEWTRODIT_ERROR_INVALID_INBOUND[] = "Invalid inbound key combination: ";
char NEWTRODIT_ERROR_RELOAD_SETTINGS[] = "Cannot reload settings. Settings file doesn't exist or it's corrupt.";
char NEWTRODIT_ERROR_OUT_OF_MEMORY[] = "Out of memory.";



// Command-line arguments
char NEWTRODIT_ERROR_MISSING_ARGUMENT[] = "Missing argument. See 'newtrodit --help'";
char NEWTRODIT_ERROR_INVALID_COLOR[] = "Invalid color. Color range is between 0 and F.";


// File IO error dialogs
char NEWTRODIT_FS_FILE_TOO_LARGE[] = "File too large: ";
char NEWTRODIT_FS_FILE_CORRUPTED[] = "File corrupted and/or unreadable: ";
char NEWTRODIT_FS_FILE_OPEN_ERR[] = "Cannot open the file.";
char NEWTRODIT_FS_FILE_SAVE_ERR[] = "Cannot save the file.";
char NEWTRODIT_FS_FILE_INVALID_NAME[] = "Invalid file name: ";
char NEWTRODIT_FS_FILE_NAME_TOO_LONG[] = "File name too long: ";
char NEWTRODIT_FS_FILE_NOT_FOUND[] = "File not found: ";
char NEWTRODIT_FS_FILE_RENAME[] = "Cannot rename the file.";
char NEWTRODIT_FS_FILE_DELETE[] = "Cannot delete the file.";


// Prompts

char NEWTRODIT_PROMPT_FIND_STRING[] = "String to find: ";
char NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE[] = "String to find (case insensitive): ";
char NEWTRODIT_PROMPT_REPLACE_STRING[] = "String to replace: ";
char NEWTRODIT_PROMPT_REPLACE_STRING_INSENSITIVE[] = "String to replace (case insensitive): ";
char NEWTRODIT_PROMPT_FOPEN[] = "File to open: ";
char NEWTRODIT_PROMPT_GOTO_LINE[] = "Line number: ";
char NEWTRODIT_PROMPT_RENAME_FILE[] = "New file name: ";

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



// Informational dialogs
char NEWTRODIT_FILE_SAVED[] = "File saved successfully.";
char NEWTRODIT_FILE_RELOADED[] = "File reloaded successfully.";
char NEWTRODIT_NEW_FILE_CREATED[] = "New file created successfully.";
char NEWTRODIT_FILE_CLOSED[] = "File closed successfully.";
char NEWTRODIT_FILE_RENAMED[] = "File renamed successfully to: ";
char NEWTRODIT_SETTINGS_RELOADED[] = "Settings reloaded successfully.";

char NEWTRODIT_FIND_STRING_NOT_FOUND[] = "String not found: ";
char NEWTRODIT_DEV_TOOLS[] = "Developer tools: ";
char NEWTRODIT_TAB_CONVERSION[] = "Tab conversion: ";
char NEWTRODIT_NULL_CONVERSION[] = "Null character to spaces conversion: ";
char NEWTRODIT_LINE_COUNT[] = "Line count: ";


// Other dialogs

char NEWTRODIT_DIALOG_BOTTOM_HELP[] = "For help, press F1 | ";
char NEWTRODIT_DIALOG_ENABLED[] = "Enabled";
char NEWTRODIT_DIALOG_DISABLED[] = "Disabled";
char NEWTRODIT_FUNCTION_ABORTED[] = "Function aborted.";

char NEWTRODIT_MANUAL_MAGIC_NUMBER[] = "$NEWTRODIT_MANUAL";