#include "test_huffman.h"

bool test_create_node_data()
{
    node *testNode = createNode();
    return assert(testNode->data == 0xFFFFFFFF);
}

bool test_create_node_left_null()
{
    node *testNode = createNode();
    return assert(testNode->left == NULL);
}

bool test_create_node_right_null()
{
    node *testNode = createNode();
    return assert(testNode->right == NULL);
}

bool test_insert_left()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 0, 1, 1);
    return assert(testHuffman->left->data == 1);
}

bool test_insert_right()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 1, 1);
    return assert(testHuffman->right->data == 1);
}

bool test_insert_empty()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 0, 1);
    return assert(testHuffman->right == NULL);
}

bool test_search_right()
{
    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 1, 10);
    insertCode(testHuffman, 11, 2, 20);
    insertCode(testHuffman, 111, 3, 30);
   
    return assert(searchCode(testHuffman, 111, 3) == 30); 
}

bool test_search_left()
{

    node *testHuffman = createNode();
    insertCode(testHuffman, 0, 1, 15);
    insertCode(testHuffman, 10, 2, 25);
    insertCode(testHuffman, 110, 3, 35);

    return assert(searchCode(testHuffman, 110, 3) == 35);
}

bool test_search_non_leaf()
{

    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 1, 10);
    insertCode(testHuffman, 11, 2, 20);
    insertCode(testHuffman, 111, 3, 30);

    return assert(searchCode(testHuffman, 11, 2) == 20);
}

bool test_search_multiple()
{

    node *testHuffman = createNode();
    insertCode(testHuffman, 1, 1, 10);
    insertCode(testHuffman, 11, 2, 20);
    insertCode(testHuffman, 111, 3, 30);
   
    insertCode(testHuffman, 0, 1, 15);
    insertCode(testHuffman, 10, 2, 25);
    insertCode(testHuffman, 110, 3, 35);

    searchCode(testHuffman, 1, 1);
    searchCode(testHuffman, 11, 2);
    searchCode(testHuffman, 111, 3);

    searchCode(testHuffman, 0, 1);
    searchCode(testHuffman, 10, 2);
    return assert(searchCode(testHuffman, 110, 3) == 35);

}

static test test_list[NUM_TESTS_HUFFMAN]= 
{
    {"create node data", test_create_node_data},
    {"create node left null", test_create_node_left_null},
    {"create node right null", test_create_node_right_null},
    {"insert left", test_insert_left},
    {"insert right", test_insert_right},
    {"insert empty", test_insert_empty},
    {"search left", test_search_left},
    {"search right", test_search_right},
    {"search non leaf node", test_search_non_leaf},
    {"search multiple", test_search_multiple},
};


extern test_suite huffman_suite = 
{
    .suite_name = "huffman test suite",
    .number_of_tests = NUM_TESTS_HUFFMAN,
    .test_list = test_list, 
};
