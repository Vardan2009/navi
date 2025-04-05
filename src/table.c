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

    if (height < 20 || width < 20) {
        win_height = height;
        win_width = width;
    }

    start_y = (height - win_height) / 2;
    start_x = (width - win_width) / 2;
}

void draw_horizontal_border(WINDOW *win, int y, const char *left,
                            const char *middle, const char *right,
                            int win_width, int win_height) {
    mvwprintw(win, y, 0, "%s", left);
    for (int x = 1; x < win_width - 1; ++x) mvwprintw(win, y, x, "%s", middle);
    mvwprintw(win, y, win_width - 1, "%s", right);
}

void draw_vertical_borders(WINDOW *win, int win_width, int win_height) {
    for (int y = 1; y < win_height - 1; ++y) {
        mvwprintw(win, y, 0, "│");
        mvwprintw(win, y, win_width - 1, "│");
    }
}

void print_table_borders(WINDOW *win, int win_width, int win_height,
                         int divisor_y) {
    draw_horizontal_border(win, 0, "┌", "─", "┐", win_width, win_height);
    draw_vertical_borders(win, win_width, win_height);
    draw_horizontal_border(win, win_height - 1, "└", "─", "┘", win_width,
                           win_height);
    if (divisor_y != -1)
        draw_horizontal_border(win, divisor_y, "├", "─", "┤", win_width,
                               win_height);
}
