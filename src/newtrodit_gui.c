
/* void TopHelpBar()
{
    SetColor(fg_color);
    ClearPartial(0, 0, ed.xsize, 1);
    printf("%.*s", ed.xsize, NEWTRODIT_DIALOG_MANUAL_TITLE);
    SetColor(bg_color);
    return;
}

void BottomHelpBar()
{
    SetColor(fg_color);
    PrintBottomString(NEWTRODIT_DIALOG_MANUAL);
    SetColor(bg_color);
    return;
}
 */

void newtrodit_name_load()
{

    set_color(title_font_color);
    set_color(title_bg_color);
    clear_partial(0, 0, ed.xsize, 1);
    printf(" Newtrodit %s\n", newtrodit_version);
}

int center_text(char *text, int yps) // Algorithm: (ed.xsize / 2) - (len / 2)
{

    int center_text = (ed.xsize / 2) - (utf8len_n(text) / 2);
    gotoxy(center_text, yps);
    int pos = wrapSize - utf8len_n(text);
    if (pos < 0)
        pos = abs(pos);

    printf("%.*s", pos, text);
    return center_text;
}

void display_file_type(File *tstack)
{
    int x = ed.xsize - utf8len_n(tstack->language) - 2;
    gotoxy(x, 0);
    fputs(tstack->language, stdout);
}

void display_bottom_bar(char *status)
{
    gotoxy(0, ed.ysize - 1);
    printf("%s | ", (status != NULL) ? status : NEWTRODIT_DIALOG_BOTTOM_HELP);
}

void print_message(const char *str, ...)
{

    va_list args;
    va_start(args, str);
    char *printbuf = calloc(ed.xsize + 1, sizeof(char));
    vsnprintf(printbuf, ed.xsize + 1, str, args);
    clear_partial(0, ed.ysize - 1, ed.xsize, 1);
    gotoxy(0, ed.ysize - 1);
    printf(str, printbuf);
    // printf("%.*s", ed.xsize, printbuf); // Don't get out of the buffer
    free(printbuf);
    va_end(args);
    return;
}

int set_display_pos(File *tstack)
{
    if (tstack->xpos - (ed.lineNumbers ? (tstack->linenumber_wide + tstack->linenumber_padding) : 0) > ed.xsize - 1)
        tstack->begin_display.x = tstack->xpos - (ed.lineNumbers ? (tstack->linenumber_wide + tstack->linenumber_padding) : 0) - (ed.xsize - 1);
    else
        tstack->begin_display.x = 0;

    if (tstack->ypos >= ed.ysize - 3)
    {
        tstack->begin_display.y = tstack->ypos - ed.ysize + 4; // When reaching the penultimate row, start scrolling
    }
    else
    {
        tstack->begin_display.y = 0;
        tstack->scroll_pos.y = tstack->ypos;
    }

    tstack->scroll_pos.x = 0;
    return 0;
}

void display_cursor_pos(File *tstack, char *status)
{
    size_t len = 3; // Not zero because it already accounts for the " | " separator between position and status message
    if (!status)
        len += utf8len_n(NEWTRODIT_DIALOG_BOTTOM_HELP);
    else
        len += utf8len_n(status);

    clear_partial(len, ed.ysize, ed.xsize - len, 1);
    if (fullCursorInfoDisplay)
        printf(longPositionDisplay ? "Line %zu/%zu (%zu%%), Column %zu/%zu (%zu%%)" : "Ln %zu/%zu (%zu%%), Col %zu/%zu (%zu%%)", tstack->ypos, tstack->linecount + !tstack->line[tstack->linecount]->len, 100 * tstack->ypos / (tstack->linecount + !tstack->line[tstack->linecount]->len), tstack->uxpos + 1, tstack->line[_ypos]->ulen + 1, (size_t)100 * (tstack->uxpos) / (!tstack->line[_ypos]->ulen ? 1 : tstack->line[_ypos]->ulen));
    else
        printf(longPositionDisplay ? "Line %zu, Column %zu" : "Ln %zu, Col %zu", tstack->ypos, tstack->uxpos + 1); // +1 because it's zero indexed
    
}

void display_status(File *tstack, char *status_msg)
{
    display_bottom_bar(status_msg);
    display_cursor_pos(tstack, status_msg);
}


void function_aborted(File *tstack, char* status_msg)
{
    print_message(NEWTRODIT_FUNCTION_ABORTED);
    getch_n();
    display_status(tstack, status_msg);
    return;
}


void load_line_numbering(File *tstack)
{
    if (ed.lineNumbers)
    {
        set_color(line_number_bg_color);
        set_color(line_number_font_color);
        clear_partial(0, 1, tstack->linenumber_wide, tstack->scroll_pos.y);
        for (size_t i = tstack->begin_display.y; i < tstack->begin_display.y + tstack->scroll_pos.y; i++)
        {
            printf("%zu", i + 1);
            if (i < tstack->begin_display.y + tstack->scroll_pos.y - 1)
                putchar('\n');
        }
        set_color(bg_color);
        set_color(fg_color);
    }
}

void display_line_numbering(File *tstack, size_t ypos)
{
    if (ed.lineNumbers)
    {
        if ((size_t) (log10(_ypos) + 1) >= tstack->linenumber_wide)
        {
            tstack->linenumber_wide = (size_t) (log10(_ypos) + 2);
            load_line_numbering(tstack);
        }
        set_color(line_number_bg_color);
        set_color(line_number_font_color);
        clear_partial(0, tstack->scroll_pos.y, tstack->linenumber_wide, 1);
        printf("%zu", ypos);
        set_color(bg_color);
        set_color(fg_color);
    }
}

void load_all_newtrodit(File *tstack, char *status)
{
    clear_screen();
    set_display_pos(tstack);
    newtrodit_name_load();
    center_text(tstack->filename, 0);
    display_file_type(tstack);
    set_color(fg_color);
    set_color(bg_color);
    display_status(tstack, status);
    load_line_numbering(tstack);
    gotoxy(tstack->linenumber_wide + tstack->linenumber_padding, 1);
}

void set_cursor_settings(int visible, int size)
{
#ifdef _WIN32
    HANDLE Cursor = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = size;
    info.bVisible = visible;
    SetConsoleCursorInfo(Cursor, &info);
#else
    /* 	info.dsize = size;
        info.bVisible = visible; */

    // Set cursor size
    if (size > 50)
    {
        printf("\x1B[\x31 q"); // Blinking block
    }
    else
    {
        printf("\x1B[\x33 q"); // Blinking underline
    }

    // Set cursor visibility
    if (visible)
    {
        printf("\x1B[?25h");
    }
    else
    {
        printf("\x1B[?25l");
    }
#endif
}

/* void UpdateTitle(File *tstack)
{
    if (tstack->is_saved)
    {
        tstack->fullpath = utf8dup(FullPath(tstack->filename));
    }
    if (tstack->is_modified)
    {
        SetTitle("Newtrodit - %s (Modified)", (fullPathTitle && tstack->is_saved && !tstack->is_untitled) ? tstack->fullpath : tstack->filename);
    }
    else
    {
        SetTitle("Newtrodit - %s", (fullPathTitle && tstack->is_saved && !tstack->is_untitled) ? tstack->fullpath : tstack->filename);
    }
} */

int yes_no_prompt(bool default_yes)
{
    return tolower(getch_n()) == (default_yes ? 'y' : 'n');
}

void quit_newtrodit(File *tstack)
{
    if (tstack->file_flags & IS_MODIFIED)
    {
        print_message(NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);
        if (!yes_no_prompt(false))
            save_file(tstack, tstack->filename, !!(tstack->file_flags & IS_UNTITLED)); // tstack->file_flags &~ IS_SAVED isn't needed due to being redundant
    }
    print_message(NEWTRODIT_PROMPT_QUIT);
    if (yes_no_prompt(true))
    {
        end_editor();
        exit(1);
    }
    ed.dirty = true;
}



int display_line(File *tstack, size_t linenum, size_t rcount)
{
    gotoxy(file[ed.file_index]->linenumber_wide + file[ed.file_index]->linenumber_padding, tstack->scroll_pos.y);

    fwrite(tstack->line[linenum]->render, rcount < tstack->line[linenum]->rlen ? rcount : tstack->line[linenum]->rlen, sizeof(char), stdout);
    return 0;
}

int toggle_option(int *option, char* msg)
{
    *option ^= 1;
    set_status_msg(false, "%s%s", msg, *option ? NEWTRODIT_DIALOG_ENABLED : NEWTRODIT_DIALOG_DISABLED);
}

/* char *TypingFunction(int min_ascii, int max_ascii, int max_len, char *oldbuf)
{
    int chr = 0, index = 0;
    char *num_str = calloc(max_len + 1, sizeof(utf8_int32_t));
    int startx = 0, starty = ed.ysize-1, orig_cursize = GetConsoleInfo(CURSOR_SIZE);
    bool overwrite_mode = false;
    size_t cblen = 0, n = -1;
    char *clipboard = NULL;

    while (chr != ENTER) // Loop while enter isn't pressed
    {
        chr = getch_n();
        if (chr == ESC)
        {
            memset(num_str, 0, max_len); // Empty the string
            break;
        }
        if (chr == BS) // Backspace
        {
            if (index > 0)
            {
                DeleteChar(num_str, --index);
                ClearPartial(startx + index, starty, (startx + strlen(num_str) - index) >= XSIZE ? (XSIZE - startx - index) : startx + strlen(num_str) - index, 1);
                printf("%s", num_str + index);
                gotoxy(startx + index, starty);
            }
            continue;
        }

        if (chr == CTRLC) // ^C (Copy to clipboard)
        {
            SetClipboardNewtrodit(num_str);
            continue;
        }
        if (chr == CTRLV) // ^V (Paste from clipboard)
        {
            clipboard = GetClipboardNewtrodit(&cblen, true); // Distinguish it from the WinAPI one
            if (clipboard)
            {
                n = 0;
                while (clipboard[n] >= min_ascii && clipboard[n] <= max_ascii && n < cblen) // Only count until valid characters are found
                {
                    n++;
                }
                if (strlen_n(num_str) + n <= max_len && n > 0)
                {
                    num_str = InsertStr(num_str, clipboard, n, false, max_len);
                    gotoxy(startx, starty);
                    fputs(num_str, stdout);
                    index += n;
                }
            }
            else
            {
                printf("\a");
            }
            continue;
        }

        if (chr & BIT_ESC0)
        {
            switch (chr & ~(BIT_ESC0))
            {
            case ALTF4:

                QuitProgram(SInf.color);
                break;
            default:
                break;
            }
        }
        if (chr & BIT_ESC224) // Special keys: 224 (0xE0)
        {
            switch (chr & (~BIT_ESC224))
            {
            case LEFT:
                if (index > 0)
                {
                    putchar('\b');
                    index--;
                }
                break;
            case RIGHT:
                if (index < max_len && num_str[index] != '\0')
                {
                    putchar(num_str[index++]);
                }
                break;
            case UP:
                if (oldbuf != NULL)
                {
                    memset(num_str, 0, max_len + 1);
                    index = strlen_n(oldbuf);
                    memcpy(num_str, oldbuf, index);

                    ClearPartial(startx, starty, (startx + strlen(num_str)) >= XSIZE ? (XSIZE - startx) : startx + strlen(num_str), 1);
                    fputs(num_str, stdout);
                    gotoxy(startx + index, starty);
                }
            case DEL:
                if (index < max_len)
                {
                    DeleteChar(num_str, index);
                    ClearPartial(startx, starty, (startx + strlen(num_str)) >= XSIZE ? (XSIZE - startx) : startx + strlen(num_str), 1);
                    fputs(num_str, stdout);
                    gotoxy(startx + index, starty);
                }
                break;
            case INS:
                overwrite_mode = !overwrite_mode;
                SetCursorSettings(true, overwrite_mode ? CURSIZE_INS : CURSIZE);
                break;
            case HOME:
                index = 0;
                gotoxy(startx, starty);
                break;
            case END:
                index = strlen_n(num_str);
                if (startx + index < XSIZE)
                {
                    gotoxy(startx + index, starty);
                }
                break;
            default:
                break;
            }
            continue;
        }
        if (chr >= min_ascii && chr <= max_ascii && chr != 0 && ((!overwrite_mode && (strlen(num_str) < max_len && index <= max_len)) || (overwrite_mode && (strlen(num_str) <= max_len && index < max_len)))) // Check if character is a between the range
        {
            if (overwrite_mode || index >= strlen(num_str))
            {
                num_str[index++] = chr;
                putchar(chr);
            }
            else
            {

                InsertChar(num_str, chr, index++, false, max_len);
                gotoxy(startx, starty);
                fputs(num_str, stdout);
                gotoxy(startx + index, starty);
            }
        }
        else
        {
            if (chr != ENTER)
            {
                putchar('\a');
            }
        }
    }
    SetCursorSettings(cursor_visible, orig_cursize);
    bool corrupted = false;
    if (max_ascii > 0x7f)
        for (int i = 0; i < strlen_n(num_str); i++)
        {
            if (num_str[i] < min_ascii || num_str[i] > max_ascii)
            {
                corrupted = true;
                num_str[i] = min_ascii;
            }
        }
    if (corrupted)
    {
        PrintBottomString("Warning: Possible stack corruption detected. Please report this issue.");
        getch_n();
    }
    return num_str;
}
 */
