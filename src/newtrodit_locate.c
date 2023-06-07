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

int GetNumberFilesInDir(char *search_pattern)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFiles;
	size_t count = 0;
	if ((hFindFiles = FindFirstFile(search_pattern, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hFindFiles, &FindFileData))
		{
			count++;
		}
	}
	else
	{
		WriteLogFile(GetErrorDescription(GetLastError()));
	}
	return count;
}

void BottomLocateFiles(int index, int n, int total)
{
	PrintBottomString(NEWTRODIT_DIALOG_LOCATE_POS, index, index + n, total);
}

void LoadLocateFilesUI(int box_x, int box_y, int box_margin_x, int box_margin_y, int index, int n, int total, bool bottomMenu)
{
	ClearPartial(box_x, box_y, XSIZE - 2 * box_x, YSIZE - 2 * box_y);
	SetCursorSettings(false, GetConsoleInfo(CURSOR_SIZE));
	gotoxy(box_x, box_y);
	printf("Current directory: %s\n", getcwd(NULL, 0));
	if (bottomMenu)
	{
		BottomLocateFiles(index, n, total);
	}
}

int LocateFiles(bool show_dir, char *file, int startpos)
{
	int n = startpos, total = startpos, index = startpos;

	int key = 0;

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
	int box_x = 5;
	int box_y = 2;
	int box_margin_x = 4;
	int box_margin_y = 1;

	LoadLocateFilesUI(box_x, box_y, box_margin_x, box_margin_y, 0, 0, 0, true);
	// Get all files in the current directory

	total = GetNumberFilesInDir(search_pattern);

	if (!total)
	{
		PrintBottomString("%s%s", NEWTRODIT_FS_FILE_NOT_FOUND, file);
		last_known_exception = NEWTRODIT_FS_FILE_NOT_FOUND;

		chdir(dir_tmp);
		return 0;
	}
	if ((hFindFiles = FindFirstFile(search_pattern, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		while (!quitFindFiles)
		{
			ys = YSIZE;

			if (index < total && showNextPage)
			{
				LoadLocateFilesUI(box_x, box_y, box_margin_x, box_margin_y, index, n, total, true);

				for (int i = 0; i < (ys - (3 + box_y)); i++) // Display listed files
				{
					if ((FindString(FindFileData.cFileName, file) != -1 || isWildcard) && index < total) // Ignore the search result if wildcards are used
					{
						gotoxy(box_x + box_margin_x, n + box_margin_y + box_y);

						FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &st);
						if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && show_dir)
						{

							// Time format is "YYYY/MM/DD hh:mm:ss"
							printf(" %04d/%02d/%02d %02d:%02d:%02d\t<DIR>\t%14lu\t%.*s\n",  st.wYear, st.wMonth,st.wDay,  st.wHour, st.wMinute, st.wSecond, FindFileData.nFileSizeLow, max_print, FindFileData.cFileName);
						}
						else
						{
							printf(" %04d/%02d/%02d %02d:%02d:%02d\t\t%14lu\t%.*s\n", st.wYear, st.wMonth,st.wDay, st.wHour, st.wMinute, st.wSecond, FindFileData.nFileSizeLow, max_print, FindFileData.cFileName);
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
			case ' ':
			case PGDW | BIT_ESC224:
			{
				if (index <= total)
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
			}

			case CTRLB:
			case CTRLD:
			{
				showNextPage = false;
				if (key == CTRLD)
				{
					PrintBottomString("Directory: ");
					typeptr = TypingFunction(32, 255, MAX_PATH, typeptr);
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
				total = GetNumberFilesInDir(search_pattern);
				index = 0;
				n = 0;
				
				hFindFiles = FindFirstFile(search_pattern, &FindFileData);
				showNextPage = true;
				break;
			}
			case ESC:
			case CTRLL:
			case CTRLX:
			case CTRLQ:
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

	return 0;
}
