#include "table.h"

int height, width;
int win_height, win_width;
int start_y, start_x;
WINDOW *win;

int cursor_selected = 0;
int scroll_top = 0;

void recalculate_table_bounds() {
    height = LINES;
    width = COLS;

    win_height = (height * 70 / 100);
    win_width = (width * 70 / 100);

    if(height < 20 || width < 20) {
        win_height = height;
        win_width = width;
    }

    start_y = (height - win_height) / 2;
    start_x = (width - win_width) / 2;
}

void draw_horizontal_border(int y, const char* left, const char* middle, const char* right) {
    mvwprintw(win, y, 0, "%s", left);
    for (int x = 1; x < win_width - 1; ++x)
        mvwprintw(win, y, x, "%s", middle);
    mvwprintw(win, y, win_width - 1, "%s", right);
}

void draw_vertical_borders() {
    for (int y = 1; y < win_height - 1; ++y) {
        mvwprintw(win, y, 0, "│");
        mvwprintw(win, y, win_width - 1, "│");
    }
}


void print_table_borders() {
    draw_horizontal_border(0, "┌", "─", "┐");
    draw_vertical_borders();
    draw_horizontal_border(win_height - 1, "└", "─", "┘");
    draw_horizontal_border(3, "├", "─", "┤");
}