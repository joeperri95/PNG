#include "huffman.h"
#include "logging.h"
#include "test.h"
#include <assert.h>

void test_create_node_data()
{
    node *testNode = createNode();
    assert(testNode->data == 0xFFFFFFFF);
}

void test_create_node_left_null()
{
    node *testNode = createNode();
    assert(testNode->left == NULL);
}

void test_create_node_right_null()
{
    node *testNode = createNode();
    assert(testNode->right == NULL);
}

void test_insert_left()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 0, 1, 1);
    assert(testHuffman->left->data == 1);
}

void test_insert_right()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 1, 1);
    assert(testHuffman->right->data == 1);
}

void test_insert_empty()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 0, 1);
    assert(testHuffman->right == NULL);
}
