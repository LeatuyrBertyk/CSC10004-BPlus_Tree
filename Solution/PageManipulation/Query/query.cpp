#include "query.h"

#include "../../bplus_tree_disk.h"
#include "../../global_variables.h"
#include "../ReadWrite/read_write.h"
#include "../Search/search.h"

bool pointQueryBTreeStyleRecursive(int current_offset, int target_id,
                                   bool use_binary_search) {
  BPlusNode node;
  readNode(current_offset, node);

  if (node.is_leaf == true) {
    if (use_binary_search) {
      int left = 0, right = node.num_keys - 1;
      while (left <= right) {
        int mid = left + (right - left) / 2;
        if (node.leaf.records[mid].id == target_id) {
          return true;
        } else if (node.leaf.records[mid].id < target_id) {
          left = mid + 1;
        } else {
          right = mid - 1;
        }
      }
      return false;
    } else {
      for (int i = 0; i < node.num_keys; i++) {
        if (node.leaf.records[i].id == target_id) {
          return true;
        }
      }
      return false;
    }
  } else {
    int child_idx = node.num_keys;

    if (use_binary_search) {
      int left = 0, right = node.num_keys - 1;
      while (left <= right) {
        int mid = left + (right - left) / 2;

        if (node.internal.keys[mid] == target_id) {
          return true;
        } else if (node.internal.keys[mid] > target_id) {
          child_idx = mid;
          right = mid - 1;
        } else {
          left = mid + 1;
        }
      }
    } else {
      for (int i = 0; i < node.num_keys; i++) {
        if (node.internal.keys[i] == target_id) {
          return true;
        }
        if (node.internal.keys[i] > target_id) {
          child_idx = i;
          break;
        }
      }
    }

    return pointQueryBTreeStyleRecursive(
        node.internal.children_offsets[child_idx], target_id,
        use_binary_search);
  }
}

bool pointQueryBTreeStyle(int target_id, bool use_binary_search) {
  if (header.root_offset == -1) {
    return false;
  }

  return pointQueryBTreeStyleRecursive(header.root_offset, target_id,
                                       use_binary_search);
}

bool pointQueryBPlusStyle(int target_id, bool use_binary_search) {
  if (header.root_offset == -1) {
    return false;
  }

  BPlusNode node;
  int current_offset = header.root_offset;

  while (true) {
    readNode(current_offset, node);

    if (node.is_leaf == true) {
      // Reached leaf
      if (use_binary_search) {
        int left = 0, right = node.num_keys - 1;
        while (left <= right) {
          int mid = left + (right - left) / 2;
          if (node.leaf.records[mid].id == target_id) {
            return true;
          } else if (node.leaf.records[mid].id < target_id) {
            left = mid + 1;
          } else {
            right = mid - 1;
          }
        }
        return false;
      } else {
        for (int i = 0; i < node.num_keys; i++) {
          if (node.leaf.records[i].id == target_id) {
            return true;
          }
        }
        return false;
      }
    } else {
      // Still internal node
      int child_index = node.num_keys;

      if (use_binary_search) {
        child_index =
            binarySearch(node.internal.keys, node.num_keys, target_id);
      } else {
        child_index =
            linearSearch(node.internal.keys, node.num_keys, target_id);
      }

      current_offset = node.internal.children_offsets[child_index];
    }
  }
}

int rangeQueryBTreeStyleRecursive(int current_offset, int start_id, int end_id,
                                  bool use_binary_search) {
  BPlusNode node;
  readNode(current_offset, node);

  if (node.is_leaf == true) {
    if (node.leaf.records[0].id > end_id ||
        node.leaf.records[node.num_keys - 1].id < start_id) {
      return 0;
    }

    int start = node.num_keys, end = node.num_keys;

    if (use_binary_search) {
      int left = 0, right = node.num_keys - 1;
      while (left <= right) {
        int mid = left + (right - left) / 2;
        if (node.leaf.records[mid].id >= start_id) {
          start = mid;
          right = mid - 1;
        } else {
          left = mid + 1;
        }
      }

      left = 0, right = node.num_keys - 1;
      while (left <= right) {
        int mid = left + (right - left) / 2;
        if (node.leaf.records[mid].id > end_id) {
          end = mid;
          right = mid - 1;
        } else {
          left = mid + 1;
        }
      }
    } else {
      for (int i = 0; i < node.num_keys; i++) {
        if (start == node.num_keys && node.leaf.records[i].id >= start_id) {
          start = i;
        }
        if (node.leaf.records[i].id > end_id) {
          end = i;
          break;
        }
      }
    }
    return (end - start);
  }

  int start = 0, end = node.num_keys;

  if (use_binary_search) {
    start = binarySearch(node.internal.keys, node.num_keys, start_id);
    end = binarySearch(node.internal.keys, node.num_keys, end_id + 1);
  } else {
    start = linearSearch(node.internal.keys, node.num_keys, start_id);
    end = linearSearch(node.internal.keys, node.num_keys, end_id + 1);
  }

  int total_id = 0;
  for (int i = start; i <= end; i++) {
    total_id += rangeQueryBTreeStyleRecursive(
        node.internal.children_offsets[i], start_id, end_id, use_binary_search);
  }

  return total_id;
}

int rangeQueryBTreeStyle(int start_id, int end_id, bool use_binary_search) {
  if (header.root_offset == -1) {
    return 0;
  }

  int current_offset = header.root_offset;
  return rangeQueryBTreeStyleRecursive(current_offset, start_id, end_id,
                                       use_binary_search);
}

int rangeQueryBPlusStyle(int start_id, int end_id, bool use_binary_search) {
  if (header.root_offset == -1) {
    return 0;
  }

  BPlusNode node;
  int current_offset = header.root_offset;
  while (true) {
    readNode(current_offset, node);

    if (node.is_leaf) {
      break;
    } else {
      int child_index = node.num_keys;
      if (use_binary_search) {
        child_index = binarySearch(node.internal.keys, node.num_keys, start_id);
      } else {
        child_index = linearSearch(node.internal.keys, node.num_keys, start_id);
      }

      current_offset = node.internal.children_offsets[child_index];
    }
  }

  int total_id = 0;
  int start = node.num_keys, end;

  if (use_binary_search) {
    int left = 0, right = node.num_keys - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      if (node.leaf.records[mid].id >= start_id) {
        start = mid;
        right = mid - 1;
      } else {
        left = mid + 1;
      }
    }
  } else {
    for (int i = 0; i < node.num_keys; i++) {
      if (node.leaf.records[i].id >= start_id) {
        start = i;
        break;
      }
    }
  }

  while (true) {
    if (node.leaf.records[node.num_keys - 1].id <= end_id) {
      end = node.num_keys;
    } else {
      if (use_binary_search) {
        int left = 0, right = node.num_keys - 1;
        while (left <= right) {
          int mid = left + (right - left) / 2;
          if (node.leaf.records[mid].id > end_id) {
            end = mid;
            right = mid - 1;
          } else {
            left = mid + 1;
          }
        }
      } else {
        for (int i = 0; i < node.num_keys; i++) {
          if (node.leaf.records[i].id > end_id) {
            end = i;
            break;
          }
        }
      }
    }

    total_id += (end - start);

    if (end == node.num_keys &&
        node.leaf.records[node.num_keys - 1].id == end_id) {
      break;
    }

    if (end < node.num_keys) {
      break;
    }

    current_offset = node.leaf.next_leaf_offset;
    if (current_offset == -1) {
      break;
    } else {
      readNode(current_offset, node);
    }
    start = 0;
  }

  return total_id;
}
