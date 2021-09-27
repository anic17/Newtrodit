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
	gotoxy(0, i);
	if (strsave[i + 1][0] == '\0')
	{
		gotoxy(0, i + 1);
		for (int i = 0; i < LINECOUNT_WIDE; ++i)
		{
			printf(" ");
		}
		gotoxy(0, i);
	}
	SetColor(0x80);
	for (int i = 0; i < LINECOUNT_WIDE; ++i)
	{
		printf(" ");
	}
	gotoxy(0, i);
	printf("%i", i);

	i++;
	SetColor(bg_color);
}

int DisplayFileContent(char *strsave[], char newlinestring[], FILE *fstream)
{
	for (int i = 1; i < BUFFER_Y; i++)
	{
		if (strsave[i][0] != '\0')
		{
			for (int j = 0; j < LINECOUNT_WIDE; j++) // For line count function
			{
				putchar(' ');
			}

			if (strsave[i][0] != 13 && strsave[i][0] != 10)
			{
				fputs(strsave[i], fstream);
			}
			else
			{

				fputs(newlinestring, fstream);
			}
		}
	}
	fflush(fstream);
	return 0;
}

int SaveFile(char *strsave[], char *filename, int size, int *isModified, int *isUntitled)
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

	FILE *fp = fopen(filename, "wb");
	MakePause();
	DisplayFileContent(strsave, newlinestring, fp);

	LoadAllNewtrodit();
	DisplayFileContent(strsave, newlinestring, stdout); // Display file content on screen

	if (fp)
	{
		PrintBottomString(NEWTRODIT_FILE_SAVED);
		getch();
	}
	else
	{
		PrintBottomString(NEWTRODIT_FS_FILE_SAVE_ERR);
		getch();
	}
	ShowBottomMenu();
	fflush(fp);
	fclose(fp);

	*isModified = false;
	*isUntitled = false;
	return 0;
}

void LoadFile(char *strsave[], char *filename, int relativexpos[], char newlinestr[], FILE *fpread)
{
	if (CountLines(fpread) >= BUFFER_Y)
	{
		ClearLine(BOTTOM);
		PrintBottomString(join(NEWTRODIT_FS_FILE_TOO_LARGE, filename));
		MakePause();
		ShowBottomMenu();
		return;
	}
	gotoxy(0, 1);
	for (int i = 0; i < BUFFER_Y; i++)
	{
		EmptyString(strsave[i]);
	}
	int c, read_x = 0, read_y = 1, spc = 0;

	while ((c = getc(fpread)) != EOF)
	{
		if (c == 10)
		{
			strsave[read_y][read_x] = c;

			read_x = 0;
			read_y++;
		}
		else
		{

			if (c == 9)
			{
				spc = 0;

				if (convertTabtoSpaces == true)
				{
					for (int i = 0; i < TAB_WIDE + 1; i++)
					{
						strsave[read_y][read_x++] = ' ';
					}
				}
				else
				{
					relativexpos[read_y] += TAB_WIDE + 1; // +1 because a character wide is always 1
				}
			}
			else
			{
				strsave[read_y][read_x++] = c;
			}
		}
		if (read_x > BUFFER_X || read_y > BUFFER_Y)
		{
			ClearLine(BOTTOM);
			PrintBottomString(NEWTRODIT_FS_FILE_TOO_LARGE);
		}
	}

	for (int j = 1; j < read_y + 1; j++) // Increment read_y by one because it starts at 1
	{
		printf("%s", strsave[j]);
	}
	fclose(fpread);

	return;
}

int NewFile(int *isModified, int *isUntitled, int lineCount, char *str_save[], int *xpos, int *ypos, char *filename, int *relativexpos, int *relativeypos)
{
	if (*isModified == true)
	{
		PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);
		if (tolower(getch()) != 'n')
		{
			SaveFile(str_save, filename_text, BUFFER_Y, isModified, isUntitled);
		}
		*isModified = false;
	}
	EmptyString(filename);

	strcpy(filename, filename_text_); // Restore default filename
	LoadAllNewtrodit();
	for (int i = 0; i < lineCount; i++)
	{
		EmptyString(str_save[i]);
	}

	for (int i = 0; i < sizeof(relativexpos); i++)
	{
		relativexpos[i] = 0;
		relativeypos[i] = 0;
	}
	*relativexpos = 0;
	*relativeypos = 0;

	*xpos = 0;
	*ypos = 1;
	*isUntitled = true;
	DisplayCursorPos(*xpos, *ypos);

	gotoxy(*xpos + LINECOUNT_WIDE, *ypos);

	return 0;
}

void ReloadFile(int *isUntitled, char *strsave[], int xps, int yps, int relativexpos[], int relativeypos[], FILE *fstream)
{
	//void LoadFile(char *strsave[], char *filename, int relativexpos[], char newlinestr[], FILE *fpread)
	LoadAllNewtrodit();
	if (*isUntitled == false)
	{
		LoadFile(strsave, filename_text, relativexpos, newlinestring, fstream);
	}
	else
	{
		DisplayFileContent(strsave, newlinestring, stdout);
	}

	ClearLine(BOTTOM);
	PrintBottomString(NEWTRODIT_FILE_RELOADED);
	MakePause();
	ShowBottomMenu();
	DisplayCursorPos(xps, yps);
	gotoxy(xps + relativexpos[yps] + LINECOUNT_WIDE, yps + relativeypos[xps]);

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