#ifndef _NAVI_TABLE_H
#define _NAVI_TABLE_H

#include <ncurses.h>

extern int height, width;
extern int win_height, win_width;
extern int start_y, start_x;
extern WINDOW *win;

extern int cursor_selected;
extern int scroll_top;

void recalculate_table_bounds();
void draw_horizontal_border(int y, const char *left, const char *middle,
                            const char *right);
void draw_vertical_borders();
void print_table_borders();

#endif  // _NAVI_TABLE_H
