#ifndef _NAVI_SYSDEP_H
#define _NAVI_SYSDEP_H

#include <stddef.h>

#include "file.h"

void navi_get_pwd(char *, size_t);

int navi_count_entries(const char *path);
int navi_list_dir(const char *path, file_t *buffer, int max_buffer_sz);

#endif  // _NAVI_SYSDEP_H