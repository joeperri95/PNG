#include "huffman.h"

node *createNode()
{

    node *newNode =  malloc(sizeof(node)); 
    
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
       printf("code %d data %d length %d NOPE\n",code, data, length);
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
            // Node is already inserted
        }
    }    

    str[length + 1] = '\0';

    printf("%s code %d data %d length %d \n", str,  code ,data, length);        
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
            //printf("node not found\n");
            return 0xFFFFFFFF;
        }
    }

    if( currentNode->data == 0xFFFFFFFF) 
    {
        // Not a leaf node
        //printf("not a leaf node\n");
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
    if(tree->data != 255)
        printf("depth %d value %d\n",depth, (*tree).data);
    
}
