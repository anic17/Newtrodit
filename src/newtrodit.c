#include "newtrodit.h"
#include "line.h"
#include "newtrodit_gui.h"
#include "unicode.c"
#include "input.h"
#include "fileio.h"

void sigsegv_handler()
{
    /* signal(SIGSEGV, sigsegv_handler);
    alternate_buffer(false);
    fprintf(stderr, "Fatal Newtrodit exception! (segmentation violation)\nReport this issue to the project's GitHub.");
    fflush(stdout);
    exit(errno); */
}

int init_editor()
{

#ifdef WIN32

    if (!_isatty(_fileno(stdout)) || !_isatty(_fileno(stderr)))
#else
    if (!isatty(STDOUT_FILENO))
#endif
    {
        /*
             If for some reason stdout or stderr are still redirected to a file, show an error and quit
        */
        fprintf(stderr, "%s\n", NEWTRODIT_ERROR_REDIRECTED_TTY);
        exit(1);
    }

    signal(SIGSEGV, sigsegv_handler);
    signal(SIGINT, NULL);
    ed.open_files = 1;
    ed.file_index = 0;
    ed.log_file_name = NULL;

    ed.useLogFile = false;
    ed.lineNumbers = true;
    ed.dirty = false;
    ed.displayStatusOnce = false;
    // ed.status_msg = calloc(DEFAULT_ALLOC_SIZE, sizeof(utf8_int32_t));
    ed.status_msg = NULL;
    file = calloc(open_files, sizeof(File *));

#ifdef _WIN32
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    SetConsoleCtrlHandler(NULL, TRUE);
    DWORD dwState = 0;

    // GetConsoleMode(hStdin, &dwState);
    // SetConsoleMode(hStdin, dwState);

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        fprintf(stderr, "%s", NEWTRODIT_ERROR_FAILED_CONSOLE_ATTRIB);
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    orig_termios.c_iflag |= BRKINT | ICRNL | INPCK | ISTRIP | IXON;
    /* output modes - disable post processing */
    // raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    orig_termios.c_cflag &= ~(CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    orig_termios.c_lflag |= ECHO | ICANON | IEXTEN | ISIG;
#endif

    get_cols_rows(&ed.xsize, &ed.ysize);
    vt_settings(true);
    //alternate_buffer(true);

    allocate_buffer(&file[ed.file_index]); // Pass the address of the file pointer
    load_all_newtrodit(file[ed.file_index], NULL);

    return 1;
}

int end_editor()
{
    alternate_buffer(false);
    echo_on();
    canon_on();
    return 1;
}

int handle_keystrokes(InputUTF8 ch, File *tstack)
{
    if (_ypos >= tstack->alloc_lines - 1)
        change_allocated_lines(tstack, tstack->alloc_lines, tstack->alloc_lines + LINE_Y_INCREASE);

    char *ptr = NULL, *ptr2 = NULL;
    utf8_int32_t codep = 0;
    if (ch.flags != 0)
    {
        switch (ch.flags)
        {
        case LEFT:
            if (_uxpos > 0)
            {
                previous_char_xpos(tstack->line[_ypos]->str, &_xpos);
                                _uwxpos -= previous_char_uwlen(tstack->line[_ypos]->str, _xpos);

                _uxpos--;
            }
            else if (_ypos > 1)
            {
                _xpos = tstack->line[--_ypos]->len;
                _uxpos = tstack->line[_ypos]->ulen;
                _uwxpos = utf8wnlen(tstack->line[_ypos]->str, _uxpos);
                decrease_scroll(tstack, 1);
            }
            break;
        case RIGHT:
            if (_uxpos < tstack->line[_ypos]->ulen)
            {

                _uwxpos += next_char_uwlen(tstack->line[_ypos]->str, _xpos);
                next_char_xpos(tstack->line[_ypos]->str, &_xpos);

                _uxpos++;
            }
            else if (_ypos < tstack->linecount)
            {
                _xpos = 0;
                _uxpos = 0;
                _uwxpos = 0;
                _ypos++;
                increase_scroll(tstack, 1);
            }
            break;
        case DOWN:
            if (_ypos <= tstack->linecount)
            {
                _ypos++;
                if (_uxpos > tstack->line[_ypos]->ulen)
                    _uxpos = tstack->line[_ypos]->ulen;
                _xpos = utf8getxpos(tstack->line[_ypos]->str, _uxpos);
                set_status_msg(false, "Down: %zu", utf8wnlen(tstack->line[_ypos]->str, _uxpos));
                _uwxpos = utf8wnlen(tstack->line[_ypos]->str, _uxpos);
                increase_scroll(tstack, 1);
            }

            break;
        case UP:
            if (_ypos > 1)
            {
                _ypos--;

                if (_uxpos > tstack->line[_ypos]->ulen)
                {
                    _uxpos = tstack->line[_ypos]->ulen;
                    _xpos = utf8getxpos(tstack->line[_ypos]->str, _uxpos);
                    _uwxpos = utf8wnlen(tstack->line[_ypos]->str, _uxpos);
                }
            }
            break;
        case DEL:
            if (_xpos < tstack->line[_ypos]->len)
            {
                ptr = tstack->line[_ypos]->str + _xpos;
                ptr2 = utf8codepoint(tstack->line[_ypos]->str + _xpos, &codep);
                if (delete_str(tstack->line[_ypos], _xpos, ptr2 - ptr))
                    tstack->line[_ypos]->ulen--;
            }
            else
            {
                /* if (_ypos > 1)
                {
                    _ypos--;
                    tstack->uxpos = tstack->line[_ypos]->ulen;
                    tstack->xpos = tstack->line[_ypos]->len;
                    tstack->linecount--;
                } */
            }
            break;

        case CTRLF4:
        case ALTF4:
            quit_newtrodit(tstack);
            break;
        case 0xc0000043:
            if (ch.flags & ALT_BITMASK)
            {
                printf("Debug initiated crash\n");
                raise(SIGSEGV);
            }
            break;
        }
    }
    if (ch.utf8char == CTRLI && devMode)
    {
        if (ch.flags & SHIFT_BITMASK)
        {
            char inbuf[100];
            clear_partial(0, ed.ysize - 2, ed.xsize, 1);
            printf("Insert hex: ");
            scanf("%s", inbuf);
            //(inbuf, sizeof inbuf, stdin);
            // inbuf[strcspn(inbuf, "\r\n")] = '\0';
            display_status(tstack, NULL);
            display_cursor_pos(tstack, NULL);
            ch.utf8char = strtol(inbuf, NULL, 16);
            ch.flags = 0;
        }
    }

    if (ch.utf8char != 0)
    {
        switch (ch.utf8char)
        {
        case BS:
            if (_xpos > 0)
            {
                ptr = tstack->line[_ypos]->str + _xpos;
                ptr2 = utf8rcodepoint(tstack->line[_ypos]->str + _xpos, &codep);
                size_t temp_xpos = previous_char_uwlen(tstack->line[_ypos]->str, _xpos - 1);

                if (delete_str(tstack->line[_ypos], _xpos - (ptr - ptr2), ptr - ptr2))
                {
                    // printf("delstr ok %zu: %zu", temp_xpos, _xpos);

                    _xpos -= ptr - ptr2;
                    _uwxpos -= temp_xpos;

                    _uxpos--;
                    tstack->line[_ypos]->ulen--;
                }
            }
            else
            {
                if (_ypos > 1)
                {
                    _ypos--;
                    tstack->uxpos = tstack->line[_ypos]->ulen;
                    tstack->xpos = tstack->line[_ypos]->len;
                    tstack->uwxpos = utf8wlen(tstack->line[_ypos]->str);
                    tstack->linecount--;
                }
            }
            break;
        case ENTER:
                split_row(tstack, _xpos, &_ypos);
                increase_scroll(tstack, 1);
            
            break;
        case CTRLS:
            save_file(tstack, tstack->filename, ch.flags & SHIFT_BITMASK); // If Shift is pressed, always display save as dialog
            break;
        case CTRLW:
            close_file(tstack);
            break;
        case CTRLO:
            open_file(tstack);
            break;
        case CTRLQ:
            quit_newtrodit(tstack);
            break;
        case CTRLC:
            break;
        case CTRLD:
            if (ch.flags & SHIFT_BITMASK)
                toggle_option(&devMode, NEWTRODIT_DEV_TOOLS);
            else
                set_status_msg(true, "%s", tstack->line[_ypos]->str);

            break;
        case ESC:
            clear_status_msg();
            display_status(tstack, NULL);
            break;
        default:

            if (ch.utf8char > 31)
            {
                if (tstack->linecount == 0)
                    tstack->linecount++;
                insert_utf8_char(tstack->line[_ypos], ch.utf8char, _xpos);
                _xpos += codepoint_len(ch.utf8char);
                _uxpos++;
                _uwxpos += codepoint_width(NULL, ch.utf8char);
                tstack->file_flags |= IS_MODIFIED;
            }
            break;
        }
    }
    if ((ch.utf8char > 31 || (ch.utf8char == 8 && !(ch.flags & SHIFT_BITMASK))) && !syntaxHighlighting)
        render_line(tstack, _ypos);

    return 0;
}

int editor_main()
{
    InputUTF8 ch;
    Line *lptr;

    while (1)
    {

        set_display_pos(file[ed.file_index]);

        display_line_numbering(file[ed.file_index], _ypos);
        gotoxy(_uwxpos + file[ed.file_index]->linenumber_wide + file[ed.file_index]->linenumber_padding, file[ed.file_index]->scroll_pos.y);

        ch = get_newtrodit_input();

        handle_keystrokes(ch, file[ed.file_index]);
        if (ed.status_msg == NULL && ed.dirty)
        {
            display_bottom_bar(NULL);
            ed.dirty = false;
        }

        if (ed.displayStatusOnce && !ed.dirty)
        {
            clear_status_msg();
            display_status(file[ed.file_index], ed.status_msg);
            ed.displayStatusOnce = false;
        }

        if (_xpos >= file[ed.file_index]->line[_ypos]->bufx - 1)
        {
            lptr = increase_line(file[ed.file_index]->line[_ypos], LINE_SIZE);
            if (!lptr)
            {
                print_message(NEWTRODIT_ERROR_ALLOCATION_FAILED);
                getch();
                ed.dirty = true;
            }
        }
        correct_position(file[ed.file_index]->line[_ypos], &_xpos, &_uxpos, &_uwxpos);

        if (ed.dirty)
        {
            display_status(file[ed.file_index], ed.status_msg);
            if (ed.displayStatusOnce)
                ed.dirty = false;
        }
        else
        {
            display_cursor_pos(file[ed.file_index], NULL);
        }
        set_scroll(file[ed.file_index]);
        display_line(file[ed.file_index], _ypos, file[ed.file_index]->line[_ypos]->rlen);
    }
    return 1;
}

int main()
{

    setlocale(LC_ALL, ".utf8");
    init_editor();

    editor_main();

    end_editor();
}