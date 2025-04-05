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
void draw_horizontal_border(WINDOW *win, int y, const char *left,
                            const char *middle, const char *right,
                            int win_width, int win_height);
void draw_vertical_borders(WINDOW *win, int win_width, int win_height);
void print_table_borders(WINDOW *win, int win_width, int win_height,
                         int divisor_y);

#endif  // _NAVI_TABLE_H
