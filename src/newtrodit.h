#ifndef NEWTRODIT_H
#define NEWTRODIT_H

#include "newtrodit_gui.h"
#include "fileio.h"
#include "input.h"
#include "line.h"

void sigsegv_handler();
int init_editor(int argc, char* argv[]);
int end_editor();
void quit_newtrodit(File *tstack);
int handle_keystrokes(InputUTF8 ch, File *tstack);
int editor_main();

#endif // NEWTRODIT_H