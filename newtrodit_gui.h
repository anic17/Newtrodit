void TopHelpBar()
{
	gotoxy(0, 0);
	SetColor(0x70);
	ClearLine(0);
	gotoxy(0, 0);
	printf("Newtrodit help");
	SetColor(0x07);
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

void ShowBottomMenu()
{
	ClearLine(YSIZE - 2);
	ClearLine(YSIZE - 1);

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

int QuitProgram(int color_quit)
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
		SetColor(0x07);
	}
	return 0;
}

void NewtroditNameLoad()
{
	gotoxy(0,0);
	SetColor(0x70);
	ClearLine(0);
	gotoxy(0,0);
	printf(" Newtrodit %s", newtrodit_version);
	SetColor(0x07);
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

void LoadAllNewtrodit()
{
	SetColor(0x07);
	ClearScreen();
	NewtroditNameLoad();
	CenterFileName(filename_text);

	ShowBottomMenu();

	CursorSettings(TRUE, 20);
}