#ifndef __TEST_HUFFMAN_H__
#define __TEST_HUFFMAN_H__

#define NUM_TESTS_HUFFMAN 10

#include "huffman.h"
#include "test.h"

bool test_create_node_data();
bool test_create_node_left_null();
bool test_create_node_right_null();
bool test_insert_left();
bool test_insert_right();
bool test_insert_empty();
bool test_search_left();
bool test_search_right();
bool test_search_non_leaf();
bool test_search_multiple();

#endif
