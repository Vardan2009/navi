#ifndef _NAVI_EXT_TABLE_H
#define _NAVI_EXT_TABLE_H

#define EXT_TABLE_SZ 3

typedef struct {
    char extension[16];
    unsigned int icon;
} file_extension_t;

typedef struct chain_node {
    file_extension_t data;
    struct chain_node *next;
} chain_node_t;

unsigned int ext_hash(char *str);
void ext_hash_insert(char *ext, unsigned int icon);
unsigned int ext_hash_lookup(char *ext);

extern chain_node_t *ext_hashtable[EXT_TABLE_SZ];

#endif  // _NAVI_EXT_TABLE_H
