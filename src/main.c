#include "main.h"

#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfg_parser.h"
#include "config.h"
#include "ext_table.h"
#include "file.h"
#include "macros.h"
#include "path.h"
#include "sysdep.h"
#include "table.h"

FILE *og_stdout;

file_t *flisting;
int flisting_sz;
char pwd[256];
char findbuf[256];
bool flags[256];

char *prompt_message = NULL;
bool pathset = false;

int findbuflen = 0;

char kbchar;

int scroll_top_stack[256];
int cursor_selected_stack[256];

int scroll_top_stackptr = 0;
int cursor_selected_stackptr = 0;

int fg_color = COLOR_WHITE;
int bg_color = COLOR_BLACK;
int hl_color = COLOR_RED;
int dim_color = 8;
bool use_nf = true;

unsigned int default_dir_icon = 0xf413, default_file_icon = 0xf4a5;

bool open_dir(char *dir) {
    int count = navi_count_entries(dir);
    if (count == -1) return false;
    flisting_sz = count;
    flisting = (file_t *)malloc(sizeof(file_t) * flisting_sz);
    navi_list_dir(dir, flisting, flisting_sz);
    return true;
}

void init() {
    // this is pretty stupid, i know
    // i couldn't find a better solution to this
    og_stdout = stdout;
    stdout = stderr;

    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();

    parse_apply_navi_cfg();

    init_pair(_NAVI_COLORS_LISTING_NORMAL, fg_color, bg_color);
    init_pair(_NAVI_COLORS_LISTING_HIGHLIGHTED, fg_color, hl_color);
    init_pair(_NAVI_COLORS_LISTING_DIMMED, dim_color, bg_color);
    init_pair(_NAVI_COLORS_LISTING_DIMMED_HIGHLIGHTED, dim_color, hl_color);

    recalculate_table_bounds();

    win = newwin(win_height, win_width, start_y, start_x);
    bkgd(COLOR_PAIR(_NAVI_COLORS_LISTING_NORMAL));
    wbkgd(win, COLOR_PAIR(_NAVI_COLORS_LISTING_NORMAL));

    if (!pathset) navi_get_pwd(pwd, sizeof(pwd));
    if (!open_dir(pwd)) {
        perror("navi: ");
        exit(1);
    }
}

void draw_background_win() {
    werase(stdscr);
    mvprintw(1, 1, "NAVI %s\n", _NAVI_VERSION);

    if (use_nf) {
        mvprintw(height - 5, 1, "[  󰍽] Scroll");
        mvprintw(height - 4, 1, "[] Enter Directory");
        mvprintw(height - 3, 1, "[] Parent Directory");
        mvprintw(height - 2, 1, "[󰌑] Select");
    } else {
        mvprintw(height - 5, 1, "[up/down/scroll] Scroll");
        mvprintw(height - 4, 1, "[right arrow] Enter Directory");
        mvprintw(height - 3, 1, "[left arrow] Parent Directory");
        mvprintw(height - 2, 1, "[return] Select");
    }
}

int format_size(size_t size, char *buffer, size_t buffer_sz) {
    static const char *units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    double size_in_units = size;
    int unit_index = 0;

    while (size_in_units >= 1024 && unit_index < 5) {
        size_in_units /= 1024;
        unit_index++;
    }

    return snprintf(buffer, buffer_sz, "%.1f %s", size_in_units,
                    units[unit_index]);
}

void navi_errorf(const char *title, const char *format, ...) {
    char buffer[1024];
    va_list args;

    va_start(args, format);

    int written = vsnprintf(buffer, sizeof(buffer), format, args);

    int win_height = 12;
    int win_width = written * 2;

    int start_y = (LINES - win_height) / 2;
    int start_x = (COLS - win_width) / 2;

    WINDOW *errwin = newwin(win_height, win_width, start_y, start_x);

    mvwprintw(errwin, 1, 2, use_nf ? " %s" : "[ERROR] %s", title);
    mvwprintw(errwin, 3, 2, "%s", buffer);
    mvwprintw(errwin, win_height - 2, 2, use_nf ? "[󰌌] OK" : "[any key] OK");

    print_table_borders(errwin, win_width, win_height,
                        (int[]){2, win_height - 3}, 2);

    refresh();
    wrefresh(errwin);

    getch();
    delwin(errwin);

    va_end(args);
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (argv[1][0] != '-' && !pathset) {
            strcpy(pwd, realpath(argv[1], NULL));
            pathset = true;
            continue;
        }

        if (strlen(argv[i]) != 2 || argv[i][0] != '-') {
            fprintf(stderr, "navi: invalid flag\n");
            exit(1);
        }

        char flag = argv[i][1];
        if (flags[flag]) {
            fprintf(stderr, "navi: flag -%c set multiple times\n", flag);
            exit(1);
        }
        flags[flag] = true;
        switch (flag) {
            case 'v':
                printf("NAVI: [NAV]igation [I]nterface\nVersion: %s\n",
                       _NAVI_VERSION);
                exit(0);
                break;
            case 'm':
                if (i >= argc - 1) {
                    fprintf(stderr, "navi: -m requires input message\n");
                    exit(1);
                }
                prompt_message = argv[++i];
                break;
        }
    }

    init();

    draw_background_win();

    while (true) {
        werase(win);

        mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y, _NAVI_PWD_DRAWING_TOP_X,
                  use_nf ? "󱞊 %s" : "[PWD] %s", pwd);

        if (findbuflen > 0) {
            mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y + 1, _NAVI_PWD_DRAWING_TOP_X,
                      use_nf ? " %s█" : "[SEARCH] %s█", findbuf);
        } else if (flags['m'] && prompt_message)
            mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y + 1, _NAVI_PWD_DRAWING_TOP_X,
                      use_nf ? " %s" : "[PROMPT] %s", prompt_message);

        mvwprintw(win, win_height - 1 - 1, _NAVI_PWD_DRAWING_TOP_X,
                  "%d of %d entries", cursor_selected + 1, flisting_sz);

        mvwprintw(win, win_height - 2,
                  win_width - 6 - _NAVI_LISTING_DRAWING_TOP_X,
                  use_nf ? " %d%%" : "  %d%%",
                  (int)(((cursor_selected + 1) / (float)flisting_sz) * 100));

        if (flisting_sz != 0) {
            int curpos = 0;
            for (int i = scroll_top;
                 curpos < MIN((win_height - _NAVI_LISTING_DRAWING_TOP_Y -
                               _NAVI_LISTING_DRAWING_BOTTOM_Y),
                              flisting_sz - scroll_top);
                 ++i) {
                bool greyed_out = false;
                if ((findbuflen > 0 &&
                     !strcasestr(flisting[i].name, findbuf)) ||
                    (flags['d'] && flisting[i].type != FT_DIRECTORY &&
                     !flags['f']) ||
                    (flags['f'] && flisting[i].type != FT_FILE &&
                     !flags['d'])) {
                    greyed_out = true;
                    wattron(win, COLOR_PAIR(_NAVI_COLORS_LISTING_DIMMED));
                }

                int color_pair =
                    COLOR_PAIR(greyed_out ? _NAVI_COLORS_LISTING_DIMMED
                                          : _NAVI_COLORS_LISTING_NORMAL);

                if (i == cursor_selected)
                    color_pair = COLOR_PAIR(
                        greyed_out ? _NAVI_COLORS_LISTING_DIMMED_HIGHLIGHTED
                                   : _NAVI_COLORS_LISTING_HIGHLIGHTED);

                wattron(win, color_pair);

                int col1off = win_width - _NAVI_LISTING_DRAWING_TOP_X;

                file_t file = flisting[i];
                wchar_t icon =
                    file.type == FT_DIRECTORY
                        ? default_dir_icon
                        : ext_hash_lookup(strrchr(flisting[i].name, '.') + 1);
                mvwprintw(win, curpos + _NAVI_LISTING_DRAWING_TOP_Y,
                          _NAVI_LISTING_DRAWING_TOP_X, "%lc %s", icon,
                          file.name);

                if (file.type != FT_DIRECTORY) {
                    static char szbuf[64];
                    int len = format_size(file.size, szbuf, 64);

                    if (len >= 0)
                        mvwprintw(
                            win, curpos + _NAVI_LISTING_DRAWING_TOP_Y,
                            _NAVI_LISTING_DRAWING_TOP_X + col1off - len - 1,
                            "%s", szbuf);
                }

                // if (i == cursor_selected || greyed_out)
                wattroff(win, color_pair);
                ++curpos;
            }
        } else
            mvwprintw(win, _NAVI_LISTING_DRAWING_TOP_Y,
                      _NAVI_LISTING_DRAWING_TOP_X, "<empty>");

        print_table_borders(win, win_width, win_height,
                            (int[]){3, win_height - 3}, 2);

        refresh();
        wrefresh(win);

        kbchar = wgetch(win);

        if (kbchar == _NAVI_KEY_RESIZE) {
            recalculate_table_bounds();
            mvwin(win, start_y, start_x);
            wresize(win, win_height, win_width);
            wresize(stdscr, height, width);
            draw_background_win();

        } else if (kbchar == 0x1b) {
            kbchar = wgetch(win);
            if (kbchar == 0x5b) {
                kbchar = wgetch(win);
                if (kbchar == _NAVI_KEY_SCROLLUP) {
                    cursor_selected--;
                    if (cursor_selected < 0) cursor_selected = 0;
                    if (cursor_selected < scroll_top)
                        scroll_top = cursor_selected;
                } else if (kbchar == _NAVI_KEY_SCROLLDOWN) {
                    cursor_selected++;
                    if (cursor_selected >= flisting_sz)
                        cursor_selected = flisting_sz - 1;
                    if (cursor_selected >= scroll_top + win_height -
                                               _NAVI_LISTING_DRAWING_TOP_Y -
                                               _NAVI_LISTING_DRAWING_BOTTOM_Y)
                        scroll_top = cursor_selected - win_height +
                                     _NAVI_LISTING_DRAWING_TOP_Y +
                                     _NAVI_LISTING_DRAWING_BOTTOM_Y + 1;
                } else if (kbchar == _NAVI_KEY_BACK) {
                    char *newpath = path_dir(pwd);
                    if (open_dir(newpath)) {
                        strcpy(pwd, newpath);
                        if (scroll_top_stackptr >= 1 &&
                            cursor_selected_stackptr >= 1) {
                            scroll_top =
                                scroll_top_stack[--scroll_top_stackptr];
                            cursor_selected = cursor_selected_stack
                                [--cursor_selected_stackptr];
                        } else
                            scroll_top = cursor_selected = 0;
                        wrefresh(win);
                    }
                    free(newpath);
                } else if (kbchar == _NAVI_KEY_OPEN) {
                    if (flisting[cursor_selected].type == FT_DIRECTORY) {
                        char *newpath =
                            path_concat(pwd, flisting[cursor_selected].name);
                        if (open_dir(newpath)) {
                            strcpy(pwd, newpath);
                            if (scroll_top_stackptr < 256 &&
                                cursor_selected_stackptr < 256) {
                                scroll_top_stack[scroll_top_stackptr++] =
                                    scroll_top;
                                cursor_selected_stack
                                    [cursor_selected_stackptr++] =
                                        cursor_selected;
                            }
                            scroll_top = cursor_selected = 0;
                            wrefresh(win);
                        }
                        free(newpath);
                    }
                } else
                    ungetch(kbchar);
            } else
                ungetch(kbchar);
        } else if (kbchar == _NAVI_KEY_SELECT) {
            if ((flags['f'] && flisting[cursor_selected].type == FT_FILE) ||
                (flags['d'] &&
                 flisting[cursor_selected].type == FT_DIRECTORY) ||
                (!flags['f'] && !flags['d'])) {
                char *newpath =
                    path_concat(pwd, flisting[cursor_selected].name);
                strcpy(pwd, newpath);
                free(newpath);
                break;
            }
        } else if (kbchar == 127) {
            if (findbuflen != 0) findbuf[--findbuflen] = 0;
        } else if (isprint(kbchar)) {
            findbuf[findbuflen++] = kbchar;
            scroll_top = cursor_selected = 0;
        }
    }

    endwin();

    fprintf(og_stdout, "%s\n", pwd);

    return 0;
}
