/*
	Newtrodit: A console text editor
	Copyright (C) 2021 anic17 Software

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
#pragma once

void DisplayLineCount(char *strsave[], int size, int yps);
int SaveFile(char *strsave[], char *filename, int size, int *is_modified, int *is_untitled);
int DisplayFileContent(char *strsave[], char newlinestring[], FILE *fstream);

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

void DisableVT()
{
	DWORD lmode; // Process ANSI escape sequences
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(hStdout, &lmode);
	SetConsoleMode(hStdout, lmode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
}

int NewtroditHelp()
{
	SetConsoleTitle("Newtrodit help");

	CursorSettings(FALSE, CURSIZE);
	int manual_ch;
	ClearScreen();
	TopHelpBar();

	FILE *manual;

	DWORD l_mode; // Process ANSI escape sequences
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(hStdout, &l_mode);
	SetConsoleMode(hStdout, l_mode |
								ENABLE_VIRTUAL_TERMINAL_PROCESSING |
								DISABLE_NEWLINE_AUTO_RETURN);
	SetColor(bg_color);

	const char reset_color[] = "\e[0m";

	int quit_manual = 0, end_manual = 0, max_manual_lines = 0;
	int man_line_count = 0, disable_clear = 0;

	int find_escape = false;

	char **manual_buf = calloc(MANUAL_BUFFER_Y, MANUAL_BUFFER_X); // Allocate 300 char pointers
	for (int i = 0; i < MANUAL_BUFFER_X; ++i)
	{
		manual_buf[i] = malloc(MANUAL_BUFFER_Y); // Allocate 2048 bytes for each string
	}

	char *gotoline_man = (char *)malloc(MANUAL_BUFFER_X);
	manual = fopen(manual_file, "r");
	if (!manual)
	{
		PrintBottomString(join(NEWTRODIT_ERROR_MISSING_MANUAL, strlasttok(manual_file, '\\')));
		CursorSettings(TRUE, CURSIZE);
		MakePause();
		DisableVT();
		return -1;
	}

	if (CountLines(manual) >= MANUAL_BUFFER_Y)
	{
		PrintBottomString(NEWTRODIT_ERROR_MANUAL_TOO_BIG);
		MakePause();
		DisableVT();
		return 1;
	}

	while (fgets(manual_buf[man_line_count], MANUAL_BUFFER_X, manual)) // Load manual into memory
	{
		if(man_line_count == 0 && strncmp(NEWTRODIT_MANUAL_MAGIC_NUMBER, manual_buf[man_line_count], strlen(NEWTRODIT_MANUAL_MAGIC_NUMBER)))
		{
			PrintBottomString(join(NEWTRODIT_ERROR_INVALID_MANUAL, manual_file));
			MakePause();
			return 1;
		}
		if (man_line_count + 1 >= MANUAL_BUFFER_X)
		{
			PrintBottomString(NEWTRODIT_ERROR_MANUAL_TOO_BIG);
			MakePause();
			DisableVT();
			return 1;
		}
		man_line_count++;
		max_manual_lines++;
	}

	man_line_count = 1; // Reset line count

	while (quit_manual == 0)
	{
		if (end_manual == 1)
		{
			quit_manual = 1;
		}
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
						for (int k = 0; k < (unsigned)strlen(manual_buf[man_line_count]); ++k)
						{

							if (manual_buf[man_line_count][k] == '$' && manual_buf[man_line_count][k + 1] != '\0')
							{
								find_escape = true;

								switch (manual_buf[man_line_count][k + 1])
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

						printf("%.*s\n", MANUAL_BUFFER_X, manual_buf[man_line_count]);
					}

					man_line_count++;
					printf("%s", reset_color); // Clear all text attributes
				}
				else
				{
					printf("%s", reset_color);

					man_line_count = max_manual_lines - (YSIZE - 4);
					disable_clear = false;
					quit_manual = 1;
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
				DisableVT();
				return 0;
				break;
			case 27: // ESC
				CursorSettings(TRUE, CURSIZE);
				DisableVT();
				return 0;
				break;
			case 0:

				manual_ch = getch();
				if (manual_ch == 107) // A-F4
				{
					QuitProgram(start_color);
					printf("%s", reset_color); // Clear all text attributes
					BottomHelpBar();
					manual_ch = 0;
					break;
				}
				if (manual_ch == 59) // F1
				{
					CursorSettings(TRUE, CURSIZE);
					DisableVT();
					for (int i = 0; i < MANUAL_BUFFER_Y; i++)
					{
						free(manual_buf[i]);
					}
					return 0;
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

					if ((man_line_count - (YSIZE - 3)) > (YSIZE - 3))
					{
						man_line_count -= (2 * (YSIZE - 3)); // Multiplied by 2
					}
					else
					{
						man_line_count = 1; // Set manual position to first line
					}

					break;
				case 71: // HOME key
					printf("%s", reset_color);

					ClearScreen();
					man_line_count = 1;
					disable_clear = true;
					break;
				case 119: // ^HOME key
					printf("%s", reset_color);

					man_line_count = 1;
					disable_clear = true;
					break;

				case 79: // END key
					printf("%s", reset_color);

					ClearScreen();
					man_line_count = max_manual_lines - (YSIZE - 4);
					break;
				case 117: // ^END key
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
					// man_line_count = man_line_count - (1 * (YSIZE - 3));

					break;
				}
				break;
			case 7: // ^G
				PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);

				gotoline_man = TypingFunction('0', '9', strlen(itoa_n(MANUAL_BUFFER_Y)));
				if (atoi(gotoline_man) < 1 || atoi(gotoline_man) > max_manual_lines || atoi(gotoline_man) >= MANUAL_BUFFER_Y) // Line is less than 1
				{
					PrintBottomString(NEWTRODIT_ERROR_INVALID_YPOS);
					MakePause();
					man_line_count = man_line_count - (1 * (YSIZE - 3));
					continue;
				}
				else
				{
					if ((man_line_count - (1 * (YSIZE - 3) + 1)) > 0)
					{
						man_line_count = atoi(gotoline_man) - (1 * (YSIZE - 3) + 1);
					}
				}
				ClearScreen();
				break;
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
				ClearBuffer();
			}
		}
	}
	for (int i = 0; i < MANUAL_BUFFER_Y; i++)
	{
		free(manual_buf[i]);
	}
	CursorSettings(TRUE, CURSIZE);
	DisableVT();
	return 0;
}