size_t codepoint_len(int32_t value)
{
    size_t len = 0;
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        if (((value >> shift) & 0xFF))
            len++;
    }
    return len;
}


char *int32_to_char_array(int32_t value, char *output)
{
    // Extract each byte from the int32_t and assign it to the char array
    size_t i = 0, val = 0;
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        if ((val = ((value >> shift) & 0xFF)))
            output[i++] = val;
    }
    output[4] = '\0';
    //printf("\nString: '%s'", output);

    return output;
}




char *insert_utf8_char(Line *line, int32_t value, size_t index)
{
    if (line->len + sizeof(int32_t) > line->bufx - 1)
    {
        increase_line(line, LINE_SIZE);
    }
    /* size_t len_check[4] = {0};
    for (size_t i = 0; i < 4; i++)
    {
        len_check[i] = utf8nlen_n(line->str, (index + i) > line->len ? line->len : (index + i));
    } */
    char* insert_str_utf8 = calloc(sizeof(int32_t)+1, 1);
    int32_to_char_array(value, insert_str_utf8);
    size_t len = strlen_n(insert_str_utf8);
    memmove(line->str + index + len, line->str + index, line->len - index);
    memcpy(line->str + index, insert_str_utf8, len);
    line->str[line->len + len + index] = '\0';
    line->len = strlen_n(line->str);
    line->ulen++;
    return line->str;
}

int display_char(int32_t chr, FILE* fp)
{
    char outbuf[5] = {0};
    int32_to_char_array(chr, outbuf);
    return fprintf(fp, "%s", outbuf);
}
