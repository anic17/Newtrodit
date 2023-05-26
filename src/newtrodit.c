/*
    Newtrodit: A console text editor
    Copyright (c) 2021-2023 anic17 Software

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

/*

    Some remarks about the code:
    - 'c' is used for various purposes, and always has a negative value which means a the code of a key.
    - 'ch' is the character typed by the user.
    - 'Tab_stack[file_index].strsave' is the buffer where the file is stored on RAM.

    The source code of Newtrodit is composed by:^D

        dialog.h           : Dialogs
        globals.h          : Global variables
        manual.c           : Manual
        newtrodit.c        : Main source file
        newtrodit_core.h   : All core functions
            ~ win32/newtrodit_core_win.h
            ~ linux/newtrodit_core_linux.h
        newtrodit_func.c   : Main functions
            ~ win32/newtrodit_func_win.c
            ~ linux/newtrodit_func_linux.c
        newtrodit_gui.c    : GUI loading
        newtrodit_shared.c : Shared functions
        newtrodit_locate.c : File explorer
        newtrodit_syntax.h : Syntax highlighting

     See 'newtrodit --help'

*/

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include "manual.c"

void sigsegv_handler(int signum)
{
    signal(SIGSEGV, sigsegv_handler);
    NewtroditCrash(join("Segmentation fault.\nSignum code: ", itoa_n(signum)), errno);
    fflush(stdout);
    exit(errno);
}

void sigtrap_handler(int signum)
{
    signal(SIGTRAP, sigtrap_handler);
    NewtroditCrash(join("Sigtrap.\nSignum code: ", itoa_n(signum)), errno);
    fflush(stdout);
    exit(errno);
}

void sigabrt_handler(int signum)
{
    signal(SIGABRT, sigabrt_handler);
    NewtroditCrash(join("Abort signal.\nSignum code: ", itoa_n(signum)), errno);
    fflush(stdout);
    exit(errno);
}

void sigfpe_handler(int signum)
{
    signal(SIGFPE, sigabrt_handler);
    NewtroditCrash(join("Arithmetic exception.\nSignum code: ", itoa_n(signum)), errno);
    fflush(stdout);
    exit(errno);
}

int LoadSettings(char *newtrodit_config_file, char *macro, int *sigsegv, File_info *tstack)
{
    /*
        Settings are stored in an INI-like format.
        The format is:
            key=value
            ;comment

    */

    chdir(SInf.location);

    WriteLogFile("Loading settings file: %s", FullPath(newtrodit_config_file));

    FILE *settings = fopen(newtrodit_config_file, "rb");
    if (!settings)
    {
        return EXIT_FAILURE;
    }

    char setting_buf[1024]; // Max 1 kB per line
    char *iniptr = malloc(sizeof(setting_buf) + 1), *token = malloc(sizeof(setting_buf) + 1);
    char *settingname;
    int cnt = 0;
    bool isCorrectSetting = true;
    char equalchar[] = "=";
    /*
        Available settings:
            "autoindent",
            "autosyntax",
            "codepage",
            "convertnull",
            "converttab",
            "curinsert",
            "cursize",
            "devmode",
            "fontcolor",
            "linecount",
            "linecountwide",
            "macro",
            "manfile",
            "menucolor",
            "mouse",
            "newline",
            "oldkeybindings",
            "sigsegv",
            "syntax",
            "tabwide",
            "trimlonglines",
            "xsize",
            "ysize",
   */

    // Set the non changing settings
    SetColor(FG_DEFAULT);
    SetColor(BG_DEFAULT);
    default_color = DEFAULT_SYNTAX_COLOR;
    strncpy_n(Tab_stack[file_index].newline, "\n", strlen(Tab_stack[file_index].newline));
    run_macro[0] = 0;
    last_known_exception = NEWTRODIT_CRASH_INVALID_SETTINGS;
    while (fgets(setting_buf, sizeof(setting_buf), settings))
    {
        setting_buf[strcspn(setting_buf, "\n")] = 0; // Remove newline

        cnt = strspn(setting_buf, " \t");
        // memmove(&setting_buf[0], &setting_buf[cnt], (strlen(setting_buf) - cnt));
        //  memset(&setting_buf[strlen(setting_buf) - cnt], 0, (strlen(setting_buf) - cnt));
        // snprintf(setting_buf, sizeof(setting_buf), "%s", setting_buf - 1);

        if (setting_buf[cnt] == ';' || setting_buf[cnt] == 0) // Comment or newline found
        {
            continue;
        }
        iniptr = strtok(setting_buf, "=");
        settingname = strdup(iniptr);
        strlwr(settingname);

        while (iniptr != NULL) // Loop through the settings
        {
            isCorrectSetting = true;
            // iniptr[strcspn(iniptr, "\n")] = '\0';
            token = strtok(NULL, equalchar);
            if (!strcmp(settingname, "fontcolor"))
            {
                bg_color = HexStrToDec(token) % 256;
                default_color = bg_color;
            }
            else if (!strcmp(settingname, "autoindent"))
            {
                SetBoolValue(&autoIndent, token);
            }
            else if (!strcmp(settingname, "autosyntax"))
            {
                SetBoolValue(&autoLoadSyntaxRules, token);
            }
            else if (!strcmp(settingname, "codepage"))
            {
                int cp = atoi(token);
#ifdef _WIN32
                SetConsoleOutputCP(cp);
#else
                // Linux TODO
#endif
            }
            else if (!strcmp(settingname, "convertnull"))
            {
                SetBoolValue(&convertNull, token);
            }
            else if (!strcmp(settingname, "converttab"))
            {
                SetBoolValue(&convertTabtoSpaces, token);
            }
            else if (!strcmp(settingname, "curinsert"))
            {

                SetBoolValue(&cursorSizeInsert, token);
            }
            else

                if (!strcmp(settingname, "cursize"))
            {
                CURSIZE = atoi(token);
                SetCursorSettings(true, CURSIZE);
            }
            else if (!strcmp(settingname, "devmode"))
            {
                SetBoolValue(&devMode, token);
            }
            else if (!strcmp(settingname, "findinsensitive"))
            {
                SetBoolValue(&findInsensitive, token);
            }
            else if (!strcmp(settingname, "linecount"))
            {
                SetBoolValue(&lineCount, token);
            }
            else if (!strcmp(settingname, "linecountwide"))
            {
                LINECOUNT_WIDE = abs(atoi(token)) % sizeof(long long);
            }
            else if (!strcmp(settingname, "linehighlight"))
            {
                SetBoolValue(&linecountHighlightLine, token);
            }
            else if (!strcmp(settingname, "macro"))
            {

                if (ValidString(token))
                {
                    strncpy_n(run_macro, token, MACRO_ALLOC_SIZE);
                }
            }
            else if (!strcmp(settingname, "manfile"))
            {
                RemoveQuotes(token, strdup(token)); // Remove quotes

                if (ValidFileName(token))
                {
                    strncpy_n(manual_file, token, sizeof(manual_file));
                    manual_file[strcspn(manual_file, "\n")] = 0;
                }
                else
                {
                    WriteLogFile("%s%s", NEWTRODIT_FS_FILE_INVALID_NAME, token);
                }
            }
            else if (!strcmp(settingname, "menucolor"))
            {
                fg_color = (HexStrToDec(token) * 16) % 256;
            }
            else if (!strcmp(settingname, "mouse"))
            {
                SetBoolValue(&partialMouseSupport, token);
            }
            else

                if (!strcmp(settingname, "newline"))
            {

                if (!strncmp(token, "0x", 2))
                {
                    ParseHexString(token);
                }
                else
                {
                    strncpy_n(Tab_stack[file_index].newline, token, strlen(Tab_stack[file_index].newline));
                }
            }
            else

                if (!strcmp(settingname, "oldkeybindings"))
            {
                SetBoolValue(&useOldKeybindings, token);
            }
            else if (!strcmp(settingname, "sigsegv"))
            {
                SetBoolValue(sigsegv, token);
            }
            else if (!strcmp(settingname, "syntax"))
            {
                RemoveQuotes(token, strdup(token)); // Remove quotes
                if (ValidFileName(token))
                {

                    if (!strcmp(token, "1") || !strcmp(token, "true")) // Enable syntax highlighting but don't load any file
                    {
                        syntaxHighlighting = true;
                    }
                    else if (!strcmp(token, "0") || !strcmp(token, "false"))
                    {
                        syntaxHighlighting = false;
                    }
                    else
                    {
                        strncpy_n(syntax_filename, token, sizeof(syntax_filename));
                        syntax_filename[strcspn(syntax_filename, "\n")] = 0;
                        if ((syntaxKeywordsSize = LoadSyntaxScheme(syntax_filename, &Tab_stack[file_index])))
                        {
                            Tab_stack[file_index].Syntaxinfo.keyword_count = syntaxKeywordsSize;
                        }

                        syntaxHighlighting = true;
                    }
                }
            }
            else if (!strcmp(settingname, "tabwide"))
            {
                TAB_WIDE = atoi(token);
            }
            else if (!strcmp(settingname, "trimlonglines"))
            {
                SetBoolValue(&trimLongLines, token);
            }
            else if (!strcmp(settingname, "wholeword"))
            {
                SetBoolValue(&matchWholeWord, token);
            }
            else if (!strcmp(settingname, "xsize"))
            {
                int xs = atoi(token);
                SetConsoleSize(xs, YSIZE, xs, YSIZE);
            }
            else if (!strcmp(settingname, "ysize"))
            {
                int ys = atoi(token);
                SetConsoleSize(XSIZE, ys, XSIZE, ys);
            }
            else
            {
                isCorrectSetting = false;
            }
            if (isCorrectSetting)
            {
                WriteLogFile("Loaded setting: %s (value '%s')", settingname, token);
            }
            else
            {
                WriteLogFile("Unexistent setting: %s", settingname);
            }
            iniptr = strtok(NULL, "=");
        }
    }
    WriteLogFile("Finished loading settings file");

    fclose(settings);
    chdir(SInf.dir);

    return 0;
}

int main(int argc, char *argv[])
{
    // Newtrodit initialization begins

    char *startup_info = calloc(sizeof(char), MAX_PATH * 2); // *2 for safety
#ifdef _WIN32
    GetModuleFileNameA(NULL, startup_info, MAX_PATH);
#else
    startup_info = argv[0];   // Program name
#endif

    // Generate log file name
    SInf.log_file_name = GetLogFileName();
    SInf.using_log = !!WriteLogFile("\nNewtrodit started");

    WriteLogFile("Logfile opened with name: %s", SInf.log_file_name);


#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE), hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdout, &dwStdoutMode);
    GetConsoleMode(hStdin, &dwStdinMode);
    // Disable wrapping to avoid (or at least reduce) graphical bugs
    WriteLogFile("Changing console output mode: %s", (SetConsoleMode(hStdout, dwStdoutMode & ~ENABLE_WRAP_AT_EOL_OUTPUT)) ? "Succeeded" : "Failed");
    WriteLogFile("Changing console input mode: %s", (SetConsoleMode(hStdin, ENABLE_MOUSE_INPUT | 0x80 | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT)) ? "Succeeded" : "Failed"); // 0x40 is quick edit mode and 0x80 are extended flags

    WriteLogFile("Loading startup data");
#endif

    char *sinf_ptr = calloc(sizeof(char), MAX_PATH * 2);
    if (get_path_directory(startup_info, sinf_ptr) != NULL)
    {
        SInf.dir = strdup(getcwd(NULL, 0));
        SInf.location = strdup(sinf_ptr);
    }
    else
    {
        memset(SInf.location, 0, sizeof(char) * MAX_PATH * 2);
    }
    free(sinf_ptr);

    SInf.argv = argv; // Save only its memory address, not the actual value
    SInf.argc = argc;
    SInf.xsize = XSIZE;
    SInf.ysize = YSIZE;
    SInf.xbuf = GetConsoleInfo(XBUFFER_SIZE);
    SInf.ybuf = GetConsoleInfo(YBUFFER_SIZE);
#ifdef _WIN32
    SetConsoleSize(SInf.xsize, SInf.ysize, SInf.xsize, SInf.ysize); // Remove all the borders
#endif
    SInf.manual_open = 0; // Times manual has been open
    SInf.save_buffer = false;

    clearAllBuffer = true;
    LINECOUNT_WIDE = LINECOUNT_WIDE_; // Set line count wide

    int hasNewLine = false; // Bool for newline in insert char
    int insertChar = false; // Bool to check if replace instead of insert
    int sigsegvScreen = true;
    int listDir = true;
    int autocompleteGoodMatch = false; // Set automatically

    char wholeWordDelims[] = "`~!@#$%^&*()[]{}-=+\\|/;:\'\",.<>? \n\t\r";

    for (int i = 0; i < MAX_TABS; i++)
    {
        Tab_stack[i].filename = calloc(MAX_PATH, sizeof(char));
    }

    // Allocate buffer

    if (!AllocateBufferMemory(&Tab_stack[file_index]))
    {

        printf("%.*s\n", wrapSize, NEWTRODIT_ERROR_OUT_OF_MEMORY);
        ExitRoutine(ENOMEM);
    }

    old_open_files = malloc(MIN_BUFSIZE * sizeof(char *));

    for (int i = 1; i < MIN_BUFSIZE; i++)
    {
        old_open_files[i] = calloc(MAX_PATH, sizeof(char));
    }

    run_macro = calloc(sizeof(char), MACRO_ALLOC_SIZE + 1);
    SetWrapSize();
    WriteLogFile("Setting console signal handlers");
#ifdef _WIN32

    signal(SIGBREAK, SIG_IGN); // Ctrl-Break handler
#else
    signal(SIGTSTP, SIG_IGN); // Ctrl-Z handler
#endif
    signal(SIGINT, SIG_IGN);          // Ctrl-C handler
    signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
    signal(SIGABRT, sigabrt_handler); // Abort handler
    signal(SIGTRAP, sigtrap_handler);
    signal(SIGFPE, sigfpe_handler);
    WriteLogFile("Finished setting console signal handlers");

    LoadSettings(settings_file, run_macro, &sigsegvScreen, &Tab_stack[file_index]); // Load settings from settings file

    if (!sigsegvScreen)
    {
        signal(SIGSEGV, SIG_DFL);
    }

    if (BUFFER_X < MIN_BUFSIZE || BUFFER_Y < MIN_BUFSIZE)
    {
        char *err_msg = malloc(sizeof(char) * MIN_BUFSIZE);

        snprintf(err_msg, MIN_BUFSIZE, "Buffer is too small (Current size is %dx%d and minimum size is %dx%d)", BUFFER_X, BUFFER_Y, MIN_BUFSIZE, MIN_BUFSIZE);
#ifdef _WIN32
        MessageBox(NULL, err_msg, "Newtrodit", MB_ICONERROR);
#endif
        WriteLogFile(err_msg);
        fprintf(stderr, "%s", err_msg);
        free(err_msg);
        ExitRoutine(1);
    }
    char *temp_strsave = calloc(sizeof(char), BUFFER_X + 1);
    char *tmp = calloc(sizeof(char), BUFFER_X + 1);

    int undo_stack_tree = 0;
    // Declare variables
    int old_x_size = 0, old_y_size = 0;
    int bs_tk = 0;

    SInf.color = GetConsoleInfo(COLOR);
    int replace_count = 0;
    char *save_dest = calloc(sizeof(char), MAX_PATH + 1);
    char find_string[512] = {0}, replace_string[512] = {0};
    int find_string_index = 0;
    char *replace_str_ptr;

    char inbound_ctrl_key[100] = {0};
    char newname[MAX_PATH], syntaxfile[MAX_PATH], locate_file[MAX_PATH], macro_input[MACRO_ALLOC_SIZE], fcomp1[MAX_PATH], fcomp2[MAX_PATH], command_palette[DEFAULT_ALLOC_SIZE] = {0};
    convertTabtoSpaces = true;
    int n = 0, n2 = 0;
    char *ptr = calloc(sizeof(char), BUFFER_X), *buffer_clipboard;

    // File variables
    FILE *fp_savefile = {0};
    SetCurrentDirectory(SInf.dir);

    // Position variables
    int *relative_xpos = calloc(sizeof(int) * Tab_stack[file_index].bufy, BUFFER_X);
    int *relative_ypos = calloc(sizeof(int) * BUFFER_X, Tab_stack[file_index].bufy);

    // getch() variables
    int ch = 0;

    int *file_arguments = {0}; // Array of ints for arguments that aren't switches
    int file_arguments_count = 0;

    file_arguments = calloc(sizeof(int *), argc); // Allocate memory for file_arguments for each argument
    WriteLogFile("Finished loading startup data");

    WriteLogFile("Parsing command-line arguments");
    ParseArguments(argc, argv, file_arguments, &file_arguments_count);

    if (file_arguments_count > 0)
    {
        open_files = file_arguments_count;
        for (int i = 1; i <= file_arguments_count; i++)
        {

            Tab_stack[file_index].filename = argv[file_arguments[i]];
            if ((n = LoadFile(&Tab_stack[file_index], Tab_stack[file_index].filename)) <= -1)
            {
                fprintf(stderr, "%s", ErrorMessage(-n, Tab_stack[file_index].filename));
                return errno;
            }

            if (file_arguments_count > 1 && i < file_arguments_count) // file_arguments_count - 1 iterations to not allocate an extra buffer
            {
                if (file_index < MAX_TABS)
                {
                    file_index++;
                    if (!AllocateBufferMemory(&Tab_stack[file_index])) // Allocate more memory for next file
                    {
                        printf("%.*s\n", wrapSize, NEWTRODIT_ERROR_OUT_OF_MEMORY);
                        ExitRoutine(ENOMEM);
                    }
                }
            }
        }

        LoadAllNewtrodit();
        DisplayFileContent(&Tab_stack[file_index], stdout, 0);
    }
    else
    {
        LoadAllNewtrodit();
    }

    clearAllBuffer = false;

    if (partialMouseSupport)
    {
        WriteLogFile("Mouse enabled.");
    }
    while (1)
    {
        UpdateTitle(&Tab_stack[file_index]);

        old_y_size = YSIZE;
        old_x_size = XSIZE;

        SetDisplayY(&Tab_stack[file_index]);

        if ((lineCount && !isprint(ch) && c != -32) || linecountHighlightLine) // -32 = scroll
        {
            DisplayLineCount(&Tab_stack[file_index], YSIZE - 3, Tab_stack[file_index].display_y);
        }
        if (c == -32)
        {
            c = 0;
        }

        if (c != -2) // Clear bottom line
        {
            DisplayCursorPos(&Tab_stack[file_index]);
        }
        SetWrapSize();
        SetDisplayX(&Tab_stack[file_index]);

        SetDisplayCursorPos(&Tab_stack[file_index]);
#ifdef _WIN32
        CompareFileWriteTime(&Tab_stack[file_index]);
#endif

        if (Tab_stack[file_index].selection.is_selected)
        {
            WriteLogFile("Selection: (%d,%d) to (%d,%d)", Tab_stack[file_index].selection.start.x, Tab_stack[file_index].selection.start.y, Tab_stack[file_index].selection.end.x, Tab_stack[file_index].selection.end.y);
            SelectPrint(&Tab_stack[file_index], _ypos);
        }

        if (devMode && (Tab_stack[file_index].strsave[_ypos][_xpos] > 127 || Tab_stack[file_index].strsave[_ypos][_xpos] < 0))
        {
            PrintBottomString("Dev mode warning: Possible stack overrun found: '%c' (0x%02x) at X=%d Y=%d", Tab_stack[file_index].strsave[_ypos][_xpos], Tab_stack[file_index].strsave[_ypos][_xpos], _xpos, _ypos);
            c = -2;
        }
        ch = GetNewtroditInput(&Tab_stack[file_index]); // Register all input events, not only key presses

        if ((_xpos < 0 || _ypos < 1) || _xpos > strlen_n(Tab_stack[file_index].strsave[_ypos]) || _ypos > Tab_stack[file_index].linecount + 1 || _ypos >= Tab_stack[file_index].bufy)
        {
            PrintBottomString("%s", NEWTRODIT_ERROR_INVALID_POS_RESET);
            WriteLogFile("(%d,%d) %s", _xpos, _ypos, NEWTRODIT_ERROR_INVALID_POS_RESET);
            getch_n();
            _xpos = 0;
            _ypos = 1;
            c = -2;
        }

        if (c == -2) // Inbound invalid control key
        {
            ShowBottomMenu();
            DisplayCursorPos(&Tab_stack[file_index]);

            gotoxy(_xpos + relative_xpos[_ypos] + (lineCount ? Tab_stack[file_index].linecount_wide : 0), _ypos + relative_ypos[_xpos]);
            c = 0;
        }
        if (!allowAutomaticResizing)
        {
            if (old_x_size != XSIZE || old_y_size != YSIZE) // Check if size has been modified
            {
                n = 0;
                while (!ValidSize()) // At 3 message boxes, close the program
                {
                    if (n == 2)
                    {
                        ClearScreen();
                        fprintf(stderr, "%s\n", NEWTRODIT_ERROR_WINDOW_TOO_SMALL);
                        ExitRoutine(1);
                    }
                    n++;
                }
                clearAllBuffer = true;

                LoadAllNewtrodit();
                clearAllBuffer = false;

                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
            }
        }

        if (ch == CTRLA) // S-^A = Toggle auto syntax highlighting loading
        {
            if (CheckKey(VK_SHIFT))
            {
                ToggleOption(&autoLoadSyntaxRules, NEWTRODIT_AUTO_SYNTAX_LOAD, false);
                c = -2;
                ch = 0;
                continue;
            }
        }

        if (ch == CTRLC || ch == CTRLK) // ^C = Copy line to clipboard; ^K = Cut line
        {
            if (!CheckKey(VK_SHIFT))
            {
                if (Tab_stack[file_index].strsave[_ypos][0] != '\0')
                {

#ifdef _WIN32
                    SetClipboardNewtrodit(Tab_stack[file_index].strsave[_ypos]);

                    if (ch == CTRLK && useOldKeybindings)
                    {
                        if (Tab_stack[file_index].strsave[_ypos][0] != '\0')
                        {
                            memset(Tab_stack[file_index].strsave[_ypos], 0, strlen(Tab_stack[file_index].strsave[_ypos]));
                            strncpy_n(Tab_stack[file_index].strsave[_ypos], Tab_stack[file_index].newline, strlen(Tab_stack[file_index].newline));
                            ClearPartial((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y, XSIZE - (lineCount ? Tab_stack[file_index].linecount_wide : 0), 1);
                            _xpos = 0;
                        }
                    }
#endif
                }
                ch = 0;
                continue;
            }
            else
            {
                if (ch == CTRLK)
                {
                    ToggleOption(&useOldKeybindings, NEWTRODIT_OLD_KEYBINDINGS, true);

                    c = -2;
                    ch = 0;
                    continue;
                }
                if (ch == CTRLC) // S-^C = File compare
                {
                    memset(fcomp1, 0, sizeof(fcomp1));
                    memset(fcomp2, 0, sizeof(fcomp2));
                    ClearPartial(0, YSIZE - 2, XSIZE, 2);
                    printf("%.*s\n", wrapSize, NEWTRODIT_PROMPT_FIRST_FILE_COMPARE);
                    printf("%.*s", wrapSize, NEWTRODIT_PROMPT_SECOND_FILE_COMPARE);
                    gotoxy(strlen(NEWTRODIT_PROMPT_FIRST_FILE_COMPARE), YSIZE - 2);
                    fgets(fcomp1, sizeof(fcomp1), stdin);
                    if (NoLfLen(fcomp1) <= 0)
                    {
                        FunctionAborted(&Tab_stack[file_index]);
                        continue;
                    }
                    gotoxy(strlen(NEWTRODIT_PROMPT_SECOND_FILE_COMPARE), BOTTOM);

                    fgets(fcomp2, sizeof(fcomp2), stdin);
                    if (NoLfLen(fcomp2) <= 0)
                    {
                        FunctionAborted(&Tab_stack[file_index]);
                        continue;
                    }

                    fcomp1[strcspn(fcomp1, "\n")] = 0;
                    fcomp2[strcspn(fcomp2, "\n")] = 0;
                    RemoveQuotes(fcomp1, strdup(fcomp1));
                    RemoveQuotes(fcomp2, strdup(fcomp2));

                    LoadAllNewtrodit();
                    DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                    if (!strncmp(fcomp1, fcomp2, sizeof(fcomp1)))
                    {
                        PrintBottomString("%s", NEWTRODIT_FS_SAME_FILE);
                        getch_n();
                        ShowBottomMenu();
                        DisplayCursorPos(&Tab_stack[file_index]);
                        continue;
                    }

                    FileCompare(fcomp1, fcomp2);
                    ShowBottomMenu();
                    DisplayCursorPos(&Tab_stack[file_index]);
                    ch = 0;
                    continue;
                }
            }
        }

        if (ch == CTRLG) // ^G = Go to line; S-^G = Go to column
        {

            GotoBufferPosition(&Tab_stack[file_index], 0, CheckKey(VK_SHIFT));

            ch = 0;
            continue;
        }

        if (ch == CTRLI && !CheckKey(VK_TAB)) // ^I = File info
        {
            if (!CheckKey(VK_SHIFT))
            {
                CountBufferLines(&Tab_stack[file_index]);
                PrintBottomString("File: \'%s\', size: %lld bytes (%u lines). File type: %s. Syntax highlighting: %s", StrLastTok(Tab_stack[file_index].filename, PATHTOKENS), Tab_stack[file_index].size, Tab_stack[file_index].linecount, Tab_stack[file_index].language, Tab_stack[file_index].Syntaxinfo.syntax_lang);
                c = -2;
                ch = 0;
            }
        }

        if (ch == CTRLL) // ^L = Toggle line count; S-^L = Locate files
        {
            if (!CheckKey(VK_SHIFT))
            {
                ToggleOption(&lineCount, NEWTRODIT_LINE_COUNT, true);
                c = -2;
            }
            else
            {
                // List all files in the directory
                memset(locate_file, 0, sizeof(locate_file));
                PrintBottomString("%s", NEWTRODIT_PROMPT_LOCATE_FILE);
                fgets(locate_file, sizeof locate_file, stdin);
                if (NoLfLen(locate_file) <= 0)
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    continue;
                }
                locate_file[strcspn(locate_file, "\n")] = 0;

                LoadAllNewtrodit();
                ClearPartial(0, 1, XSIZE, YSIZE - 1);
                LocateFiles(listDir, locate_file, 0);
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                ClearPartial(0, 1, XSIZE, YSIZE - 2);
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
            }
            continue;
        }

        if (ch == CTRLN) // ^N = New file
        {
            if (!CheckKey(VK_SHIFT))
            {

                if ((n = NewFile(&Tab_stack[file_index])) > 0)
                {
                    PrintBottomString("%s", NEWTRODIT_NEW_FILE_CREATED);
                }
                else
                {
                    ErrorMessage(n, Tab_stack[file_index].filename);
                }
            }
            else
            {

                ToggleOption(&convertNull, NEWTRODIT_NULL_CONVERSION, false);
            }
            c = -2;

            ch = 0;
            continue;
        }
        if (ch == CTRLO) // ^O = Open file
        {
            if (!CheckKey(VK_SHIFT))
            {
                OpenNewtroditFile(&Tab_stack[file_index], NULL);
            }
            ch = 0;
        }
        if (ch == CTRLP) // ^P = Command palette
        {
            ch = 0;

            PrintBottomString("> ");
            fgets(command_palette, sizeof(command_palette), stdin);
            command_palette[strcspn(command_palette, "\n")] = 0;
            LoadAllNewtrodit();
            DisplayFileContent(&Tab_stack[file_index], stdout, 0);
            if (NoLfLen(command_palette) < 0)
            {
                FunctionAborted(&Tab_stack[file_index]);
                continue;
            }
            if (!ParseCommandPalette(&Tab_stack[file_index], command_palette))
            {
                PrintBottomString(NEWTRODIT_ERROR_UNKNOWN_COMMAND, command_palette);
                c = -2;
            }
        }

        if (ch == CTRLT) // S-^T = Toggle tab conversion
        {
            /* if (CheckKey(VK_SHIFT) && devMode)
            {
                ToggleOption(&convertTabtoSpaces, NEWTRODIT_TAB_CONVERSION, false);
                c = -2;

                ch = 0;
                continue;
            } */
            if (CheckKey(VK_SHIFT)) // S-^T
            {
                if (old_open_files[0][0] != '\0' && oldFilesIndex > 0)
                {
                    if (Tab_stack[file_index].is_modified)
                    {
                        PrintBottomString("%s", NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);

                        if (YesNoPrompt())
                        {
                            if (SaveFile(&Tab_stack[file_index], NULL))
                            {
                                LoadAllNewtrodit();
                                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                                continue;
                            }
                        }
                    }
                    PrintBottomString("%s", NEWTRODIT_PROMPT_REOPEN_FILE);
                    if (YesNoPrompt())
                    {
                        if (LoadFile(&Tab_stack[file_index], old_open_files[oldFilesIndex]))
                        {
                            strncpy_n(old_open_files[oldFilesIndex], Tab_stack[file_index].filename, MAX_PATH);
                            LoadAllNewtrodit();
                            DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                        }
                    }
                }
                continue;
            }
        }

        if (ch == CTRLR) // ^R = Reload file and S-^R = Reload settings
        {
            if (CheckKey(VK_SHIFT)) // S-^R = Reload settings
            {
                PrintBottomString("%s", NEWTRODIT_PROMPT_RELOAD_SETTINGS);
                if (YesNoPrompt())
                {
                    if (LoadSettings(settings_file, run_macro, &sigsegvScreen, &Tab_stack[file_index]) == 0) // Reload settings
                    {
                        LoadAllNewtrodit();
                        DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                        PrintBottomString("%s", NEWTRODIT_SETTINGS_RELOADED);
                    }
                    else
                    {
                        PrintBottomString("%s", NEWTRODIT_ERROR_RELOAD_SETTINGS);
                    }
                    getch_n();
                }
                DisplayCursorPos(&Tab_stack[file_index]);
                ShowBottomMenu();
            }
            else
            {

                // ^R = Reload file
                PrintBottomString("%s", NEWTRODIT_PROMPT_RELOAD_FILE);

                if (YesNoPrompt())
                {
                    ReloadFile(&Tab_stack[file_index]);
                    if (Tab_stack[file_index].is_readonly && !Tab_stack[file_index].is_untitled)
                    {
                        PrintBottomString("%s", NEWTRODIT_WARNING_READONLY_FILE);
                        c = -2;
                    }
                }
                else
                {
                    ShowBottomMenu();
                }
            }
            ch = 0;

            continue;
        }

        if (ch & BIT_ESC224) // Special keys: 224 (0xE0)
        {

            c = -32;
            SelectCheck(&Tab_stack[file_index]);

#ifdef _WIN32
            switch (ch & (~BIT_ESC224))
#else
            switch (ch)
#endif
            {

            case UP:
                // Up arrow
                if (_ypos > 1)
                {
                    _ypos--;

                    if (Tab_stack[file_index].strsave[_ypos][_xpos] == '\0')
                    {

                        _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                        WriteLogFile("%d", _xpos);
                        SetDisplayX(&Tab_stack[file_index]);
                    }
                    else
                    {

                        /*
                        DEBUG
                        printf("'%c:%d'", Tab_stack[file_index].strsave[_ypos][_xpos], Tab_stack[file_index].strsave[_ypos][_xpos]);
                        getch_n(); */
                    }
                    SelectEnd(&Tab_stack[file_index], _xpos, _ypos);
                }
                if (!UpdateHorizontalScroll(&Tab_stack[file_index], false))
                {
                    UpdateScrolledScreen(&Tab_stack[file_index]);
                }
                break;

            case LEFT:
                // Left arrow

                if (_xpos > 0)
                {
                    if (Tab_stack[file_index].strsave[_ypos][_xpos - 1] == 9)
                    {
                        relative_xpos[_ypos] -= TAB_WIDE;
                    }

                    _xpos--;

                    SetDisplayX(&Tab_stack[file_index]);
                    UpdateHorizontalScroll(&Tab_stack[file_index], false);
                }
                else
                {
                    if (_ypos > 1)
                    {

                        _xpos = NoLfLen(Tab_stack[file_index].strsave[--_ypos]);
                        SetDisplayX(&Tab_stack[file_index]);
                        UpdateScrolledScreen(&Tab_stack[file_index]);
                    }
                }

                break;
            case RIGHT:
                // Right arrow

                if (Tab_stack[file_index].strsave[_ypos][_xpos] != '\0')
                {
                    if (_xpos == NoLfLen(Tab_stack[file_index].strsave[_ypos]))
                    {
                        if (_ypos < Tab_stack[file_index].bufy - 1)
                        {
                            if (Tab_stack[file_index].strsave[_ypos + 1][0] != '\0' || LineContainsNewLine(&Tab_stack[file_index], _ypos))
                            {
                                _xpos = 0;
                                _ypos++;
                                if (!UpdateHorizontalScroll(&Tab_stack[file_index], true))
                                {
                                    UpdateScrolledScreen(&Tab_stack[file_index]);
                                }
                            }
                        }
                    }
                    else
                    {

                        _xpos++;
                        SelectEnd(&Tab_stack[file_index], _xpos, _ypos);

                        if (_xpos > wrapSize)
                        {
                            Tab_stack[file_index].display_x++;
                            UpdateHorizontalScroll(&Tab_stack[file_index], false);
                        }
                    }
                }
                if (BufferLimit(&Tab_stack[file_index]))
                {
                    ShowBottomMenu();
                    continue;
                }

                break;
            case DOWN:
                // Down arrow

                if (_ypos < Tab_stack[file_index].linecount)
                {
                    n2 = _ypos;
                    if (_ypos < Tab_stack[file_index].bufy - 1)
                    {
                        if (Tab_stack[file_index].strsave[_ypos + 1][0] != '\0' || LineContainsNewLine(&Tab_stack[file_index], _ypos))
                        {

                            if (Tab_stack[file_index].strsave[_ypos + 1][_xpos] == '\0')
                            {
                                _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos + 1]); // Add tab wide
                            }
                            _ypos++;
                        }
                    }
                    if (BufferLimit(&Tab_stack[file_index]))
                    {
                        _ypos = n2; // Restore ypos if a position outside the buffer is reached
                        ShowBottomMenu();
                        continue;
                    }
                    if (!UpdateHorizontalScroll(&Tab_stack[file_index], true))
                    {
                        if (_ypos <= Tab_stack[file_index].linecount)
                        {
                            UpdateScrolledScreen(&Tab_stack[file_index]);
                        }
                    }
                }

                break;

            case PGUP:

                n = _xpos;

                (_ypos < YSCROLL - 1) ? (_ypos = 1) : (_ypos -= YSCROLL - 1);
                if (NoLfLen(Tab_stack[file_index].strsave[_ypos]) < n)
                {
                    _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                }
                if (!UpdateHorizontalScroll(&Tab_stack[file_index], true))
                {
                    UpdateScrolledScreen(&Tab_stack[file_index]);
                }
                break;
            case PGDW:
                if (_ypos < Tab_stack[file_index].linecount)
                {
                    n = _xpos;

                    (_ypos + YSCROLL - 1 > Tab_stack[file_index].linecount) ? (_ypos = Tab_stack[file_index].linecount) : (_ypos += YSCROLL - 1);
                    if(_ypos >= Tab_stack[file_index].bufy)
                    {
                        _ypos = Tab_stack[file_index].bufy - 1;
                    }
                    if (NoLfLen(Tab_stack[file_index].strsave[_ypos]) < n)
                    {
                        _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                    }
                    if (!UpdateHorizontalScroll(&Tab_stack[file_index], true))
                    {
                        UpdateScrolledScreen(&Tab_stack[file_index]);
                    }
                }
                break;
            case HOME:
                // HOME key
                _xpos = 0;

                UpdateHorizontalScroll(&Tab_stack[file_index], true);

                break;
            case END:
                // END key
                n = _xpos;
                _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                if (n == _xpos) // Optimization
                {
                    break;
                }
                SetDisplayX(&Tab_stack[file_index]);
                UpdateHorizontalScroll(&Tab_stack[file_index], false);
                break;
            case CTRLEND:
                if (_ypos < Tab_stack[file_index].linecount && _xpos < NoLfLen(Tab_stack[file_index].strsave[_ypos]))
                {
                    // ^END key
                    _ypos = (Tab_stack[file_index].linecount >= Tab_stack[file_index].bufy) ? Tab_stack[file_index].bufy-1 : Tab_stack[file_index].linecount;
                    Tab_stack[file_index].last_y = _ypos;
                    _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                    SetDisplayX(&Tab_stack[file_index]);

                    if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                    {
                        UpdateHorizontalScroll(&Tab_stack[file_index], false);
                    }
                }

                break;
            case CTRLHOME:
                // ^HOME key

                _xpos = 0;
                _ypos = 1;

                if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                {
                    UpdateHorizontalScroll(&Tab_stack[file_index], true);
                }

                Tab_stack[file_index].last_y = _ypos;

                break;

            case INS:
                // INS key
                insertChar = !insertChar;
                if (cursorSizeInsert)
                {
                    insertChar ? SetCursorSettings(true, CURSIZE_INS) : SetCursorSettings(true, CURSIZE);
                }

                break;
            case F12: // F12
            {
                if (devMode)
                {
                    PrintBottomString("Keyword count: %d. Language: %s.", Tab_stack[file_index].Syntaxinfo.keyword_count, Tab_stack[file_index].Syntaxinfo.syntax_file);
                    c = -2;
                }
                break;
            }
            case DEL: // DEL and S-DEL
                SelectClear(&Tab_stack[file_index]);
                if (CheckKey(VK_SHIFT)) // S-DEL
                {
                    if (_ypos > 0)
                    {
                        memset(Tab_stack[file_index].strsave[_ypos], 0, Tab_stack[file_index].linesize[_ypos]);

                        DeleteRow(Tab_stack[file_index].strsave, _ypos, Tab_stack[file_index].bufy);

                        if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                        {
                            ClearPartial((lineCount ? (Tab_stack[file_index].linecount_wide) : 0), Tab_stack[file_index].display_y, XSIZE - (lineCount ? (Tab_stack[file_index].linecount_wide) : 0), (YSIZE - Tab_stack[file_index].display_y) - 1);
                            DisplayFileContent(&Tab_stack[file_index], stdout, Tab_stack[file_index].display_y - 1);
                        }

                        _xpos = 0;
                        Tab_stack[file_index].is_modified = true;
                        Tab_stack[file_index].linecount--;
                    }
                }
                else // DEL key
                {
                    if (strlen(Tab_stack[file_index].strsave[_ypos]) > 0) // TODO: Fix this bug in the line number 1
                    {
                        SetDisplayY(&Tab_stack[file_index]);
                        DeleteChar(Tab_stack[file_index].strsave[_ypos], _xpos);
                        n = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                        if (_xpos < n || (Tab_stack[file_index].strsave[_ypos][_xpos] == '\0' && _ypos >= Tab_stack[file_index].linecount))
                        {

                            gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                            print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                            ClearPartial(n + (lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y, 1, 1);
                            Tab_stack[file_index].is_modified = true;
                        }
                        else
                        {
                            if (_ypos < Tab_stack[file_index].bufy - 1 && _ypos < Tab_stack[file_index].linecount) // Don't try to delete a non-existing row
                            {

                                memset(Tab_stack[file_index].strsave[_ypos] + n, 0, BUFFER_X - n);                                                                         // Empty the new line
                                strncat(Tab_stack[file_index].strsave[_ypos], Tab_stack[file_index].strsave[_ypos + 1], strlen(Tab_stack[file_index].strsave[_ypos + 1])); // Concatenate the next line

                                DeleteRow(Tab_stack[file_index].strsave, _ypos + 1, Tab_stack[file_index].bufy); // Delete the old row, shifting other rows up
                                Tab_stack[file_index].linecount--;
                                if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                                {
                                    ClearPartial(0, Tab_stack[file_index].display_y, XSIZE, (YSIZE - Tab_stack[file_index].display_y) - 1);
                                    DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                                }
                                Tab_stack[file_index].is_modified = true;
                            }
                        }
                    }
                }

                break;
            }

            if (ch != 83)
            {
                ch = 0;
            }
            SelectEnd(&Tab_stack[file_index], _xpos, _ypos);

            continue;
        }
#ifdef _WIN32
        if (ch == CTRLM && !CheckKey(VK_RETURN) && !_NEWTRODIT_OLD_SUPPORT) // ^M = Toggle mouse
        {

            if (!CheckKey(VK_SHIFT))
            {

                ToggleOption(&partialMouseSupport, NEWTRODIT_MOUSE, false);
                c = -2;
                ch = 0;
                continue;
            }
        }

        if ((ch == ENTER && CheckKey(VK_RETURN)) || (ch == ENTER && _NEWTRODIT_OLD_SUPPORT)) // Newline character: CR (13)
#else
        if (ch == ENTER)
#endif
        {
            if (_ypos < Tab_stack[file_index].bufy - 1)
            {
                n = 1;
                if (Tab_stack[file_index].strsave[_ypos + 1][0] != '\0' || _xpos <= NoLfLen(Tab_stack[file_index].strsave[_ypos]))
                {
                    /*  n = strlen(Tab_stack[file_index].strsave[_ypos]) + 1;
                     Tab_stack[file_index].Ustack = (Undo_stack *)realloc(Tab_stack[file_index].Ustack, sizeof(Undo_stack) * (sizeof(&Tab_stack[file_index].Ustack) + 1));
                     Tab_stack[file_index].Ustack->size = n;

                     Tab_stack[file_index].Ustack->line = malloc(Tab_stack[file_index].Ustack->size);
                     memset(Tab_stack[file_index].Ustack->line, 0, Tab_stack[file_index].Ustack->size);
                     Tab_stack[file_index].Ustack->line = strdup(Tab_stack[file_index].strsave[_ypos]); // If it's not a duplicate, the value will change
                     Tab_stack[file_index].Ustack->line_count = _ypos;
                     Tab_stack[file_index].Ustack->line_pos = _xpos;
                     Tab_stack[file_index].Ustack->create_nl = true;
                     Tab_stack[file_index].Ustack++->delete_nl = false; */
                    InsertNewRow(&Tab_stack[file_index], &_xpos, &_ypos, Tab_stack[file_index].display_y, BUFFER_X);
                    _xpos = AutoIndent(&Tab_stack[file_index]); // Set the X position depending if auto indent is enabled or not
                }
                else
                {
                    strncat(Tab_stack[file_index].strsave[_ypos++], Tab_stack[file_index].newline, strlen(Tab_stack[file_index].newline)); // Add newline to current line
                    _xpos = AutoIndent(&Tab_stack[file_index]);

                    n = 0;
                }
                if (!UpdateHorizontalScroll(&Tab_stack[file_index], true))
                {
                    UpdateScrolledScreen(&Tab_stack[file_index]);
                }
                if (Tab_stack[file_index].linecount < Tab_stack[file_index].bufy)
                {
                    Tab_stack[file_index].linecount++; // Increment line count only if everything fits in the buffer
                }
                Tab_stack[file_index].is_modified = true;
            }

            continue;
        }
        if (ch == CTRLE) // ^E = Toggle syntax highlighting / S-^E = Set syntax highlighting rules file
        {
            if (CheckKey(VK_SHIFT))
            {
                PrintBottomString("%s", NEWTRODIT_PROMPT_SYNTAX_FILE);

                memset(syntaxfile, 0, sizeof syntaxfile);
                fgets(syntaxfile, sizeof syntaxfile, stdin);
                if (NoLfLen(syntaxfile) <= 0)
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    continue;
                }
                LoadAllNewtrodit();
                syntaxfile[strcspn(syntaxfile, "\n")] = 0;

                if ((syntaxKeywordsSize = LoadSyntaxScheme(syntaxfile, &Tab_stack[file_index])) != 0) // Change keywords size
                {
                    syntaxHighlighting = true;
                    Tab_stack[file_index].Syntaxinfo.syntax_file = FullPath(syntaxfile);
                    Tab_stack[file_index].Syntaxinfo.keyword_count = syntaxKeywordsSize;
                    PrintBottomString(NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED, Tab_stack[file_index].Syntaxinfo.syntax_lang);
                }
                else
                {
                    PrintBottomString("%s", NEWTRODIT_SYNTAX_HIGHLIGHTING_FAILED);
                }
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                getch_n();

                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                ch = 0;
                continue;
            }
            else
            {
                ToggleOption(&syntaxHighlighting, NEWTRODIT_SYNTAX_HIGHLIGHTING, false);
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                c = -2;
            }

            ch = 0;
            continue;
        }
        if (ch == CTRLF) // ^F = Find string
        {

            // Empty values
            memset(find_string, 0, sizeof find_string);
            find_string_index = 0;
            n2 = _ypos;

            c = 0;

            findInsensitive = false;
            if (CheckKey(VK_SHIFT))
            {
                findInsensitive = true;
                PrintBottomString("%s", NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE);
            }
            else
            {
                PrintBottomString("%s", NEWTRODIT_PROMPT_FIND_STRING);
            }

            fgets(find_string, sizeof find_string, stdin);
            if (NoLfLen(find_string) <= 0)
            {
                FunctionAborted(&Tab_stack[file_index]);
                continue;
            }
            find_string[strcspn(find_string, "\n")] = '\0';
            LoadAllNewtrodit();
            DisplayFileContent(&Tab_stack[file_index], stdout, 0);

            for (size_t i = 0; i <= Tab_stack[file_index].linecount; i++)
            {
                n = 0;
                if (Tab_stack[file_index].strsave[i][0] == '\0') // Shouldn't happen, but just an extra check
                {
                    break;
                }

                find_string_index = 0;
                while (n >= 0 && c != 27)
                {

                    n = ReturnFindIndex(findInsensitive, Tab_stack[file_index].strsave[i] + find_string_index, find_string);
                    if (n >= 0)
                    {
                        DEBUG

                        /* printf("{%.*s, %d:%d:%d:%d}", strlen(find_string), Tab_stack[file_index].strsave[i] + find_string_index + n, IsWholeWord(Tab_stack[file_index].strsave[i] + find_string_index + n, find_string, wholeWordDelims), i, find_string_index, n);
                        getch_n(); */
                        if (!matchWholeWord || (matchWholeWord && IsWholeWord(Tab_stack[file_index].strsave[i] + find_string_index + n, find_string, wholeWordDelims)) > 0)
                        {
                            DEBUG

                            n2 = i; //
                            _ypos = i;

                            // Calculate scroll position

                            // SetDisplayX(&Tab_stack[file_index]); TODO: Add horizontal scholl suport
                            SetDisplayY(&Tab_stack[file_index]);

                            UpdateScrolledScreen(&Tab_stack[file_index]);
                            SetCharColor(strlen(find_string), FIND_HIGHLIGHT_COLOR, find_string_index + n + (lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                            SetColor(bg_color);
                            ShowFindMenu();
                            gotoxy(find_string_index + n + (lineCount ? Tab_stack[file_index].linecount_wide : 0) + strlen(find_string), Tab_stack[file_index].display_y);

                            c = getch_n();
                            if (c != BIT_ESC224)
                            {
                                if (c == ESC) // Quit find
                                {
                                    break;
                                }
                                else if (c & BIT_ESC0 && (c & ~BIT_ESC0) == F3) // F3 = Next
                                {

                                    if (i < Tab_stack[file_index].bufy || i <= Tab_stack[file_index].linecount)
                                    {
                                        c = -3;
                                    }
                                }
                                else if (c & BIT_ESC0 && (c & ~BIT_ESC0) == F4) // F4 = Toggle case sensitive
                                {
                                    ToggleOption(&findInsensitive, NEWTRODIT_FIND_CASE_SENSITIVE, false);
                                    gotoxy(find_string_index + n + (lineCount ? Tab_stack[file_index].linecount_wide : 0) + strlen(find_string), Tab_stack[file_index].display_y); // Keep the cursor in the same place
                                    getch_n();
                                    c = -1;
                                }
                                else if (c & BIT_ESC0 && (c & ~BIT_ESC0) == F5) // F5 = Full words
                                {
                                    ToggleOption(&matchWholeWord, NEWTRODIT_FIND_MATCH_WHOLE_WORD, false);
                                    gotoxy(find_string_index + n + (lineCount ? Tab_stack[file_index].linecount_wide : 0) + strlen(find_string), Tab_stack[file_index].display_y); // Keep the cursor in the same place
                                    getch_n();
                                    c = -1;
                                }
                                else if (c & BIT_ESC0 && ((c & ~BIT_ESC0) == ALTF4)) // A-F4
                                {
                                    QuitProgram(SInf.color);
                                    c = -1;
                                }
                            }
                        }
                        else
                        {
                            c = -3;
                        }
                        if (c == -3)
                        {
                            find_string_index += n + 1; // Next
                        }
                    }
                }

                i++;

                _xpos = find_string_index + strlen(find_string);
                if (c == ESC)
                {
                    break;
                }
            }
            if (c != ESC)
            {
                PrintBottomString("%s", NEWTRODIT_FIND_NO_MORE_MATCHES);
                c = -2;
            }

            ch = 0;
            continue;
        }
#ifdef _WIN32
        if (ch & BIT_ESC0)
#else
        if (ch & BIT_ESC224)
#endif
        {
            switch (ch & ~(BIT_ESC0))
            {
            case CTRLALTR: // ^A-R (ROT13)
                memset(temp_strsave, 0, BUFFER_X);
                strncpy_n(temp_strsave, Tab_stack[file_index].strsave[_ypos], strlen(Tab_stack[file_index].strsave[_ypos]));
                if (rot13(Tab_stack[file_index].strsave[_ypos]))
                {
                    ClearPartial(0, Tab_stack[file_index].display_y, XSIZE, 1);
                    gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                    print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                }
                break;
            case CTRLALTU: // A-^U (Uppercase)
                for (int i = 0; i < strlen(Tab_stack[file_index].strsave[_ypos]); i++)
                {
                    Tab_stack[file_index].strsave[_ypos][i] = toupper(Tab_stack[file_index].strsave[_ypos][i]);
                }
                gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                break;

            case CTRLALTL: // A-^L (Lowercase)
                for (int i = 0; i < strlen(Tab_stack[file_index].strsave[_ypos]); i++)
                {
                    Tab_stack[file_index].strsave[_ypos][i] = tolower(Tab_stack[file_index].strsave[_ypos][i]);
                }
                gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                break;
            case CTRLALTN: // A-^N (New file and save)

                if (CheckKey(VK_MENU))
                {

                    NewFile(&Tab_stack[file_index]);
                    PrintBottomString("%s", NEWTRODIT_NEW_FILE_CREATED);
                    getch_n();

                    SaveFile(&Tab_stack[file_index], NULL);

                    DisplayCursorPos(&Tab_stack[file_index]);

                    ShowBottomMenu();
                }
                break;

            case F1: // F1 key

                NewtroditHelp();

                LoadAllNewtrodit();

                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                DisplayCursorPos(&Tab_stack[file_index]);
                break;
            case F2: // F2 key
                if (Tab_stack[file_index].is_untitled || !Tab_stack[file_index].is_saved)
                {
                    if (!SaveFile(&Tab_stack[file_index], NULL))
                    {
                        ShowBottomMenu();
                        DisplayCursorPos(&Tab_stack[file_index]);
                        ch = 0;
                        continue;
                    }
                }

                PrintBottomString("%s", NEWTRODIT_PROMPT_RENAME_FILE);
                fgets(newname, sizeof(newname), stdin);
                newname[strcspn(newname, "\n")] = 0;
                LoadAllNewtrodit();
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                if (NoLfLen(newname) > 0)
                {
                    if (!CheckFile(newname))
                    {
                        PrintBottomString("%s", NEWTRODIT_PROMPT_OVERWRITE);
                        if (YesNoPrompt())
                        {
                            if (remove(newname) != 0)
                            {
                                PrintBottomString("%s%s", NEWTRODIT_FS_FILE_DELETE, newname);
                                WriteLogFile("%s%s", NEWTRODIT_FS_FILE_DELETE, newname);
                                getch_n();
                                ShowBottomMenu();
                                DisplayCursorPos(&Tab_stack[file_index]);
                                continue;
                            }
                        }
                    }
#ifdef _WIN32
                    if (MoveFile(Tab_stack[file_index].filename, newname))
#else
                    if (rename(Tab_stack[file_index].filename, newname))
#endif
                    {
                        PrintBottomString("%s", NEWTRODIT_FILE_RENAMED, newname);
                        WriteLogFile("%s", NEWTRODIT_FILE_RENAMED, newname);
                        strncpy_n(Tab_stack[file_index].filename, newname, MAX_PATH);

                        UpdateTitle(&Tab_stack[file_index]);
                        NewtroditNameLoad();
                        CenterText(StrLastTok(Tab_stack[file_index].filename, PATHTOKENS), 0);
                        DisplayTabIndex(&Tab_stack[file_index]);

                        RightAlignNewline();
                    }
                    else
                    {
                        PrintBottomString("%s", NEWTRODIT_FS_FILE_RENAME);
                        WriteLogFile("%s", NEWTRODIT_FS_FILE_RENAME);
                    }

                    getch_n();
                }
                else
                {
                    PrintBottomString("%s", NEWTRODIT_FUNCTION_ABORTED);
                    getch_n();
                }
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                ch = 0;
                break;
            case F5: // F5 key = Run macro
                if (!run_macro)
                {
                    if (!Tab_stack[file_index].is_untitled && Tab_stack[file_index].is_saved)
                    {
#ifdef _WIN32
                        GetFullPathName(Tab_stack[file_index].filename, sizeof(Tab_stack[file_index].filename), tmp, NULL);
#else
                        tmp = strdup(FullPath(Tab_stack[file_index].filename));
                        StartProcess(tmp);
#endif
                        StartProcess(tmp);
                    }
                }
                else
                {
                    tmp = strdup(run_macro);
                    tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_CURRENT_FILE, StrLastTok(Tab_stack[file_index].filename, PATHTOKENS), &n);
                    if (Tab_stack[file_index].is_untitled)
                    {
                        tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_FULL_PATH, Tab_stack[file_index].filename, &n);
                        tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_CURRENT_DIR, SInf.dir, &n);
                    }
                    else
                    {
                        tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_FULL_PATH, FullPath(Tab_stack[file_index].filename), &n);
                        get_path_directory(FullPath(Tab_stack[file_index].filename), ptr);
                        tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_CURRENT_DIR, ptr, &n);
                        tmp = ReplaceString(strdup(tmp), NEWTRODIT_MACRO_CURRENT_EXTENSION, StrLastTok(Tab_stack[file_index].filename, "."), &n);
                    }
                    WriteLogFile("Running macro: %s", tmp);
                    StartProcess(tmp);
                }

                ch = 0;
                break;
            case F6: // F6 key = Insert date and time

                temp_strsave = GetTime(showMillisecondsInTime);

                if (_xpos >= BUFFER_X - BUFFER_INCREMENT)
                {
                    tmp = realloc_n(Tab_stack[file_index].strsave[_ypos], BUFFER_X, BUFFER_X + BUFFER_INCREMENT);
                    if (!tmp)
                    {
                        PrintBottomString("%s", NEWTRODIT_ERROR_OUT_OF_MEMORY);
                        WriteLogFile("%s", NEWTRODIT_ERROR_OUT_OF_MEMORY);
                        getch_n();

                        break;
                    }
                    BUFFER_X += BUFFER_INCREMENT;

                    free(Tab_stack[file_index].strsave[_ypos]);
                    Tab_stack[file_index].strsave[_ypos] = tmp;
                }

                InsertStr(Tab_stack[file_index].strsave[_ypos], temp_strsave, _xpos);

                _xpos += strlen(temp_strsave);

                gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);

                if (_xpos <= XSIZE)
                {
                    print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                }

                break;

            case F9: // F9 = Compile
                if (!Tab_stack[file_index].is_untitled)
                {
                    snprintf(tmp, DEFAULT_BUFFER_X, "%s %s %s -o %s.exe", Tab_stack[file_index].Compilerinfo.path, Tab_stack[file_index].Compilerinfo.flags, FullPath(Tab_stack[file_index].filename), FullPath(Tab_stack[file_index].filename));
                    StartProcess(tmp);
                    WriteLogFile("Started compiler (command line '%s') ", tmp);
                }

                break;
            case F10:                           // F10 key
                StartProcess("explorer.exe ."); // Open current directory in explorer
                break;
            case SHIFTF5: // S-F5 (Set macro)
                PrintBottomString("%s", NEWTRODIT_PROMPT_CREATE_MACRO);
                fgets(macro_input, sizeof(macro_input), stdin);
                if (NoLfLen(macro_input) <= 0)
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    break;
                }
                macro_input[strcspn(macro_input, "\n")] = 0;

                LoadAllNewtrodit();
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                if (!ValidString(macro_input))
                {
                    PrintBottomString("%s", NEWTRODIT_ERROR_INVALID_MACRO);
                    getch_n();
                    ShowBottomMenu();
                    DisplayCursorPos(&Tab_stack[file_index]);
                    break;
                }

                strncpy_n(run_macro, macro_input, MACRO_ALLOC_SIZE);
                PrintBottomString("%s%s", NEWTRODIT_MACRO_SET, macro_input);
                WriteLogFile("%s%s", NEWTRODIT_MACRO_SET, macro_input);
                getch_n();
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                break;
            case SHIFTF10:               // S-F10 key
                StartProcess("cmd.exe"); // Open command prompt
                break;
            case CTRLF1: // ^F1 key
                SetCursorSettings(false, GetConsoleInfo(CURSOR_SIZE));
                ClearPartial(0, 1, XSIZE, YSIZE - 1);
                n = strlen(join("Contribute at ", newtrodit_repository));
                ptr = ProgInfo();
                if (n < strlen(ptr))
                {
                    n += (strlen(ptr) - n);
                }
                SetColor(fg_color);
                ClearPartial((XSIZE / 2) - (n / 2) - 1, (YSIZE / 2) - 3, n + 2, 7); // Create a box
                CenterText("About Newtrodit", (YSIZE / 2) - 2);
                CenterText(ptr, (YSIZE / 2));
                // I know it's not the best way to do it, but it works
                CenterText(join("Contribute at ", newtrodit_repository), (YSIZE / 2) + 2);
                free(ptr);

                getch_n();
                ClearPartial(0, 1, XSIZE, YSIZE - 1);
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                SetCursorSettings(true, GetConsoleInfo(CURSOR_SIZE));

                break;
            case CTRLF4: // ^F4
                if (!CheckKey(VK_SHIFT))
                {

                    NewFile(&Tab_stack[file_index]);
                    PrintBottomString("%s", NEWTRODIT_FILE_CLOSED);
                    c = -2;
                    ch = 0;
                    break;
                }

            case ALTF4: // A-F4 key
                if (!CheckKey(VK_CONTROL))
                {
                    QuitProgram(SInf.color);
                    ShowBottomMenu();
                    continue;
                }
                break;

            /* This one (148) doesn't work on Linux */
            case 148: // ^TAB / S-^TAB (Switch file)
                SwitchTab(&Tab_stack[file_index], CheckKey(VK_SHIFT));
                break;

            case ALTHOME: // A-HOME key "smart home" (go to the first non-whitespace character)
                _xpos = strspn(Tab_stack[file_index].strsave[_ypos], " \t");
                UpdateHorizontalScroll(&Tab_stack[file_index], true);

                break;
            case ALTEND: // A-END key "smart end" (go to the last non-whitespace character)
                n = NoLfLen(Tab_stack[file_index].strsave[_ypos]);

                while ((Tab_stack[file_index].strsave[_ypos][n - 1] == ' ' || Tab_stack[file_index].strsave[_ypos][n - 1] == '\t') && Tab_stack[file_index].strsave[_ypos][n - 1] != '\0')
                {
                    n--;
                }

                _xpos = n + (TokCount(Tab_stack[file_index].strsave[_ypos], "\t") * TAB_WIDE);

                if (_xpos < 0)
                {
                    _xpos = 0;
                }

                break;
            }

            ch = 0;
            continue;
        }

        if (ch == CTRLS) // ^S
        {
            (CheckKey(VK_SHIFT)) ? (n = 2) : (n = 0);
            n2 = false; // Refresh screen
            memset(save_dest, 0, MAX_PATH);
            strncpy_n(save_dest, Tab_stack[file_index].filename, MAX_PATH); // If no input is given
            if (!Tab_stack[file_index].is_saved || n == 2)
            {
                PrintBottomString("%s", n != 2 ? NEWTRODIT_PROMPT_SAVE_FILE : NEWTRODIT_PROMPT_SAVE_FILE_AS);
                fgets(save_dest, MAX_PATH, stdin);
                if (NoLfLen(save_dest) <= 0)
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    continue;
                }
                save_dest[strcspn(save_dest, "\n")] = 0;
                RemoveQuotes(save_dest, strdup(save_dest));

                if (!CheckFile(save_dest))
                {
                    LoadAllNewtrodit();
                    DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                    PrintBottomString("%s", NEWTRODIT_PROMPT_OVERWRITE);

                    if (!YesNoPrompt())
                    {
                        ShowBottomMenu();
                        DisplayCursorPos(&Tab_stack[file_index]);
                        continue;
                    }
                }
                n2 = true;
                LoadAllNewtrodit();
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
            }
            if (!Tab_stack[file_index].is_untitled)
            {
#ifdef _WIN32
                if (GetFileAttributes(Tab_stack[file_index].filename) & FILE_ATTRIBUTE_READONLY)
#else
                if (Tab_stack[file_index].is_readonly)
#endif
                {
                    if (!n2)
                    {
                        LoadAllNewtrodit();
                        DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                    }

                    PrintBottomString("%s", NEWTRODIT_FS_READONLY_SAVE);
                    getch_n();
                    ShowBottomMenu();
                    DisplayCursorPos(&Tab_stack[file_index]);
                    continue;
                }
            }

            fp_savefile = fopen(save_dest, "wb"); // (Re)open the file to write
            if (!fp_savefile || !ValidFileName(save_dest))
            {
                if (!n2)
                {
                    LoadAllNewtrodit();
                    DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                }
                PrintBottomString("%s", NEWTRODIT_FS_FILE_SAVE_ERR);
                getch_n();
                ShowBottomMenu();
                continue;
            }
            if (WriteBuffer(fp_savefile, &Tab_stack[file_index])) // Write the file content
            {
                if(Tab_stack[file_index].is_untitled)
                {
                    AutoLoadSyntaxRules(&Tab_stack[file_index], save_dest);
                }
                Tab_stack[file_index].is_saved = true;
                Tab_stack[file_index].is_modified = false;
                Tab_stack[file_index].is_untitled = false;

                UpdateTitle(&Tab_stack[file_index]);
                Tab_stack[file_index].filename = strdup(save_dest);
                NewtroditNameLoad();
                DisplayTabIndex(&Tab_stack[file_index]);
                CenterText(StrLastTok(Tab_stack[file_index].filename, PATHTOKENS), 0);

                RightAlignNewline();
                DisplayTabIndex(&Tab_stack[file_index]);

                PrintBottomString("%s", NEWTRODIT_FILE_SAVED);
                WriteLogFile("%s%s", NEWTRODIT_FILE_WROTE_BUFFER, Tab_stack[file_index].filename);

                c = -2;
            }
            else
            {
                PrintBottomString("%s", NEWTRODIT_FS_FILE_SAVE_ERR);
                WriteLogFile("%s", NEWTRODIT_FS_FILE_SAVE_ERR);

                getch_n();
            }

            fclose(fp_savefile);

            ch = 0;
            continue;
        }
        if (ch == CTRLV) // ^V
        {
#ifdef _WIN32

            if (OpenClipboard(0))
            {
                buffer_clipboard = (char *)GetClipboardData(CF_TEXT);
                if (buffer_clipboard != NULL)
                {
                    printf("[%s]", buffer_clipboard);
                    getch_n();
                    ptr = strtok(buffer_clipboard, "\n");
                    while (ptr != NULL)
                    {
                        n = strlen(ptr);
                        tmp = calloc(n + 10 + strlen(Tab_stack[file_index].newline), sizeof(char));
                        memcpy(tmp, ptr, n);

                        print_line(tmp, _ypos);
                        InsertRow(Tab_stack[file_index].strsave, _xpos, Tab_stack[file_index].bufy, tmp);
                        ptr = strtok(NULL, "\n");
                        if (ptr)
                        {
                            strncat(Tab_stack[file_index].strsave[_ypos], Tab_stack[file_index].newline, strlen(Tab_stack[file_index].newline));

                            Tab_stack[file_index].linecount++;
                            _ypos++;
                        }

                        free(tmp);
                    }

                    _xpos = NoLfLen(Tab_stack[file_index].strsave[_ypos]);
                }
            }
            else
            {
                PrintBottomString("%s", NEWTRODIT_ERROR_CLIPBOARD_COPY);
                WriteLogFile("%s", NEWTRODIT_ERROR_CLIPBOARD_COPY);
                getch_n();
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
            }
            CloseClipboard();
#else
            PrintBottomString("This feature is not supported in Linux.");
            c = -2;
#endif
            ch = 0;
            continue;
        }

        if (ch == CTRLD) // ^D (Debug tool/dev mode) / S-^D = Toggle dev mode
        {
            ch = 0;
            if (CheckKey(VK_SHIFT))
            {
                ToggleOption(&devMode, NEWTRODIT_DEV_TOOLS, false);
            }
            else
            {
                if (devMode)
                {
                    PrintBottomString("\"%.*s\"", strcspn(Tab_stack[file_index].strsave[_ypos], Tab_stack[file_index].newline), Tab_stack[file_index].strsave[_ypos]);
                }
            }

            c = -2;
            continue;
        }
        if (ch == CTRLW) // ^W
        {
            if (!CheckKey(VK_SHIFT))
            {
                if ((n = CloseFile(&Tab_stack[file_index])) == 1)
                {
                    PrintBottomString("%s", NEWTRODIT_FILE_CLOSED);
                    WriteLogFile("%s", NEWTRODIT_FILE_CLOSED);
                }
                else if (n < 0)
                {
                    PrintBottomString("%s", NEWTRODIT_ERROR_FAILED_CLOSE_FILE);
                    WriteLogFile("%s", NEWTRODIT_ERROR_FAILED_CLOSE_FILE);
                }

                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                c = -2;
                ch = 0;
                continue;
            }
            else
            {
                if (devMode)
                {
                    PrintBottomString("[Dev mode] Are you sure you want to insert junk data? (y/n)");
                    if (YesNoPrompt())
                    {
                        memcpy(Tab_stack[file_index].strsave[_ypos] + _xpos, "äº\\&9=)='A€€¨Y¨ª¨ÿ´^^*Ç", 37);
                        PrintBottomString("Warning: Junk data inserted for dev mode testing.");
                    }
                    c = -2;

                    ch = 0;
                    continue;
                }
            }
        }

        if (ch == CTRLQ) // ^Q = Quit program ; S-^Q = Count lines of code in files
        {
            if (!CheckKey(VK_SHIFT))
            {
                QuitProgram(SInf.color);
                ShowBottomMenu();
                SetColor(bg_color);
                ch = 0;
                continue;
            }
            else
            {
                n = 0;
                for (int i = 0; i < open_files; i++)
                {
                    n += Tab_stack[i].linecount;
                    n2 += Tab_stack[i].size;
                }
                PrintBottomString("%d lines of code (total %d bytes) in %d opened files.", n, n2, open_files);
                ch = 0;
                c = -2;
                continue;
            }
        }
        if (ch == CTRLX) // ^X = Cut
        {
            if (useOldKeybindings)
            {
                if (!CheckKey(VK_SHIFT))
                {
                    QuitProgram(SInf.color);
                    ShowBottomMenu();
                    SetColor(bg_color);
                    ch = 0;
                }
            }
            else
            {
                if (!CheckKey(VK_SHIFT))
                {
                    if (Tab_stack[file_index].strsave[_ypos][0] != '\0')
                    {

                        Tab_stack[file_index].Ustack->size = strlen(Tab_stack[file_index].strsave[_ypos]);
                        Tab_stack[file_index].Ustack->line = malloc(sizeof(char) * (Tab_stack[file_index].Ustack->size + 1));
                        memset(Tab_stack[file_index].Ustack->line, 0, Tab_stack[file_index].Ustack->size);

                        Tab_stack[file_index].Ustack->line = strdup(Tab_stack[file_index].strsave[_ypos]);
                        Tab_stack[file_index].Ustack->line_count = _ypos;
                        Tab_stack[file_index].Ustack->line_pos = _xpos;
                        Tab_stack[file_index].Ustack->create_nl = false;
                        Tab_stack[file_index].Ustack++->create_nl = false;
#ifdef _WIN32
                        SetClipboardNewtrodit(Tab_stack[file_index].strsave[_ypos]);
#endif
                        memset(Tab_stack[file_index].strsave[_ypos], 0, strlen(Tab_stack[file_index].strsave[_ypos]));
                        ClearPartial((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y, XSIZE - (lineCount ? Tab_stack[file_index].linecount_wide : 0), 1);
                        _xpos = 0;
                    }

                    ch = 0;
                    continue;
                }
            }

            continue;
        }
        if (ch == CTRLBS) // ^Backspace
        {
            if (_xpos > 0)
            {
                /*  Tab_stack[file_index].Ustack->size = strlen(Tab_stack[file_index].strsave[_ypos]) + 1;
                 Tab_stack[file_index].Ustack->line = malloc(Tab_stack[file_index].Ustack->size);
                 memset(Tab_stack[file_index].Ustack->line, 0, Tab_stack[file_index].Ustack->size);
                 Tab_stack[file_index].Ustack->line = strdup(Tab_stack[file_index].strsave[_ypos]); // If it's not a duplicate, the value will change
                 Tab_stack[file_index].Ustack->line_count = _ypos;
                 Tab_stack[file_index].Ustack->line_pos = _xpos;
                 Tab_stack[file_index].Ustack->create_nl = false;
                 Tab_stack[file_index].Ustack->delete_nl = false; */

                bs_tk = TokBackPos(Tab_stack[file_index].strsave[_ypos], "()[]{}\t ", "?!");
                if (Tab_stack[file_index].strsave[_ypos][_xpos] == '\0')
                {

                    if (Tab_stack[file_index].strsave[_ypos][_xpos] == 9)
                    {
                        relative_xpos[_ypos] -= TAB_WIDE;
                    }
                    Tab_stack[file_index].strsave[_ypos][bs_tk] = 0;
                    n = strlen(Tab_stack[file_index].strsave[_ypos]);
                    for (int i = 0; i < bs_tk; i++)
                    {
                        Tab_stack[file_index].strsave[_ypos][n + i] = 0;
                    }
                    for (int i = _xpos; i >= bs_tk; i--)
                    {
                        if (Tab_stack[file_index].strsave[_ypos][i] == 9)
                        {
                            relative_xpos[_ypos] -= TAB_WIDE;
                        }
                        Tab_stack[file_index].strsave[_ypos][i] = '\0';
                    }
                    n = XSIZE;
                    for (int i = 0; i < _xpos - bs_tk; ++i)
                    {

                        if ((_xpos - i) < n)
                        {
                            if (Tab_stack[file_index].strsave[_ypos][_xpos] == 9)
                            {
                                ptr = PrintTab(TAB_WIDE);
                                fputs(ptr, stdout);
                                free(ptr);
                            }
                            else
                            {
                                fputs("\b \b", stdout);
                            }
                        }
                    }
                    _xpos = bs_tk;
                }
                else
                {

                    Tab_stack[file_index].strsave[_ypos] = DeleteStr(Tab_stack[file_index].strsave[_ypos], _xpos, _xpos - bs_tk);
                    RefreshLine(&Tab_stack[file_index], _ypos, Tab_stack[file_index].display_y, true);
                }
            }
            else
            {
                if (_ypos > 1)
                {
                    InsertDeletedRow(&Tab_stack[file_index]);
                    if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                    {
                        ClearPartial(0, 1, XSIZE, YSIZE - 2);
                        DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                    }
                    Tab_stack[file_index].linecount--;
                }
            }

            if (_xpos < 0) // Don't allow xpos to be negative
            {
                _xpos = 0;
            }

            ch = 0;
        }

        if (ch == CTRLY) // ^Y = Redo
        {

            /* strncpy_n(undo_stack, Tab_stack[file_index].strsave[undo_stack_line], BUFFER_X);
            strncpy_n(Tab_stack[file_index].strsave[undo_stack_line], redo_stack, BUFFER_X);
            LoadAllNewtrodit();
            FunctionAborted(&Tab_stack[file_index]);
            fflush(stdout); */

            ch = 0;
            continue;
        }

        if (ch == CTRLZ) // ^Z = Undo
        {
            if (undo_stack_tree > 0)
            {
                undo_stack_tree--;
                _xpos = Tab_stack[file_index].Ustack->line_pos;
                _ypos = Tab_stack[file_index].Ustack->line_count;
                if (Tab_stack[file_index].Ustack->create_nl == true)
                {
                    InsertDeletedRow(&Tab_stack[file_index]);
                }
                if (Tab_stack[file_index].Ustack->delete_nl == true)
                {
                    InsertRow(Tab_stack[file_index].strsave, _xpos, _ypos, Tab_stack[file_index].Ustack->line);
                    Tab_stack[file_index].strsave[Tab_stack[file_index].Ustack->line_count] = strdup(Tab_stack[file_index].Ustack->line);
                }
                if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                {
                    ClearPartial(0, Tab_stack[file_index].display_y, XSIZE, YSIZE - Tab_stack[file_index].display_y - 1);
                    DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                }
                else
                {
                    Tab_stack[file_index].strsave[_ypos] = Tab_stack[file_index].Ustack->line;
                }
                /* if (strlen(Tab_stack[file_index].Ustack->line) < strlen(Tab_stack[file_index].strsave[ypos]))
                {
                    xpos = NoLfLen(Tab_stack[file_index].Ustack->line);
                } */

                gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), _ypos);
                print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
            }

            ch = 0;
            continue;
        }

        if (ch == CTRLH && !CheckKey(BS) && CheckKey(VK_CONTROL)) // ^H = Replace string / S-^H = Same as F1 (opens help)
        {
            ch = 0;

            if (CheckKey(VK_SHIFT))
            {
                NewtroditHelp();

                LoadAllNewtrodit();

                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                DisplayCursorPos(&Tab_stack[file_index]);
            }
            else
            {
                ClearPartial(0, YSIZE - 2, XSIZE, 2);

                printf("%.*s\n", wrapSize, NEWTRODIT_PROMPT_FIND_STRING);
                printf("%.*s", wrapSize, NEWTRODIT_PROMPT_REPLACE_STRING);

                n = _xpos;
                gotoxy(strlen(NEWTRODIT_PROMPT_FIND_STRING), YSIZE - 2);

                fgets(find_string, sizeof find_string, stdin);
                gotoxy(strlen(NEWTRODIT_PROMPT_REPLACE_STRING), BOTTOM);
                if (NoLfLen(find_string) <= 0)
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    continue;
                }
                fgets(replace_string, sizeof find_string, stdin);
                find_string[strcspn(find_string, "\n")] = 0;
                replace_string[strcspn(replace_string, "\n")] = 0;
                if (!ValidString(find_string) || !ValidString(replace_string))
                {
                    FunctionAborted(&Tab_stack[file_index]);
                    continue;
                }

                for (int i = 1; i < Tab_stack[file_index].bufy; i++) // Line 0 is unused
                {

                    replace_str_ptr = ReplaceString(Tab_stack[file_index].strsave[i], find_string, replace_string, &replace_count);
                    if (replace_str_ptr)
                    {
                        if (strlen(replace_str_ptr) < strlen(Tab_stack[file_index].strsave[i]))
                        {
                            _xpos = strlen(replace_str_ptr);
                        }

                        strncpy_n(Tab_stack[file_index].strsave[i], replace_str_ptr, BUFFER_X);
                    }
                }

                if (strlen(Tab_stack[file_index].strsave[_ypos]) < n)
                {
                    _xpos = strlen(Tab_stack[file_index].strsave[_ypos]);
                }
                LoadAllNewtrodit();
                DisplayFileContent(&Tab_stack[file_index], stdout, 0);
                PrintBottomString("Replaced %d occurrences of '%s'", replace_count, find_string);
                c = -2;
                ShowBottomMenu();
                DisplayCursorPos(&Tab_stack[file_index]);
                if (replace_count > 0)
                {
                    Tab_stack[file_index].is_modified = true;
                }
            }
            continue;
        }

#ifdef _WIN32
        if ((ch == BS && _NEWTRODIT_OLD_SUPPORT == 1) || (ch == BS && CheckKey(BS) && !CheckKey(VK_CONTROL))) // Backspace key (Not Control-H)
#else
        if (ch == BS)
#endif
        {
            c = -8;
            if (_xpos > 0)
            {
                Tab_stack[file_index].is_modified = true;

                if (Tab_stack[file_index].strsave[_ypos][_xpos - 1] == 9) // TAB key
                {
                    relative_xpos[_ypos] -= TAB_WIDE;
                    gotoxy(_xpos + relative_xpos[_ypos] + (lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y + relative_ypos[_xpos]);
                }

                if (Tab_stack[file_index].strsave[_ypos][_xpos] != '\0')
                {
                    strncpy_n(temp_strsave, Tab_stack[file_index].strsave[_ypos] + strlen(Tab_stack[file_index].strsave[_ypos]), strlen(Tab_stack[file_index].strsave[_ypos]) - strlen(Tab_stack[file_index].newline));
                    hasNewLine = false;
                    // Remove ending newline character from Tab_stack[file_index].strsave[ypos]

                    if (!strcmp(temp_strsave, Tab_stack[file_index].newline))
                    {
                        Tab_stack[file_index].strsave[_ypos][strlen(Tab_stack[file_index].strsave[_ypos]) - strlen(Tab_stack[file_index].newline)] = '\0';
                        hasNewLine = true;
                    }

                    DeleteChar(Tab_stack[file_index].strsave[_ypos], --_xpos);
                    _xpos--;
                    if (hasNewLine)
                    {
                        strncat(tmp, Tab_stack[file_index].newline, strlen(Tab_stack[file_index].newline)); // strcat for CRLF newline
                    }

                    // strncpy_n(Tab_stack[file_index].strsave[_ypos], tmp, BUFFER_X);
                    ClearPartial((lineCount ? Tab_stack[file_index].linecount_wide : 0) + (NoLfLen(Tab_stack[file_index].strsave[_ypos])), Tab_stack[file_index].display_y, 2, 1); // Clear the character
                    gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                    print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                }
                else
                {
                    Tab_stack[file_index].strsave[_ypos][_xpos - 1] = '\0';
                    fputs("\b \b", stdout);

                    if (syntaxHighlighting)
                    {
                        gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                        print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                    }
                    _xpos -= 2;
                }
            }
            else
            {
                /* Act as END key */
                if (_ypos > 1)
                {
                    Tab_stack[file_index].is_modified = true;

                    n = NoLfLen(Tab_stack[file_index].strsave[_ypos - 1]); // Store the length of the line so we can change the X position later

                    if (Tab_stack[file_index].strsave[_ypos][0] == '\0' && !LineContainsNewLine(&Tab_stack[file_index], _ypos))
                    {
                        memset(Tab_stack[file_index].strsave[_ypos - 1] + NoLfLen(Tab_stack[file_index].strsave[_ypos - 1]), 0, Tab_stack[file_index].bufx - NoLfLen(Tab_stack[file_index].strsave[_ypos - 1]));
                    }
                    else
                    {
                        memcpy(Tab_stack[file_index].strsave[_ypos - 1] + NoLfLen(Tab_stack[file_index].strsave[_ypos - 1]), Tab_stack[file_index].strsave[_ypos], NoLfLen(Tab_stack[file_index].strsave[_ypos]));
                        DeleteRow(Tab_stack[file_index].strsave, _ypos, Tab_stack[file_index].bufy);
                    }

                    Tab_stack[file_index].linecount--;
                    _ypos--;
                    _xpos = n; // Assign the X position to the old length of the previous file
                    Tab_stack[file_index].last_dispy = Tab_stack[file_index].display_y;
                    Tab_stack[file_index].last_y = _ypos;

                    SetDisplayY(&Tab_stack[file_index]);
                    if (!UpdateScrolledScreen(&Tab_stack[file_index]))
                    {
                        ClearPartial(
                            0,
                            Tab_stack[file_index].display_y,
                            XSIZE,
                            YSIZE - Tab_stack[file_index].display_y - 1);

                        ClearPartial(0, Tab_stack[file_index].display_y, (lineCount ? Tab_stack[file_index].linecount_wide : 0), 1);
                        DisplayFileContent(&Tab_stack[file_index], stdout, Tab_stack[file_index].display_y - 1);
                    }
                }

                _xpos--; // Because 1 will be added to xpos in the next iteration
            }
        }
        else
        {
            if (ch > 31 || (ch == TAB && CheckKey(TAB))) // Printable character
            {

                Tab_stack[file_index].is_modified = true;

                if (!insertChar && ch != TAB) // Insert key not pressed
                {

                    if (Tab_stack[file_index].strsave[_ypos][_xpos] != '\0')
                    {

                        Tab_stack[file_index].strsave[_ypos] = InsertChar(strdup(Tab_stack[file_index].strsave[_ypos]), ch, _xpos);
                        RefreshLine(&Tab_stack[file_index], _ypos, Tab_stack[file_index].display_y, false); // No need to clear the line as it will get overwritten
                        ch = 0;
                    }
                }

                if (CheckKey(VK_TAB) && ch == TAB) // TAB key
                {

                    if (!AutoComplete(&Tab_stack[file_index], _xpos, _ypos))
                    {
                        if (convertTabtoSpaces)
                        {
                            Tab_stack[file_index].strsave[_ypos] = InsertStr(Tab_stack[file_index].strsave[_ypos], PrintTab(TAB_WIDE), _xpos);
                            _xpos += (TAB_WIDE - 1);
                            gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0), Tab_stack[file_index].display_y);
                            print_line(Tab_stack[file_index].strsave[_ypos], _ypos);
                        }
                    }
                }
                else

                {
                    if (ch != 0)
                    {
                        Tab_stack[file_index].strsave[_ypos][_xpos] = (int)ch; // Add character to buffer
                        if (_xpos < XSIZE - (lineCount ? Tab_stack[file_index].linecount_wide : 0))
                        {
                            putchar(ch);
                            if (syntaxHighlighting)
                            {
                                color_line(Tab_stack[file_index].strsave[_ypos], 0, Tab_stack[file_index].Syntaxinfo.override_color, _ypos);
                            }
                        }
                    }
                }
            }
            else
            {
                if (ch != 0 && ch <= 26 && ch != ENTER)
                {
                    memset(inbound_ctrl_key, 0, sizeof(inbound_ctrl_key)); // Clear the string for the next key
                    if (CheckKey(VK_MENU))
                    {
                        strcat(inbound_ctrl_key, "A-");
                    }
                    if (CheckKey(VK_SHIFT))
                    {
                        strcat(inbound_ctrl_key, "S-");
                    }
                    if (CheckKey(VK_ESCAPE))
                    {
                        strcat(inbound_ctrl_key, "^^[");
                    }
                    if (CheckKey(VK_CONTROL))
                    {
                        strcat(inbound_ctrl_key, "^^");
                    }
                    inbound_ctrl_key[strlen(inbound_ctrl_key) - 1] = ch + 64; // Convert getch return value to ASCII
                    PrintBottomString("%s%s", NEWTRODIT_ERROR_INVALID_INBOUND, inbound_ctrl_key);
                    c = -2; // For later use
                }
                _xpos--;
            }
        }

        _xpos++;
        if ((Tab_stack[file_index].strsave[1][0] == '\0' && Tab_stack[file_index].strsave[1][1] == '\0') || Tab_stack[file_index].strsave[1][0] == EOF) // If the document is empty set modified to false
        {
            Tab_stack[file_index].is_modified = false;
            UpdateTitle(&Tab_stack[file_index]);
        }

        if (strlen(Tab_stack[file_index].strsave[_ypos]) >= BUFFER_X - (TAB_WIDE * 2) || _ypos > Tab_stack[file_index].bufy || _xpos >= BUFFER_X - (TAB_WIDE * 2)) // Avoid buffer overflows by resizing the buffer
        {
            tmp = realloc_n(Tab_stack[file_index].strsave[_ypos], BUFFER_X, BUFFER_X + BUFFER_INCREMENT);
            BUFFER_X += BUFFER_INCREMENT;

            if (!tmp)
            {
                PrintBottomString(NEWTRODIT_ERROR_OUT_OF_MEMORY);
                getch_n();
                SaveFile(&Tab_stack[file_index], NULL);
                ExitRoutine(ENOMEM);
            }
            else
            {
                free(Tab_stack[file_index].strsave[_ypos]);
                Tab_stack[file_index].strsave[_ypos] = tmp;
            }
        }
    }
    NewtroditCrash("Unexpected program end.", EFAULT);
    SetColor(SInf.color);
    return 0;
}