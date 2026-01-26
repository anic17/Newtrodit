#ifndef UNICODE_H
#define UNICODE_H
#include "utf8/utf8.h"
#include "newtrodit_core.h"
#include "line.h"

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

int get_file_encoding(char *header, size_t header_size, size_t *p_bom_len);
size_t codepoint_len(int32_t value);
int valid_position(char *s, size_t pos);
size_t utf8wnlen(char *s, size_t max_upos);
size_t utf8wlen(char *s);
size_t correct_position(Line *line, size_t *xpos, size_t *uxpos, size_t *uwxpos);
char *int32_to_char_array(utf8_int32_t value, char *output);
int codepoint_width(const char *utf8_char, utf8_int32_t val);
size_t next_char_uwlen(const char *s, size_t xps);
size_t previous_char_uwlen(const char *s, size_t xps);
size_t next_char_xpos(const char *s, size_t *xpos);
size_t previous_char_xpos(const char *s, size_t *xpos);
size_t utf8getxpos(char *s, size_t uxpos);
char *insert_utf8_char(Line *line, int32_t value, size_t index);
char *insert_str(Line *line, char *s2, size_t pos);
char *delete_str(Line *line, size_t pos, size_t count);
char *delete_char(Line *line, size_t pos);
int display_char(utf8_int32_t chr, FILE *fp);
uint16_t swap_le_be(uint16_t num);
int is_utf16(const unsigned char *header, size_t header_size, size_t file_size);
size_t utf16_to_utf8(const uint16_t *u16_str, size_t u16_str_len, uint8_t *u8_str, size_t u8_str_size, bool big_endian);

#endif // UNICODE_H