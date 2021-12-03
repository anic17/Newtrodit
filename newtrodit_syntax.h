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


    This file is from my other project syntax, with some modifications.
    https://github.com/anic17/syntax

*/

#ifdef SYNTAX_H
#error "syntax.h already included"
#else
#define SYNTAX_H
#endif

#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define YELLOW 0x6
#define WHITE 0x7
#define GRAY 0x8
#define LIGHTBLUE 0x9
#define LIGHTGREEN 0xa
#define LIGHTCYAN 0xb
#define LIGHTRED 0xc
#define LIGHTMAGENTA 0xd
#define LIGHTYELLOW 0xe
#define LIGHTWHITE 0xf

#define default_color WHITE
#define comment_color GRAY
#define quote_color LIGHTYELLOW
#define num_color GREEN

#define LINE_MAX 8192

// Boolean type definition
#ifndef __bool_true_false_are_defined
#define bool unsigned short
#define false 0
#define true 1
#endif

struct
{
    char *keyword;
    int color;
} keywords[] = {
    {"break", 5},
    {"continue", 5},
    {"return", 5},

    {"const", 9},
    {"volatile", 9},
    {"extern", 9},
    {"static", 9},

    {"char", 9},
    {"int", 9},
    {"short", 9},
    {"float", 9},
    {"double", 9},
    {"long", 9},
    {"bool", 9},
    {"void", 9},

    {"double_t", 0xa},
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

    {"int8_t", 0xa},
    {"int16_t", 0xa},
    {"int32_t", 0xa},
    {"int64_t", 0xa},

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

    {"include", 0xb},
    {"pragma", 0xb},
    {"define", 0xb},
    {"ifdef", 0xb},
    {"ifndef", 0xb},
    {"endif", 0xb},
    {"undef", 0xb},
    {"error", 0xb},

    {"main", 6},

    {"0x", 0xa},
    {"0b", 0xa},

    // Macro constants
    {"NULL", 0x9},
    {"true", 0x9},
    {"false", 0x9},

};

struct
{
    char *keyword;
    int color;
} comment[] = {
    {"//", LIGHTCYAN},

};

size_t keyword_size = sizeof(keywords) / sizeof(keywords[0]);

int is_separator(int c) // Taken from https://github.com/antirez/kilo/blob/69c3ce609d1e8df3956cba6db3d296a7cf3af3de/kilo.c#L366
{
    return c == '\0' || isspace(c) || strchr("#,.()+-/*=~[];{}", c) != NULL;
}

char *color_line(char *line)
{
    size_t comment_size = sizeof(comment) / sizeof(comment[0]);

    // Make syntax highlighting
    int isComment = false;

    static int multiLineComment = false; // Must be static, otherwise it will be reset to false after the first line
    int pos, line_num = 0;
    SetColor(default_color);

    size_t len = strlen(line);
    for (int i = 0; i < len; i++)
    {
        if (isdigit(line[i]) && !isComment)
        {

            if (is_separator(line[i - 1]))
            {
                SetColor(num_color);
                while (isdigit(line[i]) || tolower(line[i]) == 'x' || tolower(line[i]) == 'b')
                {
                    putchar(line[i++]);
                }
                SetColor(default_color);
            }
        }

        if (!strncmp(line + i, "//", 2) && !isComment)
        {
            SetColor(comment_color);
            printf("%.*s", wrapSize, line + i);
            SetColor(default_color);
           break;
        }

        
        if (line[i] == '\"' && !isComment)
        {
            SetColor(quote_color);
            putchar(line[i++]);
            if (i == len)
            {
                break;
            }
            while (line[i] != '\"')
            {
                if (i < len || line[i] != '\0')
                {
                    putchar(line[i++]);
                }
                else
                {
                    break;
                }
            }
            putchar('\"'); // Works but it's very bad codded

            SetColor(default_color);
        }
        else
        {
            for (int k = 0; k < keyword_size; k++)
            {

                if (pos = FindString(line + i, keywords[k].keyword) != -1 && is_separator(line[i + pos + strlen(keywords[k].keyword)]))
                {

                    if (is_separator(line[i - 1]) || i == 0)
                    {
                        if (!strncmp(keywords[k].keyword, line + pos + i - 1, strlen(keywords[k].keyword)))
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
    }
    SetColor(0x07);
}