#ifndef NEWTRODIT_GUI_H
#define NEWTRODIT_GUI_H
#include <stdio.h>
#include "newtrodit_core.h"
#include "input.h"

Color title_bg_color = {.r = 204, .g = 204, .b = 204, .background = true};
Color title_font_color = {.r = 12, .g = 12, .b = 12, .background = false};

Color fg_color = {.r = 204, .g = 204, .b = 204, .background = false};
Color bg_color = {.r = 12, .g = 12, .b = 12, .background = true};

Color line_number_bg_color = {.r = 224, .g = 130, .b = 80, .background = true};
Color line_number_font_color = {.r = 30, .g = 30, .b = 30, .background = false};

/* void print_message(const char *str, ...);
int save_file(File *tstack, char *savefile, bool saveDialog);
int getch_n(); */
void newtrodit_name_load();
int center_text(char *text, int yps);
void display_file_type(File *tstack);
void display_bottom_bar(char *status);
void print_message(const char *str, ...);
void display_status(File *tstack, char *status_msg);
void function_aborted(File *tstack, char *status_msg);
int set_display_pos(File *tstack);
void display_cursor_pos(File *tstack, char *status);
void load_line_numbering(File *tstack);
void display_line_numbering(File *tstack, size_t ypos);
void load_all_newtrodit(File *tstack, char *status);
void set_cursor_settings(int visible, int size);
int yes_no_prompt();
int display_line(File *tstack, size_t linenum, size_t rcount);
int set_scroll(File *tstack);
int increase_scroll(File *tstack, size_t amount);
int decrease_scroll(File *tstack, size_t amount);
int toggle_option(int *option, char *msg);

#endif
