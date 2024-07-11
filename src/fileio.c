
int check_file(char *filename)
{
#ifdef WIN32
    return (((_access(filename, 0)) != -1 && (_access(filename, 6)) != -1)) ? 0 : 1;
#else
    return 1;
#endif
}

int open_file(File *tstack)
{
    return 1;
}

int load_file(File *tstack)
{
    return 1;
}

int write_file(File *tstack, FILE *fp)
{
    for (size_t i = 0; i < tstack->linecount; i++)
    {
        fputs(tstack->line[i]->str, fp);
        if (i < tstack->linecount - 1)
        {
            fputs(tstack->newline, fp);
        }
    }
    return 1;
}

int save_file(File *tstack, char *savefile, bool saveDialog)
{
    char *tmp_filename = calloc(MAX_PATH + 1, sizeof(utf8_int32_t));
    if (savefile)
        utf8cpy(tmp_filename, savefile);

    if (tstack->file_flags & IS_UNTITLED || saveDialog)
    {

        print_message(NEWTRODIT_PROMPT_SAVE_FILE);
        fgets(tmp_filename, MAX_PATH, stdin);

        tmp_filename[utf8cspn(tmp_filename, "\r\n")] = L'\0';

        if (tmp_filename[0] == '\0')
        {
            // FunctionAborted(tstack, tmp_filename);
            return 0;
        }

        remove_quotes(tmp_filename);

        if (!valid_file_name(tmp_filename))
        {
            print_message("%s%s", NEWTRODIT_FS_FILE_INVALID_NAME, tmp_filename);
            getch_n();
            return 0;
        }
    }

    FILE *savefp = fopen(tmp_filename, "wb");
    if (!savefp)
    {
        print_message(NEWTRODIT_FS_FILE_SAVE_ERR, tmp_filename);
        getch_n();
        return 0;
    }
    for (size_t i = 1; i <= tstack->linecount; i++)
    {
        fputs(tstack->line[i]->str, savefp);
        if (i < tstack->linecount)
            fputs(tstack->newline, savefp);
    }
    fclose(savefp);
    print_message(NEWTRODIT_FILE_SAVED);
    ed.dirty = true;
    return 0;
}