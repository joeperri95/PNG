#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"
#include "bitstream.h"

// Node data structure
typedef struct _node
{
    uint32_t data;    
    struct _node *left;
    struct _node *right;
} node;


// Tree data structure
typedef struct _tree
{
    node head;
    uint32_t num_nodes;
    uint32_t num_leaf_nodes;
} tree;

// Given a set of codes construct the huffman tree
node *constructHuffman(uint32_t *codes, uint32_t length, uint32_t max_bits);

// Traverse the tree and free all nodes 
void freeHuffman(node *tree);

// Allocate memory for a tree element
node *createNode();

// Free memory allocated for a node
void freeNode(node *node);

// Insert a code into the tree
void insertCode(node *tree, uint32_t code, uint32_t length, uint32_t data);

// Search the tree using a code with known length
uint32_t searchCode(node *tree, uint32_t code, uint32_t length);

// Function to be executed on a node
typedef void *(strategy)(node *);

// Pre/in/post order traversal with function pointer strategy
void traverse_preorder(node *tree, strategy func); 
void traverse_inorder(node *tree, strategy func); 
void traverse_postorder(node *tree, strategy func); 

// Search the tree using a bitstream
// Useful when code length is unknown
uint32_t search(bitstream_t *b, node *tree);

#endif
