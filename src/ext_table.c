#include "ext_table.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "main.h"

chain_node_t *ext_hashtable[EXT_TABLE_SZ];

int strcicmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a) return d;
    }
}

unsigned int ext_hash(char *str) {
    unsigned int hashval = 0;
    while (*str) hashval = (hashval * 31) + tolower(*str++);
    return hashval % EXT_TABLE_SZ;
}

void ext_hash_insert(char *ext, unsigned int icon) {
    unsigned int idx = ext_hash(ext);

    chain_node_t *node = (chain_node_t *)malloc(sizeof(chain_node_t));
    strcpy(node->data.extension, ext);
    node->data.icon = icon;
    node->next = ext_hashtable[idx];

    ext_hashtable[idx] = node;
}

unsigned int ext_hash_lookup(char *ext) {
    if (ext == (char *)1 || ext == NULL) return default_file_icon;
    unsigned int idx = ext_hash(ext);

    chain_node_t *current = ext_hashtable[idx];

    while (current) {
        if (strcicmp(current->data.extension, ext) == 0)
            return current->data.icon;
        current = current->next;
    }

    return default_file_icon;
}
