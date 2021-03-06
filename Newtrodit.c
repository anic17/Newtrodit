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
#include "dialog.h"

char filename_text[MAX_FILENAME] = "Untitled";

int line = 0;
int col = 0;

/*



"Hola"

----------

*/

//XSIZE-((XSIZE/2)+len/2)-1

void CenterFileName(char *filename)
{
	SetColor(0x70);
	int cols = GetConsoleXDimension();
	gotoxy(0, 0);
	int center_filename = cols - ((cols / 2) + strlen(filename) / 2) - 1;
	gotoxy(center_filename, 0);
	printf("%s", filename);
	SetColor(0x07);
}

void ShowBottomMenu()
{
	gotoxy(0, GetConsoleYDimension() - 2);
	SetColor(0x07);
	printf("Basic shortcuts: ");
	SetColor(0x70);
	printf("^C");
	SetColor(0x07);
	printf(" Copy  ");
	SetColor(0x70);
	printf("^V");
	SetColor(0x07);
	printf(" Paste  ");
	SetColor(0x70);
	printf("^X");
	SetColor(0x07);
	printf(" Quit Newtrodit\n");
	SetColor(0x07);
	printf("                 ");
	SetColor(0x70);
	printf("^O");
	SetColor(0x07);
	printf(" Open  ");
	SetColor(0x70);
	printf("^S");
	SetColor(0x07);
	printf(" Save   ");
	SetColor(0x70);
	printf("F1");
	SetColor(0x07);
	printf(" Help");
	return;
}

void CursorSettings(int visible, int size)
{
	HANDLE Cursor = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = size;
	info.bVisible = visible;
	SetConsoleCursorInfo(Cursor, &info);
}

void NewtroditNameLoad()
{
	gotoxy(0, 0);
	SetColor(0x70);
	ClearLine(0);
	gotoxy(0, 0);
	printf(" Newtrodit %s", newtrodit_version);
}

void BottomLineInput(const char *bottom_string)
{
	SetColor(0x07);
	ClearLine(GetConsoleYDimension() - 1);
	printf("%s", bottom_string);
}

void BS()
{
	putchar('\b');
	putchar(' ');
	putchar('\b');
}

int QuitProgram(int posx, int posy, int color_quit)
{
	ClearLine(GetConsoleYDimension() - 1);
	gotoxy(0, GetConsoleYDimension() - 1);
	printf("%s", NEWTRODIT_PROMPT_QUIT);
	int confirmquit = getch();
	if (confirmquit == 89 || confirmquit == 121)
	{
		SetColor(color_quit);
		ClearScreen();
		exit(0);
	}
	else
	{
		if (confirmquit == 0 && getch() == 107)
		{
			printf("\a");
		}

		ShowBottomMenu();
		gotoxy(posx, posy);
		SetColor(0x07);
	}
	return 0;
}

void LoadAllNewtrodit()
{
	ClearScreen();
	SetColor(0x07);
	NewtroditNameLoad();
	CenterFileName(filename_text);

	ShowBottomMenu();

	SetColor(0x07);
	CursorSettings(TRUE, 20);
}

int main(int argc, char *argv[])
{
	
	bool isSaved = false;
	int codepage_save = GetConsoleOutputCP();
	char **str_save = malloc(6144); // allocates 6144 char pointers
	for (int i = 0; i < 6144; ++i)
	{
		str_save[i] = malloc(2048); // allocates 2048 bytes for each string
	}
	int shift_virtual_key, alt_virtual_key;
	int old_x_size, old_y_size;
	for (int i = 0; i < YSIZE; i++)
	{
		str_save[i][0] = '\0';
	}
	int edit_file = 0;
	int start_color = GetColor();
	int lines_paste = 0;

	if (XSIZE < 54 || YSIZE < 6)
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}
	int argc_shift = 1;
	if (argc > 1)
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
		argc_shift++;
	}
	int open_argv_x, open_argv_y;
	if (argc == argc_shift + 1)
	{
		FILE *newtrodit_write_argv;
		newtrodit_write_argv = fopen(argv[argc_shift], "r");
		if (!newtrodit_write_argv)
		{
			printf("arg: %s: %s", argv[argc_shift], strerror(errno));
			exit(EXIT_FAILURE);
		}
		int c_read;
		gotoxy(0, 1);
		strcpy(filename_text, argv[argc_shift]);

		while ((c_read = getc(newtrodit_write_argv)) != EOF)
		{
			str_save[open_argv_y][open_argv_x++] = c_read;
			if (c_read == 10 || c_read == 13)
			{
				open_argv_y++;
			}
			putchar(c_read);
		}
		getch();
		SetColor(start_color);
	}

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
	LoadAllNewtrodit();
	int xpos = 0, ypos = 1;

	gotoxy(xpos, ypos);

	int index = 0;
	int c;
	int ch;

	SetColor(0x07);
	while (1)
	{

		//show_cursor_pos_window_size = GetConsoleYDimension();
		gotoxy(0, GetConsoleYDimension() - 1);
		for (int i = 0; i < 17; ++i)
		{
			putchar(' ');
		}
		gotoxy(0, GetConsoleYDimension() - 1);
		printf("Ln %d, Col %d", ypos, xpos + 1);
		gotoxy(xpos, ypos);
		old_x_size = XSIZE;
		old_y_size = YSIZE;	
		ch = getch();
		if(old_x_size != XSIZE || old_y_size != YSIZE)
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
			printf("yeee");
			xpos = 0, ypos = 1;
			gotoxy(xpos, ypos);
			c = getc(fileread);
			putchar(c);
			while ((c = getc(fileread)) != EOF)
			{
				puts("yunm");
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
					line_number_str[line_number_chr_index--] = '\0';
					BS();
				}
				if (line_number_chr == YSIZE-3)
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
		if (ch == 224)
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
				if ((ypos + 1) >= YSIZE-3)
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
			QuitProgram(xpos, ypos, start_color);
			SetColor(0x07);
			// _! continue;
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 107)
			{
				QuitProgram(xpos, ypos, start_color);
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
			for (int i = 1; i < YSIZE; i++)
			{
				if (str_save[i] != NULL)
				{
					fprintf(fp_savefile, "%s\n", str_save[i]);
				}
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
				printf("%s", buffer_clipboard);
				printf("\ncrash");
				final_paste_strrchr = strrchr(buffer_clipboard, '\n')+1;
				printf("\nprotegent");
				lines_paste = 0;
				for (int i = 0; i < strlen(buffer_clipboard); i++)
				{
					if (buffer_clipboard[i] == '\n')
					{
						lines_paste++;
					}
				}
				xpos += strlen(final_paste_strrchr + 1);
				ypos += lines_paste;
			}
			printf("crashhhhhhh");
			CloseClipboard();
			// _! continue;
		}
		if (ch == 27)
		{
			printf("\"%s\"", str_save[ypos]);
		}

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
					gotoxy(xpos, --ypos);
				}
				else
				{
					Alert();
				}
				xpos--;
			}
		}
		else
		{
			if (ch > 31)
			{
				str_save[ypos][xpos] = ch;
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