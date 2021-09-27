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

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <errno.h>
#include <stdbool.h>
#include <locale.h>
#include "manual.c"
#include <signal.h>
#include <time.h>

char **str_save;

struct SMALL_RECT
{
	SHORT left;
	SHORT top;
	SHORT right;
	SHORT bottom;
};

void sigint_handler(int signum)
{
	signal(SIGINT, sigint_handler);
	fflush(stdout);
}

void sigsegv_handler(int signum)
{
	signal(SIGSEGV, sigint_handler);

	NewtroditCrash("SIGSEGV signal was received (segmentation fault).", errno);
	fflush(stdout);
	exit(errno);
}

int LoadSettings(char *newtrodit_config_file, int *sigsegv, int *linecount, int *devmode)
{
	/*
		Settings are stored in INI format.
		The format is:
			key=value
			;comment

	*/

	FILE *settings = fopen(newtrodit_config_file, "rb");

	char buf[1024];
	char *iniptr;
	int tmp;

	char equalchar[] = "=";
	char comment = ';'; // Comments start with ; or #
	char *setting_list[] = {
		"bgcolor",
		"codepage",
		"converttab",
		"cursize",
		"devmode",
		"fgcolor",
		"linecount",
		"linecountwide",
		"manfile",
		"newline",
		"sigsegv",
		"tabwide",
		"xsize",
		"ysize",
	}; // List of settings that can be changed

	while (fgets(buf, sizeof(buf), settings))
	{
		if (buf[0] == comment || buf[0] == 10 || buf[0] == 13) // Comment or newline found
		{
			continue;
		}
		iniptr = strtok(buf, "=");

		while (iniptr != NULL) // Loop through the settings
		{
			for (int i = 0; i < sizeof(setting_list) / sizeof(char *); i++)
			{
				if (!strncmp(iniptr, setting_list[i], strlen(setting_list[i])))
				{
					if (!strcmp(setting_list[i], "bgcolor"))
					{
						bg_color = atoi(strtok(NULL, equalchar));
					}
					if (!strcmp(setting_list[i], "codepage"))
					{
						int cp = atoi(strtok(NULL, equalchar));
						SetConsoleOutputCP(cp);
					}
					if (!strcmp(setting_list[i], "cursize"))
					{
						CURSIZE = atoi(strtok(NULL, equalchar));
					}
					if (!strcmp(setting_list[i], "converttab"))
					{
						if (atoi(strtok(NULL, equalchar)) == 1)
						{
							convertTabtoSpaces = true;
						}
						else
						{
							convertTabtoSpaces = false;
						}
					}
					if (!strcmp(setting_list[i], "devmode"))
					{
						printf("%d", abs(atoi(strtok(NULL, equalchar))));
						MakePause();
						if (1 == 1)
						{
							*devmode = true;
						}
						else
						{
							*devmode = false;
						}
					}
					if (!strcmp(setting_list[i], "fgcolor"))
					{
						fg_color = atoi(strtok(NULL, equalchar));
					}
					if (!strcmp(setting_list[i], "manfile"))
					{
						strcpy(manual_file, strtok(NULL, equalchar));
						manual_file[strcspn(manual_file, "\n")] = 0;
					}
					if (!strcmp(setting_list[i], "newline"))
					{
						strncpy_n(newlinestring, strtok(NULL, equalchar), sizeof(newlinestring));
						newlinestring[strcspn(newlinestring, "\n")] = 0; // Remove newline
					}

					if (!strcmp(setting_list[i], "linecount"))
					{
						if (atoi(strtok(NULL, equalchar)) == 1)
						{
							*linecount = true;
						}
						else
						{
							*linecount = false;
						}
					}
					if (!strcmp(setting_list[i], "linecountwide"))
					{
						LINECOUNT_WIDE = atoi(strtok(NULL, equalchar));
					}
					if (!strcmp(setting_list[i], "tabwide"))
					{
						TAB_WIDE = atoi(strtok(NULL, equalchar));
					}

					if (!strcmp(setting_list[i], "sigsegv"))
					{
						if (atoi(strtok(NULL, equalchar)) == 1)
						{
							*sigsegv = true;
						}
						else
						{
							*sigsegv = false;
						}
					}

					if (!strcmp(setting_list[i], "xsize"))
					{
						int xs = atoi(strtok(NULL, equalchar));
						SetConsoleSize(xs, YSIZE);
					}
					if (!strcmp(setting_list[i], "ysize"))
					{
						int ys = atoi(strtok(NULL, equalchar));
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
	int hasNewLine = false; // Bool for newline in insert char
	int lineCount = false;	// Bool for line count
	int dev_tools = true;	// Bool to enable or disable the dev tools
	int isSaved = false;	// Bool to check if file is saved
	int insertChar = false; // Bool to check if replace instead of insert
	int isModified = false;
	int findInsensitive = false;
	int sigsegvScreen = false;
	int isUntitled = true;

	LoadSettings(settings_file, &sigsegvScreen, &lineCount, &dev_tools); // Load settings from settings file

	signal(SIGINT, sigint_handler);	  // Ctrl+C handler
	signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
	if (sigsegvScreen == true)
	{
		signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
	}
	if (ValidSize() == false)
	{
		return 1;
	}

	str_save = calloc(BUFFER_Y, BUFFER_X); // Allocate 1024 char pointers
	for (int i = 0; i < BUFFER_Y; ++i)
	{
		str_save[i] = calloc(BUFFER_Y, BUFFER_X); // Allocate 1024 bytes for each string
	}

	if (str_save[0] == NULL) // If calloc() fails, return an error and quit the program
	{
		printf("%s%d (%s)", NEWTRODIT_ERROR_CALLOC_FAILED, errno, strerror(errno));

		return 0;
	}

	char *temp_strsave = (char *)malloc(BUFFER_X) + 1;

	char *undo_stack = (char *)malloc(BUFFER_X) + 1; // For undo stack (^Z)
	char *redo_stack = (char *)malloc(BUFFER_X) + 1; // For undo stack (^Z)

	int undo_stack_line;

	if (lineCount == false)
	{
		LINECOUNT_WIDE = 0;
	}

	// Declare variables
	int old_x_size, old_y_size;
	int bs_tk;

	int start_color = GetColor();
	int old_argc_shift;
	int line_number_chr = 0, line_number_chr_index = 0;

	char save_destination[FILENAME_MAX];
	char line_number_str[10];

	char find_string[512], find_string_original[512];
	int find_string_index, find_string_index_old;
	char *final_paste_strrchr;
	char fileopenread[FILENAME_MAX];
	char *insert_str;

	char inbound_ctrl_key[40];
	char newname[FILENAME_MAX];

	int n; // General variable
	// File variables
	FILE *fileread;
	FILE *fp_savefile;
	FILE *newtrodit_open_argv;

	// Position variables
	int xpos = 0, ypos = 1, relative_xpos[BUFFER_Y] = {0}, relative_ypos[BUFFER_X] = {0};

	int c;
	// getch() variables
	int ch, save_ch;

	SetColor(bg_color);
	int argc_shift = 1; // Can't be 0, because the first argument is the program name

	while (argc_shift < argc)
	{

		if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) // Version parameter
		{
			printf("%s", ProgInfo());
			return 0;
		}
		if (!strcmp(argv[argc_shift], "--help") || !strcmp(argv[argc_shift], "-h")) // Manual parameter
		{
			NewtroditHelp();
			SetColor(start_color);
			ClearScreen();
			return 0;
		}

		if (!strcmp(argv[argc_shift], "--sigsegv") || !strcmp(argv[argc_shift], "-s")) // Sigsegv parameter
		{
			signal(SIGSEGV, sigsegv_handler); // Segmentation fault handler
			argc_shift++;
			continue;
		}

		if (!strcmp(argv[argc_shift], "--fg") || !strcmp(argv[argc_shift], "-f")) // Foreground color parameter
		{
			if (argv[argc_shift + 1] != NULL)
			{
				fg_color = strtol(argv[argc_shift + 1], NULL, 16);
				if (fg_color > 0x0F || fg_color < 0)
				{
					printf("%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
					return 1;
				}
				fg_color *= 16;
				argc_shift++;
			}
			else
			{
				printf("%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
				return 1;
			}

			argc_shift++;
		}

		if (!strcmp(argv[argc_shift], "--bg") || !strcmp(argv[argc_shift], "-b")) // Background color parameter
		{
			if (argv[argc_shift + 1] != NULL)
			{
				argc_shift++;

				bg_color = strtol(argv[argc_shift + 1], NULL, 16);
				if (bg_color > 0x0F || bg_color < 0)
				{
					printf("%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
					return 1;
				}
			}
			else
			{
				printf("%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
				return 1;
			}
			argc_shift++;
		}

		if (!strcmp(argv[argc_shift], "--line") || !strcmp(argv[argc_shift], "-l")) // Display line count
		{
			lineCount = true;
			LINECOUNT_WIDE = LINECOUNT_WIDE_; // Set the line count width
			argc_shift++;
		}

		if (!strcmp(argv[argc_shift], "--lfunix") || !strcmp(argv[argc_shift], "-n")) // Use UNIX new line
		{
			strcpy(newlinestring, "\n");
			argc_shift++;
		}

		if (!strcmp(argv[argc_shift], "--lfwin") || !strcmp(argv[argc_shift], "-w")) // Use Windows new line
		{
			strcpy(newlinestring, "\r\n");
			argc_shift++;
		}
	}

	if (argc_shift > 1)
	{

		strcpy(filename_text, argv[argc_shift - 1]);
		isUntitled = false;
		newtrodit_open_argv = fopen(filename_text, "rb");

		if (!newtrodit_open_argv)
		{

			printf("%s: %s", filename_text, strerror(errno));
			exit(errno);
		}
		LoadAllNewtrodit();
		gotoxy(0, 1);
		fseek(newtrodit_open_argv, 0, SEEK_END); // Go to the end of the file

		if (ftell(newtrodit_open_argv) > (long)BUFFER_X * BUFFER_Y) // If file is larger than 1 MB
		{
			PrintBottomString(join(NEWTRODIT_FS_FILE_TOO_LARGE, filename_text));
			MakePause();
			ClearScreen();
			return EFBIG; // File too big
		}
		fseek(newtrodit_open_argv, 0, SEEK_SET); // Return to the beginning of the file
		LoadFile(str_save, filename_text, relative_xpos, newlinestring, newtrodit_open_argv);
		fflush(stdout);
		CenterText(filename_text, 0);
	}
	else
	{
		LoadAllNewtrodit();
	}

	while (1)
	{
		if (isModified == true)
		{
			SetConsoleTitle(join(join("Newtrodit - ", filename_text), " (Modified)"));
		}
		else
		{
			SetConsoleTitle(join("Newtrodit - ", filename_text));
		}
		if (lineCount == true)
		{
			DisplayLineCount(str_save, YSIZE - 3, ypos + relative_ypos[xpos]);
		}
		if (c != -2)
		{
			DisplayCursorPos(xpos, ypos);
		}
		old_x_size = XSIZE;
		old_y_size = YSIZE;
		gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]); // Relative position is for tab key

		ch = getch(); // Get key pressed
		if (c == -2)  // Inbound invalid control key
		{
			ShowBottomMenu();
			DisplayCursorPos(xpos, ypos);
			gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
		}
		if (old_x_size != GetConsoleXDimension() || old_y_size != GetConsoleYDimension()) // Check if size has been modified
		{

			LoadAllNewtrodit();

			gotoxy(0, 1);
			DisplayFileContent(str_save, newlinestring, stdout);
		}

		if (ch == 3) // ^C = Copy string to clipboard
		{
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(str_save[ypos]) + 1);
			memcpy(GlobalLock(hMem), str_save[ypos], strlen(str_save[ypos]) + 1); // Copy line to the clipboard
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
			ch = 0;
			continue;
		}

		if (ch == 12) // S-^L
		{
			if (dev_tools == true)
			{
				if (CheckKey(VK_SHIFT))
				{
					if (lineCount == true)
					{
						lineCount = false;
						LINECOUNT_WIDE = 0;
					}
					else
					{
						PrintBottomString(NEWTRODIT_PROMPT_LINE_COUNT);

						n = YesNoPrompt();
						if (n == 1)
						{
							lineCount = true;
							LINECOUNT_WIDE = LINECOUNT_WIDE_; // Backup original wide
							DisplayLineCount(str_save, YSIZE - 4, ypos + relative_ypos[xpos]);
						}
					}
					LoadAllNewtrodit();
					DisplayFileContent(str_save, newlinestring, stdout);
					DisplayCursorPos(xpos, ypos);

					gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
					ch = 0;
					continue;
				}
			}
		}

		if (ch == 7) // ^G = Goto line
		{
			ch = 0;
			PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);
			line_number_chr = 0, line_number_chr_index = 0;
			for (int i = 0; i < sizeof(line_number_str); i++)
			{
				line_number_str[i] = 0;
			}
			while (line_number_chr != 13) // Loop while enter isn't pressed
			{
				line_number_chr = getch();
				if (line_number_chr == 27)
				{
					break;
				}
				if (line_number_chr >= 48 && line_number_chr <= 57) // Check if character is a number
				{
					if (strlen(line_number_str) > 4 || atoi(line_number_str) >= BUFFER_Y) // If line number is bigger than 10000, alert
					{
						Alert();
					}
					else
					{
						line_number_str[line_number_chr_index++] = line_number_chr;
						putchar(line_number_chr);
					}
				}
				if (line_number_chr == 8) // Backspace
				{
					if (line_number_chr_index > 0)
					{
						line_number_str[line_number_chr_index--] = '\0';
						printf("\b \b");
					}
				}
			}
			if (atoi(line_number_str) < 1) // Line is less than 1
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				MakePause();
				ShowBottomMenu();
				continue;
			}
			if (atoi(line_number_str) > BUFFER_Y) // Line is bigger than buffer
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				MakePause();
				ShowBottomMenu();
				continue;
			}
			if (str_save[atoi(line_number_str)][0] == '\0' && atoi(line_number_str) != 1) // Line is less than 1
			{
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				MakePause();
				ShowBottomMenu();
				continue;
			}
			if (atoi(line_number_str) > GetConsoleYDimension() - 3) // Line too big for the console buffer
			{
				PrintBottomString(NEWTRODIT_ERROR_GOTO_BUFFER_SIZE);
				MakePause();
				ShowBottomMenu();
			}
			else
			{
				gotoxy(0, atoi(line_number_str));
				xpos = 0;
				ypos = atoi(line_number_str);
				ShowBottomMenu();
			}
			continue;
		}
		if (ch == 14) // ^N = New file
		{

			NewFile(&isModified, &isUntitled, BUFFER_Y, str_save, &xpos, &ypos, filename_text, relative_xpos, relative_ypos);
			PrintBottomString(NEWTRODIT_NEW_FILE_CREATED);
			MakePause();

			ShowBottomMenu();

			ch = 0;
			continue;
		}
		if (ch == 15) // ^O = Open file
		{
			if (!CheckKey(VK_SHIFT)) // Shift must not be pressed
			{
				if (isModified == true)
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);
					c = getch();
					if (tolower(c) != 'n' && c != 0 && c != 0xe0)
					{
						if (SaveFile(str_save, filename_text, YSIZE, &isModified, &isUntitled) != 0)
						{
							LoadAllNewtrodit();
							DisplayFileContent(str_save, newlinestring, stdout);
							continue;
						}
					}
				}
				gotoxy(1, BOTTOM);
				PrintBottomString(NEWTRODIT_PROMPT_FOPEN);

				fgets(fileopenread, sizeof fileopenread, stdin);
				gotoxy(0, 1);

				ClearLine(YSIZE - 1);
				// Remove trailing LF
				fileopenread[strcspn(fileopenread, "\n")] = 0;
				fileread = fopen(fileopenread, "r");
				n = errno;
				if (!fileread) // Failed to open the file
				{
					LoadAllNewtrodit();

					DisplayFileContent(str_save, newlinestring, stdout);
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

				strcpy(filename_text, fileopenread);
				LoadAllNewtrodit();

				LoadFile(str_save, filename_text, relative_xpos, newlinestring, fileread);
				fflush(stdout);

				xpos = 0, ypos = 1;
				isSaved = true;
				fclose(fileread);
				ch = 0;
				continue;
			}
		}
		if (ch == 18) // ^R = Reload file
		{
			newtrodit_open_argv = fopen(filename_text, "rb");
			PrintBottomString(NEWTRODIT_PROMPT_RELOAD_FILE);
			c = getch();
			if (tolower(c) == 'y' && c != 0 && c != 0xE0)
			{
				ReloadFile(&isUntitled, str_save, xpos, ypos, relative_xpos, relative_ypos, newtrodit_open_argv);
			}
			else
			{
				ShowBottomMenu();
			}

			ch = 0;
			continue;
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
						gotoxy(xpos, ypos);
					}
					else
					{
						if (str_save[ypos - 1][xpos] == 9)
						{
							xpos += relative_xpos[ypos - 1];
						}
						gotoxy(xpos, --ypos);
					}
				}
				break;
			case 75:
				// Left arrow

				if (xpos >= 1)
				{
					if (str_save[ypos][xpos - 1] == 9)
					{
						relative_xpos[ypos] -= TAB_WIDE;
					}
					gotoxy(--xpos, ypos);
				}
				else
				{
					if (ypos > 1)
					{

						xpos = nolflen(str_save[--ypos]);
						gotoxy(xpos, ypos);
					}
				}

				break;
			case 77:
				// Right arrow
				if (str_save[ypos][xpos] != '\0')
				{
					if (xpos == nolflen(str_save[ypos]))
					{
						xpos = 0;
						ypos++;
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
				break;

			case 80:
				// Down arrow
				if ((ypos + 1) > YSIZE - 3)
				{
					Alert();
				}
				else
				{
					if (str_save[ypos + 1][0] != '\0')
					{
						relative_xpos[ypos + 1] = 0;

						if (str_save[ypos + 1][xpos] == '\0')
						{

							xpos = nolflen(str_save[++ypos]);
						}
						else
						{
							ypos++;
						}
					}
				}

				break;

			case 71:
				// HOME key
				xpos = 0;
				gotoxy(xpos, ypos);
				break;
			case 79:
				// END key

				xpos = nolflen(str_save[ypos]);

				break;
			case 119:
				// ^HOME key
				xpos = 0;
				ypos = 1;
				gotoxy(xpos, ypos);
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
				//gotoxy(xpos, ypos);
				break;
			case 82:
				// INS key
				insertChar = !insertChar;

				break;

			case 83:
				// DEL key
				if (CheckKey(VK_SHIFT))
				{
					if (ypos > 0)
					{
						EmptyString(str_save[ypos]);

						ClearLine(ypos);
						strncat(str_save[ypos], newlinestring, strlen(newlinestring));
					}
				}
				else
				{
					if (strlen(str_save[ypos]) > 0)
					{
						strcpy(undo_stack, str_save[ypos]);
						undo_stack_line = ypos;
						insert_str = delete_char(str_save[ypos], xpos);
						if (strlen(insert_str) >= xpos)
						{
							strcpy(str_save[ypos], insert_str);
							ClearLine(ypos);

							gotoxy(0, ypos);
							printf("%s", str_save[ypos]);
						}
					}
				}

				break;

				/* case 133: // F11 (fullscreen)
		
				if (isFullScreen == false)
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
				if (dev_tools == 1)
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
				ch = -1; // For undo stack
			}
			continue;
		}

		if (ch == 13 && CheckKey(VK_RETURN)) // Newline character: CR (13)
		{
			// TODO: Add a newline to the undo stack
			if (!(ypos < YSIZE - 4))
			{
				Alert();
				continue;
			}
			strcat(str_save[ypos], newlinestring); // Add newline to current line

			if (xpos == nolflen(str_save[ypos]) && str_save[ypos + 1][0] != '\0')
			{
				for (int i = YSIZE; i >= xpos; i--)
				{
					memmove(str_save[i + 1], str_save[i], strlen(str_save[i]) + 1);
					//ClearLine(i + 1);
					//printf("%s", str_save[i]);
				}
				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);

				continue;
			}

			if (xpos == (strlen(str_save[ypos]) - strlen(newlinestring)) && str_save[ypos + 1][0] != '\0')
			{

				strcpy(str_save[ypos] + xpos + strlen(newlinestring), newlinestring);

				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
			}

			xpos = 0;
			ypos++;
			continue;
		}

		if (ch == 6) // ^F = Find string
		{
			// Empty values
			EmptyString(find_string_original);
			EmptyString(find_string);
			find_string_index = 0;
			c = 0;

			findInsensitive = false;
			if (CheckKey(VK_SHIFT))
			{
				findInsensitive = true;
			}
			if (findInsensitive == true)
			{
				PrintBottomString(NEWTRODIT_PROMPT_FIND_STRING_INSENSITIVE);
			}
			else
			{
				PrintBottomString(NEWTRODIT_PROMPT_FIND_STRING);
			}

			fgets(find_string, sizeof find_string, stdin);
			if (nolflen(find_string) <= 0)
			{
				FunctionAborted(str_save, newlinestring);
				continue;
			}
			find_string[strcspn(find_string, "\n")] = '\0';

			strcpy(find_string_original, find_string);
			LoadAllNewtrodit();
			gotoxy(0, 1);
			DisplayFileContent(str_save, newlinestring, stdout);
			for (int i = 1; i < YSIZE - 1; i++)
			{
				if (findInsensitive == true)
				{
					find_string_index = FindString(str_lwr(str_save[i]), str_lwr(find_string));
					if (find_string_index >= 0)
					{
						strncpy_n(find_string, str_save[i] + find_string_index, strlen(find_string));
					}
				}
				else
				{
					find_string_index = FindString(str_save[i], find_string);
				}

				while (find_string_index >= 0)
				{

					gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);
					SetColor(0x0e);
					printf("%s", find_string);
					gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);

					SetColor(bg_color);
					ShowFindMenu();
					gotoxy(find_string_index + strlen(find_string_original) + relative_xpos[i] + LINECOUNT_WIDE, i);
					c = getch();
					if (c == 27) // ESC
					{
						// Quit the while loop
						c = -6;
						break;
					}
					if (c == 0) // If key is 0
					{
						c = getch();
						if (c == 61)
						{
							if (findInsensitive == true)
							{
								find_string_index = FindString(str_lwr(str_save[i]), str_lwr(find_string));
							}
							else
							{
								find_string_index = FindString(str_save[i], find_string);
							}
							if (find_string_index == find_string_index_old)
							{
								c = -6;
								printf("ok");
								MakePause();
								break;
							}
							if (find_string_index >= 0)
							{
								strncpy_n(find_string, str_save[i] + find_string_index, strlen(find_string));
								strncpy_n(temp_strsave, str_save[i] + find_string_index + strlen(find_string), strlen(str_save[i]) - find_string_index);
								gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);
								SetColor(0x0e);
								printf("%s", find_string);
								gotoxy(find_string_index + relative_xpos[i] + LINECOUNT_WIDE, i);

								SetColor(bg_color);
								ShowFindMenu();
								gotoxy(find_string_index + strlen(find_string_original) + relative_xpos[i] + LINECOUNT_WIDE, i);
							}
						}

						else
						{
							c = -6; // -6 means F but in reverse
						}
						MakePause();
					}

					ShowBottomMenu();
					strcpy(find_string, find_string_original);

					xpos = find_string_index + strlen(find_string_original);
					ypos = i;
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
			ch = 0;
			continue;
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 107) // A-F4 key
			{
				QuitProgram(start_color);
				ShowBottomMenu();
				SetColor(bg_color);
				ch = 0;
				continue;
			}
			if (ch == 59) // F1 key
			{
				NewtroditHelp();

				LoadAllNewtrodit();
				gotoxy(0, 1);

				// Print again the text already written on the screen
				DisplayFileContent(str_save, newlinestring, stdout);
				gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
				DisplayCursorPos(xpos, ypos);
				ch = 0;
				continue;
			}
			if (ch == 60) // F2 key
			{
				if (isSaved == false)
				{
					if (SaveFile(str_save, filename_text, YSIZE, &isModified, &isUntitled) != 0)
					{
						LoadAllNewtrodit();
						DisplayFileContent(str_save, newlinestring, stdout);
					}
				}

				PrintBottomString(NEWTRODIT_PROMPT_RENAME_FILE);
				fgets(newname, sizeof(newname), stdin);
				newname[strcspn(newname, "\n")] = 0;
				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
				if (nolflen(newname) > 0)
				{
					if (CheckFile(newname) == 0)
					{
						PrintBottomString(NEWTRODIT_PROMPT_OVERWRITE);
						n = YesNoPrompt();
						if (n == 1)
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
					if (rename(filename_text, newname) == 0)
					{
						PrintBottomString(join(NEWTRODIT_FILE_RENAMED, newname));
						strcpy(filename_text, newname);
						NewtroditNameLoad();
						CenterText(filename_text, 0);
						RightAlignNewline();
					}
					else
					{
						PrintBottomString(join(NEWTRODIT_FS_FILE_RENAME, strerror(errno)));
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
			}
			if (ch == 63) // F5 key
			{
				temp_strsave = GetTime();
				for (int i = 0; i < strlen(temp_strsave); ++i)
				{
					insert_str = insert_char(str_save[ypos], temp_strsave[i], xpos + i);
					strcpy(str_save[ypos], insert_str);
				}
				xpos += strlen(temp_strsave);

				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
				DisplayCursorPos(xpos, ypos);
				ch = 0;
			}
			if (ch == 93) // S-F10 key
			{
				StartProcess("cmd.exe");
			}
			if (ch == 68) // F10 key
			{
				StartProcess("explorer.exe .");
			}
			if (ch == 94) // ^F1 key
			{
				LoadAllNewtrodit();
				CenterText("About Newtrodit", (YSIZE / 2) - 2);
				CenterText(ProgInfo(), (YSIZE / 2));
				// I know it's not the best way to do it, but it works
				CenterText("Contribute at https://github.com/anic17/Newtrodit", (YSIZE / 2) + 2);

				MakePause();
				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
			}

			ch = 0;
			continue;
		}

		if (ch == 19) // ^S
		{
			if (CheckKey(VK_LSHIFT))
			{
				isSaved = 2;
			}

			strcpy(save_destination, filename_text); // If no input is given
			if (isSaved == 0 || isSaved == 2)
			{
				if (isSaved == 0)
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE);
				}
				else
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE_AS);
				}
				fgets(save_destination, sizeof save_destination, stdin);
				if (nolflen(save_destination) <= 0)
				{
					FunctionAborted(str_save, newlinestring);
					continue;
				}
				isSaved = 1;

				gotoxy(0, 0);
				save_destination[strcspn(save_destination, "\n")] = 0;

				fp_savefile = fopen(save_destination, "rb");

				if (CheckFile(save_destination) == 0)
				{
					PrintBottomString(NEWTRODIT_PROMPT_OVERWRITE);
					save_ch = getch();
					LoadAllNewtrodit();
					DisplayFileContent(str_save, newlinestring, stdout);
					if (tolower(save_ch) != 'y')
					{
						fclose(fp_savefile);
						continue;
					}
				}

				fclose(fp_savefile);
				fp_savefile = fopen(save_destination, "wb");
				if (!fp_savefile)
				{
					PrintBottomString(strerror(errno));
					continue;
				}
			}
			gotoxy(0, 1);
			if (isModified == true)
			{
				isModified = false;
			}
			fp_savefile = fopen(save_destination, "wb"); // Reopen the file to write
			if (!fp_savefile)
			{
				PrintBottomString(NEWTRODIT_FS_FILE_SAVE_ERR);
				MakePause();
				continue;
			}
			isSaved = true;
			isUntitled = false;
			DisplayFileContent(str_save, newlinestring, fp_savefile); // Write the file content

			strcpy(filename_text, save_destination);

			LoadAllNewtrodit();

			DisplayFileContent(str_save, newlinestring, stdout);
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
				strcpy(undo_stack, str_save[ypos]);
				undo_stack_line = ypos;
				buffer_clipboard = (char *)GetClipboardData(CF_TEXT);
				if (buffer_clipboard != NULL)
				{
					printf("%s", buffer_clipboard);
					final_paste_strrchr = strlasttok(buffer_clipboard, '\n');

					for (int i = 0; i < strlen(final_paste_strrchr); ++i)
					{
						temp_strsave = insert_char(str_save[ypos], final_paste_strrchr[i], i + xpos);
						strcpy(str_save[ypos], temp_strsave);
					}
					xpos += strlen(final_paste_strrchr);
					if (xpos > BUFFER_X || ypos > BUFFER_Y)
					{
						PrintBottomString(NEWTRODIT_ERROR_CON_BUFFER_LIMIT);
						MakePause();
					}
					ch = -1;
				}
				else
				{
					PrintBottomString(NEWTRODIT_ERROR_CLIPBOARD_COPY);
				}
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
				MakePause();
				ShowBottomMenu();
				continue;
			}
			if (dev_tools == true)
			{
				gotoxy(strlen(str_save[ypos]) + LINECOUNT_WIDE, ypos);
				printf("\"%s\"", str_save[ypos]);
				MakePause();
				for (int i = 0; i < strlen(str_save[ypos]) + 2; i++) // Also add double quotes
				{
					printf("\b \b");
				}
			}
		}
		if (ch == 23) // ^W
		{
			PrintBottomString(NEWTRODIT_PROMPT_CLOSE_FILE);
			n = getch();

			if (tolower(n) == 'y' && n != 0 && n != 0xE0)
			{
				if (fseek(fp_savefile, 0, SEEK_SET) == 0)
				{
					fclose(fp_savefile);
				}
				fclose(fp_savefile);
				memset(filename_text, 0, sizeof(filename_text)); // Empty the array
				strcpy(filename_text, filename_text_);			 // Restore the filename to "Untitled"

				// Empty the file
				for (int i = 0; i < BUFFER_Y; i++)
				{
					free(str_save[i]);
				}
				xpos = 0, ypos = 1;
				LoadAllNewtrodit();
			}
			if (n == 0 || n == 0xE0)
			{
				getch();
			}
			ShowBottomMenu();
			ch = 0;
			continue;
		}

		if (ch == 24) // ^X = Quit program
		{
			QuitProgram(start_color);
			ShowBottomMenu();
			SetColor(bg_color);
			ch = 0;
		}
		if (ch == 127) // ^BS
		{
			if (xpos > 0)
			{
				strcpy(undo_stack, str_save[ypos]);
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
					for (int i = 0; i < xpos - bs_tk; ++i)
					{
						printf("\b \b");
					}
					xpos = bs_tk;
				}
				else
				{

					// xpos 20
					// token 15

					bs_tk = tokback_pos(str_save[ypos], "()[]{}\t ", "?!");
					for (int i = 0; i < (xpos - bs_tk); i++)
					{
						temp_strsave = delete_char_left(str_save[ypos], bs_tk);
						strcpy(str_save[ypos], temp_strsave);
					}

					ClearLine(ypos);
					gotoxy(0, ypos);
					printf("%s", str_save[ypos]);
					MakePause();
				}
			}

			ch = 0;
		}

		/* if (ch == 25) // ^Y = Redo
		{
			if (undo_stack[0] != 0)
			{
				if (strcmp(undo_stack, redo_stack) != 0)
				{
					strcpy(str_save[undo_stack_line], redo_stack);
					LoadAllNewtrodit();
					DisplayFileContent(str_save, newlinestring, stdout);				}
			}
			ch = 0;
		}
 		*/
		if (ch == 26) // ^Z = Undo
		{
			if (undo_stack[0] != 0)
			{

				strcpy(str_save[undo_stack_line], undo_stack);
				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
				EmptyString(undo_stack);
			}
			else
			{
				strcpy(undo_stack, str_save[undo_stack_line]);
				LoadAllNewtrodit();
				DisplayFileContent(str_save, newlinestring, stdout);
			}
			ch = 0;
		}

		if (ch == 8 && CheckKey(0x08)) // BS key
		{
			c = -8; // Negative to avoid conflict
			if (xpos > 0)
			{

				if (str_save[ypos][xpos - 1] == 9) // TAB key
				{
					relative_xpos[ypos] -= TAB_WIDE;
					gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
				}

				if (str_save[ypos][xpos] != 0)
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

					strcpy(str_save[ypos], insert_str);
					ClearLine(ypos);
					gotoxy(0, ypos);
					printf("%s", insert_str);
					xpos -= 2;
				}
				else
				{
					str_save[ypos][xpos - 1] = '\0';
					printf("\b \b");
					xpos -= 2;
				}
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
				}
				xpos--;
			}
		}
		else

		{
			if (ch > 31 || ch == 9 && CheckKey(0x09)) // Printable character
			{
				if (isModified == false)
				{
					isModified = true;
				}

				if (insertChar == 0) // Insert key not pressed
				{

					if (str_save[ypos][xpos] != '\0')
					{
						if (ch == 9)
						{
							relative_xpos[ypos] += TAB_WIDE;
						}
						insert_str = insert_char(str_save[ypos], ch, xpos);
						strcpy(str_save[ypos], insert_str);
						gotoxy(0, ypos);
						printf("%s", str_save[ypos]);

						ch = 0;
					}
				}

				if (CheckKey(0x09) && ch == 9) // TAB key
				{
					if (convertTabtoSpaces == true)
					{
						for (int i = 0; i < TAB_WIDE; i++) // i is also character 9 :)
						{
							str_save[ypos][xpos] = ' ';
						}
						xpos += TAB_WIDE;
					}
					else
					{
						str_save[ypos][xpos] = 9;
						PrintTab(TAB_WIDE);
						relative_xpos[ypos] += TAB_WIDE;
						gotoxy(xpos + relative_xpos[ypos] + LINECOUNT_WIDE, ypos + relative_ypos[xpos]);
					}
				}
				else
				{
					if (ch != 0)
					{
						putchar(ch);
						str_save[ypos][xpos] = (int)ch;
					}
				}
			}
			else
			{
				if (ch != 0 && ch <= 26)
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

		if ((strlen(str_save[ypos]) > BUFFER_X || ypos > BUFFER_Y) && (ch != 8))
		{
			PrintBottomString(NEWTRODIT_ERROR_CON_BUFFER_LIMIT);
			MakePause();
			ShowBottomMenu();
			if (ch != 8)
			{
				xpos = 0;
				ypos = 1;
			}
			if (xpos >= 6615) // This is the maximum length that can be stored in the buffer without crashing Newtrodit
			{
				NewtroditCrash(NEWTRODIT_ERROR_BUFFER_OVERFLOW, 105);
			}
		}
	}

	SetColor(start_color);
	return 0;
}
