#include "newtrodit.h"
#include "dialog.h"
#include "globals.h"
#include "line.h"
#include "unicode.c"
#include "input.h"


Editor ed;

File **file;

#include "newtrodit_gui.h"
#include "fileio.h"

int allocate_buffer(File **tstack)
{
    *tstack = calloc(1, sizeof(File));
    (*tstack)->file_flags = IS_UNTITLED;

    (*tstack)->filename = calloc(MAX_PATH + 1, sizeof(char));
    memcpy((*tstack)->filename, default_filename, MAX_PATH);

    (*tstack)->fwrite_time = time(NULL);
    (*tstack)->fread_time = time(NULL);
    (*tstack)->language = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(char));
    memcpy((*tstack)->language, default_language, utf8len_n(default_language));
    (*tstack)->linenumber_wide = 3;
    (*tstack)->linenumber_padding = 1;
    (*tstack)->linecount = 0;
    (*tstack)->xpos = 0;
    (*tstack)->ypos = 1;
    (*tstack)->display_start = 0;
    (*tstack)->size = 0;
    (*tstack)->alloc_lines = DEFAULT_ALLOC_LINES;
    (*tstack)->line = calloc(DEFAULT_ALLOC_LINES, sizeof(Line *));
    for (size_t i = 0; i < DEFAULT_ALLOC_LINES; i++)
    {
        create_line(*tstack, i);
    }

    (*tstack)->newline = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(char));
    memcpy((*tstack)->newline, default_newline, utf8len_n(default_newline));

    (*tstack)->display_end = (*tstack)->ypos;

    return 1;
}

int init_editor()
{

    ed.open_files = 1;
    ed.file_index = 0;
    ed.log_file_name = NULL;

    ed.useLogFile = false;
    ed.lineNumbers = true;
    ed.dirty = false;
    // ed.status_msg = calloc(DEFAULT_ALLOC_SIZE, sizeof(utf8_int32_t));
    ed.status_msg = NULL;
    file = calloc(sizeof(File *), open_files);

#ifdef _WIN32
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001");
#endif

    get_cols_rows(&ed.xsize, &ed.ysize);
    vt_settings(true);
    alternate_buffer(true);

    allocate_buffer(&file[ed.file_index]); // Pass the address of the File pointer
    load_all_newtrodit(file[ed.file_index], NULL);
    echo_on();
    canon_on();
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
    if (ch.flags != 0)
    {
        switch (ch.flags)
        {
        case LEFT:
            if (_xpos > 0)
            {
                _xpos--;
            }
            break;
        case RIGHT:
            //        if(tstack->line[_ypos]->bufx[])
            _xpos++;
            break;
        case DOWN:
            _ypos++;
            break;
        case ENTER:
            _ypos++;
            _xpos = 0;

            break;
        case UP:
            if (_ypos > 1)
            {
                _ypos--;
            }
            break;
        }
    }
    else if (ch.utf8char != 0)
    {
        switch (ch.utf8char)
        {
        case BS:
            if (_xpos > 0)
            {
                fputs("\b \b", stdout);
                _xpos--;
                _uxpos--;
            }
            break;
        case CTRLS:
            save_file(tstack, tstack->filename, false);
            printf("OK");
            break;
        case CTRLQ:
            quit_newtrodit(tstack);
            break;
        default:
            if (tstack->linecount == 0)
                tstack->linecount++;
            insert_utf8_char(tstack->line[_ypos], ch.utf8char, _xpos);
                    //gotoxy(_xpos + file[ed.file_index]->linenumber_wide + file[ed.file_index]->linenumber_padding, _ypos);
            display_char(ch.utf8char, stdout);
            _xpos += codepoint_len(ch.utf8char);
            _uxpos++;
            tstack->file_flags |= IS_MODIFIED;
            
        }
    }
    return 0;
}

int editor_main()
{
    InputUTF8 ch = {0};
    Line *lptr;
    while (1)
    {
        // display_cursor_pos(file[ed.file_index]);
        gotoxy(_uxpos + file[ed.file_index]->linenumber_wide + file[ed.file_index]->linenumber_padding, _ypos);

        ch = GetNewtroditInput();
        if (_ypos >= file[ed.file_index]->alloc_lines - 1)
        {

            change_allocated_lines(file[ed.file_index], file[ed.file_index]->alloc_lines, file[ed.file_index]->alloc_lines + LINE_Y_INCREASE);
            print_message("Increasing line number by %zu", LINE_Y_INCREASE);
            getchar();
        }
        /* if (_xpos >= file[ed.file_index]->line[_ypos]->bufx - 1)
        {
            lptr = increase_line(file[ed.file_index]->line[_ypos], LINE_SIZE);
            if (!lptr)
            {
                print_message(NEWTRODIT_ERROR_ALLOCATION_FAILED);
            }
        } */
        handle_keystrokes(ch, file[ed.file_index]);
        if (ed.dirty)
        {
            display_status(file[ed.file_index], ed.status_msg);
        }
        else
        {
            display_cursor_pos(file[ed.file_index], NULL);
        }
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