size_t correct_position(Line *line, size_t *xpos, size_t *uxpos, size_t *uwxpos);
void print_message(const char *str, ...);
int set_scroll(File *tstack);
int increase_scroll(File *tstack, size_t amount);

Line *create_line(File *tstack, size_t ypos)
{
    tstack->line[ypos] = calloc(1, sizeof(Line));
    tstack->line[ypos]->str = calloc(LINE_SIZE, sizeof(utf8_int32_t));
    tstack->line[ypos]->render = calloc(LINE_SIZE, sizeof(utf8_int32_t));
    tstack->line[ypos]->bufx = 10;
    tstack->line[ypos]->render_bufx = LINE_SIZE;
    tstack->line[ypos]->len = 0;
    tstack->line[ypos]->rlen = 0;
    tstack->line[ypos]->ulen = 0;
    tstack->line[ypos]->rnlen = 0;
    tstack->alloc_lines++;
    return tstack->line[ypos];
}

size_t change_allocated_lines(File *tstack, size_t old_line_count, size_t new_line_count)
{
    Line **linestack = realloc_n(tstack->line, old_line_count * sizeof(Line *), new_line_count * sizeof(Line *));
    if (!linestack)
        return old_line_count;

    for (size_t i = old_line_count; i < new_line_count; i++)
        create_line(tstack, i);

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
    if (startpos > arrsize)
        return NULL;
    memmove(lines + startpos + 1, lines + startpos, (arrsize - startpos) * sizeof(Line *));
    // lines[startpos] = arrvalue;
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
    size_t uwlen = 0;                    // Number of Unicode characters rendered (taking into account the different widths)
   // size_t old_rnlen = tstack->line[yps]->rnlen;
    size_t ulen_increase = 0;            // Bytes to advance for the next Unicode character

    char *s_ptr = tstack->line[yps]->str; // Pointer to the start of the line string
    utf8_int32_t null_symbol = 0xe29080;  // Symbol to represent control characters
    utf8_int32_t out_codepoint = 0;       // Variable to hold the decoded codepoint
    size_t lnum = (ed.lineNumbers ? (tstack->linenumber_wide + tstack->linenumber_padding + 1) : 0);

    size_t display_uchars = len; // Characters to display on the screen
    if (display_uchars > ed.xsize - lnum)
        display_uchars = ed.xsize - lnum;

    while (uwlen < display_uchars && *s_ptr != '\0')
    {
        // Decode the next codepoint and get the byte length
        char *next_ptr = utf8codepoint(s_ptr, &out_codepoint);
        size_t byte_len = next_ptr - s_ptr;
        uwlen += mk_wcwidth(out_codepoint); // Increase the Unicode width counter

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
    tstack->line[yps]->rnlen = rlen;
    if (uwlen < ed.xsize - lnum)
    {
        /* printf("uwlen: %zu, rlen: %zu, lnum: %zu, yps: %zu. Filling: %zu\n", uwlen, rlen, lnum, yps, ed.xsize - lnum - uwlen);
        getch(); */
        /* printf("rlen: %zu\n", rlen);
        getch(); */

        memset(&tstack->line[yps]->render[rlen], ' ', ed.xsize - lnum - uwlen);
                        rlen = ed.xsize - lnum;

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
}

void see_render_buffer(Line **lines, size_t linecount)
{
    for (size_t i = 1; i <= linecount; i++)
        printf("rdr[%zu]=%s\n", i, lines[i]->render);
}

int split_row(File *tstack, size_t split_x, size_t *yps)
{

    // Split a line in position split_x into a new row like the Enter key is pressed in the middle of a line

    Line *curr_line = tstack->line[*yps];

    size_t len = curr_line->len - split_x;

    tstack->linecount++;

    insert_row(tstack->line, *yps, tstack->linecount, NULL);
    create_line(tstack, (*yps) + 1);
    tstack->alloc_lines++;

    // Copy all characters starting from xpos from the old line to the new line

    memcpy(tstack->line[(*yps) + 1]->str, &curr_line->str[split_x], curr_line->len - split_x);
    memset(&curr_line->str[split_x], 0, curr_line->bufx - split_x);
    tstack->line[(*yps) + 1]->len = len;
    curr_line->len = split_x;
    // We cannot use strlen, strlen_n, utf8len or utf8len_n here because string are not null-terminated

    curr_line->ulen = utf8len_null(curr_line->str, curr_line->len);
    tstack->line[(*yps) + 1]->ulen = utf8len_null(tstack->line[(*yps) + 1]->str, tstack->line[(*yps) + 1]->len);

    tstack->xpos = 0;
    tstack->uxpos = 0;
    tstack->uwxpos = 0;
/*     see_buffer(tstack->line, tstack->linecount);
    getch(); */

    render_line(tstack, *yps);
    render_line(tstack, ++(*yps));
/*         see_render_buffer(tstack->line, tstack->linecount);
    getch(); */
    // see_render_buffer(tstack->line, tstack->linecount);
        tstack->ypos = *yps;

    increase_scroll(tstack, 1);
    return *yps;
}

int display_contents(File *tstack)
{
    size_t disp_lines = tstack->linecount - tstack->begin_display.y;
    for (size_t i = tstack->begin_display.y; i <= disp_lines; i++)
    {
        if (tstack->post_load_rendering)
            render_line(tstack, i);

        display_line(tstack, i, tstack->line[i]->rlen);
    }
    return 0;
}