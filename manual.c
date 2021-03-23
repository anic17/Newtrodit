#include "newtrodit_core.h"


void TopHelpBar()
{
	ClearScreen();
	gotoxy(0,0);
    SetColor(0x70);
	ClearLine(0);
	gotoxy(0,0);
	printf("Newtrodit help");
	SetColor(0x07);
}


int NewtroditHelp()
{
	int variable_dollar_character;

	TopHelpBar();
	gotoxy(0,0);

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
		char error_manual_not_found[512];
		strcpy(error_manual_not_found, manual_file);
		strcat(error_manual_not_found, ": ");
		strcat(error_manual_not_found, strerror(errno));
		PrintBottomString(error_manual_not_found);
		return errno;
	}
	int finished_manual = 1;
	char manual_buffer[8192];

	int y_console_size = YSIZE;
	while(fgets(manual_buffer, sizeof manual_buffer, manual))
	{

		for(int i = 0; i < GetConsoleYDimension()-1; i++)
		{
			
			//printf("it's being executed\n");
			if(fgets(manual_buffer, sizeof manual_buffer, manual))
			{
				printf("%s", manual_buffer);
			}
			
			

		}

		SetColor(0x70);
		ClearLine(y_console_size-1);

		PrintBottomString("^X  Close help  M-F4 Quit Newtrodit\r");
		SetColor(0x07);
		getch();
		TopHelpBar();
		y_console_size+YSIZE;


	}

	
	return 10;


	
	
}