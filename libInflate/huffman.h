#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

// huffman.h
/*
    The huffman module contains functions for constructing a huffman tree as 
    well as ones for its application to the inflate algorithm. 
*/

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"
#include "bitstream.h"

// Data Structures

/* Node data structure */
typedef struct _node
{
    uint32_t data;    
    struct _node *left;
    struct _node *right;
} node;


/* 
    Tree data structure 
    Not currently in use
*/
typedef struct _tree
{
    node head;
    uint32_t num_nodes;
    uint32_t num_leaf_nodes;
} tree;

/* Function to be executed on a node */
typedef void *(strategy)(node *);

// Public Functions

/* Given a set of codes construct the huffman tree */
node *construct_huffman(uint32_t *codes, uint32_t length, uint32_t max_bits);

/* Traverse the tree and free all nodes  */
void free_huffman(node *tree);

/* Allocate memory for a tree element */
node *create_node();

/* Free memory allocated for a node */
void free_node(node *node);

/* Insert a code into the tree */
void insert_code(node *tree, uint32_t code, uint32_t length, uint32_t data);

/* Search the tree using a code with known length */
uint32_t search_code(node *tree, uint32_t code, uint32_t length);

/* Pre-order traversal with function pointer strategy */
void traverse_preorder(node *tree, strategy func); 

/* In-order traversal with function pointer strategy */
void traverse_inorder(node *tree, strategy func); 

/* Post-order traversal with function pointer strategy */
void traverse_postorder(node *tree, strategy func); 

/* 
    Search the tree using a bitstream 
    Useful when code length is unknown 
*/
uint32_t search(bitstream_t *b, node *tree);

#endif
