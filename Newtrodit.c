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

int line = 0;
int col = 0;

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

int LoadSettings(char *newtrodit_config_file)
{
	/*
		Brief explanation of the format:

		Different settings have different names:

		CP (Default code page)
		XS (Console size in columns)
		YS (Console size in lines)

	*/

	FILE *settings = fopen(newtrodit_config_file, "rb");
	if (!settings)
	{
		return errno;
	}

	unsigned char buf_settings[20];
	unsigned char setting_code[5];
	while (fgets(buf_settings, sizeof buf_settings, settings))
	{
		strncpy(setting_code, buf_settings, 2);
		if (!strcmp(setting_code, "CP"))
		{
			SetConsoleOutputCP(buf_settings[2] + (256 * buf_settings[3]));
		}
		if (!strcmp(setting_code, "XS"))
		{
			SetConsoleSize(buf_settings[2] + (256 * buf_settings[3]), GetConsoleYDimension());
		}
		if (!strcmp(setting_code, "YS"))
		{
			SetConsoleSize(GetConsoleXDimension(), buf_settings[2] + (256 * buf_settings[3]));
		}
	}
	fclose(settings);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sigint_handler);
	LoadSettings(settings_file);

	if (XSIZE < 54 || YSIZE < 6) // Check for console size
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}

	unsigned short dev_tools = true;				   // Bool to enable or disable the dev tools
	unsigned short isSaved = false;					   // Bool to check if file is saved
	unsigned short insertChar = false;				   // Bool to check if replace instead of insert
	unsigned short isFullScreen = false;			   // Bool to check if full screen
	unsigned short stringFound = false;				   // Bool to check if string was found
	unsigned int insert_temp, delete_temp;			   // Temp variables for insert and delete functions
	unsigned int codepage_save = GetConsoleOutputCP(); // Get codepage
	char **str_save = calloc(BUFFER_Y, BUFFER_X);	   // Allocate 6144 char pointers
	for (int i = 0; i < BUFFER_X; ++i)
	{
		str_save[i] = calloc(BUFFER_Y, BUFFER_Y); // Allocate 2048 bytes for each string
	}
	if (str_save[0] == NULL) // If calloc() failed, return an error and quit the program
	{
		printf("%s", NEWTRODIT_ERROR_CALLOC_FAILED);
		return 0;
	}
	// Declare variables
	int shift_virtual_key, alt_virtual_key;
	int old_x_size, old_y_size;
	int bs_tk;

	int start_color = GetColor();
	int lines_paste = 0;

	char save_destination[FILENAME_MAX];

	char find_string[512];
	int find_string_index;
	char *basenameopen;
	char *final_paste_strrchr;
	char fileopenread[FILENAME_MAX], bottom_string[FILENAME_MAX];

	char newlinestring[10] = "\n";
	// File variables
	FILE *fileread;
	FILE *fp_savefile;
	FILE *newtrodit_open_argv;

	int xpos = 0, ypos = 0, relative_xpos[700] = {0}, relative_ypos[200] = {0};

	int c_open, old_c_open;
	int c;
	// getch() variables
	int ch, save_ch;
	if (argc > 1)
	{
		if (!strcmp(argv[1], "--man")) // Manual parameter
		{
			NewtroditHelp();
			SetColor(start_color);
			ClearScreen();
			exit(GetLastError());
		}
		if (!strcmp(argv[1], "--help")) // Help menu parameter
		{
			printf("Usage: newtrodit [file] [switches]\n\n");
			printf("See 'newtrodit --man' for a complete manual\n");
			return 0;
		}
	}

	SetColor(0x07);
	int argc_shift = argc;
	while (argc_shift > 1)
	{
		if (!strcmp(argv[argc_shift - 1], "--utf8")) // File will be opened as UTF-8
		{
			setlocale(LC_ALL, "");
		}
		if (!strcmp(argv[argc_shift - 1], "--lfunix")) // Use UNIX new line
		{
			newlinestring[0] = '\n';
		}
		if (!strcmp(argv[argc_shift - 1], "--lfwin")) // Use Windows new line
		{
			strcpy(newlinestring, "\r\n");
		}
		if (!strcmp(argv[argc_shift - 1], "--lfmac")) // Use Mac new line
		{
			newlinestring[0] = '\r';
		}
		if (!strcmp(argv[argc_shift - 1], "--ws")) // Write settings to file
		{
			if (argv[argc_shift] == NULL)
			{
				fprintf(stderr, NEWTRODIT_ERROR_ARGUMENT_WS);
				return 1;
			}
		}
		argc_shift--;
	}

	int open_argv_x = 0, open_argv_y = 1;

	if (argc > 1)
	{
		strcpy(filename_text, argv[1]);
		newtrodit_open_argv = fopen(filename_text, "rb");
		if (!newtrodit_open_argv)
		{

			printf("%s: %s", filename_text, strerror(errno));
			exit(errno);
		}
		LoadAllNewtrodit();
		gotoxy(0, 1);

		while ((c_open = getc(newtrodit_open_argv)) != EOF)
		{

			if (c_open == 10)
			{
				open_argv_x = 0;
				str_save[open_argv_y][open_argv_x] = '\n';
				putchar('\n');
				gotoxy(open_argv_x, ++open_argv_y);
			}

			else
			{
				str_save[open_argv_y][open_argv_x++] = c_open;
				if (str_save[open_argv_y][open_argv_x] == 9)
				{
					relative_xpos[open_argv_y] += TAB_WIDE;
					PrintTab(TAB_WIDE);
				}
				else
				{
					putchar(c_open);
				}
			}
		}
		CenterFileName(filename_text);
	}
	LoadAllNewtrodit();
	gotoxy(0, ypos++);
	while (1)
	{

		gotoxy(0, GetConsoleYDimension() - 1);
		for (int i = 0; i < strlen(NEWTRODIT_DIALOG_BASIC_SHORTCUTS); ++i) // Clear first characters of the last line
		{
			putchar(' ');
		}
		gotoxy(0, GetConsoleYDimension() - 1);
		printf("Ln %d, Col %d", ypos, xpos + 1);						// Add one to xpos because it's zero indexed
		gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]); // Relative position is for tab key
		old_x_size = XSIZE;
		old_y_size = YSIZE;

		ch = getch(); // Get key pressed

		if (ch == 15) // ^O
		{

			gotoxy(1, YSIZE - 1);
			ClearLine(YSIZE - 1);
			PrintBottomString(NEWTRODIT_PROMPT_FOPEN);
			fgets(fileopenread, sizeof fileopenread, stdin);
			gotoxy(0, -1);

			ClearLine(YSIZE - 1);
			// Remove trailing LF
			fileopenread[strcspn(fileopenread, "\n")] = 0;
			fileread = fopen(fileopenread, "r");

			if (!fileread) // Failed to open the file
			{
				gotoxy(0, YSIZE - 2);
				printf("%s: %s", fileopenread, strerror(errno));
				getch();
				NewtroditNameLoad();
				CenterFileName(filename_text);

				ShowBottomMenu();
				ClearLine(YSIZE - 3);
				gotoxy(xpos, ypos);
				SetColor(0x07);
				continue;
			}
			ClearScreen();
			strcpy(filename_text, fileopenread);
			ClearLine(0);
			NewtroditNameLoad();
			CenterFileName(filename_text);

			xpos = 0, ypos = 1;
			gotoxy(xpos, ypos);
			while ((c = getc(fileread)) != EOF)
			{
				if (c == 13 || c == 10)
				{
					xpos = 0;
					putchar('\n');
					gotoxy(xpos, ++ypos);
				}
				else
				{
					str_save[ypos][xpos++] = c;
					if (str_save[ypos][xpos] == 9)
					{
						relative_xpos[ypos] += TAB_WIDE;
						PrintTab(TAB_WIDE);
					}
					else
					{
						putchar(c);
					}
				}
			}
			gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]);
			ShowBottomMenu();
			fclose(fileread);
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
			continue;
		}

		if (ch == 7) // ^G = Goto line
		{
			ClearLine(YSIZE - 1);
			PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);
			char line_number_str[6];
			int line_number_chr = 0, line_number_chr_index = 0;

			while (line_number_chr != 13) // Loop while enter isn't pressed
			{
				line_number_chr = getch();
				if (line_number_chr == 27)
				{
					ShowBottomMenu();
					gotoxy(xpos, ypos);
					continue;
				}
				if (line_number_chr >= 48 && line_number_chr <= 57) // Check if character is a number
				{
					if (atoi(line_number_str) >= 10000) // If line number is bigger than 10000, alert
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
						BS();
					}
				}
				if (line_number_chr == YSIZE - 3)
				{
					ShowBottomMenu();
					gotoxy(xpos, ypos);
					break;
				}
			}
			if (atoi(line_number_str) < 1) // Line is less than 1
			{
				ClearLine(GetConsoleYDimension() - 1);
				PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
				getch();
				ShowBottomMenu();
				continue;
			}
			if (atoi(line_number_str) > GetConsoleYDimension() - 3) // Line too big for the console buffer
			{
				ClearLine(GetConsoleYDimension() - 1);
				PrintBottomString(NEWTRODIT_ERROR_GOTO_BUFFER_SIZE);
				getch();
				ShowBottomMenu();
			}
			else
			{
				gotoxy(0, atoi(line_number_str));
				xpos = 0;
				ypos = atoi(line_number_str);
				ShowBottomMenu();
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
					if (str_save[ypos - 1][ypos] == NULL)
					{
						xpos = nolflen(str_save[--ypos]);
						gotoxy(xpos, ypos);
					}
					else
					{
						gotoxy(xpos, --ypos);
					}
				}
				break;
			case 75:
				// Left arrow

				if (xpos >= 1)
				{
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
				if (str_save[ypos][xpos] != NULL)
				{
					if (xpos == strlen(str_save[ypos]))
					{
						xpos = 0;
						ypos++;
					}
					else
					{
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

					xpos = nolflen(str_save[++ypos]);
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
				for (int i = 1; i < YSIZE; i++)
				{
					// Go to the end of the file
					if (str_save[i][0] != NULL)
					{
						xpos = nolflen(str_save[i]);
						ypos = i;
						break;
					}
				}
				//gotoxy(xpos, ypos);
				break;
			case 82:
				// INS key
				if (insertChar == false)
				{
					insertChar = true;
				}
				else
				{
					insertChar = false;
				}

				break;

			case 83:
				// DEL key

				shift_left(str_save[ypos], xpos + 1);
				gotoxy(0, ypos);

				printf("%s ", str_save[ypos]);
				break;

			case 134:
				if (dev_tools == 1)
				{
					printf("%d:%d\t%d:%d", XSIZE, YSIZE, old_x_size, old_y_size);
				}

				break;
			}

			ch = 0;
			continue;
		}

		if (ch == 13) // Newline character: CR (13)
		{

			if (!(ypos < YSIZE - 3))
			{
				Alert();
				continue;
			}
			if (!strcmp(newlinestring, "\r\n"))
			{
				str_save[ypos][xpos] = '\r';
				str_save[ypos][xpos + 1] = '\n';
			}
			else
			{
				str_save[ypos][xpos] = '\n';
			}
			xpos = 0;
			ypos++;
			continue;
		}

		if (ch == 6) // ^F = Find string
		{
			ClearLine(GetConsoleYDimension() - 1);
			PrintBottomString(NEWTRODIT_PROMPT_FIND_STRING);
			fgets(find_string, sizeof find_string, stdin);
			find_string[strlen(find_string) - 1] = '\0';
			LoadAllNewtrodit();
			gotoxy(0, 1);
			for (int i = 1; i < YSIZE; i++)
			{
				if (str_save[i][0] != NULL)
				{
					if (str_save[i][0] != 13 && str_save[i][0] != 10)
					{
						fprintf(stdout, "%s\n", str_save[i]);
					}
					else
					{
						putc('\n', stdout);
					}
				}
			}
			for (int i = 1; i < YSIZE - 1; i++)
			{

				find_string_index = FindString(str_save[i], find_string);

				if (find_string_index >= 0)
				{
					gotoxy(find_string_index, i);
					SetColor(0x0e);
					printf("%s", find_string);
					SetColor(0x07);
					ShowFindMenu();
					c = getch();
					if(getch() == 0 && getch() == 61)
					{
						printf("Feature not available yet");
						getche();
					}
					break;
				}
			}
			/*
			if (strstr(str, findstring) != NULL)
			{
				find_substring = strstr(str, findstring);
				find_substring_index = find_substring - str;
				find_substring_len = strlen(str) - find_substring_index;
				gotoxy(0, ypos);
				gotoxy(find_substring_index, 2);
				SetColor(0x0e);
				printf("yellow for find ^F");
			}
			*/
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 107) // M-F4 key
			{
				QuitProgram(start_color);
				SetColor(0x07);
				ch = 0;
			}
			if (ch == 59) // F1 key
			{
				ch = 0;
				NewtroditHelp();
				LoadAllNewtrodit();
				gotoxy(0, 1);
				// Print again the text already written on the screen
				for (int i = 1; i < YSIZE; i++)
				{
					if (str_save[i][0] != NULL)
					{
						if (str_save[i][0] != 13 && str_save[i][0] != 10)
						{
							for (int k = 0; k < strlen(str_save[i]); ++k)
							{
								// Check if it's a a tab
								if (str_save[i][k] == 9)
								{
									relative_xpos[i] += TAB_WIDE;
								}
							}
							fprintf(stdout, "%s\n", str_save[i]);
						}
						else
						{
							putc('\n', stdout);
						}
					}
				}
				gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]);
			}
			if (ch >= 60 && ch <= 67) // F2 to F9 keys
			{
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
			ch = 0;
		}

		if (ch == 19) // ^S
		{
			if (CheckKey(VK_LSHIFT))
			{
				isSaved = 2;
			}

			if (isSaved == 0 || isSaved == 2)
			{

				ClearLine(GetConsoleYDimension() - 1);
				if (isSaved == 0)
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE);
				}
				else
				{
					PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE_AS);
				}
				isSaved = 1;
				fgets(save_destination, sizeof save_destination, stdin);
				gotoxy(0, 0);
				save_destination[strcspn(save_destination, "\n")] = 0;
				if (save_destination[0] < 32 || save_destination[0] > 126)
				{
					PrintBottomString(join(NEWTRODIT_ERROR_INVALID_XPOS, save_destination));
				}
				fp_savefile = fopen(save_destination, "rb");

				if (CheckFile(save_destination) == 0)
				{
					PrintBottomString(NEWTRODIT_PROMPT_OVERWRITE);
					save_ch = getch();
					ShowBottomMenu();
					if (toupper(save_ch) != 'Y')
					{
						fclose(fp_savefile);
						continue;
					}
				}
				fclose(fp_savefile);
				fp_savefile = fopen(save_destination, "wb");
				if (!fp_savefile)
				{
					ClearLine(GetConsoleYDimension() - 1);
					PrintBottomString(strerror(errno));
					continue;
				}
				//fclose(fp_savefile);
			}

			int save_index_x = 0, save_index_y = 1; // Save index
			ClearScreen();
			strcpy(filename_text, fileopenread);
			SetColor(0x70);
			ClearLine(0);
			NewtroditNameLoad();
			SetColor(0x07);
			CenterFileName(strlasttok(save_destination, '\\'));

			gotoxy(0, 1);

			for (int i = 1; i < YSIZE; i++)
			{
				if (str_save[i][0] != NULL)
				{
					if (str_save[i][0] != 13 && str_save[i][0] != 10)
					{
						fprintf(stdout, "%s%s", str_save[i], newlinestring);
						fprintf(fp_savefile, "%s%s", str_save[i], newlinestring);
					}
					else
					{
						fprintf(stdout, "%s", newlinestring);
						fprintf(fp_savefile, "%s", newlinestring);
					}
				}
			}
			ShowBottomMenu();
			ClearLine(YSIZE - 1);
			PrintBottomString(NEWTRODIT_FILE_SAVED);
			getch();
			ShowBottomMenu();
			fclose(fp_savefile);
		}
		if (ch == 22) // ^V
		{
			char *buffer_clipboard;
			if (OpenClipboard(0))

			{
				buffer_clipboard = (char *)GetClipboardData(CF_TEXT);
				if (buffer_clipboard != NULL)
				{
					printf("%s", buffer_clipboard);
					final_paste_strrchr = strlasttok(buffer_clipboard, '\\');
					lines_paste = 0;
					xpos += strlen(final_paste_strrchr + 1);
					if (xpos > BUFFER_X || ypos > BUFFER_Y)
					{
						ClearLine(YSIZE - 1);
						PrintBottomString(NEWTRODIT_ERROR_BUFFER_OVERFLOW);
						while (ch != 0x27) // Wait for ESC
						{
							ch = getch();
						}
						PrintBottomString(NEWTRODIT_WARNING_UNEXPECTED_ERRORS);
					}
				}
				else
				{
					PrintBottomString(NEWTRODIT_ERROR_CLIPBOARD_COPY);
				}
			}
			CloseClipboard();
		}
		if (ch == 23 && dev_tools == 1) // ^W (Debug tool)
		{
			printf("\"%s\"", str_save[ypos]);
			getch();
			for(int i = 0; i < strlen(str_save[ypos])+2; i++)
			{
				BS();
			}
		}

		if (ch == 24) // ^X = Quit program
		{
			QuitProgram(start_color);
			SetColor(0x07);
		}

		if (old_x_size != XSIZE || old_y_size != YSIZE) // Check if size has been modified
		{
			LoadAllNewtrodit();
			gotoxy(0, 1);
			for (int i = 1; i < YSIZE; i++)
			{
				if (str_save[i][0] != NULL)
				{
					if (str_save[i][0] != 13 && str_save[i][0] != 10)
					{
						fprintf(stdout, "%s\n", str_save[i]);
					}
					else
					{
						putc('\n', stdout);
					}
				}
			}
		}
		if (ch == 127) // ^BS
		{
			if (str_save[ypos][xpos] == NULL && xpos > 0)
			{
				bs_tk = tokback_pos(str_save[ypos], ' ');
				if (bs_tk != -1)
				{
					str_save[ypos][bs_tk] = 0;
					for (int i = strlen(str_save[ypos]) - 1; i > bs_tk; i--)
					{
						str_save[ypos][i] = '\0';
					}
					for (int i = 0; i < xpos - bs_tk; ++i)
					{
						BS();
					}

					xpos = bs_tk;
				}
			}
			ch = 0;
		}
		if (ch == 8) // BS key
		{
			if (xpos > 0)
			{
				if (str_save[ypos][xpos - 1] == 0x09) // TAB key
				{
					relative_xpos[ypos] -= TAB_WIDE;
					gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]);
				}
				if (str_save[ypos][xpos] != 0)
				{
					shift_left(str_save[ypos], xpos);
					gotoxy(0, ypos);

					printf("%s ", str_save[ypos]);
				}
				else
				{
					str_save[ypos][xpos] = 0;
					printf("\b \b");
				}

				xpos -= 2;
			}
			else
			{
				/* Act as END key */
				if (ypos > 1)
				{

					str_save[ypos][xpos - 1] = '\0';

					xpos = nolflen(str_save[ypos - 1]);
					if (str_save[ypos - 1][0] == 0x0A)
					{
						xpos = 0;
					}

					gotoxy(xpos, --ypos);
				}
				xpos--;
			}
		}
		else

		{
			if (ch > 31 || ch == 9)
			{
				if (insertChar == 0) // Insert key not pressed
				{

					if (str_save[ypos][xpos + 1] != NULL)
					{

						shift_right(ch, str_save[ypos], xpos);
						gotoxy(0, ypos);
						printf("%s", str_save[ypos]);

						ch = 0;
					}
				}

				if (CheckKey(0x09) && ch == 9) // TAB key
				{
					str_save[ypos][xpos] = 9;
					PrintTab(TAB_WIDE);
					relative_xpos[ypos] += TAB_WIDE;
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
				xpos--;
			}
		}
		xpos++;
	}

	SetColor(start_color);
	return 0;
}
