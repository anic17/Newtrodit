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

void CenterText(char *text, int yline) //Algorithm: XSIZE - ((XSIZE / 2) + len / 2) - 1
{
	SetColor(fg_color);
	int cols = GetConsoleXDimension();
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

	char nl_type[100];
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
	printf("%s", nl_type);
	SetColor(bg_color);

	return;
}

void ShowFindMenu()
{
	ClearLine(YSIZE - 2);
	ClearLine(YSIZE - 1);

	gotoxy(0, GetConsoleYDimension() - 2);
	printf("%s", NEWTRODIT_FIND_STRING_F3_NEXT);
}

void ShowBottomMenu()
{
	ClearLine(YSIZE - 2);
	ClearLine(YSIZE - 1);

	gotoxy(0, GetConsoleYDimension() - 2);

	SetColor(bg_color);
	printf(NEWTRODIT_DIALOG_BASIC_SHORTCUTS);
	SetColor(fg_color);
	printf("^C");
	SetColor(bg_color);
	printf(" Copy  ");
	SetColor(fg_color);
	printf("^V");
	SetColor(bg_color);
	printf(" Paste  ");
	SetColor(fg_color);
	printf("^X");
	SetColor(bg_color);
	printf(" Quit Newtrodit\n");
	SetColor(bg_color);
	for (int i = 0; i < strlen(NEWTRODIT_DIALOG_BASIC_SHORTCUTS); i++)
	{
		printf(" ");
	}
	SetColor(fg_color);
	printf("^O");
	SetColor(bg_color);
	printf(" Open  ");
	SetColor(fg_color);
	printf("^S");
	SetColor(bg_color);
	printf(" Save   ");
	SetColor(fg_color);
	printf("F1");
	SetColor(bg_color);
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

int QuitProgram(int color_quit)
{
	ClearLine(GetConsoleYDimension() - 1);
	PrintBottomString(NEWTRODIT_PROMPT_QUIT);
	int confirmquit = getch();
	if (tolower(confirmquit) == 'y') // Y was answered
	{
		SetColor(color_quit);
		ClearScreen();
		CursorSettings(TRUE, CURSIZE);
		exit(0);
	}
	else
	{

		if (confirmquit == 0 && getch() == 107)
		{
			Alert();
		}
		ClearLine(GetConsoleYDimension() - 1);
		SetColor(bg_color);
	}
	return 0;
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
	gotoxy(0, GetConsoleYDimension() - 1);
	for (int i = 0; i < strlen(NEWTRODIT_DIALOG_BASIC_SHORTCUTS); ++i) // Clear first characters of the last line
	{
		putchar(' ');
	}
	gotoxy(0, GetConsoleYDimension() - 1);
	printf("Ln %d, Col %d", yps, xps + 1); // Add one to xpos because it's zero indexed
}

void LoadAllNewtrodit()
{
	CursorSettings(FALSE, CURSIZE); // Hide cursor to reduce flickering
	int curx = GetConsoleXCursorPos(), cury = GetConsoleYCursorPos();
	//ShowScrollBar(GetConsoleWindow(), SB_VERT, 0);
	SetColor(bg_color);
	ClearScreen();
	NewtroditNameLoad();
	CenterText(filename_text, 0);
	RightAlignNewline();
	ShowBottomMenu();
	CursorSettings(TRUE, CURSIZE);
	//DisplayCursorPos(curx, cury);
	// Temporary disabled to avoid more flickering
	gotoxy(0, 1);
}

void NewtroditCrash(char *crash_reason, int crash_retval)
{
	int errno_temp = errno;
	if (errno_temp == 0)
	{
		errno_temp = crash_retval;
	}
	Alert();
	printf("Newtrodit ran into a problem and it crashed. We're sorry.\n\nDebug info:\nerrno: 0x%x (%s)\nGetLastError: 0x%x\n\nReason: %s\n\nPress any key to quit...\n", errno_temp, strerror(errno_temp), GetLastError(), crash_reason);
	MakePause();
	exit(crash_retval);
}