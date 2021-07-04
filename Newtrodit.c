/*
  Copyright 2020 anic17 Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of 
  this software and associated documentation files (the "Software"), to deal in 
  the Software without restriction, including without limitation the rights to 
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
  the Software, and to permit persons to whom the Software is furnished to do so, 
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all 
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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

int line = 0;
int col = 0;

struct SMALL_RECT
{
	SHORT left;
	SHORT top;
	SHORT right;
	SHORT bottom;
};

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
		return EXIT_FAILURE;
	}

	unsigned char buf_settings[20];
	unsigned char setting_code[5];
	while (fgets(buf_settings, sizeof buf_settings, settings))
	{
		setting_code[0] = buf_settings[0];
		strncpy(setting_code, buf_settings, 3);

		if (setting_code[0] == 'C' && setting_code[1] == 'P')
		{
			SetConsoleOutputCP(buf_settings[2] + (256 * buf_settings[3]));
		}
		if (setting_code[0] == 'X' && setting_code[1] == 'S')
		{
			SetConsoleSize(buf_settings[2] + (256 * buf_settings[3]), GetConsoleYDimension());
		}
		if (setting_code[0] == 'Y' && setting_code[1] == 'S')
		{
			SetConsoleSize(GetConsoleXDimension(), buf_settings[2] + (256 * buf_settings[3]));
		}
	}
}

int main(int argc, char *argv[])
{
	LoadSettings(settings_file);
	// Check for console size
	if (XSIZE < 54 || YSIZE < 6)
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}

	unsigned short isSaved = false;
	unsigned int codepage_save = GetConsoleOutputCP();
	char **str_save = calloc(BUFFER_Y, BUFFER_X); // allocates 6144 char pointers
	for (int i = 0; i < BUFFER_X; ++i)
	{
		str_save[i] = calloc(BUFFER_Y, BUFFER_Y); // allocates 2048 bytes for each string
	}
	if (str_save[0] == NULL)
	{
		printf("%s", NEWTRODIT_ERROR_CALLOC_FAILED);
		return 0;
	}
	int shift_virtual_key, alt_virtual_key;
	int old_x_size, old_y_size;

	int edit_file = 0;
	int start_color = GetColor();
	int lines_paste = 0;
	size_t end_key_len;
	int biggest_y_line = 1;
	char save_destination[MAX_FILENAME];
	int show_cursor_pos_window_size, save_file_dialog;
	char *find_substring;
	int find_substring_index;
	int find_substring_len;
	char *basenameopen;
	char *final_paste_strrchr;
	char fileopenread[512], bottom_string[512];
	//char findstring[8192];
	
	int xpos = 0, ypos = 0, relative_xpos[600] = {0}, relative_ypos[100] = {0};
	gotoxy(xpos, ypos);

	gotoxy(xpos, ypos++);
	int c_open;
	int c;
	int ch;

	char newlinestring[10] = "\n";

	SetColor(0x07);
	int argc_shift = argc;

	/*if (argc_shift > 1)
	{
		if (!strcmp(argv[argc_shift], "--help"))
		{
			NewtroditHelp();
			getch();
			SetColor(start_color);
			exit(0);
		}
		if (!strcmp(argv[argc_shift], "--utf8"))
		{
			setlocale(LC_ALL, "");
		}
		if (!strcmp(argv[argc_shift], "--lfunix"))
		{
			newlinestring[0] = '\n';
		}
		if (!strcmp(argv[argc_shift], "--lfwin"))
		{
			strcpy(newlinestring, "\r\n");
		}
		if (!strcmp(argv[argc_shift], "--lfmac"))
		{
			newlinestring[0] = '\r';
		}
		argc_shift--;
	}*/

	int open_argv_x = 0, open_argv_y = 1;

	if (argc > 1)
	{
		gotoxy(0, 0);
		LoadAllNewtrodit();
		gotoxy(0, 1);
		FILE *newtrodit_open_argv;
		strcpy(filename_text, argv[1]);
		newtrodit_open_argv = fopen(filename_text, "rb");
		if (!newtrodit_open_argv)
		{
			printf("%s: %s", filename_text, strerror(errno));
			exit(EXIT_FAILURE);
		}
		while ((c_open = getc(newtrodit_open_argv)) != EOF)
		{
			str_save[open_argv_y][open_argv_x] = c_open;
			if (str_save[open_argv_y][open_argv_x] == 9)
			{
				relative_xpos[open_argv_y] += 4;
			}
			if (c_open == 10 || c_open == 13)
			{
				open_argv_y++;
				open_argv_x = 0;
				str_save[open_argv_y][open_argv_x] = '\n';
			}
			putchar(str_save[open_argv_y][open_argv_x]);
			open_argv_x++;
		}
	}
	gotoxy(0, 0);
	ClearScreen();
	LoadAllNewtrodit();
	while (1)
	{

		gotoxy(0, GetConsoleYDimension() - 1);
		for (int i = 0; i < 17; ++i)
		{
			putchar(' ');
		}
		gotoxy(0, GetConsoleYDimension() - 1);
		printf("Ln %d, Col %d", ypos, xpos + 1);
		gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]);
		old_x_size = XSIZE;
		old_y_size = YSIZE;
		ch = getch();
		if (old_x_size != XSIZE || old_y_size != YSIZE)
		{
			LoadAllNewtrodit();
		}

		/*if(ch == 14) // ^N
	{
		if()
	}*/

		if (ch == 15) // ^O
		{

			FILE *fileread;

			gotoxy(1, YSIZE - 1);
			ClearLine(YSIZE - 1);
			PrintBottomString(NEWTRODIT_PROMPT_FOPEN);
			fgets(fileopenread, sizeof fileopenread, stdin);
			// Remove trailing LF
			fileopenread[strcspn(fileopenread, "\n")] = 0;
			fileread = fopen(fileopenread, "r");

			if (!fileread)
			{
				sprintf(bottom_string, "%s: %s", fileopenread, strerror(errno));
				PrintBottomString(bottom_string);
				getch();
				ShowBottomMenu();
				gotoxy(xpos, ypos);
				SetColor(0x07);
			}
			ClearScreen();
			strcpy(filename_text, fileopenread);
			SetColor(0x70);
			ClearLine(0);
			NewtroditNameLoad();
			CenterFileName(filename_text);
			SetColor(0x07);

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
					if (str_save[ypos][xpos] == 9)
					{
						relative_xpos[ypos] += 4;
					}
					str_save[ypos][xpos++] = c;
					putchar(c);
				}
			}
			gotoxy(xpos+relative_xpos[ypos], ypos+relative_ypos[xpos]);
			ShowBottomMenu();
		}

		if (ch == 3) // ^C = Copy string to clipboard
		{
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(str_save[ypos]) + 1);
			memcpy(GlobalLock(hMem), str_save[ypos], strlen(str_save[ypos]) + 1);
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
			continue;
		}

		if (ch == 7) // ^G = Goto line
		{

			int console_cols_gotoxy = GetConsoleXDimension();
			gotoxy(0, GetConsoleYDimension() - 1);
			for (int i = 0; i < console_cols_gotoxy - 1; ++i)
			{
				putchar(' ');
			}

			gotoxy(0, GetConsoleYDimension() - 1);
			printf("%s", NEWTRODIT_PROMPT_GOTO_LINE);
			char line_number_str[6];
			int line_number_chr = 0, line_number_chr_index = 0;

			while (line_number_chr != 13)
			{
				line_number_chr = getch();
				if (line_number_chr > 47 && line_number_chr < 58)
				{
					if (atoi(line_number_str) >= 10000)
					{
						Alert();
					}
					else
					{
						line_number_str[line_number_chr_index++] = line_number_chr;
						putchar(line_number_chr);
					}
				}
				if (line_number_chr == 8)
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
			if (atoi(line_number_str) > GetConsoleYDimension() - 3)
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
		if (ch == 0xE0)
		{
			ch = getch();
			switch (ch)
			{

			case 72:

				// Up arrow
				if (ypos > 1)
				{
					gotoxy(xpos, --ypos);
				}
				break;
			case 75:
				// Left arrow
				if (xpos >= 1)
				{
					gotoxy(--xpos, ypos);
				}
				break;
			case 77:
				// Right arrow
				gotoxy(++xpos, ypos);
				break;

			case 80:
				// Down arrow
				if ((ypos + 1) >= YSIZE - 3)
				{
					Alert();
				}
				else
				{
					gotoxy(xpos, ++ypos);
				}

				break;

			case 71:
				// HOME key
				xpos = 0;
				gotoxy(xpos, ypos);
				break;
			case 79:
				// END key
				end_key_len = strlen(str_save[ypos]);
				xpos = end_key_len;
				gotoxy(end_key_len, ypos);
				break;
			case 83:
				// DEL key
				break;
			}

			continue;
			ch = 0;
		}

		if (ch == 13) // Newline character: CR
		{
			xpos = 0;
			ypos++;
			str_save[ypos][xpos] = '\n';
			printf("\n");
			if (biggest_y_line < ypos)
			{
				biggest_y_line++;
			}
			continue;
		}

		if (ch == 6) // ^F = Find string
		{
			ClearLine(GetConsoleYDimension() - 1);
			PrintBottomString("Find string function unavailable at the moment.");
			getch();
			ClearLine(GetConsoleYDimension() - 1);
			ShowBottomMenu();
			continue;

			gotoxy(1, YSIZE);
			printf("%s", NEWTRODIT_PROMPT_FIND_STRING);
			//fgets(findstring, sizeof findstring, stdin);

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
			// _! continue;
		}
		if (ch == 24) // ^X = Quit program
		{
			QuitProgram(start_color);
			SetColor(0x07);
			// _! continue;
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 107)
			{
				QuitProgram(start_color);
				SetColor(0x07);
			}
			if (ch == 59)
			{
				NewtroditHelp();
				getch();
			}
			if (ch == 93)
			{
				StartProcess("cmd.exe");
				ch = 0;
			}
			if (ch == 68)
			{
				StartProcess("explorer.exe .");
				ch = 0;
			}
		}
		if (ch == 19) // ^S
		{
			FILE *fp_savefile;
			if (isSaved == 0)
			{
				ClearLine(GetConsoleYDimension() - 1);
				PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE);

				fgets(save_destination, sizeof save_destination, stdin);
				size_t len_savedest = strlen(save_destination);
				save_destination[len_savedest - 1] = '\0';
				fp_savefile = fopen(save_destination, "wb");

				if (!fp_savefile)
				{
					ClearLine(GetConsoleYDimension() - 1);
					PrintBottomString(strerror(errno));
					continue;
				}
				isSaved = 1;
			}

			int save_index_x = 0, save_index_y = 1; // Save index

			ClearScreen();
			strcpy(filename_text, fileopenread);
			SetColor(0x70);
			ClearLine(0);
			NewtroditNameLoad();
			CenterFileName(strlasttok(save_destination, '\\'));

			SetColor(0x07);

			ShowBottomMenu();
			gotoxy(0, 1);

			for (int i = 1; i < YSIZE; i++)
			{
				if (str_save[i][0] != NULL)
				{
					printf("i=%d", i);
					if (str_save[i][0] != 13 && str_save[i][0] != 10)
					{
						fprintf(stdout, "%s\n", str_save[i]);
						fprintf(fp_savefile, "%s\n", str_save[i]);
					}
					else
					{
						putc('\n', stdout);
						putc('\n', fp_savefile);
					}
				}
			}
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
				}
				else
				{
					PrintBottomString(NEWTRODIT_ERROR_CLIPBOARD_COPY);
				}
			}
			CloseClipboard();
		}
		if (ch == 27) // ESC
		{
			printf("\"%s\"", str_save[ypos]);
		}

		if (ch == 8) // BS key
		{
			if (xpos > 0)
			{
				if (str_save[ypos][xpos - 1] == 9)
				{
					relative_xpos[ypos] -= 4;
					gotoxy(xpos + relative_xpos[ypos], ypos + relative_ypos[xpos]);
				}
				str_save[ypos][xpos + 1] = '\0';
				printf("\b \b");
				xpos -= 2;
			}
			else
			{
				/* Act as END key */
				if (ypos > 1)
				{

					str_save[ypos][xpos - 1] = '\0';
					end_key_len = strlen(str_save[ypos - 1]);
					xpos = end_key_len;
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

				if (CheckKey(0x09) && ch == 9) // TAB key
				{
					str_save[ypos][xpos] = 9;
					printf("    ");
					relative_xpos[ypos] += 4;
				}
				else
				{
					putchar(ch);
					str_save[ypos][xpos] = (int)ch;
				}
			}
			else
			{
				xpos--;
			}
		}
		xpos++;
		if (str_save[ypos][0] == '\0' && str_save[ypos][xpos] != '\0')
		{
			str_save[ypos][0] = 0x20;
		}
	}

	SetColor(start_color);
	return 0;
}