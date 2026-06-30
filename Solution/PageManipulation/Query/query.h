#ifndef QUERY_H
#define QUERY_H

bool pointQueryBTreeStyle(int target_id, bool use_binary_search);
bool pointQueryBPlusStyle(int target_id, bool use_binary_search);
int rangeQueryBTreeStyle(int start_id, int end_id, bool use_binary_search);
int rangeQueryBPlusStyle(int start_id, int end_id, bool use_binary_search);

#endif // QUERY_H
