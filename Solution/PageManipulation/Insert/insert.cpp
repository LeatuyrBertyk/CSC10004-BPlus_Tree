#include "insert.h"

#include <cstring>

#include "../../bplus_tree_disk.h"
#include "../../global_variables.h"
#include "../ReadWrite/read_write.h"
#include "../Search/search.h"

int getNextAvailableOffset() {
  int available_offset =
      sizeof(DBHeader) + header.total_nodes * sizeof(BPlusNode);

  header.total_nodes++;

  fseek(db_file, 0, SEEK_SET);
  fwrite(&header, 1, sizeof(DBHeader), db_file);

  return available_offset;
}

bool insertRecursive(int current_offset, int id, const char *payload,
                     int &new_key, int &new_offset) {
  BPlusNode node;
  readNode(current_offset, node);

  if (node.is_leaf == true) {
    // Case 1: Leaf node
    if (node.num_keys < MAX_LEAF_KEYS) {
      // Case 1.1: No split at leaf node
      int i = node.num_keys;
      while (i > 0 && node.leaf.records[i - 1].id > id) {
        node.leaf.records[i] = node.leaf.records[i - 1];
        --i;
      }
      node.leaf.records[i].id = id;
      strncpy(node.leaf.records[i].payload, payload, PAYLOAD_SIZE);
      node.leaf.records[i].payload[PAYLOAD_SIZE - 1] = '\0'; // for safety
      node.num_keys++;

      writeNode(current_offset, node);

      return false;
    } else {
      // Case 1.1: Split at leaf node
      Record temp_records[MAX_LEAF_KEYS + 1];
      int i = 0, k = 0;

      while (i < MAX_LEAF_KEYS && node.leaf.records[i].id < id) {
        temp_records[k++] = node.leaf.records[i++];
      }
      temp_records[k].id = id;
      strncpy(temp_records[k].payload, payload, PAYLOAD_SIZE);
      temp_records[k].payload[PAYLOAD_SIZE - 1] = '\0';
      ++k;
      while (i < MAX_LEAF_KEYS) {
        temp_records[k++] = node.leaf.records[i++];
      }

      int mid_index = (MAX_LEAF_KEYS + 1) / 2;

      BPlusNode new_leaf;
      memset(&new_leaf, 0, sizeof(BPlusNode));
      new_leaf.is_leaf = true;
      int new_leaf_offset = getNextAvailableOffset();

      new_leaf.leaf.next_leaf_offset = node.leaf.next_leaf_offset;
      node.leaf.next_leaf_offset = new_leaf_offset;

      for (i = 0; i < mid_index; i++) {
        node.leaf.records[i] = temp_records[i];
      }
      node.num_keys = mid_index;

      int k_new = 0;
      for (int i = mid_index; i < MAX_LEAF_KEYS + 1; i++) {
        new_leaf.leaf.records[k_new++] = temp_records[i];
      }
      new_leaf.num_keys = k_new;

      new_key = new_leaf.leaf.records[0].id;
      new_offset = new_leaf_offset;

      writeNode(current_offset, node);
      writeNode(new_leaf_offset, new_leaf);

      return true;
    }
  } else {
    // Case 2: Internal node
    int add_pos = binarySearch(node.internal.keys, node.num_keys, id);

    int child_new_key, child_new_offset;
    bool children_split =
        insertRecursive(node.internal.children_offsets[add_pos], id, payload,
                        child_new_key, child_new_offset);
    if (children_split == false) {
      // Case 2.1: No split at children
      return false;
    } else {
      // Case 2.2: Split at children
      if (node.num_keys < MAX_INTERNAL_KEYS) {
        // Case 2.2.1: No split at internal node
        int i = node.num_keys;
        while (i > 0 && node.internal.keys[i - 1] > child_new_key) {
          node.internal.keys[i] = node.internal.keys[i - 1];
          node.internal.children_offsets[i + 1] =
              node.internal.children_offsets[i];
          --i;
        }
        node.internal.keys[i] = child_new_key;
        node.internal.children_offsets[i + 1] = child_new_offset;
        node.num_keys++;

        writeNode(current_offset, node);

        return false;
      } else {
        // Case 2.2.2: Split at internal node

        int temp_keys[MAX_INTERNAL_KEYS + 1];
        int temp_children_offsets[MAX_INTERNAL_KEYS + 2];

        int i = 0, k = 0;
        while (i < MAX_INTERNAL_KEYS && node.internal.keys[i] < child_new_key) {
          temp_keys[k] = node.internal.keys[i];
          temp_children_offsets[k] = node.internal.children_offsets[i];
          ++i, ++k;
        }
        temp_children_offsets[k] = node.internal.children_offsets[i];
        temp_keys[k] = child_new_key;
        temp_children_offsets[k + 1] = child_new_offset;
        ++i, ++k;
        while (i < MAX_INTERNAL_KEYS + 1) {
          temp_keys[k] = node.internal.keys[i - 1];
          temp_children_offsets[k + 1] = node.internal.children_offsets[i];
          ++i, ++k;
        }

        int mid_index = (MAX_INTERNAL_KEYS + 1) / 2;

        BPlusNode new_internal;
        memset(&new_internal, 0, sizeof(BPlusNode));
        new_internal.is_leaf = false;
        int new_internal_offset = getNextAvailableOffset();

        node.num_keys = mid_index;
        for (i = 0; i < mid_index; i++) {
          node.internal.keys[i] = temp_keys[i];
          node.internal.children_offsets[i] = temp_children_offsets[i];
        }
        node.internal.children_offsets[mid_index] =
            temp_children_offsets[mid_index];

        new_key = temp_keys[mid_index];
        new_offset = new_internal_offset;

        int k_new = 0;
        new_internal.internal.children_offsets[0] =
            temp_children_offsets[mid_index + 1];
        for (i = mid_index + 1; i < MAX_INTERNAL_KEYS + 1; i++) {
          new_internal.internal.keys[k_new] = temp_keys[i];
          new_internal.internal.children_offsets[k_new + 1] =
              temp_children_offsets[i + 1];
          ++k_new;
        }
        new_internal.num_keys = k_new;

        writeNode(current_offset, node);
        writeNode(new_internal_offset, new_internal);

        return true;
      }
    }

    return false;
  }
}

void insertRecord(int id, const char *payload) {
  if (header.root_offset == -1) {
    // Case 1: B+ Tree is empty
    int root_offset = getNextAvailableOffset();

    BPlusNode root;
    memset(&root, 0, sizeof(BPlusNode));
    root.is_leaf = true;
    root.num_keys = 1;
    root.leaf.next_leaf_offset = -1;

    root.leaf.records[0].id = id;
    strncpy(root.leaf.records[0].payload, payload, PAYLOAD_SIZE);
    root.leaf.records[0].payload[PAYLOAD_SIZE - 1] = '\0';

    header.root_offset = root_offset;
    writeNode(root_offset, root);

    fseek(db_file, 0, SEEK_SET);
    fwrite(&header, 1, sizeof(DBHeader), db_file);

    return;
  }

  // Case 2: B+ Tree has values
  int new_key = 0;
  int new_offset = 0;

  bool root_split =
      insertRecursive(header.root_offset, id, payload, new_key, new_offset);

  // Case 2': Split at root
  if (root_split == true) {
    int new_root_offset = getNextAvailableOffset();

    BPlusNode new_root;
    memset(&new_root, 0, sizeof(BPlusNode));
    new_root.is_leaf = false;
    new_root.num_keys = 1;

    new_root.internal.keys[0] = new_key;
    new_root.internal.children_offsets[0] = header.root_offset;
    new_root.internal.children_offsets[1] = new_offset;

    header.root_offset = new_root_offset;
    writeNode(new_root_offset, new_root);

    fseek(db_file, 0, SEEK_SET);
    fwrite(&header, 1, sizeof(DBHeader), db_file);
  }
}
