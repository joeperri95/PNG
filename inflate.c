#include "inflate.h"

zlibMetaData z_processHeader(bitstream_t *input)
{
    zlibMetaData z_data;
    z_data.compression_method = read_bits(input, 4);
    z_data.compression_info = 0x01 << (read_bits(input, 4) + 0x08);
    z_data.fcheck = read_bits(input, 5);
    z_data.fdict = read_bits(input, 1);
    z_data.flevel = read_bits(input, 2);

    return z_data;
}

void z_inflate(bitstream_t *input, unsigned char *outputStream)
{
    uint8_t z_final = 0;
    uint8_t z_compression_type;
    uint32_t outputLength = 0;

     while(! z_final){
            
            z_final = read_bits(input, 1);
            z_compression_type = read_bits(input , 2);

            LOG(INFO, "final block %d\n", z_final);
            LOG(INFO, "compression type %d\n", z_compression_type);
            switch(z_compression_type)
            {
                case 0x00:
                    z_uncompressed(input, outputStream, &outputLength);
                    break;
                case 0x01:
                    z_compressed_fixed(input, outputStream, &outputLength);
                    break;
                case 0x02:
                    z_compressed_dynamic(input, outputStream, &outputLength);
                    break;
                default:
                    LOG(ERROR, "invalid compression type %d\n",z_compression_type);
                    return 0;
                    break;
            };
     }
}

void z_uncompressed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength)
{
    // Not implemented
    LOG(ERROR, "not yet implemented\n");
    return;

    input->bit_offset = 0;
    input->byte_offset++;

    uint32_t len = read_bits(input, 8);
    len = (len << 0x08) + read_bits(input, 8);

    for(int i = 0; i < len + 2; i++)
    {
        read_bits(input,8);
    }
}

void z_compressed_fixed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength)
{
    // Not implemented
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
    uint32_t z_num_codes_literal = 257 + read_bits(input, 5);
    uint32_t z_num_codes_distance = 1 + read_bits(input, 5);
    uint32_t z_num_codes_huffman = 4 + read_bits(input, 4);

    LOG(INFO, "literal codes %d\n", z_num_codes_literal);
    LOG(INFO, "distance codes %d\n", z_num_codes_distance);
    LOG(INFO, "huffman codes %d\n", z_num_codes_huffman);     

    for (int i = 0; i < z_num_codes_huffman; i++)
    {
        codes[i] = read_bits(input, 3);
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
                extra = read_bits(input, 2);
                uint32_t prev = z_codes[index - 1];
                for (int j = 0; j < 3 + extra; j++)
                {
                        z_codes[index++] = prev;
                }                		
            }
            else if (code == 17)
            {				
                    extra = read_bits(input, 3);
                    
                    for (int j = 0; j < 3 + extra; j++)
                    {
                            z_codes[index++] = 0;
                    }
            }
            else if (code == 18)
            {
                    extra = (read_bits(input, 7));

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

    node *len_huffman = constructHuffman(z_codes, z_num_codes_literal, 16);
    node *len_distance = constructHuffman(z_codes + z_num_codes_literal, z_num_codes_distance, 16);

    int newcode = 0;            

    while (newcode != 256)
    {		
        newcode = search(input, len_huffman);		                    
        
        if (newcode == -1)
        {
                LOG(ERROR,"lost the code \n");
                return -1;
        }
        else if (newcode < 256)
        {
            outputStream[(*outputLength)++] = newcode;
        }
        else if (newcode > 256)
        {
            int btr = 0;
            int check = 0;
            uint32_t length, dlength = 0;
            
            int lcode = newcode - 257;
            btr = literal_extra_bit_table[lcode];
            length  = (read_bits(input, btr));

            LOG(DEBUG_3,"Copying %d\n", length + literal_offset_table[lcode]);
            
            int dist = search(input, len_distance);
            check = distance_extra_bit_table[dist]; 

            dlength = read_bits(input, check);
            LOG(DEBUG_3,"Going back %d\n",  dlength + distance_offset_table[dist]);

            int startingPos = *outputLength;
            for(int i=0; i < length + literal_offset_table[lcode]; i++)
            {
                outputStream[(*outputLength)++] = outputStream[startingPos - (dlength + distance_offset_table[dist]) + i];
            }        
        }
    }

    free(z_codes);
    freeHuffman(len_huffman);
    freeHuffman(len_distance);
    freeHuffman(huffmantree);

}
