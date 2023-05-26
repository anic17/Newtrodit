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

    This file is from my other project syntax, with some modifications.
    https://github.com/anic17/syntax

*/

int syntaxKeywordsSize = sizeof(keywords) / sizeof(keywords[0]);

int is_separator(int c) // Taken from https://github.com/antirez/kilo/blob/69c3ce609d1e8df3956cba6db3d296a7cf3af3de/kilo.c#L366
{
    return c == '\0' || isspace(c) || strchr(Tab_stack[file_index].Syntaxinfo.separators, c) != NULL;
}

int EmptySyntaxScheme(File_info *tstack)
{

    for (int i = 0; i < tstack->Syntaxinfo.keyword_count; ++i)
    {
        if (tstack->Syntaxinfo.keywords[i][0] != '\0')
        {
            memset(tstack->Syntaxinfo.keywords[i], 0, strlen(tstack->Syntaxinfo.keywords[i]));
            tstack->Syntaxinfo.color[i] = 0;
        }
    }

    return tstack->Syntaxinfo.keyword_count;
}



int LoadSyntaxScheme(char *syntax_fn, File_info *tstack)
{

    if (!syntax_fn)
    {
        PrintBottomString("%s", NEWTRODIT_FS_FILE_INVALID_NAME);
        WriteLogFile("%s", NEWTRODIT_FS_FILE_INVALID_NAME);
        return 0;
    }

    WriteLogFile("Loading syntax highlighting scheme: %s", syntax_fn);
    char *syntax_frules;

    char *tokchar = "=";
    char *iniptr;
    char comments[] = ";\r\n";
    char def_tokenizer[] = " \t\v";
    int c = 0;
    bool isNull = false;
    // If the syntax highlighting file is not found, try to find it on the program's syntax highlighting directory
    FILE *syntaxfp = fopen(syntax_fn, "rb");
    int err = 0;
    char *ext = StrLastTok(syntax_fn, "."), *basename = GetBaseName(syntax_fn); // Get the file extension and basename

    if (!syntaxfp)
    {
        err = errno;
        syntax_frules = calloc(strlen_n(ext) + DEFAULT_ALLOC_SIZE, sizeof(char));
        snprintf(syntax_frules, sizeof(char) * (DEFAULT_ALLOC_SIZE + strlen_n(ext)), "%s\\%s\\%s%s", SInf.location, syntax_dir, basename, syntax_ext);
        WriteLogFile("%s", syntax_frules);
        syntaxfp = fopen(syntax_frules, "rb"); // Try to open the syntax highlighting file
        if (!syntaxfp)
        {
            PrintBottomString("%s", ErrorMessage(err, syntax_fn));
            WriteLogFile("%s", ErrorMessage(err, syntax_fn));
            last_known_exception = ErrorMessage(err, syntax_fn);
            return 0;
        }
    }
    free(basename);
    fseek(syntaxfp, 0, SEEK_SET); // Set the file pointer to the beginning of the file

    /* Allocate the memory for the new syntax scheme */

    char *read_syntax_buf = calloc(sizeof(char), LINE_MAX);

    char *syntax_language = calloc(sizeof(char), MAX_PATH);
    memset(syntax_language, 0, sizeof(char) * MAX_PATH);

    // Allocate the comments char pointer

    bool hasLanguage = false;
    while (fgets(read_syntax_buf, LINE_MAX, syntaxfp))
    {
        if (!strcspn(read_syntax_buf, comments))
        {
            continue;
        }
        while (isspace(*read_syntax_buf))
        {
            read_syntax_buf++;
        }
        read_syntax_buf[strcspn(read_syntax_buf, "\r\n")] = 0;
        iniptr = strtok_n(read_syntax_buf, tokchar);

        if (c == 0) // First line of the syntax highlighting rules file
        {
            if (strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_MAGIC_NUMBER, strlen(NEWTRODIT_SYNTAX_MAGIC_NUMBER)))
            {
                PrintBottomString("%s%s", NEWTRODIT_ERROR_INVALID_SYNTAX, syntax_fn);
                WriteLogFile("%s%s", NEWTRODIT_ERROR_INVALID_SYNTAX, syntax_fn);
                free(read_syntax_buf);
                free(syntax_language);
                return 0;
            }
            else
            { // Once we know that reading will succeed, empty the old syntax highlighting scheme (if it exists)
                EmptySyntaxScheme(&Tab_stack[file_index]);
                tstack->Syntaxinfo.comments = calloc(sizeof(char *), DEFAULT_ALLOC_SIZE);
                tstack->Syntaxinfo.comment_count = 0;
            }
            c++;
            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_COMMENT, def_tokenizer)) // No 'hasComment' boolean because multiple comments can be defined
        {
            tstack->Syntaxinfo.comment_count++;

            tstack->Syntaxinfo.comments = realloc_n(tstack->Syntaxinfo.comments, sizeof(char *) * tstack->Syntaxinfo.comment_count, sizeof(char *) * (tstack->Syntaxinfo.comment_count + 1));

            tstack->Syntaxinfo.comments[tstack->Syntaxinfo.comment_count - 1] = strdup(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_COMMENT) + 1);
            continue;
        }

        if (!hasLanguage && !strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_LANGUAGE, def_tokenizer))
        {

            hasLanguage = true;

            strncpy_n(syntax_language, read_syntax_buf + strlen(NEWTRODIT_SYNTAX_LANGUAGE) + 1, MAX_PATH); // Whitespace character
            tstack->Syntaxinfo.syntax_lang = strdup(syntax_language);

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_DEFAULT_COLOR, def_tokenizer))
        {
            default_color = HexStrToDec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_DEFAULT_COLOR) + 1);
            tstack->Syntaxinfo.default_color = default_color;

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_QUOTE_COLOR, def_tokenizer))
        {
            quote_color = HexStrToDec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_QUOTE_COLOR) + 1);
            tstack->Syntaxinfo.quote_color = quote_color;

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_COMMENT_COLOR, def_tokenizer))
        {

            tstack->Syntaxinfo.comment_color = strtol(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_COMMENT_COLOR) + 1, NULL, 16);

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_NUMBER_COLOR, def_tokenizer))
        {
            num_color = HexStrToDec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_NUMBER_COLOR) + 1);
            tstack->Syntaxinfo.num_color = num_color;

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_CAPITAL_COLOR, def_tokenizer))
        {
            capital_color = HexStrToDec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_CAPITAL_COLOR) + 1);
            tstack->Syntaxinfo.capital_color = capital_color;

            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_CAPITAL_MIN, def_tokenizer))
        {
            capital_min_len = HexStrToDec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_CAPITAL_MIN) + 1);
            tstack->Syntaxinfo.capital_min = capital_min_len;

            continue;
        }

        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_CAPITAL, def_tokenizer))
        {
            if (read_syntax_buf[strlen(NEWTRODIT_SYNTAX_CAPITAL)] == '\0')
            {
                tstack->Syntaxinfo.capital_enabled = true;
            }
            else
            {
                if (atoi(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_CAPITAL) + 1))
                {
                    tstack->Syntaxinfo.capital_enabled = true;
                }
                else
                {
                    tstack->Syntaxinfo.capital_enabled = false;
                }
            }
            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_SINGLE_QUOTES, def_tokenizer))
        {
            if (read_syntax_buf[strlen(NEWTRODIT_SYNTAX_SINGLE_QUOTES)] == '\0')
            {
                tstack->Syntaxinfo.single_quotes = true;
            }
            else
            {
                if (atoi(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_SINGLE_QUOTES) + 1))
                {
                    tstack->Syntaxinfo.single_quotes = true;
                }
                else
                {
                    tstack->Syntaxinfo.single_quotes = false;
                }
            }

            printf("SINGLE QUOTES OK");
            getch_n();
            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_FINISH_QUOTES, def_tokenizer))
        {
            if (read_syntax_buf[strlen(NEWTRODIT_SYNTAX_FINISH_QUOTES)] == '\0')
            {
                tstack->Syntaxinfo.finish_quotes = true;
            }
            else
            {
                if (atoi(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_FINISH_QUOTES) + 1))
                {
                    tstack->Syntaxinfo.finish_quotes = true;
                }
                else
                {
                    tstack->Syntaxinfo.finish_quotes = false;
                }
            }
            continue;
        }
        if (!strcmptok(read_syntax_buf, NEWTRODIT_SYNTAX_SEPARATORS, def_tokenizer))
        {
            strncpy_n(tstack->Syntaxinfo.separators, read_syntax_buf + strlen(NEWTRODIT_SYNTAX_SEPARATORS) + 1, sizeof syntax_separators);
            continue;
        }

        if (c >= tstack->Syntaxinfo.keyword_count)
        {
            tstack->Syntaxinfo.keywords = realloc_n(tstack->Syntaxinfo.keywords, sizeof(char *) * c, sizeof(char *) * (c + 1));
            tstack->Syntaxinfo.color = realloc_n(tstack->Syntaxinfo.color, sizeof(int) * c, sizeof(int) * (c + 1));
            tstack->Syntaxinfo.keywords[c] = calloc(DEFAULT_ALLOC_SIZE, sizeof(char)); // Allocate memory for the new keyword
        }
        if (iniptr)
        {
            tstack->Syntaxinfo.keywords[c] = strdup(iniptr); // If strdup is not used, the value will be overwritten by the next strtok call

            iniptr = read_syntax_buf + strlen(tstack->Syntaxinfo.keywords[c]) + strlen(tokchar);
            if (iniptr)
            {
                tstack->Syntaxinfo.color[c] = abs((int)strtol(iniptr, NULL, 16)) % 16; // Range from 0 to 15
                c++;
                isNull = false;
            }
        }
    }

    if (isNull)
    {
        PrintBottomString("%s", NEWTRODIT_ERROR_SYNTAX_RULES);
        getch_n();
        free(read_syntax_buf);
        free(syntax_language);
        return 0;
    }
    tstack->Syntaxinfo.keyword_count = c;
    free(read_syntax_buf);
    free(syntax_language);
    return c;
}

void color_line(char *line, int startpos, int override_color, int yps)
{
    int ycur = GetConsoleInfo(YCURSOR);
    if (override_color <= 0)
    {
        override_color = 0; // Default color
    }
    // Color a specific line
    int keyword_len = 0, skipChars = 0, tmpvar = 0; // Always initialize variables to 0 to avoid undefined behavior (= crash)
    int scrollx = 0;
    char quotechar;
    if (!Tab_stack[file_index].Syntaxinfo.multi_line_comment)
    {
        SetColor(Tab_stack[file_index].Syntaxinfo.default_color);
    }

    // int internal_bps_count = Tab_stack[file_index].Syntaxinfo.bracket_pair_count; // Used to count the number of brackets in a string
    size_t len = strlen(line);
    // This causes visual bugs
    // SetCharColor(wrapSize, Tab_stack[file_index].Syntaxinfo.default_color, lineCount ? (Tab_stack[file_index].linecount_wide - 1) : 0, ycur);

    bool line_contains_comments = false; // Check if there are comments in the line to avoid useless searches
    for (size_t k = 0; k < Tab_stack[file_index].Syntaxinfo.comment_count; k++)
    {
        if (strstr(line, Tab_stack[file_index].Syntaxinfo.comments[k]))
        {
            line_contains_comments = true;
            break;
        }
    }

    for (size_t i = 0; i < len; ++i)
    {
        if (line[i] == '\n') // Shouldn't happen (editor should already parse this), but just in case
        {
            SetColor(Tab_stack[file_index].Syntaxinfo.default_color);

            return;
        }

        if (i - startpos < wrapSize)
        {
            while (i < len && i < wrapSize && line[i] != '\0' && isspace(line[i]))
            {
                ++i;
            }

            if (isdigit(line[i]) && !Tab_stack[file_index].Syntaxinfo.multi_line_comment)
            {
                gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0) + i, ycur);
                if (i == 0 || is_separator(line[i - 1]))
                {
                    if (i < len)
                    {
                        SetColor(Tab_stack[file_index].Syntaxinfo.num_color + (16 * override_color));
                        putchar(line[i++]);
                        switch (tolower(line[i]))
                        {
                        case 'b':
                            do
                            {
                                putchar(line[i++]);
                            } while ((line[i] == '0' || line[i] == '1') && i < len && i < wrapSize);
                            break;
                        case 'o':

                            do
                            {
                                putchar(line[i++]);
                            } while (isdigit(line[i]) && line[i] < '8' && i < len && i < wrapSize);

                            break;
                        case 'x':
                            do
                            {
                                putchar(line[i++]);
                            } while (isxdigit(line[i]) && i < len && i < wrapSize);
                            break;

                        default:
                            if (isdigit(line[i]))
                            {
                                do
                                {

                                    putchar(line[i++]);

                                } while ((isdigit(line[i]) || line[i] == '.') && i < len && i < wrapSize);
                            }

                            break;
                        }
                        if (i < len && i < wrapSize)
                        {
                            switch (tolower(line[i])) // For long, unsigned and float
                            {
                            case 'l':
                                putchar(line[i++]);
                                if (i < len && tolower(line[i]) == 'l' && i < wrapSize)
                                {
                                    putchar(line[i++]);
                                }
                                break;
                            case 'u':
                                putchar(line[i++]);
                                break;
                            case 'f':
                                putchar(line[i++]);
                                break;
                            default:
                                break;
                            }
                        }

                        SetColor(Tab_stack[file_index].Syntaxinfo.default_color + (16 * override_color));
                    }
                }
            }
            if (line_contains_comments) // Small optimization
            {
                for (size_t k = 0; k < Tab_stack[file_index].Syntaxinfo.comment_count; k++)
                {

                    if (Tab_stack[file_index].Syntaxinfo.comments[k][0] != '\0')
                    {
                        if (!memcmp(line + i, Tab_stack[file_index].Syntaxinfo.comments[k], strlen(Tab_stack[file_index].Syntaxinfo.comments[k])))
                        {

                            SetCharColor((len > wrapSize) ? (wrapSize - i) : (len - i), Tab_stack[file_index].Syntaxinfo.comment_color + (16 * override_color), (lineCount ? Tab_stack[file_index].linecount_wide : 0) + (i - startpos < 0 ? 0 : i - startpos), ycur);
                            SetColor(Tab_stack[file_index].Syntaxinfo.default_color + (16 * override_color));
                            return;
                        }
                    }
                }
            }

            if ((line[i] == '\"' || (line[i] == '\'' && Tab_stack[file_index].Syntaxinfo.single_quotes)) && !Tab_stack[file_index].Syntaxinfo.multi_line_comment)
            {
                if (i < len)
                {
                    quotechar = line[i];
                    skipChars = 0;
                    do
                    {
                        skipChars++;
                    } while (line[i + skipChars] != quotechar && i + skipChars < len && i + skipChars < wrapSize);
                    if (line[i + skipChars] == quotechar)
                    {
                        int delta_x = i - Tab_stack[file_index].display_x;
                        if (Tab_stack[file_index].display_x >= i)
                        {
                            scrollx = (Tab_stack[file_index].display_x - i); // length of the quoted string
                        }
                        else
                        {
                            scrollx = 0;
                        }

                        // WriteLogFile("%d:%d:%d|%d:%d", i - Tab_stack[file_index].display_x, i, Tab_stack[file_index].display_x, wrapSize + Tab_stack[file_index].display_x, scrollx);
                        SetCharColor(skipChars + 1 - scrollx, Tab_stack[file_index].Syntaxinfo.quote_color + (16 * override_color), delta_x + (lineCount ? Tab_stack[file_index].linecount_wide : 0) + scrollx, ycur); // +1 because we want to also color the quote
                        i += skipChars;
                    }

                    continue;
                }
            }
            else
            {
                if (i == 0 || is_separator(line[i - 1]))
                {
                    for (int k = 0; k < Tab_stack[file_index].Syntaxinfo.keyword_count; k++)
                    {
                        keyword_len = strlen(Tab_stack[file_index].Syntaxinfo.keywords[k]);

                        if (is_separator(line[i + keyword_len]) && !memcmp(line + i, Tab_stack[file_index].Syntaxinfo.keywords[k], keyword_len))
                        {
                            gotoxy((lineCount ? Tab_stack[file_index].linecount_wide : 0) + i, ycur);

                            if (i + keyword_len < wrapSize)
                            {
                                SetCharColor(keyword_len, Tab_stack[file_index].Syntaxinfo.color[k] + (16 * override_color), GetConsoleInfo(XCURSOR), ycur); // +1 because we want to also color the quote
                            }
                            else
                            {
                                SetCharColor(wrapSize - i, Tab_stack[file_index].Syntaxinfo.color[k] + (16 * override_color), GetConsoleInfo(XCURSOR), ycur); // +1 because we want to also color the quote
                            }

                            i += keyword_len;
                            break;
                        }
                    }
                    if (Tab_stack[file_index].Syntaxinfo.capital_enabled && (isupper(line[i]) || line[i] == '_') && i < len && i < wrapSize) // Also add underscores because variable names can contain them
                    {
                        tmpvar = 0;
                        skipChars = 0;
                        do
                        {
                            if (line[i + skipChars] != '_' && line[i + skipChars] != '\0') // Check if the string only consists of underscores
                            {
                                tmpvar = 1;
                            }
                            skipChars++;

                        } while (i + skipChars < len && i + skipChars < wrapSize && (isupper(line[i + skipChars]) || line[i + skipChars] == '_'));
                        if (tmpvar) // Highlight if the string consists of more than just underscores
                        {
                            if (is_separator(line[i + skipChars]) && skipChars >= Tab_stack[file_index].Syntaxinfo.capital_min) // All the string is in capital and more than 1 character
                            {
                                SetCharColor(skipChars, Tab_stack[file_index].Syntaxinfo.capital_color + (16 * override_color), (lineCount ? Tab_stack[file_index].linecount_wide : 0) + (i - startpos < 0 ? 0 : i - startpos), ycur);
                                i += skipChars;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // No need to parse the line that is not rendered
            break;
        }
    }
    SetColor(bg_color);
    return;
}


int AutoLoadSyntaxRules(File_info *tstack, char* alt_name)
{
	char* syntax_frules=NULL, *ext;
    
	ext = StrLastTok(alt_name == NULL ? tstack->filename : alt_name, "."); // Get the file extension

	if (ext && autoLoadSyntaxRules)
	{
		syntax_frules = calloc(strlen_n(ext) + DEFAULT_ALLOC_SIZE, sizeof(char));
		snprintf(syntax_frules, sizeof(char) * (DEFAULT_ALLOC_SIZE + strlen_n(ext)), "%s\\%s\\%s%s", SInf.location, syntax_dir, ext, syntax_ext);
		if (!CheckFile(syntax_frules))
		{
			if (LoadSyntaxScheme(syntax_frules, tstack))
			{
				WriteLogFile(NEWTRODIT_SYNTAX_HIGHLIGHTING_LOADED, Tab_stack[file_index].Syntaxinfo.syntax_lang);
                return 1;
			}
		}
		free(syntax_frules);
	}
	return 0;
}