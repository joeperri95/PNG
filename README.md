# Inflate 

Implementations of the decoding portions of RFC 1950, 1951, 1952, and 2083

This project was originally a no dependency PNG decoder only.
Turns out in building a PNG decoder you get a free gunzip implementation since gzip is just a zlib
stream with a small header.

To do functionality
-Defiltering
-CRC validation
-ADLER32 validation
-Ancillary chunks
-Pallete chunks
-More unit tests / CI

Possibly someday a compressor.



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

depths[0] 0    
depths[1] 0    
depths[2] 0    
depths[3] 5    
depths[4] 3    
depths[5] 6    
depths[6] 0    
depths[7] 0    

next_codes[0] 0   
next_codes[1] 0   
next_codes[2] 0   
next_codes[3] 0   
next_codes[4] 10  
next_codes[5] 26  
next_codes[6] 64  
next_codes[7] 128 

0  -> 000
9  -> 001
10 -> 010
11 -> 011
17 -> 100

5  -> 1010
6  -> 1011
18 -> 1100

1  -> 11010
2  -> 11011
3  -> 11100
4  -> 11101
7  -> 11110
8  -> 11111

   
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
     0 9 10 11  17 \      /        \
                   /\    /\        /\
                  5  6  18 \      /  \
                           /\    /\  /\
                          1  2  3  4 7 8



