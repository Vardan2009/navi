#include "cfg_parser.h"

#include <ctype.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "main.h"

void trim_whitespaces(char *str) {
    while (isspace((unsigned char)*str)) ++str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = 0;
}

void split_key_value(char *str, char **key, char **value) {
    char *equals_pos = strchr(str, '=');
    if (equals_pos == NULL) {
        *key = NULL;
        *value = NULL;
        navi_errorf("navi.cfg", "Invalid syntax, '=' not found");
        return;
    }

    *equals_pos = 0;

    *key = str;
    *value = equals_pos + 1;

    trim_whitespaces(*key);
    trim_whitespaces(*value);
}

void parse_apply_navi_cfg() {
    static const size_t lnbuflen = 128;
    static const size_t lnpartbuflen = 64;

    char lnbuf[lnbuflen];
    char *keypart, *valuepart;

    char *lnbufptr = NULL;

    const char *home = getenv(_NAVI_HOME_ENV_VAR);
    if (home == NULL) {
        fprintf(stderr, "%s environment variable is not set.\n",
                _NAVI_HOME_ENV_VAR);
        return;
    }

    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "%s%c%s", home, _NAVI_PATH_DIV,
             _NAVI_CFG_FILE_LOCATION);

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) /* {
        navi_errorf("navi.cfg", "Failed to open config file at %s", file_path);
        return;
    } */
        // the navi.cfg file is optional
        return;

    while (fgets(lnbuf, lnbuflen, fp)) {
        lnbufptr = (char *)lnbuf;
        while (isspace(*lnbufptr)) ++lnbufptr;
        if (*lnbufptr == '#' || *lnbufptr == 0) continue;

        split_key_value(lnbufptr, &keypart, &valuepart);
        if (!keypart || !valuepart) continue;

        // inefficient code, a better apporach would be to use a hashmap
        char *endptr = NULL;
        if (strcmp(keypart, "NAVI_USE_NF") == 0)
            use_nf = (strcmp(valuepart, "true") == 0);
        else if (strcmp(keypart, "NAVI_FG_COL") == 0) {
            fg_color = strtol(valuepart, &endptr, 10);
            if (*endptr != '\0')
                navi_errorf("navi.cfg", "Invalid value for %s: `%s`", keypart,
                            valuepart);
        } else if (strcmp(keypart, "NAVI_BG_COL") == 0) {
            bg_color = strtol(valuepart, &endptr, 10);

            if (*endptr != '\0')
                navi_errorf("navi.cfg", "Invalid value for %s: `%s`", keypart,
                            valuepart);
        } else if (strcmp(keypart, "NAVI_HL_COL") == 0) {
            hl_color = strtol(valuepart, &endptr, 10);

            if (*endptr != '\0')
                navi_errorf("navi.cfg", "Invalid value for %s: `%s`", keypart,
                            valuepart);
        } else if (strcmp(keypart, "NAVI_DIM_COL") == 0) {
            dim_color = strtol(valuepart, &endptr, 10);

            if (*endptr != '\0')
                navi_errorf("navi.cfg", "Invalid value for %s: `%s`", keypart,
                            valuepart);
        } else
            navi_errorf("navi.cfg", "Invalid key `%s`", keypart);
    }

    fclose(fp);
}
