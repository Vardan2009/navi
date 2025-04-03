#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

char *path_concat(const char *path1, const char *path2) {
    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    size_t total_len = len1 + len2 + 2;

    char *result = (char *)malloc(total_len * sizeof(char));
    if (!result) return NULL;

    strcpy(result, path1);

    if (result[len1 - 1] != DIR_SEPARATOR) {
        result[len1] = DIR_SEPARATOR;
        len1++;
    }

    strcpy(result + len1, path2);

    return result;
}

char *path_dir(const char *path) {
    size_t len = strlen(path);

    if (len == 0 || path[len - 1] == DIR_SEPARATOR) {
        char *parent_path = (char *)malloc(2);
        strcpy(parent_path, "/");
        return parent_path;
    }

    const char *last_separator = strrchr(path, DIR_SEPARATOR);
    if (last_separator == NULL) return NULL;

    size_t parent_len = last_separator - path;
    if (parent_len == 0) {
        char *parent_path = (char *)malloc(2);
        strcpy(parent_path, "/");
        return parent_path;
    }

    char *parent_path = (char *)malloc(parent_len + 1);
    if (!parent_path) return NULL;

    strncpy(parent_path, path, parent_len);
    parent_path[parent_len] = '\0';
    return parent_path;

   
}
