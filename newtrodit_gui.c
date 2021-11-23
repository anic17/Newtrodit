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

void TopHelpBar()
{
	gotoxy(0, 0);
	SetColor(fg_color);
	ClearLine(0);
	gotoxy(0, 0);
	printf(" Newtrodit help");
	SetColor(bg_color);
	return;
}

void BottomHelpBar()
{
	SetColor(fg_color);
	ClearLine(YSIZE - 1);
	PrintBottomString("^X  Close help  A-F4 Quit Newtrodit");
	SetColor(bg_color);
	return;
}

void CenterText(char *text, int yline) // Algorithm: XSIZE - ((XSIZE / 2) + len / 2) - 1
{
	SetColor(fg_color);
	int cols = XSIZE;
	gotoxy(0, yline);
	int center_text = (cols / 2) - (strlen(text) / 2);
	gotoxy(center_text, yline);
	printf("%s", text);
	SetColor(bg_color);
	return;
}

void RightAlignNewline()
{
	SetColor(fg_color);

	char nl_type[200];
	EmptyString(nl_type);
	if (!strcmp(newlinestring, "\n"))
	{
		strcpy(nl_type, "Unix (LF)");
	}
	if (!strcmp(newlinestring, "\r\n"))
	{
		strcpy(nl_type, "Windows (CR LF)");
	}
	if (!strcmp(newlinestring, "\r"))
	{
		strcpy(nl_type, "Macintosh (CR)");
	}
	if (!strcmp(newlinestring, "\n\r"))
	{
		strcpy(nl_type, "Risc OS (LF CR)");
	}
	if (!strcmp(newlinestring, "\025"))
	{
		strcpy(nl_type, "IBM Mainframe (NL)");
	}
	if (!strcmp(newlinestring, "\x1e"))
	{
		strcpy(nl_type, "QNX pre-POSIX (RS)");
	}
	if (nl_type[0] == '\0')
	{
		strcpy(nl_type, "Unknown newline character");
	}

	gotoxy(XSIZE - strlen(nl_type) - 2, 0); // XSIZE - strlen(nl_type) - 2
	fputs(nl_type, stdout);
	SetColor(bg_color);

	return;
}

void ShowFindMenu()
{
	ClearLine(YSIZE - 2);
	ClearLine(YSIZE - 1);
	gotoxy(0, YSIZE - 2);

	SetColor(fg_color);
	fputs("F3", stdout);
	SetColor(bg_color);
	fputs(": Next occurrence\n", stdout);
	SetColor(fg_color);
	fputs("ESC", stdout);
	SetColor(bg_color);
	fputs(": Quit", stdout);
}

void ShowBottomMenu()
{
	gotoxy(0, BOTTOM);

	ClearLine(BOTTOM);
	gotoxy(0, BOTTOM);
	fputs(NEWTRODIT_DIALOG_BOTTOM_HELP, stdout);

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
	SetColor(fg_color);
	ClearLine(0);
	gotoxy(0, 0);
	printf(" Newtrodit %s", newtrodit_version);
	SetColor(bg_color);
}

int GetConsoleXCursorPos()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition.X;
}

int GetConsoleYCursorPos()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition.Y;
}

void DisplayCursorPos(int xps, int yps)
{
	size_t len = strlen(NEWTRODIT_DIALOG_BOTTOM_HELP);
	gotoxy(len, YSIZE - 1);
	for (int i = 0; i < 20; ++i) // Clear first characters of the last line
	{
		putchar(' ');
	}
	gotoxy(len, YSIZE - 1);
	printf("Ln %d, Col %d", yps, xps + 1); // Add one to xpos because it's zero indexed
}

void LoadAllNewtrodit()
{
	CursorSettings(FALSE, CURSIZE); // Hide cursor to reduce flickering
	// ShowScrollBar(GetConsoleWindow(), SB_VERT, 0);
	SetColor(bg_color);

	if (clearBufferScreen)
	{
		ClearBuffer();
	}
	else
	{
		ClearScreen();
	}
	NewtroditNameLoad();
	CenterText(strlasttok(filename_text, '\\'), 0);
	RightAlignNewline();
	ShowBottomMenu();
	SetConsoleSize(XSIZE, YSIZE);
	if (LINECOUNT_WIDE != 0)
	{
		DisplayLineCount(str_save, YSIZE - 3, 1);
	}
		CursorSettings(TRUE, CURSIZE);


	gotoxy(0, 1);
}

void NewtroditCrash(char *crash_reason, int crash_retval)
{
	LoadAllNewtrodit();
	gotoxy(0, 1);
	int errno_temp = errno;
	if (errno_temp == 0)
	{
		errno_temp = crash_retval;
	}
	Alert();
	printf("Newtrodit ran into a problem and it crashed. We're sorry.\n\nDebug info:\nerrno: 0x%x (%s)\nGetLastError: 0x%lx\n\nReason: %s\n\nPress enter to exit...\n", errno_temp, strerror(errno_temp), GetLastError(), crash_reason);

	getchar();

	exit(crash_retval);
}

int QuitProgram(int color_quit)
{
	PrintBottomString(NEWTRODIT_PROMPT_QUIT);
	if (YesNoPrompt())
	{
		if (isModified)
		{
			LoadAllNewtrodit();
			DisplayFileContent(str_save, newlinestring, stdout);

			PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);

			if (YesNoPrompt())
			{
				SaveFile(str_save, strdup(filename_text), YSIZE, &isModified, &isUntitled);
			}
		}
		SetColor(color_quit);
		ClearScreen();
		CursorSettings(TRUE, CURSIZE);
		exit(0);
	}
	else
	{
		SetColor(bg_color);
	}

	return 0;
}

void UpdateTitle(int is_saved)
{
	if (is_saved == 1 && fullPathTitle)
	{
		char *path = filename_text;
		GetFullPathName(path, sizeof(filename_text), filename_text, NULL);
	}
	if (isModified)
	{
		SetConsoleTitle(join(join("Newtrodit - ", filename_text), " (Modified)"));
	}
	else
	{
		SetConsoleTitle(join("Newtrodit - ", filename_text));
	}
}
