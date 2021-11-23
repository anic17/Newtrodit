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

int CountLines(FILE *fp)
{
	int n = 0, c;
	while ((c = getc(fp)) != EOF)
	{
		if (c == '\n')
		{
			n++;
		}
	}
	fseek(fp, 0, SEEK_SET);
	return n;
}

void DisplayLineCount(char *strsave[], int size, int yps)
{
	int i = yps;

	gotoxy(0, display_y);
	if (strlen(itoa_n(display_y)) >= LINECOUNT_WIDE - 1)
	{
		LINECOUNT_WIDE++;
	}

	if (strsave[i + 1][0] == '\0')
	{
		gotoxy(0, display_y + 1);
		for (int i = 0; i < LINECOUNT_WIDE - 1; ++i)
		{
			putchar(' ');
		}
		gotoxy(0, display_y);
	}
	SetColor(0x80);
	for (int i = 0; i < LINECOUNT_WIDE - 1; ++i)
	{
		putchar(' ');
	}
	gotoxy(0, display_y);
	printf("%d", ypos);

	i++;
	SetColor(bg_color);
}

void LoadLineCount(char *strsave[], int startpos)
{
	int n = YSIZE;
	int print_line = 0;

	if (ypos >= (n - 2))
	{
		startpos = ypos - (n - 2);
	}
	else
	{
		startpos = 0;
	}
	for (int k = startpos + 1; k < startpos + (n - 1); ++k)
	{
		if (str_save[k][0] != '\0')
		{
			print_line++;
			if (strlen(itoa_n(k)) >= LINECOUNT_WIDE - 1)
			{
				LINECOUNT_WIDE++;
			}
			gotoxy(0, print_line);
			SetColor(0x80);
			for (int i = 0; i < LINECOUNT_WIDE - 1; ++i)
			{
				putchar(' ');
			}
			gotoxy(0, print_line);
			printf("%i", k);
		}
	}
	SetColor(bg_color);
}

int WriteBuffer(FILE *fstream)
{
	for (int i = 1; i < BUFFER_Y; ++i)
	{
		if (fputs(str_save[i], fstream) == EOF)
		{
			return 1;
		}
	}
	return 0;
}

int DisplayFileContent(char *strsave[], char newlinestring[], FILE *fstream)
{
	if (LINECOUNT_WIDE != 0)
	{
		LoadLineCount(strsave, ypos);
		gotoxy(0, 1);
	}
	int startpos = 0;

	if (ypos >= (YSIZE - 2))
	{
		startpos = ypos - (YSIZE - 2);
	}

	for (int i = 1 + startpos; i < startpos + (YSIZE - 1); i++)
	{
		if (strsave[i][0] != '\0')
		{
			gotoxy(LINECOUNT_WIDE, i - startpos);

			if (wrapLine)
			{
				fputs(strsave[i], fstream);
			}
			else
			{
				printf("%.*s", wrapSize, strsave[i]);
			}
		}
	}
	return 0;
}

int ValidFileName(char *filename)
{
	return strpbrk(filename, "*?\"<>|\x1b") != NULL;
}

int SaveFile(char *strsave[], char *filename, int size, int *is_modified, int *is_untitled)
{
	if (*is_untitled == true)
	{
		ClearLine(BOTTOM);
		PrintBottomString(NEWTRODIT_PROMPT_SAVE_FILE);
		fgets(filename, _MAX_PATH, stdin); // Can't use sizeof filename because it's a pointer

		if (nolflen(filename) <= 0)
		{
			LoadAllNewtrodit();
			DisplayFileContent(strsave, newlinestring, stdout); // Display file content on screen
			PrintBottomString(NEWTRODIT_FUNCTION_ABORTED);
			MakePause();
			ShowBottomMenu();
			return 0;
		}
		filename[strcspn(filename, "\n")] = 0; // Remove newline character

		if (ValidFileName(filename))
		{
			PrintBottomString(join(NEWTRODIT_FS_FILE_INVALID_NAME, filename));
			MakePause();
			return 0;
		}
	}

	FILE *fp = fopen(filename, "wb");

	WriteBuffer(fp);

	LoadAllNewtrodit();
	DisplayFileContent(strsave, newlinestring, stdout); // Display file content on screen

	if (fp)
	{
		*is_modified = false;
		*is_untitled = false;
		PrintBottomString(NEWTRODIT_FILE_SAVED);
	}
	else
	{
		PrintBottomString(NEWTRODIT_FS_FILE_SAVE_ERR);
	}
	MakePause();
	ShowBottomMenu();
	fflush(fp);
	fclose(fp);

	return 0;
}

int LoadFile(char *strsave[], char *filename, int relativexps[], char newlinestr[], FILE *fpread)
{
	if (!trimLongLines)
	{
		fseek(fpread, 0, SEEK_END);
		if ((int)ftell(fpread) >= BUFFER_X * BUFFER_Y)
		{
			fseek(fpread, 0, SEEK_SET);
			ClearLine(BOTTOM);
			PrintBottomString(join(NEWTRODIT_FS_FILE_TOO_LARGE, filename));
			MakePause();
			ShowBottomMenu();
			return 1;
		}
		fseek(fpread, 0, SEEK_SET);

		if (CountLines(fpread) >= BUFFER_Y)
		{
			ClearLine(BOTTOM);
			PrintBottomString(join(NEWTRODIT_FS_FILE_TOO_LARGE, filename));
			MakePause();
			ShowBottomMenu();
			return 1;
		}
	}

	gotoxy(0, 1);
	for (int i = 1; i < BUFFER_Y; i++)
	{
		EmptyString(strsave[i]);
	}

	int c, read_x = 0, read_y = 1;

	while ((c = getc(fpread)) != EOF)
	{
		if (c == 0 && convertNull == true)
		{
			c = 32;
		}

		if (c == 10) // LF
		{
			strsave[read_y][read_x] = c;

			read_x = 0;
			read_y++;
		}
		else
		{

			if (c == 9)
			{
				if (convertTabtoSpaces == true) // Convert tab to spaces
				{
					for (int i = 0; i < TAB_WIDE; i++)
					{
						strsave[read_y][read_x++] = ' ';
					}
				}
				else
				{
					strsave[read_y][read_x++] = 9;
					relativexps[read_y] += TAB_WIDE;
				}
			}
			else
			{
				strsave[read_y][read_x++] = c;
			}
		}
		if (read_x > BUFFER_X || read_y > BUFFER_Y)
		{
			if (!trimLongLines)
			{
				ClearLine(BOTTOM);
				PrintBottomString(join(NEWTRODIT_FS_FILE_TOO_LARGE, filename));
				MakePause();
				ShowBottomMenu();
				return 1;
			}
		}
	}

	if (LINECOUNT_WIDE != 0)
	{
		LoadLineCount(strsave, ypos);
	}

	for (int j = 1; j < YSIZE - 1; j++) // Increment read_y by one because it starts at 1
	{
		gotoxy(LINECOUNT_WIDE, j);
		printf("%s", strsave[j]);
	}

	fclose(fpread);

	return 0;
}

int NewFile(int *is_modified, int *is_untitled, int *is_saved, int line_count, char *strsave[], int *xps, int *yps, char *filename, int *relativexps, int *relativeyps)
{
	if (*is_modified == true)
	{
		PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);
		if (YesNoPrompt())
		{
			SaveFile(strsave, filename_text, BUFFER_Y, is_modified, is_untitled);
		}
		*is_modified = false;
	}
	EmptyString(filename);

	strncpy_n(filename, filename_text_, sizeof(filename_text_)); // Restore default filename
	LoadAllNewtrodit();
	for (int i = 0; i < line_count; i++)
	{
		memset(strsave[i], 0, BUFFER_X);
	}

	for (int i = 0; i < sizeof(relativexps); i++)
	{
		relativexps[i] = 0;
		relativeyps[i] = 0;
	}
	*relativexps = 0;
	*relativeyps = 0;

	*xps = 0;
	*yps = 1;
	*is_untitled = true;
	*is_saved = false;
	*is_modified = false;
	DisplayCursorPos(*xps, *yps);

	gotoxy(*xps + LINECOUNT_WIDE, *yps);

	return 0;
}

void ReloadFile(int *is_untitled, char *strsave[], int *xps, int *yps, int relativexps[], int relativeyps[], FILE *fstream)
{
	LoadAllNewtrodit();
	if (*is_untitled == false)
	{
		LoadFile(strsave, filename_text, relativexps, newlinestring, fstream);
	}
	else
	{
		DisplayFileContent(strsave, newlinestring, stdout);
	}
	if (strsave[*yps][*xps] == '\0')
	{
		*xps = 0;
		*yps = 1;
	}
	PrintBottomString(NEWTRODIT_FILE_RELOADED);
	MakePause();
	ShowBottomMenu();

	DisplayCursorPos(*xps, *yps);
	gotoxy(*xps + relativexps[*yps] + LINECOUNT_WIDE, *yps + relativeyps[*xps]);

	return;
}

void FunctionAborted(char *strsave[], char *newlinestring)
{
	LoadAllNewtrodit();
	DisplayFileContent(strsave, newlinestring, stdout); // Display file content on screen
	PrintBottomString(NEWTRODIT_FUNCTION_ABORTED);
	MakePause();
	ShowBottomMenu();
	return;
}

void ConBufferLimit(char *strsave[], int chr, int *xps, int *yps)
{

	MakePause();
	ShowBottomMenu();
	DisplayFileContent(str_save, newlinestring, stdout);
	if (chr != 8)
	{
		*xps = 0;
		*yps = 1;
	}
}

void UpdateScrolledScreen(int linecount)
{
	if (ypos >= (YSIZE - 3))
	{
		clearBufferScreen = true;
		LoadAllNewtrodit();
		clearBufferScreen = false;
		if (linecount)
		{
			LoadLineCount(str_save, ypos);
		}
		DisplayFileContent(str_save, newlinestring, stdout);
	}
}

void UpdateHomeScrolledScreen(int linecount)
{
	if (ypos >= (YSIZE - 3))
	{
		ypos = 1;
		clearBufferScreen = true;
		LoadAllNewtrodit();
		clearBufferScreen = false;
		if (linecount)
		{
			LoadLineCount(str_save, ypos);
		}
		DisplayFileContent(str_save, newlinestring, stdout);
	}
}

char *TypingFunction(int min_ascii, int max_ascii, int max_len)
{
	int chr = 0, index = 0;
	char *num_str = (char *)malloc(max_len) + 1;
	PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);

	memset(num_str, 0, max_len); // Clear string
	while (chr != 13)			 // Loop while enter isn't pressed
	{
		chr = getch();
		if (chr == 27)
		{
			break;
		}
		if (chr >= min_ascii && chr <= max_ascii) // Check if character is a between the range
		{
			if (strlen(num_str) >= max_len) // Check if max length is reached
			{
				Alert();
			}
			else
			{
				num_str[index++] = chr;
				putchar(chr);
			}
		}
		if (chr == 8) // Backspace
		{
			if (index > 0)
			{
				num_str[index--] = '\0';
				printf("\b \b");
			}
		}
	}
	return num_str;
}