#include <stdio.h>
#include <Windows.h>
#include "newtrodit_core.h"
// Define constants
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008
#define MAX_FILENAME 8192

const char manual_file[] = "newtrodit.man";
int NewtroditHelp()
{
	int variable_dollar_character;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	ClearScreen();
	gotoxy(0,0);
    SetColor(0x70);
	for(int n = 0; n < columns; n++)
	{
		printf(" ");
	}
	gotoxy(0,0);
	printf("Newtrodit help");
	SetColor(0x07);

   DWORD l_mode;
   HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
   GetConsoleMode(hStdout,&l_mode);
   SetConsoleMode( hStdout, l_mode |
                ENABLE_VIRTUAL_TERMINAL_PROCESSING |
                DISABLE_NEWLINE_AUTO_RETURN );
	char manual_read_buffer;
	FILE *manual;
	manual = fopen(manual_file, "rb");
	if(!manual)
	{
		printf("%s: %s", manual_file, strerror(errno));
		return errno;
	}
	int finished_manual = 0;
	while(finished_manual == 0)
	{
		printf("%d:%d", GetConsoleXDimension(), GetConsoleYDimension());

		for(int i = 0; i < GetConsoleYDimension()-4; i++)
		{
			if((manual_read_buffer = getc(manual))!= EOF)
			{
				if(manual_read_buffer == 36) // $
				{
					variable_dollar_character = getc(manual);
					if(variable_dollar_character == 86)
					{
						printf("%s", newtrodit_version);
					}
				}
				printf("%c", manual_read_buffer);
			} else {
				finished_manual = 1;
			}
		}
		gotoxy(0, GetConsoleYDimension());
		printf("^X  Close help  M-F4 Quit Newtrodit\r");
		getch();
		for(int i = 0; i < GetConsoleXDimension()-1; i++);
		{
			printf(" ");
		}
		

	}
	
}