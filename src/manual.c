/*
	Newtrodit: A console text editor
	Copyright (c) 2021-2023 anic17 Software

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

#ifdef _WIN32
#include "win32/newtrodit_core_win.h"
#else
#include "linux/newtrodit_core_linux.h"
#endif
#include "newtrodit_syntax.h"

enum
{
	BOLD_INDEX,
	ITALIC_INDEX,
	UNDERLINE_INDEX,
	STRIKE_INDEX,
};

void DisplayLineCount(File_info *tstack, int size, int yps);
int SaveFile(File_info *tstack, char *fname);
int DisplayFileContent(File_info *tstack, FILE *fstream, int starty);
int NewtroditHelp();

#include "newtrodit_gui.c"

#ifdef _WIN32
#include "win32/newtrodit_func_win.c"
#include "newtrodit_locate.c"
#else
#include "linux/newtrodit_func_linux.c"
#endif
#include "newtrodit_api.c"
#include "newtrodit_shared.c"


int DownArrow(int man_line_count, int maxlines)
{
	int ys = YSIZE;
	if (man_line_count >= maxlines)
	{
		man_line_count -= (ys - 3); // Don't actually scroll
	}
	else
	{
		man_line_count -= (ys - 4); // Scroll down by 1 line
	}

	return man_line_count;
}

int ParseMarkdown(char *line)
{
	char *styles[] = {
		"\x1b[1m",
		"\x1b[3m",
		"\x1b[4m",
		"\x1b[9m",
	};
	char *reset[] = {
		"\x1b[22m",
		"\x1b[23m",
		"\x1b[24m",
		"\x1b[29m",
	};
	bool is_bold = false, is_italic = false, is_underline = false, is_strike = false;
	size_t len = strlen(line), skipchars = 0;
	bool used = 0;
	for (size_t i = 0; i < strlen(line); i++)
	{
		if (!strncmp(line + i, "**", 2))
		{
			is_bold = !is_bold;
			is_bold ? printf("%s", styles[BOLD_INDEX]) : printf("%s", reset[BOLD_INDEX]);
			i++;
			used = false;
			continue;
		}
		else if (!strncmp(line + i, "__", 2))
		{
			is_underline = !is_underline;
			is_underline ? printf("%s", styles[UNDERLINE_INDEX]) : printf("%s", reset[UNDERLINE_INDEX]);
			i++;
			used = false;
			continue;
		}
		else if (!strncmp(line + i, "~~", 2))
		{
			is_strike = !is_strike;
			is_strike ? printf("%s", styles[STRIKE_INDEX]) : printf("%s", reset[STRIKE_INDEX]);
			i++;
			used = false;
			continue;
		}
		else if (!strncmp(line + i, "*", 1) || !strncmp(line + i, "_", 1))
		{
			is_italic = !is_italic;
			is_italic ? printf("%s", styles[ITALIC_INDEX]) : printf("%s", reset[ITALIC_INDEX]);
			used = false;
			continue;
		}
		else if (!strncmp(line + i, "\\", 1))
		{
			if (i < len)
			{
				putchar(line[++i]);
			}
			else
			{
				putchar(line[i]);
			}
			used = false;
		}
		else if (!strncmp(line + i, ".  ", 3))
		{
			printf(".\n");
			i += 2;
			continue;
			used = false;
		}
		else
		{
			skipchars++;
			used = true;
		}
		if (!used && skipchars != 0)
		{
			fwrite(line + i, sizeof(char), skipchars, stdout);
			skipchars = 0;
			used = false;
		}
	}
	return 0;
}

void FreeManual(char **manbuf)
{
	for (int i = 0; i < MANUAL_BUFFER_Y; i++)
	{
		free(manbuf[i]);
	}
	free(manbuf);
}

void QuitManual(char **manbuf)
{
	FreeManual(manbuf);
	SetCursorSettings(true, GetConsoleInfo(CURSOR_SIZE));
	VTSettings(false);
	chdir(SInf.dir);
}

int NewtroditHelp()
{

	chdir(SInf.location);

	SetTitle("Newtrodit help");
	SetColor(BG_DEFAULT); // Don't change manual color (Maybe in a future update?)

	SetCursorSettings(false, GetConsoleInfo(CURSOR_SIZE));
	ClearScreen();

	TopHelpBar();
	WriteLogFile("Opening manual for reading");
	FILE *manual = fopen(manual_file, "rb");
	if (!manual)
	{
		PrintBottomString("%s%s", NEWTRODIT_ERROR_MISSING_MANUAL, StrLastTok(manual_file, PATHTOKENS));
		WriteLogFile("%s%s", NEWTRODIT_ERROR_MISSING_MANUAL, StrLastTok(manual_file, PATHTOKENS));

		SetCursorSettings(true, GetConsoleInfo(CURSOR_SIZE));
		getch_n();
		chdir(SInf.dir);

		return 1;
	}

	if (CountLines(manual) >= MANUAL_BUFFER_Y)
	{
		PrintBottomString("%s", NEWTRODIT_ERROR_MANUAL_TOO_BIG);
		WriteLogFile("%s", NEWTRODIT_ERROR_MANUAL_TOO_BIG);
		getch_n();
		chdir(SInf.dir);

		return 1;
	}

	if (!VTSettings(true) && !RGB24bit) // Process ANSI escape sequences and check if the console supports ANSI escape sequences
	{
		PrintBottomString("%s", NEWTRODIT_ERROR_LOADING_MANUAL);
		WriteLogFile("%s", NEWTRODIT_ERROR_LOADING_MANUAL);
		getch_n();
		chdir(SInf.dir);

		return 1;
	}

	char reset_color[] = "\x1b[0m";

	bool finished_manual = false;
	int man_line_count = 0, max_manual_lines = 0, manual_ch;
	char escape_char = '$';
	int escape_count = 0;
	char* strint;

	char **manual_buf = calloc(sizeof(char *), MANUAL_BUFFER_Y); // Allocate char pointers
	for (int i = 0; i < MANUAL_BUFFER_Y; ++i)
	{
		manual_buf[i] = calloc(MANUAL_BUFFER_X, sizeof(char)); // Allocate bytes for each string
	}

	char gotoline_man[MAX_PATH] = {0};

	while (fgets(manual_buf[man_line_count], MANUAL_BUFFER_X, manual)) // Load manual into memory
	{
		if (man_line_count == 0 && memcmp(NEWTRODIT_MANUAL_MAGIC_NUMBER, manual_buf[man_line_count], strlen(NEWTRODIT_MANUAL_MAGIC_NUMBER)))
		{
			PrintBottomString("%s%s", NEWTRODIT_ERROR_INVALID_MANUAL, manual_file);
			WriteLogFile("%s%s", NEWTRODIT_ERROR_INVALID_MANUAL, manual_file);
			getch_n();
			QuitManual(manual_buf);
			return 1;
		}
		if (man_line_count + 1 >= MANUAL_BUFFER_Y)
		{
			PrintBottomString("%s", NEWTRODIT_ERROR_MANUAL_TOO_BIG);
			WriteLogFile("%s", NEWTRODIT_ERROR_MANUAL_TOO_BIG);

			getch_n();
			QuitManual(manual_buf);
			return 1;
		}
		man_line_count++;
		max_manual_lines++;
	}
	WriteLogFile("Read %d lines from the the manual file (%s)", max_manual_lines, manual_file);
	man_line_count = 1; // Reset line count
	BottomHelpBar();

	while (!finished_manual)
	{

		for (int i = 0; i < YSIZE - 3; i++)
		{
			SetColor(BG_DEFAULT);
			if (man_line_count < 1)
			{
				man_line_count = 1;
			}
			if (man_line_count < max_manual_lines)
			{
				gotoxy(0, i + 1);
				manual_buf[man_line_count][strcspn(manual_buf[man_line_count], "\n")] = 0;

				if (strrchr(manual_buf[man_line_count], escape_char) != 0)
				{
					for (size_t k = 0; k < strlen(manual_buf[man_line_count]); ++k)
					{
						escape_count = 0;

						if (manual_buf[man_line_count][k] == escape_char)
						{
							switch (manual_buf[man_line_count][k + 1])
							{
							case 'Y':
								printf("%d", BUFFER_Y);
								k++;
								break;
							case 'X':
								printf("%d", BUFFER_X);
								k++;
								break;
							case 'I':
								printf("%d", BUFFER_INCREMENT);
								k++;
								break;
							case 'C':
								printf("%s", newtrodit_commit);
								k++;
								break;
							case 'B':
								printf("%s", newtrodit_build_date);
								k++;
								break;
							case 'V':
								printf("%s", newtrodit_version);
								k++;
								break;
							case 'G':
								printf("%s", newtrodit_repository);
								k++;
								break;
							case 'T':
								printf("%d", MAX_TABS);
								k++;
								break;
							default:
								printf("%c", manual_buf[man_line_count][++k]);
							}
						}
						else
						{
							while (manual_buf[man_line_count][k + escape_count] != escape_char)
							{
								escape_count++;
							}
							printf("%.*s", escape_count, manual_buf[man_line_count] + k);
							k += escape_count - 1;
						}
					}
				}
				else
				{
#if 0
									ParseMarkdown(manual_buf[man_line_count]);
#else

					printf("%.*s\n", MANUAL_BUFFER_X, manual_buf[man_line_count]);
#endif
				}
				man_line_count++;
			}
			else
			{
				getch_n();
				return 0;
			}
		}

		manual_ch = getch();
		switch (manual_ch)
		{
		case 24: // ^X
			QuitManual(manual_buf);
			return 0;
		case 27: // ESC
			QuitManual(manual_buf);
			return 0;
		case 0:

			manual_ch = getch();
			if (manual_ch == ALTF4) // A-F4
			{
				SetColor(FG_DEFAULT);
				QuitProgram(SInf.color);
				fputs(reset_color, stdout); // Clear all text attributes
				BottomHelpBar();
				manual_ch = 0;
				man_line_count -= YSIZE +3;
				break;
			}
			if (manual_ch == F1) // F1
			{
				QuitManual(manual_buf);

				chdir(SInf.dir);

				return 0;
			}

			break;
		case ENTER: // Enter
			fputs(reset_color, stdout);
			man_line_count = DownArrow(man_line_count, max_manual_lines);
			break;
		case 224:
			manual_ch = getch();
			switch (manual_ch)
			{
			case PGUP: // PageUp
				fputs(reset_color, stdout);

				if (man_line_count > 0)
				{
					man_line_count -= (2 * (YSIZE - 3)); // Multiplied by 2
				}
				else
				{
					man_line_count = 1; // Set manual position to first line
				}

				break;
			case PGDW: // PageDown
				fputs(reset_color, stdout);
				if (man_line_count+YSIZE-3 > max_manual_lines)
				{
					man_line_count = max_manual_lines-YSIZE+3;
				}
				break;
			case LEFT:
				fputs(reset_color, stdout);

				if (man_line_count > 0)
				{
					man_line_count -= (2 * (YSIZE - 3)); // Multiplied by 2
				}
				else
				{
					man_line_count = 1; // Set manual position to first line
				}
			case RIGHT:
				fputs(reset_color, stdout);
				if (man_line_count+YSIZE-3 > max_manual_lines)
				{
					man_line_count = max_manual_lines-YSIZE+3;
				}
				break;
			case HOME: // HOME key
				fputs(reset_color, stdout);

				ClearPartial(0, 1, XSIZE, YSIZE - 2);
				man_line_count = 1;
				break;
			case CTRLHOME: // ^HOME key
				fputs(reset_color, stdout);

				man_line_count = 1;
				break;

			case END: // END key
				fputs(reset_color, stdout);

				ClearPartial(0, 1, XSIZE, YSIZE - 2);

				man_line_count = max_manual_lines - (YSIZE - 3);
				break;
			case CTRLEND: // ^END key
				fputs(reset_color, stdout);
				ClearPartial(0, 1, XSIZE, YSIZE - 2);
				man_line_count = max_manual_lines - (YSIZE - 3);
				break;

			case UP: // Up arrow
				fputs(reset_color, stdout);
				if ((man_line_count - (1 * (YSIZE - 3) + 1)) > 0)
				{
					man_line_count = man_line_count - (YSIZE - 2);
				}
				else
				{
					man_line_count = man_line_count - ((YSIZE - 3));
				}
				break;
			case DOWN: // Down arrow
				fputs(reset_color, stdout);
				man_line_count = DownArrow(man_line_count, max_manual_lines);

				break;
			default:
				// man_line_count = man_line_count - (1 * (YSIZE - 3));

				break;
			}
			break;
		case CTRLG: // ^G
			SetColor(FG_DEFAULT);
			PrintBottomString(NEWTRODIT_PROMPT_GOTO_LINE);
			strint = itoa_n(max_manual_lines);
			strncpy_n(gotoline_man, TypingFunction('0', '9', strlen(strint), NULL), strlen(strint));
			free(strint);
			if (atoi(gotoline_man) < 0 || atoi(gotoline_man) > max_manual_lines || atoi(gotoline_man) >= MANUAL_BUFFER_Y) // Line is less than 1
			{
				PrintBottomString(NEWTRODIT_ERROR_MANUAL_INVALID_LINE, max_manual_lines);
				getch_n();
				man_line_count -= YSIZE - 3;
			}
			else
			{
				if ((man_line_count > 0 - (YSIZE + 2)))
				{
					man_line_count = atoi(gotoline_man) - (YSIZE - 3) + 1;
				}
			}
			BottomHelpBar();
			break;
		default:

			break;
		}
		ClearPartial(0, 1, XSIZE, YSIZE - 2);
	}
	QuitManual(manual_buf);

	return 0;
}