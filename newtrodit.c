/*
	Newtrodit: A console text editor
	Copyright (c) 2021 anic17 Software

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
	- 'str_save' is the buffer where the file is stored on RAM.

	The source code of Newtrodit is composed by:

	 newtrodit.c      : Main source file
	 newtrodit_core.h : All core functions
	 newtrodit_func.c : Main functions
	 newtrodit_gui.c  : GUI loading
	 manual.c         : Manual
	 dialog.h         : Dialogs
	 globals.h        : Global variables
	 newtrodit.man    : Manual file

	 See 'newtrodit --help'

*/

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include "manual.c"

typedef struct Undo_stack
{
	char *line;
	int line_count;
} Undo_stack;

typedef struct Startup_info
{
	char *dir;
} Startup_info;

void sigint_handler()
{
	signal(SIGINT, sigint_handler);
	fflush(stdout);
}

void sigsegv_handler(int signum)
{
	signal(SIGSEGV, sigsegv_handler);
	MakePause();
	NewtroditCrash(join("Segmentation fault. Signum code: ", itoa_n(signum)), errno);
	fflush(stdout);
	exit(errno);
}

void sigbreak_handler(int signum)
{
	signal(SIGBREAK, sigbreak_handler);
	fflush(stdout);
}

int LoadSettings(char *newtrodit_config_file, char *macro, int *sigsegv, int *linecount, int *devmode)
{
	/*
		Settings are stored in an INI-like format.
		The format is:
			key=value
			;comment

	*/

	FILE *settings = fopen(newtrodit_config_file, "rb");
	if (!settings)
	{
		return EXIT_FAILURE;
	}

	char setting_buf[1024];
	char *iniptr = (char *)malloc(sizeof(setting_buf) + 1), *token = (char *)malloc(sizeof(setting_buf) + 1);
	int cnt = 0;
	char equalchar[] = "=";
	char *setting_list[] = {
		"fontcolor",
		"codepage",
		"convertnull",
		"converttab",
		"cursize",
		"curinsert"
		"devmode",
		"menucolor",
		"linecount",
		"linecountwide",
		"macro",
		"manfile",
		"newline",
		"oldkeybinds",
		"sigsegv",
		"syntax",
		"tabwide",
		"trimlonglines",
		"xsize",
		"ysize",
	}; // List of settings that can be changed

	// Set the non changing settings
	SetColor(FG_DEFAULT);
	SetColor(BG_DEFAULT);
	default_color = DEFAULT_SYNTAX_COLOR;
	run_macro[0] = 0;

	while (fgets(setting_buf, sizeof(setting_buf), settings))
	{
		setting_buf[strcspn(setting_buf, "\n")] = 0; // Remove newline

		cnt = strspn(setting_buf, " \t");
		snprintf(setting_buf, sizeof(setting_buf), "%s", setting_buf + cnt);

		if (setting_buf[cnt] == ';' || setting_buf[cnt] == 0) // Comment or newline found
		{
			continue;
		}
		iniptr = strtok(setting_buf, "=");

		while (iniptr != NULL) // Loop through the settings
		{
			for (int i = 0; i < sizeof(setting_list) / sizeof(char *); i++)
			{
				if (!strncmp(iniptr, setting_list[i], strlen(setting_list[i])))
				{
					token = strtok(NULL, equalchar);
					if (!strcmp(setting_list[i], "fontcolor"))
					{
						bg_color = hexstrtodec(token) % 255;
						default_color = bg_color;
					}
					if (!strcmp(setting_list[i], "codepage"))
					{
						int cp = atoi(token);
						SetConsoleOutputCP(cp);
					}
					if (!strcmp(setting_list[i], "convertnull"))
					{
						if (atoi(token))
						{
							convertNull = true;
						}
						else
						{
							convertNull = false;
						}
					}
					if (!strcmp(setting_list[i], "converttab"))
					{
						if (atoi(token))
						{
							convertTabtoSpaces = true;
						}
						else
						{
							convertTabtoSpaces = false;
						}
						convertTabtoSpaces = true; // TODO: Remove this line
					}

					if (!strcmp(setting_list[i], "curinsert"))
					{
						if (atoi(token))
						{
							cursorSizeInsert = true;
						}
						else
						{
							cursorSizeInsert = false;
						}
					}

					if (!strcmp(setting_list[i], "cursize"))
					{
						CURSIZE = atoi(token);
					}

					if (!strcmp(setting_list[i], "devmode"))
					{

						if (atoi(token))
						{
							*devmode = true;
						}
						else
						{
							*devmode = false;
						}
					}
					if (!strcmp(setting_list[i], "macro"))
					{

						if (ValidString(token))
						{
							strncpy_n(run_macro, token, sizeof(setting_list));
							// printf("%s", run_macro);
						}
					}
					if (!strcmp(setting_list[i], "manfile"))
					{
						if (ValidFileName(token))
						{
							strncpy_n(manual_file, token, sizeof(manual_file));
							manual_file[strcspn(manual_file, "\n")] = 0;
						}
					}
					if (!strcmp(setting_list[i], "menucolor"))
					{
						fg_color = (hexstrtodec(token) * 16) % 255;
					}

					if (!strcmp(setting_list[i], "newline"))
					{
						if (ValidString(token))
						{
							strncpy_n(newlinestring, token, sizeof(newlinestring));
						}
					}

					if (!strcmp(setting_list[i], "linecount"))
					{
						if (atoi(token))
						{
							*linecount = true;
							LINECOUNT_WIDE = LINECOUNT_WIDE_;
						}
						else
						{
							*linecount = false;
							LINECOUNT_WIDE = 0;
						}
					}
					if (!strcmp(setting_list[i], "linecountwide"))
					{
						LINECOUNT_WIDE = atoi(token);
					}
					if (!strcmp(setting_list[i], "oldkeybinds"))
					{
						if (atoi(token))
						{
							useOldKeybinds = true;
						}
						else
						{
							useOldKeybinds = false;
						}
					}
					if (!strcmp(setting_list[i], "sigsegv"))
					{
						if (atoi(token))
						{
							*sigsegv = true;
						}
						else
						{
							*sigsegv = false;
						}
					}
					if (!strcmp(setting_list[i], "syntax"))
					{
						if (ValidFileName(token))
						{

							FILE *syntax = fopen(token, "rb");
							if (!syntax)
							{
								fprintf(stderr, "%s\b: %s\n", NEWTRODIT_FS_FILE_OPEN_ERR, token);
								MakePause();
							}
							else
							{
								strncpy_n(syntax_filename, token, sizeof(syntax_filename));
								syntax_filename[strcspn(syntax_filename, "\n")] = 0;
								EmptySyntaxScheme();
								syntaxKeywordsSize = LoadSyntaxScheme(syntax, syntax_filename);
								syntaxHighlighting = true;
							}
						}
					}

					if (!strcmp(setting_list[i], "tabwide"))
					{
						TAB_WIDE = atoi(token);
					}
					if (!strcmp(setting_list[i], "trimlonglines"))
					{
						if (atoi(token))
						{
							trimLongLines = true;
						}
						else
						{
							trimLongLines = false;
						}
					}

					if (!strcmp(setting_list[i], "xsize"))
					{
						int xs = atoi(token);
						SetConsoleSize(xs, YSIZE);
					}
					if (!strcmp(setting_list[i], "ysize"))
					{
						int ys = atoi(token);
						SetConsoleSize(XSIZE, ys);
					}
				}
			}

			iniptr = strtok(NULL, equalchar);
		}
	}
	fclose(settings);
	return 0;
}

int main(int argc, char *argv[])
{

	wrapSize = XSIZE - LINECOUNT_WIDE;
	goto_len = strlen(itoa_n(BUFFER_Y));

	int hasNewLine = false; // Bool for newline in insert char
	int lineCount = false;	// Bool for line count
	int dev_tools = true;	// Bool to enable or disable the dev tools
	int insertChar = false; // Bool to check if replace instead of insert
	int findInsensitive = false;
	int sigsegvScreen = true;
	int listDir = false;
	/* 	char *run_macro = (char *)malloc(sizeof(char) * _MAX_PATH + 1);
	 */
	str_save = (char **)malloc(BUFFER_Y * sizeof(char *));

	for (int i = 0; i < BUFFER_Y; i++)
	{
		str_save[i] = (char *)calloc(BUFFER_X, sizeof(char));
	}

	run_macro = (char *)malloc(sizeof(char) * _MAX_PATH + 1);
	memset(run_macro, 0, sizeof(char) * _MAX_PATH + 1);
	LoadSettings(settings_file, run_macro, &sigsegvScreen, &lineCount, &dev_tools); // Load settings from settings file

	signal(SIGINT, sigint_handler);		// Ctrl-C handler
	signal(SIGBREAK, sigbreak_handler); // Ctrl-Break handler
	if (sigsegvScreen)
	{
		signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
	}

	if (!ValidSize())
	{
		return 1;
	}

	// Allocate buffer
	if (BUFFER_X < MIN_BUFSIZE || BUFFER_Y < MIN_BUFSIZE)
	{
		fprintf(stderr, "Buffer is too small (Current size is %dx%d and required size is %dx%d", BUFFER_X, BUFFER_Y, MIN_BUFSIZE, MIN_BUFSIZE);
		return 1;
	}
	Line *buffer = (Line *)malloc(sizeof(Line) * BUFFER_Y);

	char *temp_strsave = (char *)malloc(BUFFER_X) + 1;
	char *tmp = (char *)malloc(BUFFER_X) + 1;

	Undo_stack Ustack[UNDO_STACK_SIZE];
	char *undo_stack = (char *)malloc(BUFFER_X) + 1; // For undo stack (^Z)
	char *redo_stack = (char *)malloc(BUFFER_X) + 1; // For redo stack (^Y)

	int undo_stack_line, undo_stack_tree;

	if (!lineCount)
	{
		LINECOUNT_WIDE = 0;
	}

	// Declare variables
	int old_x_size, old_y_size;
	int bs_tk;

	start_color = GetColor();

	char save_dest[_MAX_PATH];
	char *line_number_str;

	char find_string[512], replace_string[512];
	int find_string_index, find_string_index_old;
	char *final_paste_strrchr;
	char fileopenread[_MAX_PATH];
	char *insert_str, *replace_str_ptr;
	int replace_count = 0;

	char inbound_ctrl_key[100];
	char newname[_MAX_PATH], syntaxfile[_MAX_PATH], locate_file[_MAX_PATH], macro_input[_MAX_PATH];
	convertTabtoSpaces = true;
	int n;
	char *ptr;

	// File variables
	FILE *fileread, *fp_savefile, *newtrodit_open_argv, *macro_temp, *syntax;

	// Position variables
	int *relative_xpos = calloc(sizeof(int) * BUFFER_Y, BUFFER_X);
	int *relative_ypos = calloc(sizeof(int) * BUFFER_X, BUFFER_Y);

	int c = 0;
	// getch() variables
	int ch;

	SetColor(bg_color);
	int argc_shift = 1, shiftargc = argc; // Can't be 0, because the first argument is the program name
	for (argc_shift = 1; argc_shift < argc; argc_shift++)
	{
		if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) // Version parameter
		{
			printf("%.*s", wrapSize, ProgInfo());
			return 0;
		}
		if (!strcmp(argv[argc_shift], "--help") || !strcmp(argv[argc_shift], "-h")) // Manual parameter
		{
			NewtroditHelp();
			SetColor(start_color);
			ClearPartial(0, 0, XSIZE, YSIZE);
			return 0;
		}
		if (!strcmp(argv[argc_shift], "--sigsegv") || !strcmp(argv[argc_shift], "-s")) // Sigsegv parameter
		{
			signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
			shiftargc--;
		}

		if (!strcmp(argv[argc_shift], "--line") || !strcmp(argv[argc_shift], "-l")) // Display line count
		{
			lineCount = true;
			LINECOUNT_WIDE = LINECOUNT_WIDE_; // Set the line count width
			shiftargc--;
		}

		if (!strcmp(argv[argc_shift], "--lfunix") || !strcmp(argv[argc_shift], "-ln")) // Use UNIX new line
		{
			strncpy_n(newlinestring, "\n", 1); // Avoid any type of buffer overflows
			shiftargc--;
		}

		if (!strcmp(argv[argc_shift], "--lfwin") || !strcmp(argv[argc_shift], "-lw")) // Use Windows new line
		{
			strncpy_n(newlinestring, "\r\n", 2);
			shiftargc--;
		}

		if (!strcmp(argv[argc_shift], "--converttab") || !strcmp(argv[argc_shift], "-t")) // Use Windows new line
		{
			convertTabtoSpaces = true;
			shiftargc--;
		}
		if (!strcmp(argv[argc_shift], "--devmode") || !strcmp(argv[argc_shift], "-d")) // Enable dev mode
		{
			dev_tools = true;
			shiftargc--;
		}

		if (!strcmp(argv[argc_shift], "--menucolor") || !strcmp(argv[argc_shift], "-mc")) // Foreground color parameter
		{
			if (argv[argc_shift + 1] != NULL)
			{
				fg_color = hexstrtodec(argv[argc_shift + 1]);
				if (fg_color > 0x0F || fg_color < 0)
				{
					fprintf(stderr, "%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
					return 1;
				}
				fg_color *= 16;
				shiftargc--;
			}
			else
			{
				fprintf(stderr, "%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
				return 1;
			}
		}
		if (!strcmp(argv[argc_shift], "--fontcolor") || !strcmp(argv[argc_shift], "-fc")) // Foreground color parameter
		{
			if (argv[argc_shift + 1] != NULL)
			{
				bg_color = hexstrtodec(argv[argc_shift + 1]);
				if (bg_color > 0x0F || bg_color < 0)
				{
					fprintf(stderr, "%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
					return 1;
				}
				bg_color *= 16;
				argc_shift++;
				shiftargc -= 2;
			}
			else
			{
				fprintf(stderr, "%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
				return 1;
			}
		}
	}

	if ((shiftargc + (argc - shiftargc)) > 1)
	{
		if (strlen(argv[argc_shift - 1]) > _MAX_PATH)
		{
			fprintf(stderr, "%s%s", NEWTRODIT_FS_FILE_NAME_TOO_LONG, argv[argc_shift - 1]);
			exit(ENAMETOOLONG);
		}
		strncpy_n(filename_text, argv[argc_shift - 1], _MAX_PATH); // NOT WORKING
		newtrodit_open_argv = fopen(filename_text, "rb");

		if (!CheckFile(filename_text)) // File exists
		{
			if (!newtrodit_open_argv)
			{

				fprintf(stderr, "%s%s\n", NEWTRODIT_FS_FILE_OPEN_ERR, filename_text);
				exit(errno);
			}
		}
		else
		{
			fprintf(stderr, "%s%s\n", NEWTRODIT_FS_FILE_NOT_FOUND, filename_text);
			exit(errno);
		}

		fseek(newtrodit_open_argv, 0, SEEK_SET); // Go to the end of the file

		if (CountLines(newtrodit_open_argv) > BUFFER_Y) // Check if file is too big
		{
			fprintf(stderr, "%s%s", NEWTRODIT_FS_FILE_TOO_LARGE, filename_text);
			return EFBIG; // File too big
		}
		fseek(newtrodit_open_argv, 0, SEEK_SET); // Return to the beginning of the file

		LoadAllNewtrodit();
		LoadFile(str_save, filename_text, relative_xpos, newlinestring, newtrodit_open_argv);
		CenterText(strlasttok(filename_text, '\\'), 0);
		isSaved = true;
		isUntitled = false;
		isModified = false;
	}
	else
	{
		LoadAllNewtrodit();
	}

	while (1)
	{

		UpdateTitle(isSaved);

		old_y_size = YSIZE;
		old_x_size = XSIZE;

		if (ypos >= (YSIZE - 2))
		{
			display_y = old_y_size - 2;
		}
		else
		{
			display_y = ypos;
		}

		if (lineCount && !isprint(ch))
		{
			DisplayLineCount(str_save, YSIZE - 3, display_y);
		}
		if (c != -2) // Clear bottom line
		{
			DisplayCursorPos(xpos, ypos);
		}

		wrapSize = XSIZE - 2 - LINECOUNT_WIDE;

		if (xpos <= wrapSize)
		{
			if (ypos > YSIZE - 1)
			{
				gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, display_y); // Relative position is for tab key
			}
			else
			{
				gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, display_y + relative_ypos[xpos]);
			}
		}
		else
		{
			gotoxy(wrapSize + LINECOUNT_WIDE, display_y + relative_ypos[xpos]);
		}

		ch = getch(); // Get key pressed
		if (c == -2)  // Inbound invalid control key
		{
			ShowBottomMenu();

			DisplayCursorPos(xpos, ypos);

			gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
			c = 0;
		}
		if (old_x_size != XSIZE || old_y_size != YSIZE) // Check if size has been modified
		{
			n = 0;
			while (!ValidSize())
			{
				n++;
				if (n == 2)
				{
					ClearPartial(0, 0, XSIZE, YSIZE);
					fprintf(stderr, "%s\n", NEWTRODIT_ERROR_WINDOW_TOO_SMALL);
					return errno;
				}
				ValidSize();
			}

			LoadAllNewtrodit();
			DisplayFileContent(str_save, stdout);
		}

		if (ch == 3 || ch == 11) // ^C = Copy line to clipboard; ^K = Cut line
		{
			if (!CheckKey(VK_SHIFT))
			{
				if (str_save[ypos][0] != '\0')
				{
					HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(str_save[ypos]) + 1);
					memcpy(GlobalLock(hMem), str_save[ypos], strlen(str_save[ypos]) + 1); // Copy line to the clipboard
					GlobalUnlock(hMem);
					OpenClipboard(0);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hMem);
					CloseClipboard();
					if (ch == 11 && useOldKeybinds)
					{
						if (str_save[ypos][0] != '\0')
						{
							EmptyString(str_save[ypos]);
							// ClearPartial(0, display_y, XSIZE, 1);

							ClearPartial(LINECOUNT_WIDE, display_y, XSIZE, 1);
							xpos = 0;
						}
					}
				}
				ch = 0;
				continue;
			}
			else
			{
				if (ch == 11)
				{
					ToggleOption(&useOldKeybinds, NEWTRODIT_OLD_KEYBINDS, true);

					c = -2;
					ch = 0;
					continue;
				}
				/* if (ch == 3) // S-^C = Line information counting
				{
					n = 0, n2 = 0;
					for (int i = 1; i < BUFFER_Y; i++)
					{
						if (str_save[i][0] != '\0')
						{
							n += strlen(str_save[i]);
						}
						else
						{
							break;
						}
					}

					printf("Total: %d; Current: %d; Percent: %d%%", n, n2, (n2 - strlen(str_save[ypos]) * 100) / n);
					MakePause();
					ch = 0;
				} */
			}
		}

		if (ch == 12) // ^S-L = Locate files ; ^L = Toggle line count
		{

			if (!CheckKey(VK_SHIFT))
			{
				lineCount = !lineCount;
				if (!lineCount)
				{
					LINECOUNT_WIDE = 0;
				}
				else
				{

					LINECOUNT_WIDE = LINECOUNT_WIDE_; // Backup original wide
				}
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				if (lineCount)
				{
					PrintBottomString(join(NEWTRODIT_LINE_COUNT, NEWTRODIT_DIALOG_ENABLED));
				}
				else
				{
					PrintBottomString(join(NEWTRODIT_LINE_COUNT, NEWTRODIT_DIALOG_DISABLED));
				}

				c = -2;
			}
			else
			{
				// List all files in the directory
				PrintBottomString(NEWTRODIT_PROMPT_LOCATE_FILE);
				fgets(locate_file, sizeof locate_file, stdin);
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				LocateFiles(listDir, locate_file);
				DisplayCursorPos(xpos, ypos);
				MakePause();
				ClearPartial(0, 1, XSIZE, YSIZE - 2);
				DisplayFileContent(str_save, stdout);
			}
			continue;
		}

		if (ch == 7) // ^G = Go to line
		{
			if (CheckKey(VK_SHIFT))
			{
				c = -7;
			}
			ch = 0;
			PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);
			line_number_str = TypingFunction('0', '9', goto_len);
			if (atoi(line_number_str) < 1) // Line is less than 1
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				c = -2;

				continue;
			}

			/* if (str_save[atoi(line_number_str)][0] == '\0' && atoi(line_number_str) != 1) // Line is less than 1
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				c = -2;
				continue;
			} */
			n = ypos;
			ypos = atoi(line_number_str);

			if (BufferLimit()) // Avoid crashes
			{
				ypos = n;

				c = -2;

				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);

				continue;
			}

			if (str_save[ypos][0] == '\0')
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				ypos = n;

				MakePause();
				ShowBottomMenu();
				continue;
			}
			if (c == -7)
			{
				if (str_save[ypos][xpos] == '\0')
				{
					xpos = nolflen(str_save[ypos]);
					relative_xpos[ypos] = tokcount(str_save[ypos], "\t");
				}
			}
			else
			{
				xpos = 0;
			}
			if (ypos >= (YSIZE - 3))
			{
				UpdateScrolledScreen(lineCount);
			}
			else
			{
				UpdateHomeScrolledScreen(lineCount);
			}

			ShowBottomMenu();

			DisplayCursorPos(xpos, ypos);

			continue;
		}
		if (ch == 14) // ^N = New file
		{
			if (!CheckKey(VK_SHIFT))
			{

				NewFile(&isModified, &isUntitled, &isSaved, BUFFER_Y, str_save, &xpos, &ypos, filename_text, relative_xpos, relative_ypos);
				PrintBottomString(NEWTRODIT_NEW_FILE_CREATED);
				MakePause();
				if (CheckKey(VK_MENU))
				{
					SaveFile(str_save, filename_text, BUFFER_Y, &isModified, &isUntitled);
				}
				DisplayCursorPos(xpos, ypos);

				ShowBottomMenu();

				ch = 0;
				continue;
			}
			else
			{

				ToggleOption(&convertNull, NEWTRODIT_NULL_CONVERSION, false);

				c = -2;
				ch = 0;
				continue;
			}
		}
		if (ch == 15) // ^O = Open file
		{
			if (!CheckKey(VK_SHIFT)) // Shift not pressed
			{
				if (isModified)
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);

					if (YesNoPrompt())
					{
						if (SaveFile(str_save, strdup(filename_text), YSIZE, &isModified, &isUntitled) != 0)
						{
							LoadAllNewtrodit();
							DisplayFileContent(str_save, stdout);
							continue;
						}
					}
				}
				PrintBottomString(NEWTRODIT_PROMPT_FOPEN);

				fgets(fileopenread, sizeof fileopenread, stdin);
				if (nolflen(fileopenread) <= 0)
				{
					FunctionAborted(str_save);
					continue;
				}

				// Remove trailing LF
				fileopenread[strcspn(fileopenread, "\n")] = 0;
				fileread = fopen(fileopenread, "r");
				n = errno;
				strncpy_n(filename_text, fileopenread, sizeof(filename_text));

				LoadAllNewtrodit();

				if (!fileread || !LoadFile(str_save, strdup(fileopenread), relative_xpos, newlinestring, fileread)) // Failed to open the file
				{

					DisplayFileContent(str_save, stdout);
					if (n == 2)
					{
						PrintBottomString(join(NEWTRODIT_FS_FILE_NOT_FOUND, fileopenread));
					}
					else
					{
						PrintBottomString(NEWTRODIT_FS_FILE_OPEN_ERR);
					}

					MakePause();
					ShowBottomMenu();
					DisplayCursorPos(xpos, ypos);

					continue;
				}
				strncpy_n(filename_text, fileopenread, sizeof(filename_text));

				xpos = 0, ypos = 1;
				isSaved = true;
				isUntitled = false;
				isModified = false;
				multiLineComment = false;

				fclose(fileread);
				ch = 0;
				continue;
			}
		}

		/* if (ch == 20) // S-^T = Toggle tab conversion
		{
			if (CheckKey(VK_SHIFT))
			{
				convertTabtoSpaces = !convertTabtoSpaces;
				if (convertTabtoSpaces)
				{
					PrintBottomString(join(NEWTRODIT_TAB_CONVERSION, NEWTRODIT_DIALOG_ENABLED));
				}
				else
				{
					PrintBottomString(join(NEWTRODIT_TAB_CONVERSION, NEWTRODIT_DIALOG_DISABLED));
				}
				c = -2;

				ch = 0;
				continue;
			}
		} */

		if (ch == 18) // ^R = Reload file and S-^R = Reload settings
		{
			ch = 0;

			if (CheckKey(VK_SHIFT)) // S-^R = Reload settings
			{
				PrintBottomString(NEWTRODIT_PROMPT_RELOAD_SETTINGS);
				if (YesNoPrompt())
				{
					if (LoadSettings(settings_file, run_macro, &sigsegvScreen, &lineCount, &dev_tools) == 0) // Reload settings
					{
						LoadAllNewtrodit();
						DisplayFileContent(str_save, stdout);
						PrintBottomString(NEWTRODIT_SETTINGS_RELOADED);
					}
					else
					{
						PrintBottomString(NEWTRODIT_ERROR_RELOAD_SETTINGS);
					}
					MakePause();
				}
				DisplayCursorPos(xpos, ypos);
				ShowBottomMenu();
				continue;
			}
			else
			{
				// ^R = Reload file
				PrintBottomString(NEWTRODIT_PROMPT_RELOAD_FILE);

				if (YesNoPrompt())
				{
					newtrodit_open_argv = fopen(filename_text, "rb");

					ReloadFile(&isUntitled, str_save, &xpos, &ypos, relative_xpos, relative_ypos, newtrodit_open_argv);
				}
				else
				{
					ShowBottomMenu();
				}

				continue;
			}
		}

		if (ch == 0xE0) // Special keys: 224
		{

			ch = getch();
			switch (ch)
			{

			case 72:
				// Up arrow
				if (ypos > 1)
				{
					if (str_save[ypos - 1][xpos] == '\0')
					{
						xpos = nolflen(str_save[--ypos]);
					}
					else
					{
						if (str_save[ypos - 1][xpos] == 9)
						{
							xpos += relative_xpos[ypos - 1];
						}
						--ypos;
					}
				}
				UpdateScrolledScreen(lineCount);
				break;

			case 75:
				// Left arrow

				if (xpos >= 1)
				{
					if (str_save[ypos][xpos - 1] == 9)
					{
						relative_xpos[ypos] -= TAB_WIDE;
					}
					--xpos;
				}
				else
				{
					if (ypos > 1)
					{

						xpos = nolflen(str_save[--ypos]);
						UpdateScrolledScreen(lineCount);
					}
				}

				break;
			case 77:
				// Right arrow
				if (str_save[ypos][xpos] != '\0')
				{
					if (xpos == nolflen(str_save[ypos]))
					{
						if (str_save[ypos + 1][0] != '\0' || !strncmp(str_save[ypos] + xpos, newlinestring, strlen(newlinestring)))
						{

							xpos = 0;
							ypos++;

							UpdateScrolledScreen(lineCount);
						}
					}
					else
					{
						if (str_save[ypos][xpos] == 9)
						{

							relative_xpos[ypos] += TAB_WIDE;
						}
						xpos++;
					}
				}
				if (BufferLimit())
				{
					ShowBottomMenu();
					continue;
				}

				break;

			case 80:
				// Down arrow

				if (str_save[ypos + 1][0] == '\0' && strncmp(str_save[ypos] + (strlen(str_save[ypos]) - strlen(newlinestring)), newlinestring, strlen(newlinestring)))
				{
					break;
				}
				if (str_save[ypos + 1][xpos] == '\0')
				{

					xpos = nolflen(str_save[++ypos]) + (tokcount(str_save[ypos + 1], "\t") * TAB_WIDE); // Add tab wide
				}
				else
				{
					n = tokcount(str_save[ypos + 1] - xpos, "\t");

					relative_xpos[ypos + 1] = n * TAB_WIDE;
					ypos++;
				}
				if (BufferLimit())
				{
					ShowBottomMenu();
					continue;
				}
				UpdateScrolledScreen(lineCount);

				break;

			case 71:
				// HOME key
				xpos = 0;
				break;
			case 79:
				// END key

				xpos = nolflen(str_save[ypos]);

				break;
			case 117:
				// ^END key
				for (int i = 1; i < BUFFER_Y; i++)
				{
					if (str_save[i][0] == '\0')
					{
						ypos = i;

						if (i > 1)
						{
							ypos = i - 1;
						}

						xpos = nolflen(str_save[ypos]);
						break;
					}
				}
				UpdateScrolledScreen(lineCount);

				break;
			case 119:
				// ^HOME key

				UpdateHomeScrolledScreen(lineCount);
				xpos = 0;
				ypos = 1;

				break;

			case 82:
				// INS key
				insertChar = !insertChar;
				if (cursorSizeInsert)
				{
					if (insertChar)
					{

						CursorSettings(TRUE, 80);
					}
					else
					{
						CursorSettings(TRUE, CURSIZE);
					}
				}

				break;

			case 83:
				// DEL key
				if (CheckKey(VK_SHIFT)) // S-DEL
				{
					if (ypos > 0)
					{
						EmptyString(str_save[ypos]);

						ClearPartial(LINECOUNT_WIDE, display_y, XSIZE, 1);
						strncpy_n(str_save[ypos], newlinestring, sizeof(newlinestring));
						ypos = 0;
					}
				}
				else
				{
					if (strlen(str_save[ypos]) > 0)
					{
						undo_stack_line = ypos;
						insert_str = delete_char(str_save[ypos], xpos);
						if (strlen(insert_str) >= xpos)
						{
							strncpy_n(str_save[ypos], insert_str, BUFFER_X);
							ClearPartial(LINECOUNT_WIDE + nolflen(str_save[ypos]), display_y, 1, 1);
							gotoxy(LINECOUNT_WIDE, display_y);

							PrintLine(str_save[ypos]);
						}
					}
				}

				break;

				/* case 133: // F11 (fullscreen)

				if (!isFullScreen)
				{
					isFullScreen = true;
					ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); // Show the window in fullscreen
				}
				else
				{
					ShowWindow(GetConsoleWindow(), SW_RESTORE); // Show the window in fullscreen

					isFullScreen = false;
				}
				ch = 0;
				break;

			*/

			case 134: // F12
				if (dev_tools)
				{
					printf("%d:%d\t%d:%d", XSIZE, YSIZE, old_x_size, old_y_size);
				}

				break;
			}

			if (ch != 83)
			{
				ch = 0;
			}
			else
			{
				c = -1; // For undo stack
			}
			continue;
		}

		if ((ch == 13 && CheckKey(VK_RETURN)) || (ch == 13 && _NEWTRODIT_OLD_SUPPORT)) // Newline character: CR (13)
		{
			n = ypos;

			if (nolflen(str_save[ypos]) > xpos)
			{
				insert_row(str_save, ypos, BUFFER_Y, NULL);
				ypos++;
				str_save[ypos] = (char *)calloc(BUFFER_X, sizeof(char));
				strncpy_n(str_save[ypos], str_save[ypos - 1] + xpos, BUFFER_X);

				memset(str_save[ypos - 1] + xpos, 0, BUFFER_X - xpos);
				strncpy_n(str_save[ypos - 1], newlinestring, strlen(newlinestring));

				strncat(str_save[ypos], newlinestring, strlen(newlinestring)); // Add newline to current line

				/* } */

				DisplayFileContent(str_save, stdout);
				xpos = 0;
				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);
				continue;
			}

			{
				strncat(str_save[ypos], newlinestring, sizeof(newlinestring)); // Add newline to current line
				ypos++;
			}

			xpos = 0;

			if (BufferLimit())
			{
				ypos = n;
				ShowBottomMenu();
				continue;
			}
			UpdateScrolledScreen(lineCount);

			continue;
		}
		if (ch == 5) // ^E = Toggle syntax highlighting / S-^E = Set syntax highlighting rules file
		{
			if (CheckKey(VK_SHIFT))
			{
				PrintBottomString(NEWTRODIT_PROMPT_SYNTAX_FILE);

				EmptyString(syntaxfile);
				fgets(syntaxfile, sizeof syntaxfile, stdin);
				if (nolflen(syntaxfile) <= 0)
				{
					FunctionAborted(str_save);
					continue;
				}
				LoadAllNewtrodit();
				syntaxfile[strcspn(syntaxfile, "\n")] = 0;
				syntax = fopen(syntaxfile, "r");
				if (!syntax)
				{
					if (errno == 2)
					{
						PrintBottomString(join(NEWTRODIT_FS_FILE_NOT_FOUND, syntaxfile));
					}
					else
					{
						PrintBottomString(NEWTRODIT_FS_FILE_OPEN_ERR);
					}
				}
				else
				{
					EmptySyntaxScheme();
					if ((syntaxKeywordsSize = LoadSyntaxScheme(syntax, syntaxfile)) != 0) // Change keywords size
					{
						PrintBottomString(NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED);
					}
				}
				DisplayFileContent(str_save, stdout);

				MakePause();
				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);
				ch = 0;
				continue;
			}
			else
			{
				ToggleOption(&syntaxHighlighting, NEWTRODIT_SYNTAX_HIGHLIGHTING, true);
				c = -2;
			}

			ch = 0;
			continue;
		}
		if (ch == 6) // ^F = Find string
		{
			// Empty values
			EmptyString(find_string);
			find_string_index = 0;
			c = 0;

			findInsensitive = false;
			if (CheckKey(VK_SHIFT))
			{
				findInsensitive = true;
				PrintBottomString(NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE);
			}
			else
			{
				PrintBottomString(NEWTRODIT_PROMPT_FIND_STRING);
			}

			fgets(find_string, sizeof find_string, stdin);
			if (nolflen(find_string) <= 0)
			{
				FunctionAborted(str_save);
				continue;
			}
			find_string[strcspn(find_string, "\n")] = '\0';

			LoadAllNewtrodit();
			DisplayFileContent(str_save, stdout);
			for (int i = 1; i < BUFFER_Y; i++)
			{
				if (findInsensitive)
				{
					find_string_index = FindString(str_lwr(str_save[i]), str_lwr(strdup(find_string)));
				}
				else
				{
					find_string_index = FindString(str_save[i], find_string);
				}
				while (find_string_index >= 0) // TODO: Fix this horrible codeºº
				{

					gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);
					SetColor(0x0e);
					if (findInsensitive)
					{
						printf("%s", str_save[i] + find_string_index);
					}
					else
					{
						PrintLine(find_string);
					}
					gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);

					SetColor(bg_color);
					ClearPartial(0, YSIZE - 2, XSIZE, 1);

					ShowFindMenu();
					gotoxy(find_string_index + strlen(find_string) + relative_xpos[i] + LINECOUNT_WIDE, i);
					c = getch();
					if (c == 27) // ESC
					{
						// Quit the while loop
						c = -6;
						break;
					}
					if (c == 0)
					{
						c = getch();
						if (c == 61) // F3
						{

							if (findInsensitive)
							{
								find_string_index = FindString(str_lwr(str_save[i] + find_string_index), str_lwr(strdup(find_string)));
							}
							else
							{
								find_string_index = FindString(str_save[i] + find_string_index, find_string);
							}
							if (find_string_index == find_string_index_old)
							{
								c = -6;
								MakePause();
								break;
							}
							if (find_string_index >= 0)
							{
								strncpy_n(find_string, str_save[i] + find_string_index, strlen(find_string));
								strncpy_n(temp_strsave, str_save[i] + find_string_index + strlen(find_string), strlen(str_save[i]) - find_string_index);
								gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);
								SetColor(0x0e);
								PrintLine(find_string);
								gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);

								SetColor(bg_color);
								ClearPartial(0, YSIZE - 2, XSIZE, 1);

								ShowFindMenu();
								gotoxy(find_string_index + strlen(find_string) + relative_xpos[i] + LINECOUNT_WIDE, i);
							}
						}

						else
						{
							c = -6; // -6 means F but in negative
						}
						MakePause();
						break;
					}
					ShowBottomMenu();

					xpos = find_string_index + strlen(find_string);
					ypos = i;
				}
				if (c == -6)
				{
					ShowBottomMenu();
					DisplayCursorPos(xpos, ypos);
					break;
				}
			}

			if (find_string_index < 0)
			{
				if (c != -6)
				{
					PrintBottomString(join(NEWTRODIT_FIND_STRING_NOT_FOUND, find_string));
					MakePause();
				}
				ShowBottomMenu();
			}
			ClearPartial(0, YSIZE - 2, XSIZE, 1);

			ch = 0;
			continue;
		}
		if (ch == 0)
		{
			ch = getch();
			switch (ch)
			{
			case 19: // ^A-R (ROT13)
				EmptyString(temp_strsave);
				strncpy_n(temp_strsave, str_save[ypos], strlen(str_save[ypos]));
				if (rot13(str_save[ypos]))
				{
					ClearPartial(0, display_y, XSIZE, 1);
					gotoxy(LINECOUNT_WIDE, display_y);
					PrintLine(str_save[ypos]);
				}
				break;
			case 22: // A-^U (Uppercase)
				EmptyString(temp_strsave);
				strncpy_n(temp_strsave, str_save[ypos], strlen(str_save[ypos]));
				for (int i = 0; i < strlen(temp_strsave); i++)
				{
					str_save[ypos][i] = toupper(str_save[ypos][i]);
				}
				ClearPartial(0, display_y, XSIZE, 1);
				gotoxy(LINECOUNT_WIDE, display_y);
				PrintLine(str_save[ypos]);
				break;
			case 38: // A-^L (Lowercase)
				EmptyString(temp_strsave);
				strncpy_n(temp_strsave, str_save[ypos], strlen(str_save[ypos]));
				for (int i = 0; i < strlen(temp_strsave); i++)
				{
					str_save[ypos][i] = tolower(str_save[ypos][i]);
				}
				ClearPartial(0, display_y, XSIZE, 1);
				gotoxy(LINECOUNT_WIDE, display_y);
				PrintLine(str_save[ypos]);
				break;
			case 49: // A-^N (New file and save)

				if (CheckKey(VK_MENU))
				{

					NewFile(&isModified, &isUntitled, &isSaved, BUFFER_Y, str_save, &xpos, &ypos, filename_text, relative_xpos, relative_ypos);
					PrintBottomString(NEWTRODIT_NEW_FILE_CREATED);
					MakePause();

					SaveFile(str_save, filename_text, BUFFER_Y, &isModified, &isUntitled);

					DisplayCursorPos(xpos, ypos);

					ShowBottomMenu();
				}
				break;

			case 59: // F1 key
				NewtroditHelp();

				LoadAllNewtrodit();

				DisplayFileContent(str_save, stdout);
				DisplayCursorPos(xpos, ypos);
				break;
			case 60: // F2 key
				if (!isSaved)
				{
					SaveFile(str_save, filename_text, YSIZE, &isModified, &isUntitled);
				}

				PrintBottomString(NEWTRODIT_PROMPT_RENAME_FILE);
				fgets(newname, sizeof(newname), stdin);
				newname[strcspn(newname, "\n")] = 0;
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				if (nolflen(newname) > 0)
				{
					if (!CheckFile(newname))
					{
						PrintBottomString(NEWTRODIT_PROMPT_OVERWRITE);
						if (YesNoPrompt())
						{
							if (remove(newname) != 0)
							{
								PrintBottomString(NEWTRODIT_FS_FILE_DELETE);
								MakePause();
								ShowBottomMenu();
								DisplayCursorPos(xpos, ypos);
								continue;
							}
						}
					}
					if (MoveFile(filename_text, newname))
					{
						PrintBottomString(join(NEWTRODIT_FILE_RENAMED, newname));
						strncpy_n(filename_text, newname, _MAX_PATH);
						NewtroditNameLoad();
						CenterText(strlasttok(filename_text, '\\'), 0);
						RightAlignNewline();
					}
					else
					{
						PrintBottomString(NEWTRODIT_FS_FILE_RENAME);
					}

					MakePause();
				}
				else
				{
					PrintBottomString(NEWTRODIT_FUNCTION_ABORTED);
					MakePause();
				}
				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);
				ch = 0;
				break;
			case 63: // F5 key = Run macro
				if (run_macro == NULL)
				{
					if (!isUntitled && isSaved)
					{

						GetFullPathName(filename_text, sizeof(filename_text), tmp, NULL);
						StartProcess(tmp);
					}
				}
				else
				{

					StartProcess(run_macro);
				}

				ch = 0;
				break;
			case 64: // F6 key = Insert date and time

				temp_strsave = GetTime();
				if (xpos >= BUFFER_X - BUFFER_INCREMENT)
				{
					tmp = realloc_n(str_save[ypos], BUFFER_X, BUFFER_X + BUFFER_INCREMENT);
					BUFFER_X += BUFFER_INCREMENT;

					if (tmp == NULL)
					{
						PrintBottomString(NEWTRODIT_ERROR_OUT_OF_MEMORY);
						MakePause();
						SaveFile(str_save, filename_text, BUFFER_Y, &isModified, &isUntitled);
						return ENOBUFS;
					}
					free(str_save[ypos]);
					str_save[ypos] = tmp;
				}
				for (int i = 0; i < strlen(temp_strsave); ++i)
				{
					insert_str = insert_char(str_save[ypos], temp_strsave[i], xpos + i);
					strncpy_n(str_save[ypos], insert_str, BUFFER_X);
				}

				xpos += strlen(temp_strsave);

				gotoxy(LINECOUNT_WIDE, display_y);
				if (xpos <= XSIZE)
				{
					PrintLine(str_save[ypos]);
				}
				break;
			case 88: // S-F5 (Set macro)
				PrintBottomString(NEWTRODIT_PROMPT_CREATE_MACRO);
				fgets(macro_input, sizeof(macro_input), stdin);
				macro_input[strcspn(macro_input, "\n")] = 0;
				if (nolflen(macro_input) <= 0)
				{
					FunctionAborted(str_save);
					break;
				}
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				if (!ValidString(macro_input))
				{
					PrintBottomString(NEWTRODIT_ERROR_INVALID_MACRO);
					MakePause();
					ShowBottomMenu();
					DisplayCursorPos(xpos, ypos);
					break;
				}

				strncpy_n(run_macro, macro_input, _MAX_PATH);
				PrintBottomString(join(NEWTRODIT_MACRO_SET, macro_input));
				MakePause();
				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);
				break;
			case 68: // F10 key
				StartProcess("explorer.exe .");
				break;
			case 93: // S-F10 key
				StartProcess("cmd.exe");
				break;
			case 94: // ^F1 key
				LoadAllNewtrodit();
				CenterText("About Newtrodit", (YSIZE / 2) - 2);
				CenterText(ProgInfo(), (YSIZE / 2));
				// I know it's not the best way to do it, but it works
				CenterText("Contribute at https://github.com/anic17/Newtrodit", (YSIZE / 2) + 2);

				MakePause();
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				break;
			case 97: // ^F4
				if (!CheckKey(VK_SHIFT))

				{

					NewFile(&isModified, &isUntitled, &isSaved, BUFFER_Y, str_save, &xpos, &ypos, filename_text, relative_xpos, relative_ypos);
					PrintBottomString(NEWTRODIT_FILE_CLOSED);
					c = -2;
					ch = 0;
					break;
				}

			case 107: // A-F4 key
				if (!CheckKey(VK_CONTROL))
				{
					QuitProgram(start_color);
					ShowBottomMenu();
					SetColor(bg_color);
					continue;
				}
				break;

			case 151: // A-HOME key
				xpos = strspn(str_save[ypos], " \t");
				break;
			case 159: // A-END key
				n = nolflen(str_save[ypos]);

				while ((str_save[ypos][n - 1] == ' ' || str_save[ypos][n - 1] == '\t') && str_save[ypos][n - 1] != '\0')
				{
					n--;
				}

				xpos = n + (tokcount(str_save[ypos], "\t") * TAB_WIDE);
				if (xpos < 0)
				{
					xpos = 0;
				}

				break;
			}

			ch = 0;
			continue;
		}

		if (ch == 19) // ^S
		{
			if (CheckKey(VK_SHIFT))
			{
				isSaved = 2;
			}
			strncpy_n(save_dest, filename_text, _MAX_PATH); // If no input is given
			if (isSaved == 0 || isSaved == 2)
			{
				if (!isSaved)
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE);
				}
				else
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE_AS);
				}
				fgets(save_dest, sizeof save_dest, stdin);

				if (nolflen(save_dest) <= 0)
				{
					FunctionAborted(str_save);
					continue;
				}
				save_dest[strcspn(save_dest, "\n")] = 0;

				if (!CheckFile(save_dest))
				{

					LoadAllNewtrodit();
					DisplayFileContent(str_save, stdout);
					PrintBottomString(NEWTRODIT_PROMPT_OVERWRITE);

					if (!YesNoPrompt())
					{
						ShowBottomMenu();
						DisplayCursorPos(xpos, ypos);
						continue;
					}
				}
			}

			fp_savefile = fopen(save_dest, "wb"); // Reopen the file to write
			if (!fp_savefile || !ValidFileName(save_dest))
			{
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				PrintBottomString(NEWTRODIT_FS_FILE_SAVE_ERR);
				MakePause();
				ShowBottomMenu();
				continue;
			}
			isSaved = true;
			isUntitled = false;
			isModified = false;

			WriteBuffer(fp_savefile); // Write the file content

			strncpy_n(filename_text, save_dest, _MAX_PATH);

			LoadAllNewtrodit();
			DisplayFileContent(str_save, stdout);
			PrintBottomString(NEWTRODIT_FILE_SAVED);
			MakePause();
			ShowBottomMenu();

			fclose(fp_savefile);
			ch = 0;
			continue;
		}
		if (ch == 22) // ^V
		{
			char *buffer_clipboard;
			if (OpenClipboard(0))
			{
				strncpy_n(undo_stack, str_save[ypos], BUFFER_X);
				undo_stack_line = ypos;
				buffer_clipboard = (char *)GetClipboardData(CF_TEXT);
				if (buffer_clipboard != NULL)
				{

					final_paste_strrchr = strlasttok(buffer_clipboard, '\n');
					for (int i = 0; i < strlen(final_paste_strrchr); ++i)
					{
						temp_strsave = insert_char(str_save[ypos], final_paste_strrchr[i], i + xpos);
						if (convertTabtoSpaces)
						{
							strncpy_n(str_save[ypos], ReplaceString(temp_strsave, "\t", PrintTab(TAB_WIDE), &n), BUFFER_X);
						}
						else
						{
							strncpy_n(str_save[ypos], temp_strsave, BUFFER_X);
						}
					}
					PrintLine(str_save[ypos]);
				}
			}
			else
			{
				PrintBottomString(NEWTRODIT_ERROR_CLIPBOARD_COPY);
			}
			CloseClipboard();
			ch = 0;
			continue;
		}

		if (ch == 4) // ^D (Debug tool/dev mode)
		{
			ch = 0;
			if (CheckKey(VK_SHIFT))
			{
				if (dev_tools)
				{
					PrintBottomString(join(NEWTRODIT_DEV_TOOLS, NEWTRODIT_DIALOG_DISABLED));
				}
				else
				{
					PrintBottomString(join(NEWTRODIT_DEV_TOOLS, NEWTRODIT_DIALOG_ENABLED));
				}
				dev_tools = !dev_tools;
				c = -2;
				continue;
			}
			if (dev_tools)
			{
				gotoxy(strlen(str_save[ypos]) + LINECOUNT_WIDE, ypos);
				PrintLine(join(join("\"", str_save[ypos]), "\""));
				MakePause();
				ClearPartial(LINECOUNT_WIDE + strlen(str_save[ypos]), display_y, strlen(str_save[ypos]) + 2, 1);
				PrintLine(str_save[ypos]);
			}
		}
		if (ch == 23) // ^W
		{
			if (!CheckKey(VK_SHIFT))

			{

				NewFile(&isModified, &isUntitled, &isSaved, BUFFER_Y, str_save, &xpos, &ypos, filename_text, relative_xpos, relative_ypos);
				PrintBottomString(NEWTRODIT_FILE_CLOSED);
				c = -2;
				ch = 0;
				continue;
			}
		}

		if (ch == 17) // ^Q = Quit program (only with oldKeybinds = false)
		{
			if (!useOldKeybinds)
			{
				if (!CheckKey(VK_SHIFT))
				{
					QuitProgram(start_color);
					ShowBottomMenu();
					SetColor(bg_color);
					ch = 0;
					continue;
				}
			}
		}
		if (ch == 24) // ^X = Quit program/cut
		{
			if (useOldKeybinds)
			{
				if (!CheckKey(VK_SHIFT))
				{
					QuitProgram(start_color);
					ShowBottomMenu();
					SetColor(bg_color);
					ch = 0;
				}
			}
			else
			{
				if (!CheckKey(VK_SHIFT))
				{
					if (str_save[ypos][0] != '\0')
					{
						undo_stack_tree++;
						Ustack->line = str_save[ypos];
						Ustack->line_count = ypos;
						HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(str_save[ypos]) + 1);
						memcpy(GlobalLock(hMem), str_save[ypos], strlen(str_save[ypos]) + 1); // Copy line to the clipboard
						GlobalUnlock(hMem);
						OpenClipboard(0);
						EmptyClipboard();
						SetClipboardData(CF_TEXT, hMem);
						CloseClipboard();
						EmptyString(str_save[ypos]);
						ClearPartial(0, display_y, XSIZE, 1);
						xpos = 0;
					}

					ch = 0;
					continue;
				}
			}

			continue;
		}
		if (ch == 127) // ^BS
		{
			if (xpos > 0)
			{
				strncpy_n(undo_stack, str_save[ypos], BUFFER_X);
				undo_stack_line = ypos;
				if (str_save[ypos][xpos] == '\0')
				{

					bs_tk = tokback_pos(str_save[ypos], "()[]{}\t ", "?!");

					if (str_save[ypos][xpos] == 9)
					{
						relative_xpos[ypos] -= TAB_WIDE;
					}
					str_save[ypos][bs_tk] = 0;
					n = strlen(str_save[ypos]);
					for (int i = 0; i < bs_tk; i++)
					{
						str_save[ypos][n + i] = 0;
					}
					for (int i = xpos; i >= bs_tk; i--)
					{
						if (str_save[ypos][i] == 9)
						{
							relative_xpos[ypos] -= TAB_WIDE;
						}
						str_save[ypos][i] = '\0';
					}
					n = XSIZE;
					for (int i = 0; i < xpos - bs_tk; ++i)
					{
						// Pos-delete < WINDOW_SIZE
						// 140-30 < 120
						if ((xpos - i) < n)
						{
							printf("\b \b");
						}
					}
					xpos = bs_tk;
				}
				else
				{
					bs_tk = tokback_pos(str_save[ypos] + xpos, "()[]{}\t ", "?!");
					if (bs_tk <= 0)
					{
						bs_tk = 0;
					}
					for (int i = 0; i < (strlen(str_save[ypos]) - bs_tk); i++)
					{
						temp_strsave = delete_char_left(str_save[ypos], bs_tk);
						strncpy_n(str_save[ypos], temp_strsave, BUFFER_X);
						xpos--;
					}

					ClearPartial(LINECOUNT_WIDE + xpos, display_y, XSIZE, 1);
					gotoxy(LINECOUNT_WIDE, display_y);
					PrintLine(str_save[ypos]);
				}
			}

			ch = 0;
		}

		if (ch == 25) // ^Y = Redo
		{
			strncpy_n(undo_stack, str_save[undo_stack_line], BUFFER_X);
			strncpy_n(str_save[undo_stack_line], redo_stack, BUFFER_X);
			LoadAllNewtrodit();
			DisplayFileContent(str_save, stdout);
			fflush(stdout);

			ch = 0;
			continue;
		}

		if (ch == 26) // ^Z = Undo
		{

			strncpy_n(str_save[Ustack[undo_stack_tree].line_count], Ustack[undo_stack_tree].line, BUFFER_X);

			if (strlen(undo_stack) < strlen(str_save[undo_stack_line]))
			{
				xpos = nolflen(undo_stack);
			}
			strncpy_n(str_save[undo_stack_line], undo_stack, BUFFER_X);
			ClearPartial(LINECOUNT_WIDE, undo_stack_line, XSIZE, 1);
			memset(Ustack[undo_stack_tree].line, 0, BUFFER_X);

			gotoxy(LINECOUNT_WIDE, Ustack[undo_stack_tree].line_count);
			PrintLine(str_save[Ustack[undo_stack_tree].line_count]);
			ch = 0;
			continue;
		}

		if (ch == 8 && !CheckKey(0x08) && CheckKey(VK_CONTROL)) // ^H = Replace string / S-^H = Same as F1 (opens help)
		{
			ch = 0;

			if (CheckKey(VK_SHIFT))
			{
				NewtroditHelp();

				LoadAllNewtrodit();

				DisplayFileContent(str_save, stdout);
				DisplayCursorPos(xpos, ypos);
			}
			else
			{
				replace_count = 0;
				ClearPartial(0, YSIZE - 2, XSIZE, 2);

				gotoxy(0, YSIZE - 2);
				PrintLine(NEWTRODIT_PROMPT_FIND_STRING);
				gotoxy(0, BOTTOM);
				PrintLine(NEWTRODIT_PROMPT_REPLACE_STRING);

				n = xpos;
				gotoxy(strlen(NEWTRODIT_PROMPT_FIND_STRING), YSIZE - 2);

				fgets(find_string, sizeof find_string, stdin);
				gotoxy(strlen(NEWTRODIT_PROMPT_REPLACE_STRING), BOTTOM);
				if (!nolflen(find_string))
				{
					FunctionAborted(str_save);
					continue;
				}
				fgets(replace_string, sizeof find_string, stdin);
				find_string[strcspn(find_string, "\n")] = 0;
				replace_string[strcspn(replace_string, "\n")] = 0;
				if (!ValidString(find_string) || !ValidString(replace_string))
				{
					FunctionAborted(str_save);
					continue;
				}

				for (int i = 1; i < BUFFER_Y; i++) // Line 0 is unused
				{

					replace_str_ptr = ReplaceString(str_save[i], find_string, replace_string, &replace_count);
					if (replace_str_ptr)
					{
						if (strlen(replace_str_ptr) < strlen(str_save[i]))
						{
							xpos = strlen(replace_str_ptr);
						}

						strncpy_n(str_save[i], replace_str_ptr, BUFFER_X);
					}
				}

				if (strlen(str_save[ypos]) < n)
				{
					xpos = strlen(str_save[ypos]);
				}
				LoadAllNewtrodit();
				DisplayFileContent(str_save, stdout);
				PrintBottomString(join(join(join("Replaced ", itoa_n(replace_count)), " occurrences of "), find_string));
				MakePause();
				ShowBottomMenu();
				DisplayCursorPos(xpos, ypos);
			}
			continue;
		}

		if ((ch == 8 && _NEWTRODIT_OLD_SUPPORT == 1) || (ch == 8 && CheckKey(0x08) && !CheckKey(VK_CONTROL))) // BS key (Avoiding Control-H)
		{
			c = -8; // Negative to avoid conflict
			if (xpos > 0)
			{

				if (str_save[ypos][xpos - 1] == 9) // TAB key
				{
					relative_xpos[ypos] -= TAB_WIDE;
					gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, display_y + relative_ypos[xpos]);
				}

				if (str_save[ypos][xpos] != '\0')
				{
					/*
						TODO: CRLF COMPATIBILITY

					*/

					strncpy_n(temp_strsave, str_save[ypos] + strlen(str_save[ypos]), strlen(str_save[ypos]) - strlen(newlinestring));
					hasNewLine = false;
					// Remove ending newline character from str_save[ypos]

					if (!strcmp(temp_strsave, newlinestring))
					{
						str_save[ypos][strlen(str_save[ypos]) - strlen(newlinestring)] = '\0';
						hasNewLine = true;
					}

					insert_str = delete_char_left(str_save[ypos], xpos - 1);
					if (hasNewLine == true)
					{
						strcat(insert_str, newlinestring); // strcat for CRLF newline
					}

					strncpy_n(str_save[ypos], insert_str, BUFFER_X);
					ClearPartial(LINECOUNT_WIDE + (nolflen(str_save[ypos])), display_y, 1, 1); // Clear the character
					gotoxy(LINECOUNT_WIDE, display_y);
					PrintLine(insert_str);
				}
				else
				{
					str_save[ypos][xpos - 1] = '\0';
					printf("\b \b");
				}
				if (syntaxHighlighting)
				{
					gotoxy(LINECOUNT_WIDE, display_y);
					color_line(str_save[ypos], 0);
				}
				xpos -= 2;
			}
			else
			{
				/* Act as END key */
				if (ypos > 1)
				{
					str_save[ypos][0] = '\0';

					xpos = nolflen(str_save[ypos - 1]);
					for (int i = 0; i < (strlen(str_save[ypos - 1]) - nolflen(str_save[ypos - 1])); i++)
					{
						str_save[ypos - 1][strlen(str_save[ypos - 1]) - i] = '\0';
					}
					if (str_save[ypos - 1][0] == 0x0A)
					{
						xpos = 0;
					}
					ypos--;
					UpdateScrolledScreen(lineCount);
				}
				xpos--;
			}
		}
		else

		{
			if (ch > 31 || (ch == 9 && CheckKey(0x09))) // Printable character
			{
				if (isModified == false)
				{
					isModified = true;
				}

				if (!insertChar && ch != 9) // Insert key not pressed
				{

					if (str_save[ypos][xpos] != '\0')
					{
						if (ch == 9)
						{
							relative_xpos[ypos] += TAB_WIDE;
						}
						insert_str = insert_char(str_save[ypos], ch, xpos);
						strncpy_n(str_save[ypos], insert_str, BUFFER_X);
						gotoxy(LINECOUNT_WIDE, display_y); // Maybe this creates a bug, I don't know
						PrintLine(str_save[ypos]);

						ch = 0;
					}
				}

				if (CheckKey(VK_TAB) && ch == 9) // TAB key
				{
					if (convertTabtoSpaces == true)
					{
						for (int i = 0; i < TAB_WIDE; i++) // i is also character 9 :)
						{
							temp_strsave = insert_char(str_save[ypos], ' ', xpos);
							strncpy_n(str_save[ypos], temp_strsave, BUFFER_X);
						}
						xpos += (TAB_WIDE - 1);
						gotoxy(LINECOUNT_WIDE, display_y);
						PrintLine(str_save[ypos]);
					}
					else
					{
						temp_strsave = insert_char(str_save[ypos], 9, xpos);
						strncpy_n(str_save[ypos], temp_strsave, BUFFER_X);
						PrintTab(TAB_WIDE);
						relative_xpos[ypos] += TAB_WIDE;
						gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
					}
					
				}
				else

				{
					if (ch != 0)
					{
						str_save[ypos][xpos] = (int)ch; // Add character to buffer

						if (syntaxHighlighting)
						{
							gotoxy(LINECOUNT_WIDE, display_y);
							color_line(str_save[ypos], 0);
						}
						else
						{
							if (xpos < XSIZE - LINECOUNT_WIDE)
							{
								putchar(ch);
							}
						}
					}
				}
			}
			else
			{
				if (ch != 0 && ch <= 26 && ch != 13)
				{
					EmptyString(inbound_ctrl_key); // Clear the string for the next key
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
					PrintBottomString(join(NEWTRODIT_ERROR_INVALID_INBOUND, inbound_ctrl_key));
					c = -2; // For later use
				}
				xpos--;
			}
		}

		xpos++;

		if (strlen(str_save[ypos]) >= BUFFER_X - (TAB_WIDE * 2) || ypos > BUFFER_Y || xpos >= BUFFER_X - (TAB_WIDE * 2))
		{
			tmp = realloc_n(str_save[ypos], BUFFER_X, BUFFER_X + BUFFER_INCREMENT);
			BUFFER_X += BUFFER_INCREMENT;

			if (tmp == NULL)
			{
				PrintBottomString(NEWTRODIT_ERROR_OUT_OF_MEMORY);
				MakePause();
				SaveFile(str_save, filename_text, BUFFER_Y, &isModified, &isUntitled);
				return ENOBUFS;
			}
			if (tmp)
			{
				free(str_save[ypos]);
				str_save[ypos] = tmp;
			}
		}
	}

	SetColor(start_color);
	return 0;
}