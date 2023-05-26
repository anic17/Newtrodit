/*
    Newtrodit: A console text editor
    Copyright (c) 2021-2022 anic17 Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>
*/

int newtrodit_get_cursor_x()
{
    return Tab_stack[file_index].xpos;
}

int newtrodit_get_cursor_y()
{
    return Tab_stack[file_index].ypos;
}

char *newtrodit_set_line(char* buf, int fileindex, int line)
{
    strncpy_n(Tab_stack[fileindex].strsave[line], buf, Tab_stack[fileindex].bufx);
    return Tab_stack[file_index].strsave[line];
}

char* newtrodit_empty_line(int fileindex, int line)
{
    memset(Tab_stack[fileindex].strsave[line], 0, Tab_stack[fileindex].bufx);
    return Tab_stack[file_index].strsave[line];
}

char* newtrodit_update_cursor(int fileindex, int x, int y)
{
    Tab_stack[fileindex].xpos = x;
    Tab_stack[fileindex].ypos = y;
    return Tab_stack[file_index].strsave[y];
}