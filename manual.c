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

#include "newtrodit_core.h"
#include "newtrodit_gui.h"



int NewtroditHelp()
{
	CursorSettings(FALSE, 20);
	int manual_ch;
	ClearScreen();

	DWORD l_mode;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hStdout, &l_mode);
	SetConsoleMode(hStdout, l_mode |
								ENABLE_VIRTUAL_TERMINAL_PROCESSING |
								DISABLE_NEWLINE_AUTO_RETURN);
	char manual_read_buffer;
	FILE *manual;
	manual = fopen(manual_file, "rb");
	if (!manual)
	{
		char error_manual_not_found[512];
		sprintf(error_manual_not_found, "%s: %s", manual_file, strerror(errno));
		PrintBottomString(error_manual_not_found);
		getch();
		return errno;
	}
	int manual_buf_len = 0, old_manual_buf_len;
	int finished_manual = 1;
	char manual_buffer[8192];

	while (!feof(manual))
	{
		TopHelpBar();
		BottomHelpBar();
		old_manual_buf_len = manual_buf_len;
		manual_buf_len = 0;
		if (manual_ch != 0)
		{
			for (int i = 1; i < GetConsoleYDimension() - 2; i++)
			{

				if (fgets(manual_buffer, sizeof manual_buffer, manual))
				{
					manual_buffer[strlen(manual_buffer) - 1] = 0;
					printf("%s\n", manual_buffer);
				}
				manual_buf_len += strlen(manual_buffer);
			}
		}
		printf("\e[0m"); // Clear all text attributes
		manual_ch = getch();

		switch (manual_ch)
		{
		case 24:
			return 0;
			break;
		case 27:
			return 0;
			break;
		case 0:
			manual_ch = getch();
			if (manual_ch == 107) // M-F4
			{
				QuitProgram(0x07);
				manual_ch = 0;
			}
			break;
		case 224:

			manual_ch = getch();
			if (manual_ch == 81) // PGDW key
			{
				break;
			}
			if (manual_ch == 73) // PGUP key
			{
				ClearScreen();
				fseek(manual, -(old_manual_buf_len), SEEK_CUR);
			}
			if (manual_ch == 80) // Down arrow
			{
				ClearScreen();
				fseek(manual, -(old_manual_buf_len), SEEK_CUR);
				if (manual_ch != 0)
				{
					for (int i = 1; i < GetConsoleYDimension() - 2; i++)
					{

						if (fgets(manual_buffer, sizeof manual_buffer, manual))
						{
							manual_buffer[strlen(manual_buffer) - 1] = 0;
							printf("%s\n", manual_buffer);
						}
						manual_buf_len += strlen(manual_buffer);
					}
				}
			}
			break;
		default:
			break;
		}
		if (manual_ch != 0)
		{
			ClearScreen();
		}
		gotoxy(0, 1);
	}
	CursorSettings(TRUE, 20);
	return 0;
}