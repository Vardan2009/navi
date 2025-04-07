#ifndef _NAVI_FILE_H
#define _NAVI_FILE_H

#include <stddef.h>

typedef enum { FT_FILE, FT_DIRECTORY } file_type_t;

typedef struct {
    file_type_t type;
    char name[32];
    size_t size;
} file_t;

#endif  // _NAVI_FILE_H
