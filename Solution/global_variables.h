#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "bplus_tree_disk.h"

extern FILE* db_file;
extern DBHeader header;
extern int disk_read_count;
extern int disk_write_count;
extern int query_count;
extern int min_range_size;
extern int max_range_size;

#endif  // GLOBAL_VARIABLES_H
