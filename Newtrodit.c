#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <errno.h>
#include <stdbool.h>
//#include "newtrodit_core.h"
#include "manual.c"
#include "dialog.h"

#define XSIZE GetConsoleXDimension()
#define YSIZE GetConsoleYDimension()



char filename_text[MAX_FILENAME] = "Untitled";

int line = 0;
int col = 0;

void CenterFileName(char *filename)
{
	int cols = GetConsoleXDimension();
	gotoxy(0, 0);
	int center_filename = cols - ((cols / 2) + strlen(filename)) - 1;
	gotoxy(center_filename, 0);
	printf("%s", filename);
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

void SetBackgroundColor()
{
	SetColor(0x07);
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
	int rows = GetConsoleXDimension();
	for (int n = 0; n < rows; n++)
	{
		printf(" ");
	}
	gotoxy(0, 0);
	printf(" Newtrodit %s", newtrodit_version);
}

void BS()
{
	putchar(' ');
	putchar('\b');
	putchar('\b');
	putchar(' ');
	putchar('\b');
}


int PrintBottomString(char* bottom_string)
{
	gotoxy(0, GetConsoleYDimension()-1);
	printf("%s", bottom_string);
	return errno;
}

int QuitProgram(int posx, int posy)
{
	int rows = GetConsoleYDimension();
	gotoxy(1, rows - 1);
	SetColor(0x70);
	printf("%s", NEWTRODIT_PROMPT_QUIT);
	int confirmquit = getch();
	if (confirmquit == 89 || confirmquit == 121)
	{
		SetColor(0x07);
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
		SetBackgroundColor();
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char str_save[][] = {0};
	int edit_file = 0;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	WORD start_color;
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetConsoleScreenBufferInfo(hConsoleHandle, &csbi))
	{
		start_color = csbi.wAttributes;
	}

	int cols = GetConsoleXDimension();
	int rows = GetConsoleYDimension();
	if (cols < 54 || rows < 6)
	{
		MessageBox(0, NEWTRODIT_ERROR_WINDOW_TOO_SMALL, "Newtrodit", 16);
		return 0;
	}
	if (argc > 1)
	{
		if (!strcmp(argv[1], "--help"))
		{
			NewtroditHelp();
			getch();
			SetColor(start_color);
			exit(0);
		}
	}
	if (argc == 2)
	{
		FILE *newtrodit_write_argv;
		newtrodit_write_argv = fopen(argv[1], "r");
		if (!newtrodit_write_argv)
		{
			printf("arg: %s: %s", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}
		int c_read;
		gotoxy(0, 1);
		strcpy(filename_text, argv[1]);
		while ((c_read = getc(newtrodit_write_argv)) != EOF)
		{
			putchar(c_read);
		}
		getch();
		SetColor(start_color);
		return EXIT_SUCCESS;
	}
	int biggest_y_line = 1;
	char save_destination[MAX_FILENAME];
	int show_cursor_pos_window_size, save_file_dialog;
	char *find_substring;
	int find_substring_index;
	int find_substring_len;
	char *basenameopen;
	char fileopenread[512];
	char findstring[8192];
	ClearScreen();

	int xpos = 0, ypos = 1;
	SetColor(0x07);
	NewtroditNameLoad();
	CenterFileName(filename_text);

	ShowBottomMenu();

	SetBackgroundColor();
	CursorSettings(TRUE, 20);

	gotoxy(xpos, ypos);

	char str[65536];
	int index = 0;
	int c;
	int ch;

	SetColor(0x07);
	while (1)
	{
		show_cursor_pos_window_size = GetConsoleYDimension();
		gotoxy(0, show_cursor_pos_window_size - 1);
		for (int i = 0; i < 17; ++i)
		{
			putchar(' ');
		}
		gotoxy(0, show_cursor_pos_window_size - 1);
		printf("Ln %d, Col %d", ypos, xpos + 1);
		gotoxy(xpos, ypos);

		ch = getch();
		str[index++] = ch;

		if (ch == 8)
		{
			if (xpos > 0)
			{
				xpos--;
				index--;
			}
			else
			{
				Alert();
			}
		}
		if (ch == 15)
		{
			FILE *fileread;

			gotoxy(1, rows - 1);
			SetColor(0x70);

			printf("%s", NEWTRODIT_PROMPT_FOPEN);
			fgets(fileopenread, sizeof fileopenread, stdin);

			fileread = fopen(fileopenread, "r");
			if (!fileread)
			{
				gotoxy(1, rows - 1);
				printf("%s: %s", fileopenread, strerror(errno));
				getch();
				ShowBottomMenu();
				gotoxy(xpos, ypos);
				SetBackgroundColor();
			}
			else
			{
				printf("fopen succeeded\n");
			}

			strcpy(filename_text, fileopenread);
			CenterFileName(filename_text);
			SetBackgroundColor();
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

			size_t len = strlen(str) + 1;
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
			memcpy(GlobalLock(hMem), str, len);
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
			continue;
		}

		if (ch == 19) // ^S = Save
		{
		}
		if (ch == 8)
		{
			xpos--;
			putchar(' ');
		}

		if (ch == 7) // ^G = Goto line
		{

			int console_cols_gotoxy = GetConsoleXDimension();
			gotoxy(0, GetConsoleYDimension()-1);
			for(int i = 0; i < console_cols_gotoxy-1; ++i)
			{
				putchar(' ');
			}

			gotoxy(0, GetConsoleYDimension()-1);	
			printf("%s", NEWTRODIT_PROMPT_GOTO_LINE);
			char line_number_str[6];
			int line_number_chr = 0, line_number_chr_index = 0;

			while(line_number_chr != 13)
			{
				line_number_chr = getch();
				if(line_number_chr > 47 && line_number_chr < 58)
				{
					if(atoi(line_number_str) >= 10000)
					{
						Alert();
					} else {
						line_number_str[line_number_chr_index++] = line_number_chr;
					}
				}
				if(line_number_chr == 8)
				{
					BS();
					line_number_chr_index--;
				}
			}
			printf("%s", line_number_str);
			getch();
			exit(0);
			if(atoi(line_number_str) < GetConsoleYDimension())
			{
				gotoxy(0, GetConsoleYDimension()-1);
				printf("%s", NEWTRODIT_ERROR_CON_BUFFER_LIMIT);
			}
			ypos = atoi(line_number_str) ;
			gotoxy(0, ypos);
			printf("%d:%d", xpos, ypos);
			ShowBottomMenu();
			

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
				if((ypos+1) >= 27)
				{
					Alert();
				} else {
					gotoxy(xpos, ++ypos);
				}
				
				break;
			}
			continue;
			ch = 0;
		}

		if (ch == 13)
		{

			index = 0;

			xpos = 0;
			str_save[ypos++][xpos] = '\n';
			printf("\n");
			if(biggest_y_line < ypos)
			{
				biggest_y_line++;
			}
			continue;
		}
		if (ch == 6)
		{
			gotoxy(1, YSIZE);
			printf("%s", NEWTRODIT_PROMPT_FIND_STRING);
			fgets(findstring, sizeof findstring, stdin);

			if (strstr(str, findstring) != NULL)
			{
				find_substring = strstr(str, findstring);
				find_substring_index = find_substring - str;
				find_substring_len = strlen(str) - find_substring_index;
				gotoxy(0, ypos);
				gotoxy(find_substring_index, 2);
				SetColor(0x0e);
				printf("yellow");
			}
			// _! continue;
		}
		if (ch == 24)
		{
			QuitProgram(xpos, ypos);
			SetColor(0x07);
			// _! continue;
		}
		if (ch == 0)
		{
			ch = getch();
			if (ch == 107)
			{
				QuitProgram(xpos, ypos);
				SetColor(0x07);
			}
			// _! continue
			if (ch == 59)
			{
				NewtroditHelp();
				getch();
			}
		}
		if (ch == 19)
		{
			save_file_dialog = GetConsoleYDimension();

			gotoxy(0, save_file_dialog);
			for (int i = 0; i < save_file_dialog; ++i)
			{
				putchar(' ');
			}
			gotoxy(0, save_file_dialog);
			printf("%s", NEWTRODIT_PROMPT_SAVE_FILE);
			fgets(save_destination, sizeof save_destination, stdin);
			size_t len_savedest = strlen(save_destination);
			save_destination[len_savedest - 1] = '\0';
			FILE *fp_savefile;
			fp_savefile = fopen(save_destination, "w");


			if (!fp_savefile)
			{
				gotoxy(0, save_file_dialog);
				for (int i = 0; i < save_file_dialog; ++i)
				{
					putchar(' ');
				}
				gotoxy(0, save_file_dialog);
				printf("%s: %s", save_destination, strerror(errno));
				continue;
			}

			int save_index_x = 0, save_index_y = 1; // Save index
			ClearScreen();
			for(int i = 0; i < ypos; ++i)
			{
				printf("CHAR: \"%s\"\n", str_save[i]);
			}

			/* 
			while(str_save[save_index_y] == NULL)
			{
				//for(int i = 0; i < )
				fprintf(stdout, "%s", str_save[save_index_y]);
			}
			*/
			fclose(fp_savefile);
		}

		if (ch == 22)
		{
			char *buffer;
			if (OpenClipboard(0))

			{
				buffer = (char *)GetClipboardData(CF_TEXT);
				printf("%s", buffer);
			}

			CloseClipboard();
			// _! continue;
		}
		xpos++;
		str_save[ypos][xpos] = ch;
		if(ch == 8)
		{
			BS();
		} else {
			putchar(ch);
		}
		
	}
	SetColor(start_color);
	return 0;
}