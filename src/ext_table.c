#include "ext_table.h"

#include <stdlib.h>
#include <string.h>

#include "config.h"

chain_node_t *ext_hashtable[EXT_TABLE_SZ];

unsigned int ext_hash(char *str) {
    unsigned int hashval = 0;
    while (*str) hashval = (hashval * 31) + *str++;
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
    if (!ext) return _NAVI_DEFAULT_FILE_ICON;
    unsigned int idx = ext_hash(ext);

    chain_node_t *current = ext_hashtable[idx];

    while (current) {
        if (strcmp(current->data.extension, ext) == 0)
            return current->data.icon;
        current = current->next;
    }

    return _NAVI_DEFAULT_FILE_ICON;
}
