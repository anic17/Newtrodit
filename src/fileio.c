
int check_file(char *filename)
{
#ifdef _WIN32
    return (((_access(filename, 0)) != -1 && (_access(filename, 6)) != -1)) ? 0 : 1;
#else
    return 1;
#endif
}

char *get_full_path(const char *filename)
{
    char *full_path = calloc(MAX_PATH + 1, sizeof(char));

#ifdef _WIN32
    if (!_fullpath(full_path, filename, MAX_PATH))

#else
    full_path = realpath(filename, NULL);
    if (!full_path)
#endif
        return NULL;
    return full_path;
}

int valid_filename(char *filename)
{
    return strpbrk(filename, "*?\"<>|\x1b") == NULL;
}



int write_file(File *tstack, FILE *fp)
{
    size_t nl_len = strlen_n(tstack->newline); // Calculate the length just once and not for every iteration
    for (size_t i = 1; i <= tstack->linecount; i++)
    {
        fwrite(tstack->line[i]->str, sizeof(char), tstack->line[i]->len, fp); // DANGER: This approach is dangerous and may lead to buffer overflows
        if (i < tstack->linecount)
        {
            fwrite(tstack->newline, sizeof(char), nl_len, fp);
        }
    }
    return 1;
}

int save_file(File *tstack, char *savefile, bool saveDialog)
{
    char *tmp_filename = calloc(MAX_PATH + 1, sizeof(utf8_int32_t));
    if (savefile && !(tstack->file_flags & IS_UNTITLED))
        utf8cpy(tmp_filename, savefile);

    if (tstack->file_flags & IS_UNTITLED || saveDialog)
    {

        print_message(saveDialog ? NEWTRODIT_PROMPT_SAVE_FILE_AS : NEWTRODIT_PROMPT_SAVE_FILE);
        fgets(tmp_filename, MAX_PATH * sizeof(utf8_int32_t), stdin);

        tmp_filename[utf8cspn(tmp_filename, "\r\n")] = '\0';

        if (tmp_filename[0] == '\0')
        {
            load_all_newtrodit(tstack, NULL);
            function_aborted(tstack, tmp_filename);
            return 0;
        }

        remove_quotes(tmp_filename);

        if (!valid_file_name(tmp_filename))
        {
            load_all_newtrodit(tstack, NULL);
            print_message("%s%s", NEWTRODIT_FS_FILE_INVALID_NAME, tmp_filename);
            getch_n();
            return 0;
        }
    }
    FILE *savefp = fopen(tmp_filename, "wb");
    if (savefp)
    {
        tstack->fullpath = get_full_path(tmp_filename);
        memcpy(tstack->filename, last_token(tmp_filename, PATHDELIMS), MAX_PATH * sizeof(utf8_int32_t));
    }

    if (!savefp)
    {
        load_all_newtrodit(tstack, NULL);
        print_message(NEWTRODIT_FS_FILE_SAVE_ERR, tmp_filename);
        getch_n();
        return 0;
    }
    set_status_msg(true, NEWTRODIT_FILE_SAVED);
    load_all_newtrodit(tstack, ed.status_msg);

    write_file(tstack, savefp);
    fclose(savefp);
    tstack->file_flags &= ~(IS_UNTITLED | IS_MODIFIED);
    tstack->file_flags |= IS_SAVED;
    tstack->fwrite_time = time(NULL); // Set the write time to the current time
    return 0;
}

void save_modified_file(File *tstack)
{
    if (tstack->file_flags & IS_MODIFIED)
    {
        print_message("%s", NEWTRODIT_PROMPT_SAVE_MODIFIED_FILE);
        if (yes_no_prompt(true))
            save_file(tstack, NULL, false);
    }
}

int close_file(File *tstack)
{
    save_modified_file(tstack);
    print_message("%s", NEWTRODIT_PROMPT_CLOSE_FILE);
    if (yes_no_prompt(false))
    {
        display_status(tstack, ed.status_msg);
        return 0;
    }
    if (ed.open_files > 1)
    {
        if (free_buffer(&file[ed.file_index]))
        {
            if (ed.file_index < ed.open_files - 1) // TODO: Fix strsave not copying
            {
                for (int k = ed.file_index; k < ed.open_files; k++)
                    memcpy(&file[k], &file[k + 1], sizeof(*file));
            }
            ed.open_files--;

            if (ed.file_index > 0)
                ed.file_index--;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if ((tstack->file_flags & IS_UNTITLED && !tstack->file_flags & IS_MODIFIED) || tstack->file_flags & IS_MODIFIED || tstack->file_flags & IS_SAVED || tstack->file_flags & IS_UNTITLED)
        {
            if (free_buffer(&file[ed.file_index]))
                allocate_buffer(&file[ed.file_index]);
        }
    }
    load_all_newtrodit(tstack, NULL);
    return 1;
}

int load_file(File *tstack, FILE *fp)
{
    fseek(fp, 0, SEEK_SET); // Set fp to the beginning of the file pointer
    char *read_header_bom = calloc(4, sizeof(char));
    size_t headerbytesread = fread(read_header_bom, sizeof(char), 4, fp);
    tstack->encoding = get_file_encoding(read_header_bom, headerbytesread, &tstack->encoding_bom_len); // Identify the encoding of the file being read, assume UTF-8 if none is detected (could this be an issue?)
    free(read_header_bom);

    fseek(fp, tstack->encoding_bom_len, SEEK_SET); // Start reading the file after the BOM, if present

    char unicode_buffer[5] = {0}; // A Unicode codepoint is max 4 bytes, and make sure to make room for the null terminator
    int ch = 0;
    size_t xps = 0, uxps = 0, yps = 1, uidx = 0;
    size_t nl_len = strlen_n(tstack->newline);
    size_t nl_idx = 0;
    tstack->linecount = 0;
    tstack->size = 0;
    bool reading_unicode = false, full_unicode_sequence = false;
    const char utf8_replacement_char[3] = {0xef, 0xbf, 0xbd};
    switch (tstack->encoding)
    {
    case ENCODING_UTF8:
    case ENCODING_UTF8BOM:
    {
        while ((ch = fgetc(fp)) != EOF)
        {
            if (tstack->linecount == 0)
                tstack->linecount++;
            if (ch == tstack->newline[nl_idx])
            {
                if (++nl_idx >= nl_len)
                {
                    memset(&tstack->line[yps]->str[xps - nl_idx + 1], 0, nl_len - 1);
                    printf("(\\n)\n");
                    tstack->linecount++;
                    tstack->size += nl_len;
                    xps = 0;
                    uxps = 0;
                    yps++;
                    uidx = 0;
                    if (yps >= tstack->alloc_lines - 1)
                        change_allocated_lines(tstack, tstack->alloc_lines, tstack->alloc_lines + LINE_Y_INCREASE);
                    continue;
                }
            }
            else
            {
                nl_idx = 0;
            }
            if (ch < 0x80)
            {
                if (uidx > 0)
                    reading_unicode = true;
            }
            if (ch >= 0x80 || reading_unicode)
            {
                if (uidx < 3)
                    unicode_buffer[uidx++] = ch;
                else
                    reading_unicode = true;

                if (!utf8valid(unicode_buffer) || reading_unicode)
                {
                    printf("[%s]   (", unicode_buffer);
                    for (size_t i = 0; i < uidx; i++)
                    {
                        printf("%02x ", unicode_buffer[i]);
                    }
                    printf(") Is string valid? %s\n", !utf8valid(unicode_buffer) ? "True" : "False");
                    if (!utf8valid(unicode_buffer))
                    {
                        memcpy(&tstack->line[yps]->str[xps], unicode_buffer, uidx);
                        tstack->line[yps]->len += uidx;
                        xps += uidx;
                        tstack->size += uidx;
                    }
                    else
                    {
                        memcpy(&tstack->line[yps]->str[xps], utf8_replacement_char, sizeof(utf8_replacement_char)); // If the UTF-8 sequence is invalid, replace it with Replacement Character
                        tstack->line[yps]->len += sizeof(utf8_replacement_char);
                        xps += sizeof(utf8_replacement_char);
                        tstack->size += sizeof(utf8_replacement_char);

                        printf(" invalid unicode! ");
                    }

                    uidx = 0;
                    memset(unicode_buffer, '\0', 4);
                    reading_unicode = false;
                    tstack->line[yps]->ulen++;
                    reading_unicode = false;
                }
            }
            else
            {
                tstack->line[yps]->str[xps++] = ch;
                printf("{%02x}", ch);

                tstack->line[yps]->len++;
                tstack->size++;
                tstack->line[yps]->ulen++;
            }
        }
        break;
    }
    default:
        set_status_msg(true, NEWTRODIT_ERROR_UNSUPPORTED_ENCODING);
    }

    return 1;
}

int open_file(File *tstack)
{
    save_modified_file(tstack);
    char *tmp_openfile = calloc(sizeof(utf8_int32_t), MAX_PATH + 1);
    print_message(NEWTRODIT_PROMPT_FOPEN);
    fgets(tmp_openfile, MAX_PATH * sizeof(utf8_int32_t), stdin);
    tmp_openfile[utf8cspn(tmp_openfile, "\r\n")] = '\0';

    FILE *fp = fopen(tmp_openfile, "rb");
    if (!fp)
    {
        set_status_msg(false, NEWTRODIT_FS_FILE_OPEN_ERR, tmp_openfile);
        load_all_newtrodit(tstack, ed.status_msg);
            free(tmp_openfile);

        return 0;
    }

    tstack->fullpath = get_full_path(tmp_openfile);
    memcpy(tstack->filename, last_token(tmp_openfile, PATHDELIMS), MAX_PATH * sizeof(utf8_int32_t));

    set_status_msg(true, NEWTRODIT_FILE_OPENED);
    load_all_newtrodit(tstack, NULL);
    load_file(tstack, fp);
    free(tmp_openfile);

    return 1;
}
