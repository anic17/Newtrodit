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

int GetNumberFilesInDir(char *search_pattern, unsigned long long *totalsize)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFiles;
	size_t count = 0;
	*totalsize = 0;

	if ((hFindFiles = FindFirstFile(search_pattern, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hFindFiles, &FindFileData))
		{
			count++;
			*totalsize += FindFileData.nFileSizeHigh + FindFileData.nFileSizeLow;
		}
	}
	else
	{
		WriteLogFile(GetErrorDescription(GetLastError()));
	}
	return count;
}

void BottomLocateFiles(int index, int n, int total, unsigned long long totalsize)
{
	if (!(n & BIT_ESC224))
	{
		PrintBottomString(NEWTRODIT_DIALOG_LOCATE_POS, index, index + n, total, totalsize);
	}
}

void LoadLocateFilesUI(int box_x, int box_y, int box_margin_x, int box_margin_y, int index, int n, int total, unsigned long long totalsize, bool bottomMenu)
{
	SetTitle("Newtrodit - File browsing - %s", getcwd(NULL, 0));
	ClearPartial(box_x, box_y, XSIZE - 2 * box_x, YSIZE - 2 * box_y);
	SetCursorSettings(false, GetConsoleInfo(CURSOR_SIZE));
	gotoxy(box_x, box_y);
	printf("Current directory: %s\n", getcwd(NULL, 0));
	if (bottomMenu)
	{
		BottomLocateFiles(index, n, total, totalsize);
	}
}

void SetAsteriskSelection(int x0, int y0, int selected, int total)
{
	ClearPartial(x0, y0, 1, YSIZE - (2 * y0)+1);
	gotoxy(x0, y0 + selected);
	putchar('*');
	return;
}

void QuitLocate(char **b_filename)
{
	for (int i = 0; i < DEFAULT_ALLOC_SIZE; i++)
	{
		free(b_filename[i]);
	}
	free(b_filename);
	return;
}

int LocateFiles(bool show_dir, char *file, int startpos)
{
	int n = startpos, total = startpos, index = startpos;

	unsigned int key = 0;

	bool isWildcard = false, quitFindFiles = false, showNextPage = true;
	HANDLE hFindFiles;
	WIN32_FIND_DATA FindFileData;
	int ys = YSIZE;
	int max_print = wrapSize;
	char *search_pattern = "*";
	SYSTEMTIME st;

	if (strpbrk(file, "*?") != NULL)
	{
		search_pattern = file;
		isWildcard = true;
	}
	else
	{
		if (!ValidFileName(file))
		{
			last_known_exception = NEWTRODIT_FS_FILE_INVALID_NAME;

			PrintBottomString(NEWTRODIT_FS_FILE_INVALID_NAME, file);
			return 0;
		}
	}
	char *dir_tmp = calloc(MAX_PATH * 2, sizeof(char)), *out_dir = calloc(MAX_PATH * 2, sizeof(char));
	static char *typeptr = NULL;
	GetCurrentDirectory(MAX_PATH, dir_tmp);

	if (!isWildcard && get_path_directory(file, out_dir))
	{
		SetCurrentDirectory(out_dir);
		file = StrLastTok(file, PATHTOKENS);
	}
	SetColor(fg_color);
	int box_x = 5, box_y = 2, box_margin_x = 4, box_margin_y = 1;

	LoadLocateFilesUI(box_x, box_y, box_margin_x, box_margin_y, 0, 0, 0, 0, true);
	unsigned long long totalsize = 0;
	total = GetNumberFilesInDir(search_pattern, &totalsize);

	if (!total)
	{
		PrintBottomString("%s%s", NEWTRODIT_FS_FILE_NOT_FOUND, file);
		last_known_exception = NEWTRODIT_FS_FILE_NOT_FOUND;
		SetColor(bg_color);
		chdir(dir_tmp);
		return 0;
	}

	// For file selection and opening: Keep the file names
	int selection_index = -1;
	unsigned int browse_index = 0;
	char **b_filenames = calloc(sizeof(char *), DEFAULT_ALLOC_SIZE + 1);
	for (int i = 0; i < DEFAULT_ALLOC_SIZE; i++)
	{
		b_filenames[i] = calloc(MAX_PATH + 1, sizeof(char));
	}

	if ((hFindFiles = FindFirstFile(search_pattern, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		while (!quitFindFiles)
		{
			ys = YSIZE;

			if (index <= total && showNextPage)
			{

				LoadLocateFilesUI(box_x, box_y, box_margin_x, box_margin_y, index, n, total, totalsize, true);
				browse_index = 0;
				for (int i = 0; i < (ys - (box_y+3)); i++) // Display listed files
				{
					if ((FindString(FindFileData.cFileName, file) != -1 || isWildcard) && index < total) // Ignore the search result if wildcards are used
					{
						if (browse_index < DEFAULT_ALLOC_SIZE)
						{
							strncpy_n(b_filenames[browse_index++], FindFileData.cFileName, MAX_PATH);
						}
						gotoxy(box_x + box_margin_x, n + box_margin_y + box_y);

						FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &st);
						if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && show_dir) // Time format is "YYYY/MM/DD hh:mm:ss"
						{
							printf(" %04d/%02d/%02d %02d:%02d:%02d\t<DIR>\t%14lu\t%.*s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindFileData.nFileSizeLow, max_print, FindFileData.cFileName);
						}
						else
						{
							printf(" %04d/%02d/%02d %02d:%02d:%02d\t\t%14lu\t%.*s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindFileData.nFileSizeLow, max_print, FindFileData.cFileName);
						}

						index++;
						n++;

						if (!FindNextFile(hFindFiles, &FindFileData))
						{
							index = total;
							break;
						}
					}
				}
			}
			key = getch_n();
			switch (key)
			{
			
			case (72 | 0x80000000):
				if (selection_index > 0)
				{
					selection_index--;
				}
				showNextPage = false;
				SetAsteriskSelection(box_x + box_margin_x-1, box_y + box_margin_y, selection_index, n);
				break;
			case (80 | 0x80000000):
				if (selection_index < 0)
				{
					selection_index = 0;
				}
				else
				{
					if (selection_index < n-1)
					{
						selection_index++;
					}
				}
				showNextPage = false;
				SetAsteriskSelection(box_x + box_margin_x-1, box_y + box_margin_y, selection_index, n);
				break;

			case ' ':
			case PGDW | BIT_ESC224:

				if (index + n <= total)
				{
					showNextPage = true;
					n = 0;
				}
				else
				{
					PrintBottomString("%s%s", NEWTRODIT_LOCATE_END_REACHED, NEWTRODIT_DIALOG_BOTTOM_LOCATE);
					getch_n();
				}
				break;

			case ENTER:
				if (CheckKey(VK_RETURN))
				{
					if (selection_index >= 0)
					{
						OpenNewtroditFile(&Tab_stack[file_index], b_filenames[selection_index]);
						QuitLocate(b_filenames);
						return 0;
					}
				}
				break;

			case CTRLC:

				if (SetClipboardNewtrodit(getcwd(NULL, 0)))
				{
					PrintBottomString(NEWTRODIT_CLIPBOARD_COPIED, getcwd(NULL, 0));
					n |= BIT_ESC224;
				}
				else
				{
					PrintBottomString("%s", NEWTRODIT_ERROR_CLIPBOARD_COPY);
					getch_n();
				}
				break;

			case CTRLB:
			case ALTUP | BIT_ESC0:
			case BS:
			case CTRLD:

				showNextPage = false;
				if (key == CTRLD)
				{
					PrintBottomString("Directory: ");
					typeptr = TypingFunction(32, 255, MAX_PATH, NULL);
					if (typeptr[0] == '\0')
					{
						break;
					}
					if (!CheckFile(typeptr))
					{
						if (!(GetFileAttributes(typeptr) & FILE_ATTRIBUTE_DIRECTORY))
						{
							PrintBottomString(NEWTRODIT_FS_NOT_A_DIRECTORY, typeptr);
							getch_n();
							break;
						}
						SetCurrentDirectory(typeptr);
					}
					else
					{
						PrintBottomString(NEWTRODIT_FS_DIR_NOT_FOUND, typeptr);
						getch_n();
						break;
					}
					free(typeptr); // Free the memory allocated by TypingFunction()
				}
				else
				{
					SetCurrentDirectory("..\\");
				}
				total = GetNumberFilesInDir(search_pattern, &totalsize);
				index = 0;
				hFindFiles = FindFirstFile(search_pattern, &FindFileData);
				showNextPage = true;
				break;

			case CTRLN:
			case 'n':
			case 'N':
				PrintBottomString("Directory to create: ");
				typeptr = TypingFunction(32, 255, MAX_PATH, NULL);
				if (typeptr[0] == '\0')
				{
					free(typeptr);
					break;
				}
				if (CreateDirectory(typeptr, NULL))
				{
					PrintBottomString("Directory '%s' successfully created", typeptr);
				}
				else
				{
					PrintBottomString("Failed to create directory '%s'", typeptr);
				}
				getch_n();
				free(typeptr);
				break;

			case ESC:
			case CTRLL:
			case CTRLX:
			case CTRLQ:
			case F4 | BIT_ESC0:
			case 'q':
			case 'Q':
				quitFindFiles = true;
				break;

			default:
				showNextPage = false;
				break;
			}
		}
	}

	SetColor(bg_color);

	chdir(dir_tmp);
	free(dir_tmp);
	free(out_dir);
	SetCursorSettings(true, GetConsoleInfo(CURSOR_SIZE));

	return 1;
}
