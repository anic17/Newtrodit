#ifndef FILEIO_H
#define FILEIO_H
#include "newtrodit_core.h"
#include "line.h"
#include "newtrodit_gui.h"
#include "input.h"


int check_file(char *filename);
char *get_full_path(const char *filename);
int valid_filename(char *filename);
int write_file(File *tstack, FILE *fp);
int save_file(File *tstack, char *savefile, bool saveDialog);
int load_file(File *tstack, FILE *fp);
int open_file(File *tstack);
int close_file(File *tstack);
void save_modified_file(File *tstack);

#endif // FILEIO_H