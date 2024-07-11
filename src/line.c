Line *create_line(File *tstack, size_t ypos)
{
    tstack->line[ypos] = calloc(sizeof(Line), 1);
    tstack->line[ypos]->str = calloc(sizeof(utf8_int32_t), LINE_SIZE);
    tstack->line[ypos]->render = calloc(sizeof(utf8_int32_t), LINE_SIZE);
    tstack->line[ypos]->bufx = LINE_SIZE;
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
    for(size_t i = old_line_count; i < new_line_count; i++)
    {
        create_line(tstack, i);
    }
    tstack->alloc_lines = new_line_count;
    return new_line_count;
}

Line *increase_line(Line *line, size_t increment)
{
    if (!line)
    {
        return NULL;
    }

    size_t new_buf_size = line->bufx + increment;

    // Reallocate the line buffer
    char *new_str = realloc_n(line->str, line->bufx * sizeof(utf8_int32_t), new_buf_size * sizeof(utf8_int32_t));
    if (!new_str)
    {
        return NULL;
    }

    line->str = new_str;
    line->bufx = new_buf_size;

    return line; // Success
}

Line *increase_line_render(Line *line, size_t increment)
{
    if (!line)
    {
        return NULL;
    }

    size_t new_buf_size = line->bufx + increment;

    char *new_render = realloc_n(line->render, line->bufx * sizeof(utf8_int32_t), new_buf_size * sizeof(utf8_int32_t));
    if (!new_render)
    {
        return NULL;
    }

    line->render = new_render;
    line->render_bufx = new_buf_size;

    return line; // Success
}