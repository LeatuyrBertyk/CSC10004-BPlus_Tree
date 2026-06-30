#include "read_write.h"

#include "../../bplus_tree_disk.h"
#include "../../global_variables.h"

void readNode(int offset, BPlusNode &node) {
  fseek(db_file, offset, SEEK_SET);

  if (fread(&node, 1, sizeof(BPlusNode), db_file)) {
    disk_read_count++;
  }
}

void writeNode(int offset, BPlusNode &node) {
  fseek(db_file, offset, SEEK_SET);

  if (fwrite(&node, 1, sizeof(BPlusNode), db_file)) {
    disk_write_count++;
  }
}
