#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ext_table.h"
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
    start_color();

    init_pair(_NAVI_COLORS_LISTING_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(_NAVI_COLORS_LISTING_HIGHLIGHTED, COLOR_WHITE, COLOR_RED);
    init_pair(_NAVI_COLORS_LISTING_DIMMED, 8, COLOR_BLACK);
    init_pair(_NAVI_COLORS_LISTING_DIMMED_HIGHLIGHTED, 8, COLOR_RED);

    recalculate_table_bounds();

    win = newwin(win_height, win_width, start_y, start_x);
    bkgd(COLOR_PAIR(_NAVI_COLORS_LISTING_NORMAL));

    ext_hash_insert(".txt", 0xf15c);
    ext_hash_insert(".clang-format", 0xe615);
    ext_hash_insert(".json", 0xe60b);
    ext_hash_insert(".c", 0xe61e);
    ext_hash_insert(".cpp", 0xe61d);
    ext_hash_insert(".h", 0xf0fd);
    ext_hash_insert(".hpp", 0xf0fd);

    if (!pathset) navi_get_pwd(pwd, sizeof(pwd));
    if (!open_dir(pwd)) {
        perror("navi: ");
        exit(1);
    }
}

void draw_background_win() {
    werase(stdscr);
    mvprintw(1, 1, "NAVI %s\n", _NAVI_VERSION);

    mvprintw(height - 5, 1, "[  󰍽] Scroll");
    mvprintw(height - 4, 1, "[] Enter Directory");
    mvprintw(height - 3, 1, "[] Parent Directory");
    mvprintw(height - 2, 1, "[󰌑] Select");
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

        if (findbuflen > 0)
            mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y, _NAVI_PWD_DRAWING_TOP_X,
                      "Finding: %s█ ", findbuf);
        else
            mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y, _NAVI_PWD_DRAWING_TOP_X,
                      "%s", pwd);

        if (flags['m'] && prompt_message)
            mvwprintw(win, _NAVI_PWD_DRAWING_TOP_Y + 1, _NAVI_PWD_DRAWING_TOP_X,
                      "%s", prompt_message);

        if (flisting_sz != 0) {
            int curpos = 0;
            for (int i = scroll_top;
                 curpos <
                 MIN(win_height - _NAVI_LISTING_DRAWING_TOP_Y, flisting_sz);
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
                if (i == cursor_selected)
                    wattron(
                        win,
                        COLOR_PAIR(greyed_out
                                       ? _NAVI_COLORS_LISTING_DIMMED_HIGHLIGHTED
                                       : _NAVI_COLORS_LISTING_HIGHLIGHTED));

                file_t file = flisting[i];
                wchar_t icon =
                    file.type == FT_DIRECTORY
                        ? _NAVI_DEFAULT_DIRECTORY_ICON
                        : ext_hash_lookup(strchr(flisting[i].name, '.'));

                mvwprintw(win, curpos + _NAVI_LISTING_DRAWING_TOP_Y,
                          _NAVI_LISTING_DRAWING_TOP_X, "%lc %s", icon,
                          file.name);

                if (i == cursor_selected || greyed_out)
                    wattroff(win, COLOR_PAIR(_NAVI_COLORS_LISTING_HIGHLIGHTED));
                ++curpos;
            }
        } else
            mvwprintw(win, _NAVI_LISTING_DRAWING_TOP_Y,
                      _NAVI_LISTING_DRAWING_TOP_X, "<empty>");

        print_table_borders();

        refresh();
        wrefresh(win);

        kbchar = wgetch(win);

        if (kbchar == _NAVI_KEY_RESIZE) {
            recalculate_table_bounds();
            mvwin(win, start_y, start_x);
            wresize(win, win_height, win_width);
            wresize(stdscr, height, width);
            draw_background_win();
        } else if (kbchar == _NAVI_KEY_SCROLLUP) {
            cursor_selected--;
            if (cursor_selected < 0) cursor_selected = 0;
            if (cursor_selected < scroll_top) scroll_top = cursor_selected;
        } else if (kbchar == _NAVI_KEY_SCROLLDOWN) {
            cursor_selected++;
            if (cursor_selected >= flisting_sz)
                cursor_selected = flisting_sz - 1;
            if (cursor_selected >=
                scroll_top + win_height - _NAVI_LISTING_DRAWING_TOP_Y - 1)
                scroll_top = cursor_selected - win_height +
                             _NAVI_LISTING_DRAWING_TOP_Y + 2;
        } else if (kbchar == _NAVI_KEY_OPEN) {
            if (flisting[cursor_selected].type == FT_DIRECTORY) {
                char *newpath =
                    path_concat(pwd, flisting[cursor_selected].name);
                if (open_dir(newpath)) {
                    strcpy(pwd, newpath);
                    if (scroll_top_stackptr < 256 &&
                        cursor_selected_stackptr < 256) {
                        scroll_top_stack[scroll_top_stackptr++] = scroll_top;
                        cursor_selected_stack[cursor_selected_stackptr++] =
                            cursor_selected;
                    }
                    scroll_top = cursor_selected = 0;
                    wrefresh(win);
                }
                free(newpath);
            }
        } else if (kbchar == _NAVI_KEY_BACK) {
            char *newpath = path_dir(pwd);
            if (open_dir(newpath)) {
                strcpy(pwd, newpath);
                if (scroll_top_stackptr >= 1 && cursor_selected_stackptr >= 1) {
                    scroll_top = scroll_top_stack[--scroll_top_stackptr];
                    cursor_selected =
                        cursor_selected_stack[--cursor_selected_stackptr];
                } else
                    scroll_top = cursor_selected = 0;
                wrefresh(win);
            }
            free(newpath);
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
        } else if (isalnum(kbchar) || kbchar == ' ') {
            findbuf[findbuflen++] = kbchar;
            scroll_top = cursor_selected = 0;
        }
    }

    endwin();

    fprintf(og_stdout, "%s\n", pwd);

    return 0;
}
