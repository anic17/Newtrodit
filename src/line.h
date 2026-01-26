
#ifndef LINE_H
#define LINE_H
#include "newtrodit_core.h"

Line *create_line(File *tstack, size_t ypos);
size_t change_allocated_lines(File *tstack, size_t old_line_count, size_t new_line_count);
Line *increase_line(Line *line, size_t increment);
Line *increase_line_render(Line *line, size_t increment);
Line *insert_row(Line **lines, size_t startpos, size_t arrsize, Line *arrvalue);
Line *delete_row(Line **lines, size_t startpos, size_t arrsize);
char *render_line(File *tstack, size_t yps);
void see_buffer(Line **lines, size_t linecount);
void see_render_buffer(Line **lines, size_t linecount);
int split_row(File *tstack, size_t split_x, size_t *yps);
int display_contents(File *tstack);
int allocate_buffer(File **tstack);
int free_buffer(File **tstack);
#endif