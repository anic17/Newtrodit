size_t correct_position(Line *line, size_t *xpos, size_t *uxpos);

Line *create_line(File *tstack, size_t ypos)
{
    tstack->line[ypos] = calloc(sizeof(Line), 1);
    tstack->line[ypos]->str = calloc(sizeof(utf8_int32_t), LINE_SIZE);
    tstack->line[ypos]->render = calloc(sizeof(utf8_int32_t), LINE_SIZE);
    tstack->line[ypos]->bufx = 10;
    tstack->line[ypos]->render_bufx = LINE_SIZE;
    tstack->line[ypos]->len = 0;
    tstack->line[ypos]->rlen = 0;
    tstack->alloc_lines++;
    return tstack->line[ypos];
}

size_t change_allocated_lines(File *tstack, size_t old_line_count, size_t new_line_count)
{
    Line **linestack = realloc_n(tstack->line, old_line_count * sizeof(Line *), new_line_count * sizeof(Line *));
    if (!linestack)
    {
        return old_line_count;
    }
    for (size_t i = old_line_count; i < new_line_count; i++)
    {
        create_line(tstack, i);
    }
    tstack->alloc_lines = new_line_count;
    return new_line_count;
}

Line *increase_line(Line *line, size_t increment)
{
    if (!line)
        return NULL;

    size_t new_buf_size = line->bufx + increment;

    // Reallocate the line buffer
    char *new_str = realloc_n(line->str, line->bufx * sizeof(utf8_int32_t), new_buf_size * sizeof(utf8_int32_t));
    if (!new_str)
        return NULL;

    line->str = new_str;
    line->bufx = new_buf_size;

    return line; // Success
}

Line *increase_line_render(Line *line, size_t increment)
{
    if (!line)
        return NULL;

    size_t new_buf_size = line->render_bufx + increment;

    char *new_render = realloc_n(line->render, line->render_bufx * sizeof(utf8_int32_t), new_buf_size * sizeof(utf8_int32_t));

    if (!new_render)
        return NULL;

    line->render = new_render;
    line->render_bufx = new_buf_size;

    return line; // Success
}

Line *insert_row(Line **lines, size_t startpos, size_t arrsize, Line *arrvalue)
{
    for (size_t i = arrsize; i > startpos; i--)
        lines[i] = lines[i - 1];

    lines[startpos + 1] = arrvalue;
    return lines[startpos];
}

Line *delete_row(Line **lines, size_t startpos, size_t arrsize)
{
    for (size_t i = startpos; i < arrsize; i++)
        lines[i] = lines[i + 1];

    return lines[startpos];
}
/*
char *render_line(File *tstack, size_t ypos)
{
    memcpy(tstack->line[ypos]->render, tstack->line[ypos]->str, ed.xsize - tstack->linenumber_wide - tstack->linenumber_padding - 1); // TODO: Temporary render fix, apply relative ypos and xpos
    tstack->line[ypos]->rlen = strlen_n(tstack->line[ypos]->render);

    if (tstack->line[ypos]->rlen < ed.xsize - tstack->linenumber_wide - tstack->linenumber_padding - 1)
        memset(tstack->line[ypos]->render + tstack->line[ypos]->rlen, ' ', (ed.xsize - tstack->linenumber_wide - tstack->linenumber_padding - 1) - tstack->line[ypos]->rlen);

    tstack->line[ypos]->rlen = strlen_n(tstack->line[ypos]->render);
    return tstack->line[ypos]->render;
}
 */
char *render_line(File *tstack, size_t yps)
{
    size_t len = tstack->line[yps]->len; // Original line length in bytes
    size_t rlen = 0;                     // Render length in bytes
    size_t ulen = 0;                     // Number of characters rendered (not bytes)
    size_t ulen_increase = 0;            // Bytes to advance for the next Unicode character

    char *s_ptr = tstack->line[yps]->str; // Pointer to the start of the line string
    utf8_int32_t null_symbol = 0xe29080;  // Symbol to represent control characters
    utf8_int32_t out_codepoint = 0;       // Variable to hold the decoded codepoint
    size_t lnum = (ed.lineNumbers ? (tstack->linenumber_wide + tstack->linenumber_padding + 1) : 0);

    size_t display_uchars = len; // Characters to display on the screen
    if (display_uchars > ed.xsize - lnum)
        display_uchars = ed.xsize - lnum;

    while (ulen < display_uchars && *s_ptr != '\0')
    {
        // Decode the next codepoint and get the byte length
        char *next_ptr = utf8codepoint(s_ptr, &out_codepoint);
        size_t byte_len = next_ptr - s_ptr;

        // Increase the Unicode length counter
        ulen++;

        // If the render buffer is not big enough, increase it
        if (rlen + LINE_SIZE > tstack->line[yps]->render_bufx)
            increase_line_render(tstack->line[yps], LINE_SIZE);

        if (convertCtrlChars && out_codepoint < 0x20)
        {
            // Convert control characters to a visible representation
            utf8catcodepoint(tstack->line[yps]->render + rlen, null_symbol + out_codepoint, tstack->line[yps]->render_bufx - rlen);
            rlen += utf8codepointsize(null_symbol + out_codepoint); // Correctly calculate size of null_symbol
        }
        else
        {
            // Copy the current UTF-8 sequence to the render buffer
            memcpy(tstack->line[yps]->render + rlen, s_ptr, byte_len);
            rlen += byte_len;
        }

        // Move to the next character
        s_ptr = next_ptr;
    }

    // Fill the rest of the line with spaces if needed
    if (ulen < ed.xsize - lnum)
    {
        memset(tstack->line[yps]->render + rlen, ' ', ed.xsize - lnum - ulen);
        rlen += ed.xsize - lnum - ulen;
    }

    // Null-terminate the rendered string
    tstack->line[yps]->render[rlen] = '\0';
    tstack->line[yps]->rlen = rlen; // Store the length of the rendered line
    return tstack->line[yps]->render;
}


void see_buffer(Line **lines, size_t linecount)
{
    for (size_t i = 1; i <= linecount; i++)
        printf("str[%zu]=%s\n", i, lines[i]->str);

    getch();
}

int split_row(File *tstack, size_t *split_x, size_t *yps)
{
    if (tstack->linecount < tstack->alloc_lines - 1)
    {
        correct_position(tstack->line[*yps], split_x, yps);
        insert_row(tstack->line, (*yps), tstack->alloc_lines, NULL);

        create_line(tstack, (*yps) + 1);

        tstack->linecount++;

        // Copy all characters starting from xpos from the old line to the new line

        strncpy_n(tstack->line[*yps + 1]->str, &tstack->line[*yps]->str[*split_x], strlen_n(tstack->line[*yps]->str) - *split_x);
        memset(&tstack->line[(*yps)]->str[*split_x], 0, tstack->line[*yps]->bufx - *split_x);
        /* see_buffer(tstack->line, tstack->linecount);
        getch(); */
        render_line(tstack, *yps);
        for (int i = 0; i <= 1; i++)
        {
            tstack->line[*yps + i]->len = strlen_n(tstack->line[*yps + i]->str);
            tstack->line[*yps + i]->ulen = utf8len_n(tstack->line[*yps + i]->str);
        }
        tstack->xpos = 0;
        tstack->uxpos = 0;
        (*yps)++;
        render_line(tstack, *yps);

        /*  if (refreshScreen)
         {
             if (Tab_stack[file_index].display_y > YSIZE - 3)
             {
                 ClearPartial(0, 1, XSIZE, YSCROLL);
             }
             else
             {
                 ClearPartial((lineCount ? (tstack->linecount_wide) : 0), dispy, XSIZE - (lineCount ? (tstack->linecount_wide) : 0), YSIZE - dispy - 1);
             }
             DisplayFileContent(tstack, stdout, 0);
         } */
    }

    return *yps;
}


int display_contents(File* tstack)
{
    size_t disp_lines = tstack->linecount-tstack->begin_display.y;
    for(size_t i = tstack->begin_display.y; i <= disp_lines; i++)
    {
        if(tstack->post_load_rendering)
        {
            render_line(tstack, i);
        }
        display_line(tstack, i, tstack->line[i]->rlen);
    }
    return 0;
}