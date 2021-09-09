#include "inflate.h"

zlibMetaData z_processHeader(bitstream_t *input)
{
    zlibMetaData z_data;
    z_data.compression_method = read_bits_little_endian(input, 4);
    z_data.compression_info = 0x01 << (read_bits_little_endian(input, 4) + 0x08);
    z_data.fcheck = read_bits_little_endian(input, 5);
    z_data.fdict = read_bits_little_endian(input, 1);
    z_data.flevel = read_bits_little_endian(input, 2);

    return z_data;
}

uint32_t z_inflate(bitstream_t *input, unsigned char *outputStream)
{
    uint8_t z_final = 0;
    uint8_t z_compression_type;
    uint32_t outputLength = 0;

     while(! z_final){
            
            z_final = read_bits_little_endian(input, 1);
            z_compression_type = read_bits_little_endian(input , 2);

            LOG(INFO, "final block %d\n", z_final);
            LOG(INFO, "compression type %d\n", z_compression_type);
            switch(z_compression_type)
            {
                case 0x00:
                    z_uncompressed(input, outputStream, &outputLength);
                    break;
                case 0x01:
                    LOG(ERROR, "FIXED not yet fixed\n");
                    exit(1);
                    z_compressed_fixed(input, outputStream, &outputLength);
                    break;
                case 0x02:
                    z_compressed_dynamic(input, outputStream, &outputLength);
                    break;
                default:
                    LOG(ERROR, "invalid compression type %d\n",z_compression_type);
                    return -1; 
                    break;
            };

            LOG(INFO, "Output length %d\n", outputLength);
     }

    return outputLength;
}

void z_uncompressed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength)
{
    
    if(input->bit_offset != 0){ 
        input->bit_offset = 0;
        input->byte_offset++;
    }

    uint32_t len = read_bits_big_endian(input, 16);    
    int32_t nlen = read_bits_big_endian(input, 16);    
    
     printf("%d bytes %d\n", len, nlen);
    for(int i = 0; i < len; i++)
    {
        *(outputStream + (*outputLength)++) = read_bits_little_endian(input,8);
    }
}

void z_compressed_fixed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength)
{

    const uint32_t literal_extra_bit_table[29]  =  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    const uint32_t literal_offset_table[29]     =  {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    const uint32_t distance_extra_bit_table[30] =  {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
    const uint32_t distance_offset_table[30]    =  {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24557};

    // construct fixed huffman tree
    node *huffmanTree = createNode();     
    uint16_t code = 0x30; 

    for(int i = 0; i < 144; i++)
    {
        insertCode(huffmanTree, code, 8, i);
        code++;
    }

    code = 0x190;
    for(int i = 144; i < 256; i++)
    {
        insertCode(huffmanTree, code, 9, i);
        code++;
    }

    code = 0x00;
    for(int i = 256; i < 280; i++)
    {
        insertCode(huffmanTree, code, 7, i);
        code++;
    }

    code = 0xC0;
    for(int i = 280; i <= 287; i++)
    {
        insertCode(huffmanTree, code, 8, i);
        code++;
    }

    // construct distance tree
    node *distanceHuffman = createNode(); 

    for(int i = 0; i < 32; i++)
    {
        insertCode(distanceHuffman, i, 5, i);
    }

    int newcode = 0;            

    while (newcode != 256)
    {		
        newcode = search(input, huffmanTree);		                    
                
        if (newcode == -1)
        {
                LOG(ERROR,"lost the code \n");
                return;
        }
        else if (newcode < 256)
        {
            outputStream[(*outputLength)++] = newcode;
        }
        else if(newcode > 285)
        {
            LOG(ERROR, "Ivalid code found\n");
        }
        else if (newcode > 256)
        {
            int btr = 0;
            int check = 0;
            uint32_t length, dlength = 0;
            
            int lcode = newcode - 257;
            btr = literal_extra_bit_table[lcode];
            length  = read_bits_little_endian(input, btr);

            LOG(DEBUG_3,"Copying %d\n", length + literal_offset_table[lcode]);
            
            int dist = search(input, distanceHuffman);
            check = distance_extra_bit_table[dist]; 
            dlength = read_bits_little_endian(input, check);
            
            LOG(DEBUG_3,"Going back %d\n",  dlength + distance_offset_table[dist]);
        
            int startingPos = *outputLength;
       
            if((dlength + distance_offset_table[dist]) > startingPos){
                LOG(ERROR, "Distance code too large\n"); 
            }
           LOG(DEBUG_3, "output length %d\n", *outputLength); 
            for(int i=0; i < length + literal_offset_table[lcode]; i++)
            {
                outputStream[(*outputLength)++] = outputStream[startingPos - (dlength + distance_offset_table[dist]) + i];
            }        
        }
    }
    freeHuffman(huffmanTree);
    freeHuffman(distanceHuffman);
}


void z_compressed_dynamic(bitstream_t *input, unsigned char* outputStream, uint32_t *outputLength)
{

    const uint32_t codedict[19]                 =  {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    const uint32_t literal_extra_bit_table[29]  =  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    const uint32_t literal_offset_table[29]     =  {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    const uint32_t distance_extra_bit_table[30] =  {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
    const uint32_t distance_offset_table[30]    =  {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24557};
    uint32_t codes[19]                          =  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    uint32_t *z_codes;
    uint32_t z_num_codes_literal = 257 + read_bits_little_endian(input, 5);
    uint32_t z_num_codes_distance = 1 + read_bits_little_endian(input, 5);
    uint32_t z_num_codes_huffman = 4 + read_bits_little_endian(input, 4);

    LOG(INFO, "literal codes %d\n", z_num_codes_literal);
    LOG(INFO, "distance codes %d\n", z_num_codes_distance);
    LOG(INFO, "huffman codes %d\n", z_num_codes_huffman);     

    for (int i = 0; i < z_num_codes_huffman; i++)
    {
        codes[i] = read_bits_little_endian(input, 3);
        LOG(DEBUG_1, "num %d code %d\n", codedict[i], codes[i]);
    }

    
    //resort the codes
    uint32_t sorted[19];
    memset(sorted, -1, 19 * sizeof(uint32_t)); 
    
    for(int i = 0; i < 19; i++)
    {
        sorted[codedict[i]] = codes[i];
    }

    node *huffmantree = constructHuffman(sorted, 19, 8);

    uint32_t code;    
    uint8_t extra = 0;
    uint32_t index = 0;	

    z_codes = (uint32_t *)malloc((z_num_codes_distance + z_num_codes_literal) * sizeof(uint32_t));	
    
    while(index < z_num_codes_literal + z_num_codes_distance)
    {
        code = search(input, huffmantree);
        z_codes[index] = code;
        
        if (code >= 16)
        {
            if (code == 16)
            {
                extra = read_bits_little_endian(input, 2);
                uint32_t prev = z_codes[index - 1];
                for (int j = 0; j < 3 + extra; j++)
                {
                        z_codes[index++] = prev;
                }                		
            }
            else if (code == 17)
            {				
                    extra = read_bits_little_endian(input, 3);
                    
                    for (int j = 0; j < 3 + extra; j++)
                    {
                            z_codes[index++] = 0;
                    }
            }
            else if (code == 18)
            {
                    extra = (read_bits_little_endian(input, 7));

                    for (int j = 0; j < 11 + extra; j++)
                    {
                            z_codes[index++] = 0;
                    }
            }
        }

        else
        {
            index++;
        }
    }
    
    node *len_huffman = constructHuffman(z_codes, z_num_codes_literal , 16);    
    node *len_distance = constructHuffman(z_codes + z_num_codes_literal, z_num_codes_distance, 16);

    int newcode = 0;            

    while (newcode != 256)
    {		
        newcode = search(input, len_huffman);		                    
        
        if (newcode == -1)
        {
                LOG(ERROR,"lost the code \n");
                return;
        }
        else if (newcode < 256)
        {
            outputStream[(*outputLength)++] = newcode;
            LOG(DEBUG_3, "%d %d lit\n",*outputLength,   newcode );
        }
        else if (newcode > 285)
        {
            LOG(ERROR, "Invalid code found\n");
        }
        else if (newcode > 256)
        {
            uint32_t length, distance = 0;
            uint32_t dextra, lextra, dcode, lcode; 
            
            lcode = newcode - 257;
            lextra  = read_bits_little_endian(input, literal_extra_bit_table[lcode]);
            length = literal_offset_table[lcode] + lextra; 

            dcode = search(input, len_distance);
           
            int btr =  distance_extra_bit_table[dcode];

            dextra = read_bits_little_endian(input, distance_extra_bit_table[dcode]);
            
            distance = distance_offset_table[dcode] + dextra;

            LOG(DEBUG_3,"dcode %d Going back: %d + %d.lcode %d Copying: %d + %d\n", dcode, distance_offset_table[dcode],
            dextra,lcode,  literal_offset_table[lcode], lextra );

            if(distance > *outputLength){
                LOG(ERROR, "Distance code too large\n"); 
            }

            for(int i=0; i < length; i++)
            {
                LOG(DEBUG_3, "%d %d %d\n",*outputLength, *outputLength - distance,
                outputStream[*outputLength - distance]);
                outputStream[*outputLength] = outputStream[*outputLength - distance];
                (*outputLength)++;
            }        
        }
    }

    free(z_codes);
    freeHuffman(len_huffman);
    freeHuffman(len_distance);
    freeHuffman(huffmantree);

}

uint32_t z_readADLER32(bitstream_t *input)
{
    // read adler32
    // adler32 on next byte boundary as per the man himself. 
    // https://stackoverflow.com/questions/64814734/how-does-the-compressed-data-end-at-the-byte-boundary-in-zlib-data-format
    
    if(input->bit_offset != 0){
            input->bit_offset = 0;
            input->byte_offset++;
    }

    uint32_t adler = read_bits_big_endian(input, 32);

    return adler;
}
