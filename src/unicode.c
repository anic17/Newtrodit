#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

int get_file_encoding(char *header, size_t header_size, size_t *p_bom_len)
{
    char utf16le_bom[2] = {0xff, 0xfe}; // UTF-16LE (Little endian) BOM
    char utf16be_bom[2] = {0xfe, 0xff}; // UTF-16BE (Big Endian) BOM
    char utf32le_bom[4] = {0xff, 0xfe, 0x00, 0x00};
    char utf32be_bom[4] = {0x00, 0x00, 0xfe, 0xff};
    char utf8_bom[3] = {0xef, 0xbb, 0xbf}; // UTF-8 BOM is not recommended for use but we'll handle it anyways
    *p_bom_len = 0;

    if (header_size < 2)
        return ENCODING_UTF8;
    if (!memcmp(header, utf16le_bom, 2))
    {
        *p_bom_len = 2;
        return ENCODING_UTF16LE;
    }
    if (!memcmp(header, utf16be_bom, 2))
    {
        *p_bom_len = 2;
        return ENCODING_UTF16BE;
    }
    if (header_size < 3)
        return ENCODING_UTF8;
    if (!memcmp(header, utf8_bom, 3))
    {
        *p_bom_len = 3;
        return ENCODING_UTF8BOM;
    }
    if (header_size < 4)
        return ENCODING_UTF8;
    if (!memcmp(header, utf32le_bom, 4))
    {
        *p_bom_len = 4;
        return ENCODING_UTF32LE;
    }
    if (!memcmp(header, utf32be_bom, 4))
    {
        *p_bom_len = 4;
        return ENCODING_UTF32BE;
    }
    return ENCODING_UTF8;
}

size_t codepoint_len(int32_t value)
{
    if (value == 0) // Added check for value == 0
        return 1;
    size_t len = 0;
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        if (((value >> shift) & 0xFF))
            len++;
    }
    return len;
}


int valid_position(char *s, size_t pos)
{
    utf8_int32_t cp = 0;
    utf8codepoint(s + pos, &cp);
    return (unsigned int)cp < 0x10FFFFu; // a UTF-8 codepoint cannot be greater than U+10FFFF
}

size_t utf8wnlen(const char *s, size_t max_upos)
{
	if (!s)
		return 0;
	char *ptr = s;
	utf8_int32_t cp;
	size_t wlen = 0, ulen = 0;
	int cwidth = 0;
	while (*ptr != '\0' && ulen < max_upos)
	{
		ptr = utf8codepoint(ptr, &cp);
		ulen++;
		cwidth = mk_wcwidth(cp);
		if (cwidth < 0)
			cwidth = 0;
		wlen += cwidth;
	}
	return wlen;
}

size_t utf8wlen(const char *s)
{
	return utf8wnlen(s, (size_t)(-1)); // -1 returns the max size of a size_t
}

size_t correct_position(Line *line, size_t *xpos, size_t *uxpos)
{
    if (!valid_position(line->str, *xpos))
    {
        set_status_msg(true, NEWTRODIT_ERROR_INVALID_UNICODE_POSITION);
        utf8_int32_t cp = 0;
        char *backptr = utf8rcodepoint(line->str + *xpos, &cp);
        *xpos -= (&line->str[*xpos] - backptr); // Go back to the previous UTF-8 codepoint
        *uxpos = utf8nlen_n(line->str, *xpos);  // Set the new UTF-8 position
    }
    return *xpos;
}

char *int32_to_char_array(utf8_int32_t value, char *output)
{
    // Extract each byte from the int32_t and assign it to the char array
    size_t i = 0, val = 0;
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        if ((val = ((value >> shift) & 0xFF)))
            output[i++] = val;
    }
    output[4] = '\0';
    // printf("\nString: '%s'", output);

    return output;
}

int codepoint_width(const char  *utf8_char, utf8_int32_t val) { // This function can take either a char string or a UTF-8 int32 code


    char utf8s[5] = {0};

    
    utf8_int32_t uc = 0;
    if(!utf8_char)
    {
        int32_to_char_array(val, utf8s);
            utf8codepoint(utf8s, &uc);
    } else {
            utf8codepoint(utf8_char, &uc);
    }
    

    int cwidth = mk_wcwidth(uc);
    if(cwidth < 0) cwidth = 0;
    return cwidth; // Returns 1 for narrow, 2 for wide, or 0 if not printable/blank space
}

size_t previous_char_xpos(char *s, size_t *xpos)
{
    utf8_int32_t cp = 0;
    char *prev_ptr = s + *xpos;
    if (*xpos > 0)
    {
        prev_ptr = utf8rcodepoint(s + *xpos, &cp);
        *xpos = prev_ptr - s;
        /* printf("pdif: %zu", (prev_ptr-s));
        getch(); */
        return *xpos;
    }
    return 0;
}

size_t next_char_xpos(char *s, size_t *xpos)
{
    utf8_int32_t cp = 0;
    char *next_ptr;
    if (*xpos > 0)
    {
        next_ptr = utf8codepoint(s + *xpos, &cp);
        *xpos += next_ptr - s;
        return *xpos;
    }
    return 0;
}

size_t utf8getxpos(char *s, size_t uxpos)
{
    char *ptr = s;
    utf8_int32_t out_cp = 0;
    for (size_t i = 0; i < uxpos; i++) // Loop over the UTF-8 string
    {
        if (!valid_position(s, ptr - s))
        {
            return 0;
        }
        ptr = utf8rcodepoint(ptr, &out_cp);
    }
    return ptr - s;
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
    if (!valid_position(line->str, index))
    {
        set_status_msg(true, NEWTRODIT_ERROR_INVALID_UNICODE_POSITION);
        return NULL;
    }
    // correct_position(line, index, utf8nlen_n(line->str, index));
    char *insert_str_utf8 = calloc(sizeof(int32_t) + 1, 1);
    int32_to_char_array(value, insert_str_utf8);
    /*     printf("[%s] %zu;", insert_str_utf8, index);
        getch(); */
    size_t len = strlen_n(insert_str_utf8);

    memmove(line->str + index + len, line->str + index, line->len - index);
    memcpy(line->str + index, insert_str_utf8, len);
    line->str[line->len + len + index] = '\0';
    line->len += utf8size(insert_str_utf8);
    line->ulen++;
    free(insert_str_utf8);
    return line->str;
}

char *insert_str(Line *line, char *s2, size_t pos)
{
    size_t l2 = strlen_n(s2);
    if (line->len + l2 > line->bufx - 1)
    {
        increase_line(line, l2);
    }

    memmove(line->str + pos + l2, line->str + pos, line->len);
    memcpy(line->str + pos, s2, l2);
    line->str[line->len + l2] = '\0';
    line->len = strlen_n(line->str); // Recalculate the length in case it is wrong to avoid error accumulation

    return line->str;
}

char *delete_str(Line *line, size_t pos, size_t count)
{
    if (pos + count > line->len)
    {
        return NULL;
    }
    memmove(line->str + pos, line->str + pos + count, line->len - (pos + count));
    line->str[line->len - count] = '\0';
    line->len -= count;
    return line->str;
}

char *delete_char(Line *line, size_t pos)
{
    if (line->len > pos)
    {
        memmove(line->str + pos, line->str + pos + 1, line->len - pos);
        line->str[--line->len] = '\0';
    }
    return line->str;
}

/* char *insert_utf8_string(Line *line, char *s, size_t index)
{
    if (!valid_position(line))
        if (line->len + strlen_n(s) > line->bufx - 1)
        {
            increase_line(line, strlen_n(s));
        }
    size_t next_index = index;
}
 */
int display_char(int32_t chr, FILE *fp)
{
    char outbuf[5] = {0};
    int32_to_char_array(chr, outbuf);
    return fprintf(fp, "%s", outbuf);
}

uint16_t swap_le_be(uint16_t num)
{
    return (num >> 8) | (num << 8);
}


int is_utf16(const unsigned char *s, size_t len, size_t file_len) // Try to detect whether string of text without BOM is UTF-16, and try to guess if it's BE (wchar_t default) or LE (Windows default)
{
    if (len % 2 || file_len % 2)
        return false; // Length must be even for UTF-16 encoding

    size_t zero_count_be = 0, zero_count_le = 0;
    size_t difference_le = 0, difference_be = 0;

    // Check for zero bytes and calculate differences for endianness determination
    for (size_t i = 0; i < len; i += 2)
    {
        if (s[i] == '\0')
            zero_count_be++;

        if (i + 1 < len && (unsigned char)(s[i] - s[i + 1]) > 0x60)
            difference_be++;

        if (s[i + 1] == '\0')
            zero_count_le++;

        if (i + 1 < len && (unsigned char)(s[i + 1] - s[i]) > 0x60)
            difference_le++;
    }

    // Check if all characters are zero, which is invalid
    if (zero_count_be + zero_count_le == len)
        return false;

    bool is_bigendian = zero_count_be >= zero_count_le;

    // Check for unusual patterns that suggest non-UTF-16 text
    if ((is_bigendian && (double)difference_be / len < 0.125) || (!is_bigendian && (double)difference_le / len < 0.125))
        return false;

    if ((is_bigendian && (double)zero_count_be / len < 0.125) || (!is_bigendian && (double)zero_count_le / len < 0.125))
        return false;

    // Verify UTF-16 validity by checking surrogate pairs and code point ranges
    for (size_t i = 0; i < len;)
    {
        utf8_int32_t utfchar = 0, surrogate = 0;

        utfchar = (s[i] << 8) | s[i + 1];
        i += 2;

        // Convert endian if needed
        if (!is_bigendian)
            utfchar = swap_le_be(utfchar);

        // Check if it's a high surrogate
        if (utfchar >= 0xD800 && utfchar <= 0xDBFF)
        {
            if (i + 2 > len)
                return -1;

            surrogate = (s[i] << 8) | s[i + 1];
            i += 2;

            if (!is_bigendian)
                surrogate = swap_le_be(surrogate);

            if (surrogate < 0xDC00 || surrogate > 0xDFFF)
                return -1;
        }

        // Check if character is above valid Unicode range
        if (utfchar > 0x10FFFF)
            return -1;

        fprintf(stderr, "U+%04X\n", utfchar);
    }

    // Return 2 for BE, 1 for LE, 0 for not UTF-16
    return 1 + is_bigendian;
}


size_t utf16_to_utf8(const uint16_t *u16_str, size_t u16_str_len, uint8_t *u8_str, size_t u8_str_size) // Code from Stack Overflow
{
    size_t i = 0, j = 0;

    if (!u8_str)
        u8_str_size = u16_str_len * 4;

    while (i < u16_str_len)
    {
        uint32_t codepoint = u16_str[i++];

        // check for surrogate pair
        if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
        {
            uint16_t high_surr = codepoint;
            uint16_t low_surr = u16_str[i++];

            if (low_surr < 0xDC00 || low_surr > 0xDFFF)
                return -1;

            codepoint = ((high_surr - 0xD800) << 10) +
                        (low_surr - 0xDC00) + 0x10000;
        }

        if (codepoint < 0x80)
        {
            if (j + 1 > u8_str_size)
                return -2;

            if (u8_str)
                u8_str[j] = (char)codepoint;

            j++;
        }
        else if (codepoint < 0x800)
        {
            if (j + 2 > u8_str_size)
                return -2;

            if (u8_str)
            {
                u8_str[j + 0] = 0xC0 | (codepoint >> 6);
                u8_str[j + 1] = 0x80 | (codepoint & 0x3F);
            }

            j += 2;
        }
        else if (codepoint < 0x10000)
        {
            if (j + 3 > u8_str_size)
                return -2;

            if (u8_str)
            {
                u8_str[j + 0] = 0xE0 | (codepoint >> 12);
                u8_str[j + 1] = 0x80 | ((codepoint >> 6) & 0x3F);
                u8_str[j + 2] = 0x80 | (codepoint & 0x3F);
            }

            j += 3;
        }
        else
        {
            if (j + 4 > u8_str_size)
                return -2;

            if (u8_str)
            {
                u8_str[j + 0] = 0xF0 | (codepoint >> 18);
                u8_str[j + 1] = 0x80 | ((codepoint >> 12) & 0x3F);
                u8_str[j + 2] = 0x80 | ((codepoint >> 6) & 0x3F);
                u8_str[j + 3] = 0x80 | (codepoint & 0x3F);
            }

            j += 4;
        }
    }

    if (u8_str)
    {
        if (j >= u8_str_size)
            return -2;
        u8_str[j] = '\0';
    }

    return (long int)j;
}