PNG decoder and viewer. 
Personal interest project unassociated with schoolwork.


To do functionality
-Read file
-Get Metadata
-Read chunks
-Assemble data
-Inflate zlib stream
-CRC validation


Future wishlist
-Other filetypes (JPEG) Might be in another project
-Streaming support
-Error handling
-Ancillary chunks
-Some sort of organization


Below is an example of the run length coded runlengths

Code 16: 0       
Code 17: 3       
Code 18: 4       
Code 0: 3        
Code 8: 5        
Code 7: 5        
Code 9: 3        
Code 6: 4        
Code 10: 3       
Code 5: 4        
Code 11: 3       
Code 4: 5        
Code 12: 0       
Code 3: 5        
Code 13: 0       
Code 2: 5        
Code 14: 0
Code 1: 5

bl_count[0] 0    
bl_count[1] 0    
bl_count[2] 0    
bl_count[3] 5    
bl_count[4] 3    
bl_count[5] 6    
bl_count[6] 0    
bl_count[7] 0    

next_code[0] 0   
next_code[1] 0   
next_code[2] 0   
next_code[3] 0   
next_code[4] 10  
next_code[5] 26  
next_code[6] 64  
next_code[7] 128 

tree[17] 0       
tree[18] 10      
tree[0] 1        
tree[8] 26       
tree[7] 27       
tree[9] 2        
tree[6] 11       
tree[10] 3       
tree[5] 12       
tree[11] 4       
tree[4] 28       
tree[3] 29       
tree[2] 30              
tree[1] 31

17 -> 000
0  -> 001
9  -> 010
10 -> 011
11 -> 100

18 -> 1010
6  -> 1011
5  -> 1100

8  -> 11010
7  -> 11011
4  -> 11100
3  -> 11101
2  -> 11110
1  -> 11111

   
                    ^
                   / \
                  /   \
                 /     \
                /       \
               /         \ 
              /          /\
             /          /  \ 
            /          /    \
           /          /      \  
          /          /        \
         /          /         /\
        /\         /         /  \
       /  \       /         /    \
      /\  /\     /\        /      \ 
    17 0 9  10  11 \      /        \
                   /\    /\        /\
                  18 6  5  \      /  \
                           /\    /\  /\
                          8  7  4  3 2 1

