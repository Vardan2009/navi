#include "sysdep.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "file.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <dirent.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#else
#error "sysdep.c: Unsupported platform"
#endif

// WARN: WINDOWS FUNCTIONS UNTESTED

void navi_get_pwd(char *cwd, size_t sz) {
#ifdef _WIN32
    if (GetCurrentDirectoryA(sz, cwd) == 0) { perror("navi"); }
#else
    if (!getcwd(cwd, sz)) { perror("navi"); }
#endif  // _WIN32
}

int navi_count_entries(const char *path) {
#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(path, &findFileData);
    int count = 0;

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory\n");
        return -1;
    }

    do count++;
    while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return count;
#else
    DIR *d = opendir(path);
    int count = 0;
    if (d) {
        struct dirent *entry;
        while ((entry = readdir(d)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            count++;
        }
        closedir(d);
    } else {
        return -1;
    }
    return count;
#endif
    return -1;
}

int navi_list_dir(const char *path, file_t *buffer, int max_buffer_sz) {
    int buffer_sz = 0;
#ifdef _WIN32
    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = FindFirstFile(path, &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) { return 1; }

    do {
        file_t f;
        f.type = (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                     ? FT_DIRECTORY
                     : FT_FILE;
        strcpy(f.name, find_file_data.cFileName);
        buffer[buffer_sz++] = f;
    } while (FindNextFile(h_find, &find_file_data) != 0 &&
             buffer_sz < max_buffer_sz);

    FindClose(h_find);
#else

    DIR *d = opendir(path);
    if (!d) { return 1; }

    struct dirent *entry;
    struct stat statbuf;

    while ((entry = readdir(d)) != NULL && buffer_sz < max_buffer_sz) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &statbuf) == 0) {
            file_t f = {};
            f.type = (S_ISDIR(statbuf.st_mode)) ? FT_DIRECTORY : FT_FILE;
            strncpy(f.name, entry->d_name, 32);
            buffer[buffer_sz++] = f;
        } else {
            return 1;
        }
    }

    closedir(d);
#endif
    return 0;
}
