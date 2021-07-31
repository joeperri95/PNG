#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"
#include "bitstream.h"

typedef struct _node
{
    uint32_t data;    
    struct _node *left;
    struct _node *right;
} node;

node *constructHuffman(uint32_t *codes, uint32_t length, uint32_t max_bits);
void freeHuffman(node *tree);

node *createNode();
void freeNode(node *node);

void insertCode(node *tree, uint32_t code, uint32_t length, uint32_t data);
uint32_t searchCode(node *tree, uint32_t code, uint32_t length);

// create pr/in/post order traversal with function pointer strategy
void traverse(node *tree);

uint32_t search(bitstream_t *b, node *tree);

#endif
