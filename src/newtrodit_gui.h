Color title_bg_color = {.r = 204, .g = 204, .b = 204, .background = true};
Color title_font_color = {.r = 12, .g = 12, .b = 12, .background = false};

Color fg_color = {.r = 204, .g = 204, .b = 204, .background = false};
Color bg_color = {.r = 12, .g = 12, .b = 12, .background = true};

Color line_number_bg_color = {.r = 224, .g = 130, .b = 80   , .background = true};
Color line_number_font_color = {.r = 30, .g = 30, .b = 30, .background = false};

void print_message(const char *str, ...);
int save_file(File *tstack, char *savefile, bool saveDialog);

#include "newtrodit_gui.c"




