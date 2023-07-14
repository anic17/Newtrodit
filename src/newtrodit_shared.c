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

int SelectStart(File_info *tstack, size_t start_x, size_t start_y)
{
    if (tstack->selection.is_selected)
    {
        tstack->selection.start.x = start_x;
        tstack->selection.start.y = start_y;
    }
    return tstack->selection.is_selected;
}

int SelectEnd(File_info *tstack, size_t end_x, size_t end_y)
{
    if (tstack->selection.is_selected)
    {
        tstack->selection.end.x = end_x;
        tstack->selection.end.y = end_y;
    }
    return tstack->selection.is_selected;
}

int SelectSet(File_info *tstack, size_t start_x, size_t start_y, size_t end_x, size_t end_y)
{
    SelectStart(tstack, start_x, start_y);
    SelectEnd(tstack, start_x, start_y);
    return tstack->selection.is_selected;
}

int SelectClear(File_info *tstack)
{
    SelectStart(tstack, 0, 0);
    SelectEnd(tstack, 0, 0);
    tstack->selection.is_selected = false;
    return tstack->selection.is_selected;
}

int SelectAppend(File_info *tstack, size_t add_x, size_t add_y)
{
    tstack->selection.is_selected = true;
    tstack->selection.end.x += add_x;
    tstack->selection.end.y += add_y;
    return tstack->selection.is_selected;
}

int SelectCurrentPos(File_info *tstack)
{
    tstack->selection.is_selected = true;
    tstack->selection.end.x = tstack->xpos;
    tstack->selection.end.y = tstack->ypos;
    return tstack->selection.is_selected;
}

int SelectCheckInterval(File_info *tstack) // Check if the selection is within valid interval
{
    if (tstack->selection.is_selected)
    {
        int temp = 0;
        // This code hurts to look at, anyone got any ideas on how to improve it?
        if (tstack->selection.start.x < 0)
        {
            tstack->selection.start.x = 0;
        }
        if (tstack->selection.start.y < 1)
        {
            tstack->selection.start.y = 1;
        }
        else if (tstack->selection.start.y > tstack->linecount)
        {
            tstack->selection.start.y = tstack->linecount;
        }

        if (tstack->selection.end.x < 0)
        {
            tstack->selection.end.x = 0;
        }
        if (tstack->selection.end.y < 1)
        {
            tstack->selection.end.y = 1;
        }
        else if (tstack->selection.end.y > tstack->linecount)
        {
            tstack->selection.end.y = tstack->linecount;
        }

        if (tstack->selection.start.x > tstack->selection.end.x)
        {
            temp = tstack->selection.start.x;
            tstack->selection.start.x = tstack->selection.end.x;
            tstack->selection.end.x = temp;
        }
        if (tstack->selection.start.y > tstack->selection.end.y)
        {
            temp = tstack->selection.start.y;
            tstack->selection.start.y = tstack->selection.end.y;
            tstack->selection.end.y = temp;
        }
        if (tstack->selection.start.x == tstack->selection.end.x && tstack->selection.start.y == tstack->selection.end.y)
        {
            tstack->selection.is_selected = false;
        }
    }
    return tstack->selection.is_selected;
}

int SelectDelete(File_info *tstack, bool refresh)
{

    if (SelectCheckInterval(tstack))
    {
        // Clear the selection
        size_t linespan = tstack->selection.end.y - tstack->selection.start.y;
        if (linespan > 0)
        {
            memset(tstack->strsave[tstack->selection.start.y] + tstack->selection.start.x, 0, tstack->bufx - tstack->selection.start.x);
            for (int i = 0; i < linespan; i++)
            {
                DeleteRow(tstack->strsave, tstack->selection.start.y, tstack->bufy - 1);
                // free(tstack->strsave[i + tstack->selection.end.y - tstack->selection.start.y]);
            }
            tstack->linecount -= linespan;
            DeleteStr(tstack->strsave[tstack->selection.start.x], 0, tstack->selection.end.x);
        }
        else
        {
            DeleteStr(tstack->strsave[tstack->selection.start.y], tstack->selection.start.x, tstack->selection.end.x - tstack->selection.start.x);
            memset(tstack->strsave[tstack->selection.start.y] + strlen_n(tstack->strsave[tstack->selection.start.y]), 0, tstack->bufx - strlen_n(tstack->strsave[tstack->selection.start.y]));
        }
        _xpos = tstack->selection.start.x;
        _ypos = tstack->selection.start.y;
        if (_xpos >= NoLfLen(tstack->strsave[_ypos]))
        {
            tstack->xpos = NoLfLen(tstack->strsave[_ypos]);
        }

        SelectClear(tstack);
        if (refresh)
        {
            ClearPartial(0, 1, XSIZE, YSCROLL);
            DisplayFileContent(tstack, stdout, 0);
        }
    }
}

int SelectPrint(File_info *tstack, size_t yps)
{
    // TODO: Horizontal scroll support
    size_t startpos = tstack->selection.start.x > wrapSize ? wrapSize : tstack->selection.start.x;
    size_t endpos = tstack->selection.end.x, count = 0;
    if (tstack->selection.is_selected && tstack->selection.start.y <= yps && tstack->selection.end.y >= yps) // Current line is selected
    {
        if (tstack->selection.start.y != yps)
        {
            startpos = 0;
        }
        if (tstack->selection.end.y != yps)
        {
            endpos = NoLfLen(tstack->strsave[tstack->ypos]) > wrapSize ? wrapSize : NoLfLen(tstack->strsave[tstack->ypos]);
        }

        gotoxy((lineCount ? tstack->linecount_wide : 0), GetConsoleInfo(YCURSOR));
        printf("%.*s", startpos, tstack->strsave[yps] + tstack->display_x);
        if (startpos < wrapSize)
        {
            SetColor(SELECTION_COLOR * 16 + fg_color);
            count = endpos - startpos;
            if (count > wrapSize)
            {
                count = wrapSize;
            }
            printf("%.*s", count, tstack->strsave[yps] + startpos);
            SetColor(bg_color);

            if (startpos + count < wrapSize)
            {
                printf("%.*s", wrapSize - (startpos + count), tstack->strsave[yps] + startpos + count);
            }
        }

        WriteLogFile("Writing color at pos %d, len %X, end %d", startpos, count, endpos);
    }
    return tstack->selection.is_selected;
}

int SelectCheck(File_info *tstack) // Start selection
{
    if (CheckKey(VK_SHIFT))
    {

        if (!tstack->selection.is_selected)
        {
            tstack->selection.is_selected = true;
            SelectStart(tstack, tstack->xpos, tstack->ypos);
            SelectPrint(tstack, tstack->ypos);
        }
    }
    else
    {
        SelectClear(tstack);
    }
    return tstack->selection.is_selected;
}

int GotoBufferPosition(File_info *tstack, int numbuf, bool is_column)
{
    bool xgoto = true;
    size_t n = 0;
    char *ptr, *strnum1, *strnum2;
    static char *line_number_str = NULL;
    if (is_column)
    {
        xgoto = true;
        if (!numbuf)
        {
            PrintBottomString("%s", NEWTRODIT_PROMPT_GOTO_COLUMN);
        }
        ptr = NEWTRODIT_ERROR_INVALID_XPOS;
    }
    else
    {
        xgoto = false;
        if (!numbuf)
        {
            PrintBottomString("%s", NEWTRODIT_PROMPT_GOTO_LINE);
        }
        ptr = NEWTRODIT_ERROR_INVALID_YPOS;
    }
    if (!numbuf)
    {
        strnum1 = itoa_n(tstack->linecount);
        strnum2 = itoa_n(NoLfLen(tstack->strsave[tstack->ypos]));
        line_number_str = TypingFunction('0', '9', xgoto ? strlen(strnum2) : strlen(strnum1), line_number_str);
        numbuf = atoi(line_number_str);
        free(line_number_str);
        free(strnum1);
        free(strnum2);
        if (numbuf < 1) // Line is less than 1
        {
            PrintBottomString("%s", ptr);
            c = -2;

            return 0;
        }
    }

    if (!xgoto) // Go to line
    {
        n = tstack->ypos;
        tstack->ypos = numbuf;
        if (BufferLimit(tstack)) // Avoid crashes
        {
            tstack->ypos = n;

            ShowBottomMenu();
            DisplayCursorPos(tstack);
            c = -2;

            return 0;
        }
        if (!xgoto && tstack->ypos != 1 && tstack->strsave[tstack->ypos][0] == '\0' && !LineContainsNewLine(tstack, tstack->ypos))
        {
            PrintBottomString(ptr);
            tstack->ypos = n;
            c = -2;
            return 0;
        }
        RedrawScrolledScreen(tstack, n);
    }
    else
    { // Go to column
        n = tstack->xpos;
        tstack->xpos = numbuf - 1;

        if (NoLfLen(tstack->strsave[tstack->ypos]) < tstack->xpos)
        {
            PrintBottomString(ptr);
            tstack->xpos = n; // n-1 because it is zero indexed
            c = -2;
            return 0;
        }
    }
    if (!xgoto)
    {
        (tstack->ypos >= (YSIZE - 3)) ? UpdateScrolledScreen(tstack) : UpdateHomeScrolledScreen(tstack);
    }

    ShowBottomMenu();
    DisplayCursorPos(tstack);
    return 1;
}

int SwitchTab(File_info *tstack, int new_tab_index)
{
    bool n = 0;
    if (!new_tab_index)
    {
        n = 1;
    }
    else if (new_tab_index > 0)
    {
        n = -1;
    }
    else
    {
        n = 0;
    }

    if (open_files > 1)
    {

        if (n)
        {

            if (file_index + n < 0)
            {
                file_index = open_files - 1;
            }
            else
            {

                file_index += n;
            }
            file_index = file_index % open_files;
        }
        else
        {

            file_index = abs(new_tab_index) - 1;
        }
        if (file_index >= open_files)
        {
            PrintBottomString("%s", NEWTRODIT_ERROR_INVALID_FILE_INDEX);
            getch_n();
            file_index = 0;
        }
        LoadAllNewtrodit();
        DisplayFileContent(&Tab_stack[file_index], stdout, 0);

        PrintBottomString("%s (%d/%d).", (n == -1) ? NEWTRODIT_SHOWING_PREVIOUS_FILE : NEWTRODIT_SHOWING_NEXT_FILE, file_index + 1, open_files);
    }
    else
    {
        PrintBottomString("%s", NEWTRODIT_INFO_NO_FILES_TO_SWITCH);
    }

    c = -2;
    return file_index;
}

int ParseCommandPalette(File_info *tstack, char *command_palette)
{
    size_t numbuf = 0;
    if (IsNumberString(command_palette + 1))
    {
        numbuf = atoi(command_palette + 1);
    }
    switch (command_palette[0])
    {
    case ':':
    {
        GotoBufferPosition(tstack, numbuf, false);
        return 1;
    }
    case ',':
    {
        GotoBufferPosition(tstack, numbuf, true);
        return 1;
    }

    case '-':
    {
        if (tstack->ypos - numbuf >= 1)
        {
            tstack->ypos -= numbuf;
            if (tstack->xpos > NoLfLen(tstack->strsave[tstack->ypos]))
            {
                tstack->xpos = NoLfLen(tstack->strsave[tstack->ypos]);
            }
            UpdateScrolledScreen(tstack);
        }
        return 1;
    }

    case '+':
    {
        if (tstack->ypos + numbuf > tstack->linecount && tstack->ypos + numbuf >= tstack->bufy)
        {
            tstack->ypos += numbuf;
            if (tstack->xpos > NoLfLen(tstack->strsave[tstack->ypos]))
            {
                tstack->xpos = NoLfLen(tstack->strsave[tstack->ypos]);
            }
            UpdateScrolledScreen(tstack);
        }
    }
    case '0':
    {
        tstack->xpos = 0;

        UpdateHorizontalScroll(&Tab_stack[file_index], true);

        return 1;
    }
    case '^':
    {
        tstack->xpos = strspn(tstack->strsave[tstack->ypos], " \t");
        UpdateHorizontalScroll(&Tab_stack[file_index], true);
        return 1;
    }

    case '$':

    {
        // END key
        numbuf = tstack->xpos;
        tstack->xpos = NoLfLen(tstack->strsave[tstack->ypos]);
        if (numbuf == tstack->xpos)
        {
            return 1;
        }
        SetDisplayX(&Tab_stack[file_index]);
        UpdateHorizontalScroll(&Tab_stack[file_index], false);
        return 1;
    }
    case '@':
    {
        if (numbuf)
        {
            SwitchTab(tstack, -numbuf);
        }
        return 1;
    }
    case '?':
    {
        if (&command_palette[1])
        {
            OpenNewtroditFile(tstack, command_palette + 1);
        }
        return 1;
    }
    case '!':
    {
        if (&command_palette[1])
        {
            SaveFile(tstack, command_palette + 1, false);
        }
        return 1;
    }
    }
    return 0;
}

int ParseArguments(int argc, char **argv, int *fa, int *facount)
{
    if (argc > 1)
    {
        for (int arg_parse = 1; arg_parse < argc; ++arg_parse) // Arguments parser
        {
            if (!strcmp(argv[arg_parse], "--version") || !strcmp(argv[arg_parse], "-v")) // Version parameter
            {
                printf("%.*s", wrapSize, ProgInfo());
                ExitRoutine(0);
            }
            else if (!strcmp(argv[arg_parse], "--help") || !strcmp(argv[arg_parse], "-h")) // Manual parameter
            {
                NewtroditHelp();
                SetColor(SInf.color);
                ClearScreen();
                ExitRoutine(0);
            }
            else if (!strcmp(argv[arg_parse], "--syntax") || !strcmp(argv[arg_parse], "-s")) // Syntax parameter
            {
                if (argv[arg_parse + 1] != NULL)
                {
                    char syntaxfile[MAX_PATH + 1] = {0};
                    strncpy_n(syntaxfile, argv[arg_parse + 1], MAX_PATH); // Copy the syntax file name
                    syntaxfile[strcspn(syntaxfile, "\n")] = 0;
                    if ((syntaxKeywordsSize = LoadSyntaxScheme(syntaxfile, &Tab_stack[file_index]))) // Change keywords size
                    {
                        syntaxHighlighting = true;
                        Tab_stack[file_index].Syntaxinfo.syntax_file = FullPath(syntaxfile);
                        Tab_stack[file_index].Syntaxinfo.keyword_count = syntaxKeywordsSize;
                    }
                    getch();

                    arg_parse++;
                }
                else
                {
                    fprintf(stderr, "%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
                    ExitRoutine(1);
                }
            }
            else if (!strcmp(argv[arg_parse], "--line") || !strcmp(argv[arg_parse], "-l")) // Display line count
            {
                lineCount = true;
            }
            else if (!strcmp(argv[arg_parse], "--autosyntax") || !strcmp(argv[arg_parse], "-a")) // Auto syntax highlighting
            {
                autoLoadSyntaxRules = true;
            }
            else if (!strcmp(argv[arg_parse], "--lfunix") || !strcmp(argv[arg_parse], "-n")) // Use UNIX new line
            {
                strncpy_n(Tab_stack[file_index].newline, "\n", 1); // Avoid any type of buffer overflows
            }
            else if (!strcmp(argv[arg_parse], "--lfwin") || !strcmp(argv[arg_parse], "-w")) // Use Windows new line
            {
                strncpy_n(Tab_stack[file_index].newline, "\r\n", 2);
            }
            else if (!strcmp(argv[arg_parse], "--converttab") || !strcmp(argv[arg_parse], "-t")) // Convert tabs to spaces
            {
                convertTabtoSpaces = true;
            }
            else if (!strcmp(argv[arg_parse], "--devmode") || !strcmp(argv[arg_parse], "-d")) // Enable dev mode
            {
                devMode = true;
            }
            else if (!strcmp(argv[arg_parse], "--mouse") || !strcmp(argv[arg_parse], "-m")) // Use mouse
            {
                partialMouseSupport = true;
            }
            else if (!strcmp(argv[arg_parse], "--menucolor") || !strcmp(argv[arg_parse], "-c")) // Foreground color parameter
            {
                if (argv[arg_parse + 1] != NULL)
                {
                    fg_color = HexStrToDec(argv[arg_parse + 1]);
                    if (fg_color > 0x0F || fg_color < 0)
                    {
                        fprintf(stderr, "%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
                        return 1;
                    }
                    fg_color = (fg_color * 16) % 256;

                    arg_parse++;
                }
                else
                {
                    fprintf(stderr, "%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
                    ExitRoutine(1);
                }
            }
            else if (!strcmp(argv[arg_parse], "--fontcolor") || !strcmp(argv[arg_parse], "-f")) // Foreground color parameter
            {
                if (argv[arg_parse + 1] != NULL)
                {
                    bg_color = HexStrToDec(argv[arg_parse + 1]);
                    if (bg_color > 0x0F || bg_color < 0)
                    {
                        fprintf(stderr, "%s\n", NEWTRODIT_ERROR_INVALID_COLOR);
                        ExitRoutine(1);
                    }
                    bg_color = bg_color % 256;
                    arg_parse++;
                }
                else
                {
                    fprintf(stderr, "%s\n", NEWTRODIT_ERROR_MISSING_ARGUMENT);
                    ExitRoutine(1);
                }
            }
            else
            {
                (*facount)++;
                (fa[*facount]) = arg_parse;
            }
        }
    }
    return argc;
}