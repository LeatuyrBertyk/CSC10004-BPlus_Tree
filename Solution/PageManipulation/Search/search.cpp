#include "search.h"

int linearSearch(const int keys[], int num_keys, int target) {
  for (int i = 0; i < num_keys; i++) {
    if (keys[i] > target) {
      return i;
    }
  }

  return num_keys;
}

int binarySearch(const int keys[], int num_keys, int target) {
  int add_pos = num_keys;

  int left = 0, right = num_keys - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (keys[mid] > target) {
      add_pos = mid;
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }

  return add_pos;
}
