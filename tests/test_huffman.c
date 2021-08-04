#include "huffman.h"
#include "logging.h"
#include "test.h"
#include <assert.h>

void test_create_node_data()
{
    node *testNode = createNode();
    assert(testNode->data == 0xFFFFFFFF);
}