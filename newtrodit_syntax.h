/*

  Newtrodit: A console text editor
  Copyright (c) 2021  anic17 Software

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

typedef struct keywords
{
    char *keyword;
    int color;
} keywords_t;

keywords_t keywords[] = {
    {"break", 5},
    {"continue", 5},
    {"return", 5},

    {"auto", 9},
    {"const", 9},
    {"volatile", 9},
    {"extern", 9},
    {"static", 9},

    {"inline", 9},
    {"restrict", 9},

    {"char", 9},
    {"int", 9},
    {"short", 9},
    {"float", 9},
    {"double", 9},
    {"long", 9},
    {"bool", 9},
    {"void", 9},

    {"double_t", 0xa},
    {"div_t", 0xa},
    {"float_t", 0xa},
    {"fpos_t", 0xa},
    {"max_align_t", 0xa},
    {"mbstate_t", 0xa},
    {"nullptr_t", 0xa},
    {"ptrdiff_t", 0xa},
    {"sig_atomic_t", 0xa},
    {"size_t", 0xa},
    {"time_t", 0xa},
    {"wchar_t", 0xa},
    {"wint_t", 0xa},
    {"FILE", 0xa},

    {"uint8_t", 0xa},
    {"uint16_t", 0xa},
    {"uint32_t", 0xa},
    {"uint64_t", 0xa},
    {"uint128_t", 0xa},

    {"int8_t", 0xa},
    {"int16_t", 0xa},
    {"int32_t", 0xa},
    {"int64_t", 0xa},
    {"int128_t", 0xa},

    {"struct", 6},
    {"enum", 6},
    {"union", 6},
    {"typedef", 6},
    {"unsigned", 9},
    {"signed", 9},
    {"sizeof", 9},
    {"register", 9},

    {"do", 6},
    {"if", 6},
    {"else", 6},
    {"while", 6},
    {"switch", 6},
    {"for", 6},
    {"case", 6},
    {"default", 6},
    {"goto", 6},

    {"#include", 0xb},
    {"#pragma", 0xb},
    {"#define", 0xb},
    {"#ifdef", 0xb},
    {"#undef", 0xb},

    {"#ifndef", 0xb},
    {"#endif", 0xb},
    {"#else", 0xb},

    {"#elif", 0xb},
    {"#error", 0xb},
    {"#warning", 0xb},

    {"main", 6},

    // Macro constants
    {"NULL", 0x9},
    {"true", 0x9},
    {"false", 0x9},
    {"errno", 0x9},
    {"stdin", 0x9},
    {"stdout", 0x9},
    {"stderr", 0x9},
    {"EOF", 0x9},
    {"WEOF", 0x9},

};

struct
{
    char *keyword;
    int color;
} comment[] = {
    {"//", 0x8},
    {"/*", 0x8},
    {"*/", 0x8},
};

int syntaxKeywordsSize = sizeof(keywords) / sizeof(keywords[0]);

int is_separator(int c) // Taken from https://github.com/antirez/kilo/blob/69c3ce609d1e8df3956cba6db3d296a7cf3af3de/kilo.c#L366
{
    return c == '\0' || isspace(c) || strchr(syntax_separators, c) != NULL;
}

int EmptySyntaxScheme()
{
    size_t syntax_size = sizeof(keywords) / sizeof(keywords[0]), first_index_sz = sizeof(keywords[0]);

    for (int i = 0; i < syntax_size; ++i)
    {
        memset(keywords[i].keyword, 0, first_index_sz);
        keywords[i].color = 0;
    }
    return syntax_size;
}

int LoadSyntaxScheme(FILE *syntaxfp, char *syntax_fn)
{
    fseek(syntaxfp, 0, SEEK_SET); // Set the file pointer to the beginning of the file
    char *read_syntax_buf = (char *)malloc(sizeof(char) * LINE_MAX);

    char *syntax_language = malloc(sizeof(char) * _MAX_PATH);
    char *tokchar = "=";
    char *iniptr;
    char comments[_MAX_PATH] = ";\r\n";
    int c = 0, comment_c = 0, highlight_color;
    bool isNull = false;
    memset(syntax_language, 0, sizeof(char) * _MAX_PATH);
    bool hasComment = false, hasMagicNumber = false, hasLanguage = false;

    while (fgets(read_syntax_buf, LINE_MAX, syntaxfp))
    {
        if (strcspn(read_syntax_buf, comments) == 0)
        {
            continue;
        }
        read_syntax_buf[strcspn(read_syntax_buf, "\r\n")] = 0;

        if (c == 0)
        {
            if (!hasMagicNumber)
            {
                if (strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_MAGIC_NUMBER, strlen(NEWTRODIT_SYNTAX_MAGIC_NUMBER)))
                {
                    PrintBottomString(join(NEWTRODIT_ERROR_INVALID_SYNTAX, syntax_fn));
                    return 0;
                }
                else
                {
                    hasMagicNumber = true;
                    continue;
                }
            }
        }
        if (!hasComment && !strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_COMMENT, strlen(NEWTRODIT_SYNTAX_COMMENT)))
        {
            hasComment = true;
            comment[0].keyword = read_syntax_buf + strlen(NEWTRODIT_SYNTAX_COMMENT) + 1;

            continue;
        }

        if (!hasLanguage && !strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_LANGUAGE, strlen(NEWTRODIT_SYNTAX_LANGUAGE)))
        {

            hasLanguage = true;
            strncpy_n(syntax_language, read_syntax_buf + strlen(NEWTRODIT_SYNTAX_LANGUAGE) + 1, _MAX_PATH); // Whitespace character
            continue;
        }
        if (!strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_DEFAULT_COLOR, strlen(NEWTRODIT_SYNTAX_DEFAULT_COLOR)))
        {
            default_color = hexstrtodec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_DEFAULT_COLOR) + 1);
            continue;
        }
        if (!strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_QUOTE_COLOR, strlen(NEWTRODIT_SYNTAX_QUOTE_COLOR)))
        {
            quote_color = hexstrtodec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_QUOTE_COLOR) + 1);
            continue;
        }
        if (!strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_COMMENT_COLOR, strlen(NEWTRODIT_SYNTAX_COMMENT_COLOR)))
        {
            comment_color = hexstrtodec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_COMMENT_COLOR) + 1);
            continue;
        }
        if (!strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_NUMBER_COLOR, strlen(NEWTRODIT_SYNTAX_NUMBER_COLOR)))
        {
            num_color = hexstrtodec(read_syntax_buf + strlen(NEWTRODIT_SYNTAX_NUMBER_COLOR) + 1);
            continue;
        }
        if (!strncmp(read_syntax_buf, NEWTRODIT_SYNTAX_SEPARATORS, strlen(NEWTRODIT_SYNTAX_SEPARATORS)))
        {
            strncpy_n(syntax_separators, read_syntax_buf + strlen(NEWTRODIT_SYNTAX_SEPARATORS) + 1, sizeof syntax_separators);
            continue;
        }

        iniptr = strtok(read_syntax_buf, tokchar);
        keywords[c].keyword = strdup(iniptr); // If strdup is not used, the value will be overwritten by the next strtok call
        iniptr = strtok(NULL, tokchar);

        highlight_color = hexstrtodec(iniptr);
        keywords[c].color = highlight_color;
        c++;
        isNull = false;
    }

    if (isNull)
    {
        PrintBottomString(NEWTRODIT_ERROR_SYNTAX_RULES);
        MakePause();
        return 0;
    }
    printf("%d", c);
    return c;
}

char *color_line(char *line, int startpos)
{
    DisplayCursor(false);
    size_t comment_size = sizeof(comment) / sizeof(comment[0]);
    char *strstr_ptr;
    // Make syntax highlighting
    bool isComment = false, numBase = false; // numBase is used to check the base of the number (decimal or hexadecimal)
    int tmp_count = 0, pos, line_num = 0, skipChars = 0;

    char quotechar;
    if (!multiLineComment)
    {
        SetColor(default_color);
    }

    size_t len = strlen(line);
    for (int i = 0; i < len; ++i)
    {
        if (i < wrapSize)
        {
            while ((line[i] == 32 || line[i] == 9) && i < wrapSize && i < len)
            {
                putchar(line[i++]);
            }
            /*

           if (!memcmp(line + i, comment[1].keyword, strlen(comment[1].keyword)))
           {
                SetColor(comment[1].color);
                multiLineComment = true;
                while(memcmp(line + i, comment[2].keyword, strlen(comment[2].keyword)) && i < len)
                {
                   putchar(line[i++]);
                }

                i += strlen(comment[1].keyword);
            }
            if (!memcmp(line + i, comment[2].keyword, strlen(comment[2].keyword)))
            {
                printf("%.*s", wrapSize - i, comment[2].keyword);
                multiLineComment = false;
                SetColor(default_color);
                i += strlen(comment[2].keyword);
            }
           if (!multiLineComment)
           { */
            if (isdigit(line[i]) && !isComment)
            {
                if (i == 0 || is_separator(line[i - 1]))
                {
                    if (i < len)
                    {
                        SetColor(num_color);
                        putchar(line[i++]);
                        switch (tolower(line[i]))
                        {
                        case 'b':
                            do
                            {
                                putchar(line[i++]);
                            } while ((line[i] == '0' || line[i] == '1') && i < len);
                            break;
                        case 'o':

                            do
                            {
                                putchar(line[i++]);
                            } while (isdigit(line[i]) && i < len && line[i] < '8');

                            break;
                        case 'x':
                            do
                            {
                                putchar(line[i++]);
                            } while (isxdigit(line[i]) && i < len);
                            break;

                        default:
                            if (isdigit(line[i]))
                            {
                                do
                                {
                                    putchar(line[i++]);
                                } while (isdigit(line[i]) && i < len);
                            }

                            break;
                        }

                        SetColor(default_color);
                    }
                }
            }
            if (!memcmp(line + i, comment[0].keyword, strlen(comment[0].keyword)))
            {
                SetColor(comment_color);
                printf("%.*s", wrapSize - i, line + i);
                SetColor(default_color);
                break;
            }

            if ((line[i] == '\"' || (line[i] == '\'' && singleQuotes)) && !isComment)
            {
                if (i < len)
                {

                    SetColor(quote_color);
                    quotechar = line[i];

                    do
                    {
                        putchar(line[i++]);
                    } while (line[i] != quotechar && i < len && i < wrapSize);
                    putchar(line[i]);
                    SetColor(default_color);
                }
            }
            else
            {
                if (i == 0 || is_separator(line[i - 1]))
                {
                    for (int k = 0; k < syntaxKeywordsSize; k++)
                    {

                        if (pos = FindString(line + i, keywords[k].keyword) != -1 && is_separator(line[i + pos + strlen(keywords[k].keyword)]))
                        {
                            /*  printf("[%s]", keywords[k].keyword);
                             MakePause(); */
                            if (!memcmp(keywords[k].keyword, line + pos + i - 1, strlen(keywords[k].keyword))) // Faster than strncmp()
                            {
                                SetColor(keywords[k].color);

                                printf("%.*s", strlen(keywords[k].keyword), line + i);
                                i += strlen(keywords[k].keyword) + pos - 1;

                                SetColor(default_color);
                                break;
                            }
                        }
                    }
                }
                putchar(line[i]);
            }
            /*}
             else
            {
                putchar(line[i]);
            } */
        }
        else
        {

            break;
        }
    }
    DisplayCursor(true);
    SetColor(bg_color);
}
