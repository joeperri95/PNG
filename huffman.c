#include "huffman.h"

node *createNode()
{

    node *newNode =(node *)  malloc(sizeof(node)); 
    
    newNode->data = 0xFFFFFFFF;
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

void freeNode(node *node)
{
    free(node);
}

void insertCode(node *tree, uint32_t code, uint32_t length, uint32_t data)
{
    /*
    Code is the huffman code to insert. Since there may be prefixed zeros the length parameter has also been added to specify length of the code.
    The most significant bit is the first choice. 0 indicated left and 1 indicated right

    eg. code = 00000001 length = 3

                     | 
               /----------\
            /----\      /---\
           /\    /\    /\   /\
          /  \  /  \  /  \ /  \
         x   O  x   x x  x x   x 
    
    */

   if(length > 100)
   {       
       LOG(ERROR, "code %d data %d length %d LENGTH CODE TOO LONG\n",code, data, length);
       return;
   }

    uint32_t shift = length;
    uint32_t nextbit;
    node *nextNode;
    node *currentNode;    
    uint32_t mask = (0x01 << length) - 1;

    currentNode = tree;
    char str[100];
    // Traverse tree and create intermediate nodes as necessary

    str[length] = '\0';


    for (int i = 1; i < (length); i++)
    {
        nextbit = ((code) >> (length - i)) & 0x01;        
        
        if (nextbit == 0)
        {
            str[i - 1] = '0';
            if (currentNode->left == NULL)
            {                     
                currentNode->left = createNode();
                currentNode = currentNode->left;
            }
            else
            {                
                currentNode = currentNode->left;
            }
        }

        else
        {
            str[i - 1] = '1';
            if (currentNode->right == NULL)
            {                                
                currentNode->right = createNode();
                currentNode = currentNode->right;
            }
            else
            {                
                currentNode = currentNode->right;
            }
        }
    }

    // Now insert leaf node

    nextbit = code & 0x01;

    if (nextbit == 0)
    {
        str[length - 1] = '0';
        if (currentNode->left == NULL)
        {
            currentNode->left = createNode(); 
            currentNode = currentNode->left;
            currentNode->data = data;
            
        }
        else
        {
            LOG(ERROR, "Node is already inserted\n");
            // Node is already inserted
        }
    }

    else
    {
        str[length - 1] = '1';
        if (currentNode->right == NULL)
        {
            currentNode->right = createNode();
            currentNode = currentNode->right;
            currentNode->data = data;
        }
        else
        {
            LOG(ERROR, "Node is already inserted\n");
            // Node is already inserted
        }
    }    

    str[length + 1] = '\0';

    LOG(DEBUG_2, "%s code %d data %d length %d \n", str,  code ,currentNode->data, length);        
}

uint32_t searchCode(node *tree, uint32_t code, uint32_t length)
{

    uint32_t shift = length;
    uint32_t nextbit;
    node *currentNode;
    uint32_t mask = (0x01 << length) - 1;

    currentNode = tree;

    for (int i = 0; i < length ; i++)
    {
        shift = length - i - 1;
        nextbit = ((code & mask) >> (shift)) & 0x01;       
        
        currentNode = nextbit ? currentNode->right : currentNode->left;        
        
        if (currentNode == NULL)
        {
            // printf("node not found\n");
            return 0xFFFFFFFF;
        }
    }

    if( currentNode->data == 0xFFFFFFFF) 
    {
        // Not a leaf node
        // printf("not a leaf node\n");
        return 0xFFFFFFFF; 
    }

    //printf("code %d: node found %d\n",code, currentNode->data);
    return currentNode->data;
}

void traverse(node *tree)
{
    // dump the contents of the huffman tree
    // used for debugging

    static int depth = 0;    
    depth++;
    
    if(tree == NULL)
    {         
        depth--;       
        return;
    }
    
    traverse((*tree).left);
    traverse((*tree).right);
    
    depth--;
    
    if(tree->data != 0xFFFFFFFF)
        LOG(DEBUG, "depth %d value %d\n",depth, (*tree).data);
}


node *constructHuffman(uint32_t *codes, uint32_t length, uint32_t max_bits)
{

    node *huffman_tree = createNode();

#define MAX_HUFFMAN_SIZE 100
    uint32_t depths[MAX_HUFFMAN_SIZE];
    uint32_t next_codes[MAX_HUFFMAN_SIZE];
    
    memset(depths, 0, max_bits * sizeof(uint32_t));
    memset(next_codes, 0, max_bits * sizeof(uint32_t));

    for (int i = 0; i < length; i++)
    {
        if(codes[i] != -1)
            depths[codes[i]]++;
    }

    depths[0] = 0;
    int code = 0;

    for (int bits = 1; bits < max_bits; bits++)
    {
            code = (code + depths[bits - 1]) << 1;
            next_codes[bits] = code;
    }

    int left = 1;

    for (int len = 1; len < max_bits; len++) {
        left <<= 1;                     
        left -= depths[len];          
        if (left < 0)
            LOG(ERROR, "%d OH NO!!!\n", len);
    }

    for (int n = 0; n < length; n++)
    {
        int len = codes[n];
        if (len != 0 && len != -1)
        {
            insertCode(huffman_tree, next_codes[len], len, n);
            next_codes[len]++;
        }
    }
    return huffman_tree;
}

void freeHuffman(node *tree)
{
    // not implemented
}


uint32_t search(bitstream_t *b, node *tree)
{
    uint32_t runlen = 1;
    uint32_t lencode = read_bits(b, 1);
    uint32_t newcode = searchCode(tree, lencode, runlen);
    
    while (newcode == 0xFFFFFFFF && runlen < 16)
    {
            runlen++;
            uint32_t len_buffer = read_bits(b, 1);
            lencode = (lencode << 0x01) + len_buffer;
            newcode = searchCode(tree, lencode , runlen);
            //printNbits(lencode, runlen);
    }

    LOG(DEBUG_2, "length: %d val: %d code %d\n", runlen, newcode, lencode);
   // printNbits(lencode, runlen);

    return newcode;
}

