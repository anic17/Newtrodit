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
	SetColor(0x70);
	ClearLine(0);
	gotoxy(0, 0);
	printf(" Newtrodit help");
	SetColor(0x07);
	gotoxy(0, 1);
	return;
}

void BottomHelpBar()
{
	SetColor(0x70);
	ClearLine(YSIZE - 1);
	PrintBottomString("^X  Close help  M-F4 Quit Newtrodit");
	SetColor(0x07);
	gotoxy(0, 1);
	return;
}

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

	SetColor(0x07);
	printf(NEWTRODIT_DIALOG_BASIC_SHORTCUTS);
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
	for (int i = 0; i < strlen(NEWTRODIT_DIALOG_BASIC_SHORTCUTS); i++)
	{
		printf(" ");
	}
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

int QuitProgram(int color_quit)
{
	ClearLine(GetConsoleYDimension() - 1);
	PrintBottomString(NEWTRODIT_PROMPT_QUIT);
	int confirmquit = getch();
	if (toupper(confirmquit) == 89) // Y was answered
	{
		SetColor(color_quit);
		ClearScreen();
		RestoreBuffer();
		exit(0);
	}
	else
	{

		if (confirmquit == 0 && getch() == 107)
		{
			printf("\a");
		}
		ClearLine(GetConsoleYDimension() - 1);
		ShowBottomMenu();
		SetColor(0x07);
	}
	return 0;
}

void NewtroditNameLoad()
{
	gotoxy(0, 0);
	SetColor(0x70);
	ClearLine(0);
	gotoxy(0, 0);
	printf(" Newtrodit %s", newtrodit_version);
	SetColor(0x07);
}

void LoadAllNewtrodit()
{
	ClearScreen();
	NewtroditNameLoad();
	CenterFileName(filename_text);

	ShowBottomMenu();

	CursorSettings(TRUE, CURSIZE);
}