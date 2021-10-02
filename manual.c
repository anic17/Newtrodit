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
#include "newtrodit_gui.c"
#include "newtrodit_func.c"

int DownArrow(int man_line_count)
{
	if ((man_line_count - (1 * (YSIZE - 3) - 1)) > 0)
	{
		man_line_count = man_line_count - (1 * (YSIZE - 3) - 1);
	}
	else
	{
		man_line_count = man_line_count - (1 * (YSIZE - 3));
	}
	return man_line_count;
}

int NewtroditHelp()
{
	SetConsoleTitle("Newtrodit help");

	CursorSettings(FALSE, CURSIZE);
	int manual_ch;
	ClearScreen();

	gotoxy(0, 0);
	TopHelpBar();

	DWORD l_mode; // Process ANSI escape sequences
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(hStdout, &l_mode);
	SetConsoleMode(hStdout, l_mode |
								ENABLE_VIRTUAL_TERMINAL_PROCESSING |
								DISABLE_NEWLINE_AUTO_RETURN);
	SetColor(bg_color);

	FILE *manual;
	manual = fopen(manual_file, "rb");
	if (!manual)
	{
		char error_manual_not_found[512];
		sprintf(error_manual_not_found, "%s: %s", manual_file, strerror(errno));
		PrintBottomString(error_manual_not_found);
			CursorSettings(TRUE, CURSIZE);
		MakePause();
		return errno;
	}
	int quit_manual = 0, max_manual_lines = 0;
	int find_escape = false;

	int man_line_count = 1, disable_clear = 0;
	char **manual_buf = calloc(MANUAL_BUFFER_Y, MANUAL_BUFFER_X); // Allocate 300 char pointers
	for (int i = 0; i < MANUAL_BUFFER_X; ++i)
	{
		manual_buf[i] = malloc(MANUAL_BUFFER_Y); // Allocate 2048 bytes for each string
	}

	while (fgets(manual_buf[man_line_count], MANUAL_BUFFER_X, manual)) // Load manual into memory
	{
		if (man_line_count + 1 >= MANUAL_BUFFER_X)
		{
			PrintBottomString(NEWTRODIT_ERROR_MANUAL_TOO_BIG);
			MakePause();
			return 1;
		}
		man_line_count++;
		max_manual_lines++;
	}
	man_line_count = 1; // Reset line count

	while (quit_manual == 0)
	{

		if (manual_ch != 0)
		{
			TopHelpBar();
			BottomHelpBar();
			for (int i = 1; i < YSIZE - 2; i++)
			{

				if (man_line_count <= max_manual_lines)
				{
					gotoxy(0, i);
					if (manual_buf[man_line_count][strlen(manual_buf[man_line_count]) - 1] == '\n')
					{
						manual_buf[man_line_count][strlen(manual_buf[man_line_count]) - 1] = '\0';
					}
					if (FindString(manual_buf[man_line_count], "$") != -1)
					{
						for (int k = 0; k < strlen(manual_buf[man_line_count]); ++k)
						{

							if (manual_buf[man_line_count][k] == '$' && manual_buf[man_line_count][k + 1] != '\0')
							{
								find_escape = true;

								switch (manual_buf[man_line_count][k+1])
								{

								case 'X':
									printf("%d", BUFFER_X);
									break;
								case 'Y':
									printf("%d", BUFFER_Y);

									break;
								case 'B':
									printf("%s", newtrodit_build_date);

									break;
								case 'V':
									printf("%s", newtrodit_version);
									break;
								case 'C':
									printf("%s", newtrodit_commit);
									break;
								case '$':
									putchar('$');
									break;

								default:
								
									break;
								}
							}
							else
							{
								if (find_escape)
								{
									find_escape = !find_escape;
								}
								else
								{
									putchar(manual_buf[man_line_count][k]);
								}
							}
						}
						putchar('\n');
					}
					else
					{
						printf("%s\n", manual_buf[man_line_count]);
					}

					man_line_count++;
					printf("%s", reset_color); // Clear all text attributes
				}
				else
				{
					quit_manual = 1;
					printf("%s", reset_color);
				}
			}
		}

		if (quit_manual == 0)
		{
			manual_ch = getch();
			switch (manual_ch)
			{
			case 24: // ^X
				CursorSettings(TRUE, CURSIZE);

				return 0;
				break;
			case 27: // ESC
				CursorSettings(TRUE, CURSIZE);

				return 0;
				break;
			case 0:
				manual_ch = getch();
				if (manual_ch == 107) // A-F4
				{
					QuitProgram(bg_color);
					printf("%s", reset_color); // Clear all text attributes
					BottomHelpBar();
					manual_ch = 0;
					break;
				}
				break;
			case 13: // Enter
				printf("%s", reset_color);
				man_line_count = DownArrow(man_line_count);
				break;
			case 224:
				manual_ch = getch();
				switch (manual_ch)
				{
				case 73: // PageUp
					printf("%s", reset_color);

					ClearScreen();

					if ((man_line_count - (YSIZE - 3)) > (YSIZE - 3))
					{
						man_line_count -= (2 * (YSIZE - 3)); // Multiplied by 2
					}
					else
					{
						disable_clear = true;
						man_line_count = 1; // Set manual position to first line
					}
					break;
				case 71: // HOME key
					printf("%s", reset_color);

					ClearScreen();
					man_line_count = 1;
					disable_clear = true;
					break;

				case 79: // END key
					printf("%s", reset_color);

					ClearScreen();
					man_line_count = max_manual_lines - (YSIZE - 4);
					break;

				case 72: // Up arrow
					printf("%s", reset_color);
					if ((man_line_count - (1 * (YSIZE - 3) + 1)) > 0)
					{
						man_line_count = man_line_count - (1 * (YSIZE - 3) + 1);
					}
					else
					{
						man_line_count = man_line_count - (1 * (YSIZE - 3));
						disable_clear = true;
					}
					break;
				case 80: // Down arrow
					printf("%s", reset_color);
					man_line_count = DownArrow(man_line_count);
					break;
				default:
					//man_line_count = man_line_count - (1 * (YSIZE - 3));

					break;
				}
			default:

				break;
			}
		}

		if (manual_ch != 0)
		{
			if (disable_clear == 1)
			{
				disable_clear = 0;
			}
			else
			{
				ClearScreen();
			}
		}
		gotoxy(0, 1);
	}
	for (int i = 0; i < MANUAL_BUFFER_Y; i++)
	{
		free(manual_buf[i]);
	}
	CursorSettings(TRUE, CURSIZE);
	return 0;
}