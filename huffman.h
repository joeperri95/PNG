#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

typedef struct _node
{
    uint32_t data;    
    struct _node *left;
    struct _node *right;
} node;

// node *constructHuffman();
// void freeHuffman(node *tree);

node *createNode();
void freeNode(node *node);

void insertCode(node *tree, uint32_t code, uint32_t length, uint32_t data);
uint32_t searchCode(node *tree, uint32_t code, uint32_t length);

void traverse(node *tree);

#endif
