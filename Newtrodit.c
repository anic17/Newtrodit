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

int main(int argc, char *argv[])
{

	bool isSaved = false;
	int codepage_save = GetConsoleOutputCP();
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
	char fileopenread[512];
	char findstring[8192];
	int xpos = 0, ypos = 0;

	gotoxy(xpos, ypos);
	LoadAllNewtrodit();
	gotoxy(xpos, ypos++);
	char c_open;
	int c;
	int ch;

	char newlinestring[10] = "\n";

	SetColor(0x07);

	if (XSIZE < 54 || YSIZE < 6)
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}

	int argc_shift = argc;
	/*
	if (argc_shift > 1)
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
	}
	*/
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
	gotoxy(0,0);
	ClearScreen();
	LoadAllNewtrodit();
	while (1)
	{
		gotoxy(0, GetConsoleYDimension() - 1);
		for (int i = 0; i < 17; ++i)
		{
			putchar(' ');
		}
		//shift_virtual_key = GetKeyState(VK_SHIFT);
		//printf("%d\n\n", shift_virtual_key);
		gotoxy(0, GetConsoleYDimension() - 1);
		printf("Ln %d, Col %d", ypos, xpos + 1);
		gotoxy(xpos, ypos);
		old_x_size = XSIZE;
		old_y_size = YSIZE;
		ch = getch();
		if (old_x_size != XSIZE || old_y_size != YSIZE)
		{
			LoadAllNewtrodit();
		}

		if (ch == 15)
		{
			FILE *fileread;

			gotoxy(1, GetConsoleYDimension() - 1);
			SetColor(0x70);

			printf("%s", NEWTRODIT_PROMPT_FOPEN);
			fgets(fileopenread, sizeof fileopenread, stdin);

			fileread = fopen(fileopenread, "r");
			if (!fileread)
			{
				gotoxy(1, YSIZE - 1);
				printf("%s: %s", fileopenread, strerror(errno));
				getch();
				ShowBottomMenu();
				gotoxy(xpos, ypos);
				SetColor(0x07);
			}
			else
			{
				printf("fopen succeeded\n");
			}

			strcpy(filename_text, fileopenread);
			CenterFileName(filename_text);
			SetColor(0x07);

			xpos = 0, ypos = 1;
			gotoxy(xpos, ypos);
			c = getc(fileread);
			putchar(c);
			while ((c = getc(fileread)) != EOF)
			{

				//  || (getc(fread)) != 10
				if (c == 13)
				{
					putchar('\r');
					gotoxy(xpos, ++ypos);
				}
				{
					putchar(c);
				}
			}
		}

		if (ch == 3) // ^C
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
				xpos = -1;
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

		if (ch == 13)
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

		if (ch == 6) // ^F
		{
			ClearLine(GetConsoleYDimension() - 1);
			PrintBottomString("Find string function unavailable at the moment.");
			getch();
			ClearLine(GetConsoleYDimension() - 1);
			ShowBottomMenu();
			continue;

			gotoxy(1, YSIZE);
			printf("%s", NEWTRODIT_PROMPT_FIND_STRING);
			fgets(findstring, sizeof findstring, stdin);

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
		if (ch == 24)
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
		}
		if (ch == 19) // ^S
		{
			FILE *fp_savefile;
			if (isSaved == false)
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
					BottomLineInput(strerror(errno));
					continue;
				}
				isSaved = true;
			}

			int save_index_x = 0, save_index_y = 1; // Save index
			//SetConsoleOutputCP(65001);
			ClearScreen();
			for (int i = 1; i < YSIZE; i++)
			{

				fprintf(stdout, "%s\n", str_save[i]);
				fprintf(fp_savefile, "%s\n", str_save[i]);
			}
			NewtroditNameLoad();
			CenterFileName(strlasttok(save_destination, '\\'));
			fclose(fp_savefile);
			ShowBottomMenu();
			//SetConsoleOutputCP(codepage_save);
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
			}
			CloseClipboard();
			// _! continue;
		}
		if (ch == 27)
		{
			printf("\"%s\"", str_save[ypos]);
		}
		//printf("running");
		if (ch == 8)
		{
			if (xpos > 0)
			{

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
					// printf("strlen(%d), \"%s\"", end_key_len, str_save[ypos]);
					if (ypos == 2)
					{
						xpos = 1;
					}
					gotoxy(xpos, --ypos);
				}
				xpos--;
			}
		}
		else

		{
			if (ch > 31)
			{
				//printf("%d\t", (int)ch);
				str_save[ypos][xpos] = (int)ch;
				putchar(ch);
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