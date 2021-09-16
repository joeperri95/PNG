#include "huffman.h"

node *create_node()
{

    node *newNode =(node *)  malloc(sizeof(node)); 
    
    newNode->data = 0xFFFFFFFF;
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

void free_node(node *node)
{
    free(node);
}

void insert_code(node *tree, uint32_t code, uint32_t length, uint32_t data)
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
   if(length == 0)
   {
       return;
   }

   if(length > 100)
   {       
       LOG(glog, ERROR, "code %d data %d length %d LENGTH CODE TOO LONG\n",code, data, length);
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
                currentNode->left = create_node();
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
                currentNode->right = create_node();
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
            currentNode->left = create_node(); 
            currentNode = currentNode->left;
            currentNode->data = data;
            
        }
        else
        {
            LOG(glog, ERROR, "Node is already inserted\n");
            // Node is already inserted
        }
    }

    else
    {
        str[length - 1] = '1';
        if (currentNode->right == NULL)
        {
            currentNode->right = create_node();
            currentNode = currentNode->right;
            currentNode->data = data;
        }
        else
        {
            LOG(glog, ERROR, "Node is already inserted\n");
            // Node is already inserted
        }
    }    

    str[length + 1] = '\0';

    LOG(glog, DEBUG_2, "%s code %d data %d length %d \n", str,  code ,currentNode->data, length);        
}

uint32_t search_code(node *tree, uint32_t code, uint32_t length)
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
            LOG(glog, WARNING, "node not found\n");
            return 0xFFFFFFFF;
        }
    }

    if( currentNode->data == 0xFFFFFFFF) 
    {
        // Not a leaf node
        // printf("not a leaf node\n");
        //LOG(glog, WARNING, "not a leaf node\n");
        return 0xFFFFFFFF; 
    }

    //printf("code %d: node found %d\n",code, currentNode->data);
    return currentNode->data;
}

node *construct_huffman(uint32_t *codes, uint32_t length, uint32_t max_bits)
{

    node *huffman_tree = create_node();

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
            LOG(glog, ERROR, "%d OH NO!!!\n", len);
    }

    for (int n = 0; n < length; n++)
    {
        int len = codes[n];
        if (len != 0 && len != -1)
        {
            insert_code(huffman_tree, next_codes[len], len, n);
            next_codes[len]++;
        }
    }
    return huffman_tree;
}

void free_huffman(node *tree)
{
    traverse_postorder(tree,(strategy*) &free_node);
}


uint32_t search(bitstream_t *b, node *tree)
{
    uint32_t runlen = 1;
    uint32_t lencode = read_bits_big_endian(b, 1);
    uint32_t newcode = search_code(tree, lencode, runlen);
    
    while (newcode == 0xFFFFFFFF && runlen < 16)
    {
            runlen++;
            uint32_t len_buffer = read_bits_big_endian(b, 1);
            lencode = (lencode << 0x01) + len_buffer;
            newcode = search_code(tree, lencode , runlen);
            //printNbits(lencode, runlen);
    }

    LOG(glog, 9, "length: %d val: %d code %d\n", runlen, newcode, lencode);
    //printNbits(lencode, runlen);

    return newcode;
}

void traverse_preorder(node *tree, strategy func)
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

    func(tree);
    traverse_preorder((*tree).left, func);
    traverse_preorder((*tree).right, func);
    
    depth--;

}

void traverse_inorder(node *tree, strategy func)
{

    static int depth = 0;    
    depth++;
    
    if(tree == NULL)
    {         
        depth--;       
        return;
    }
    
    traverse_inorder((*tree).left, func);
    func(tree);
    traverse_inorder((*tree).right, func);
    
    depth--;
    
}

void traverse_postorder(node *tree, strategy func)
{

    static int depth = 0;    
    depth++;
    
    if(tree == NULL)
    {         
        depth--;       
        return;
    }
    
    traverse_postorder((*tree).left, func);
    traverse_postorder((*tree).right, func);
    
    depth--;

    func(tree);
}
