#ifndef READ_WRITE_H
#define READ_WRITE_H

#include "../../bplus_tree_disk.h"

void readNode(int offet, BPlusNode &node);
void writeNode(int offset, BPlusNode &node);

#endif // READ_WRITE_H
