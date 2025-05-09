#ifndef _NAVI_MAIN_H
#define _NAVI_MAIN_H

#include <stdbool.h>

extern int fg_color;
extern int bg_color;
extern int hl_color;
extern int dim_color;
extern bool use_nf;

extern unsigned int default_dir_icon, default_file_icon;

void navi_errorf(const char *title, const char *fmt, ...);

#endif  // _NAVI_MAIN_H
