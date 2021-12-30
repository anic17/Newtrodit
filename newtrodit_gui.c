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
	int xs = XSIZE;

	SetColor(fg_color);
	ClearPartial(0, 0, xs, 1);
	printf("%.*s", xs, NEWTRODIT_DIALOG_MANUAL_TITLE);
	SetColor(bg_color);
	return;
}

void BottomHelpBar()
{
	SetColor(fg_color);
	PrintBottomString(NEWTRODIT_DIALOG_MANUAL);
	SetColor(bg_color);
	return;
}

void CenterText(char *text, int yline) // Algorithm: (XSIZE / 2) - (len / 2)
{
	SetColor(fg_color);
	int center_text = (XSIZE / 2) - (strlen(text) / 2);
	gotoxy(center_text, yline);
	printf("%.*s", wrapSize, text);
	SetColor(bg_color);
	return;
}

void RightAlignNewline()
{
	SetColor(fg_color);

	char *nl_type = (char *)malloc(sizeof(char) * 256);
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
	free(nl_type);
	SetColor(bg_color);

	return;
}

void ShowFindMenu()
{
	SetColor(bg_color);
	ClearPartial(0, YSIZE - 1, XSIZE, 1);

	SetColor(fg_color);
	fputs("F3", stdout);
	SetColor(bg_color);
	fputs(": Next occurrence | ", stdout);
	SetColor(fg_color);
	fputs("ESC", stdout);
	SetColor(bg_color);
	fputs(": Quit", stdout);
}

void ShowBottomMenu()
{
	PrintBottomString(NEWTRODIT_DIALOG_BOTTOM_HELP);
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
	SetColor(fg_color);
	ClearPartial(0, 0, XSIZE, 1);
	printf(" Newtrodit %s", newtrodit_version);
	SetColor(bg_color);
}

void DisplayCursorPos(int xps, int yps)
{
	size_t len = strlen(NEWTRODIT_DIALOG_BOTTOM_HELP);
	ClearPartial(len, YSIZE - 1, wrapSize - len, 1);
	if (longPosition) // I don't know why I added this
	{
		printf("Line %d, Column %d", yps, xps + 1); // Add one to xpos because it's zero indexed
	}
	else
	{
		printf("Ln %d, Col %d", yps, xps + 1);
	}
}

void LoadAllNewtrodit()
{
	CursorSettings(FALSE, CURSIZE); // Hide cursor to reduce flickering
	SetColor(bg_color);

	ClearPartial(0, 0, XSIZE, YSIZE);

	/* if (clearBufferScreen)
	{
		ClearPartial(0, 1, XSIZE, YSIZE - 2);
	}
	else
	{
	} */
	NewtroditNameLoad();
	CenterText(strlasttok(filename_text, PATHTOKENS), 0);
	RightAlignNewline();
	ShowBottomMenu();
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
	int errno_temp = errno;
	if (!errno_temp)
	{
		errno_temp = crash_retval;
	}
	Alert();
	printf("Newtrodit ran into a problem and it crashed. We're sorry.\nReport this issue to %s/issues\n\nDebug info:\nerrno: 0x%x (%s)\nGetLastError: 0x%lx\n\nReason: %s\n\nPress enter to exit...\n", newtrodit_repository, errno_temp, strerror(errno_temp), GetLastError(), crash_reason);
	DisplayCursor(true);
	getchar();

	ExitRoutine(crash_retval);
}

int QuitProgram(int color_quit)
{
	if (isModified)
	{
		PrintBottomString(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);

		if (YesNoPrompt())
		{
			SaveFile(str_save, strdup(filename_text), YSIZE, &isModified, &isUntitled);
		}
	}
	PrintBottomString(NEWTRODIT_PROMPT_QUIT);
	if (YesNoPrompt())
	{
		SetColor(color_quit);
		ClearPartial(0, 0, XSIZE, YSIZE);
		CursorSettings(TRUE, CURSIZE);
		ExitRoutine(0);
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

void PrintLine(char *line)
{
	horizontalScroll = xpos - (XSIZE - 2);
	if (horizontalScroll < 0)
	{
		horizontalScroll = 0;
	}
	if (syntaxHighlighting)
	{
		color_line(line + horizontalScroll, 0);
	}
	else
	{

		if (wrapLine)
		{
			printf("%s", line + horizontalScroll);
		}
		else
		{
			printf("%.*s", wrapSize, line + horizontalScroll);
		}
	}
}

void ToggleOption(int *option, char *text, int reloadScreen)
{
	PrintBottomString(text);
	*option = !*option;

	if (reloadScreen)
	{
		LoadAllNewtrodit();
		DisplayFileContent(str_save, stdout, 0);
	}

	*option ? PrintBottomString(join(text, NEWTRODIT_DIALOG_ENABLED)) : PrintBottomString(join(text, NEWTRODIT_DIALOG_DISABLED));
}
