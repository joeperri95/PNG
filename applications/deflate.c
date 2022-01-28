#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "fileutils.h"

#define WINDOW_SIZE 32767 
#define SEARCH_SIZE 3
#define SYMBOLS 286

uint32_t histogram[SYMBOLS];

typedef struct _match{
	uint32_t distance;
	uint32_t length;
	uint8_t literal;
} match;

match find_matches(uint8_t *buffer, uint32_t start_window, uint32_t start_search)
{
	int max_len = 0;
	int distance = 0;
	
	for(int i = start_window; i < start_search; i++)
	{
		int j = 0;
		while(buffer[i + j] == buffer[start_search + j])
		{
			j++;
		}	

		// match found	
		if(j >= 3)
		{
		    if(j > max_len){
				max_len = j;
				distance = start_search - i;
			}	
		}	
	}
	if(max_len != 0)	
		LOG(glog, INFO, "Found best match of length %d at distance %d\n", max_len, distance);

	match result = {
		.distance = distance,
		.length = max_len,
		.literal = buffer[start_search], 
	}; 

	return result;
}

int16_t find_code(uint32_t length)
{
    if(length < 3)
    {
        LOG(glog, WARNING, "Can not have length less than 3\n");
    }
    else if(length > 285)
    {
        LOG(glog, WARNING, "Can not have length greater than 285\n");
    }
    else
    {
#define NUM_CODES 28
        int lengths[NUM_CODES] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
        int ret = 257;

        for(int i = 0; i < NUM_CODES; i++)
        {
            if(length == lengths[i])
            {
                return 257 + i;
            }
            else if (length > lengths[i] && length < lengths[i + 1])
            {
                return 257 + i;
            }
        }
    }

    return -1;
}


struct tree_node
{
    struct tree_node *left;
    struct tree_node *right;
    int data;
};

struct tree
{
    struct tree_node *root;
    int value;
};

void gen_huffman(uint32_t *frequencies)
{
    struct tree_node root = 
    {
        .left = NULL,
        .right = NULL,
        .data = 0,
    };
    struct tree t = 
    {
        .root = &root,
        .value = 0,
    };

    for(int i = 0; i < SYMBOLS; i++)
    {
    }

}

int main(int argc, char *argv[])
{
	glog = create_logger("/dev/stdout", DEBUG_3);

	char filename[1024];
	const char *outfilename = "output.z";
	
	if (argc > 1){
		strcpy(filename, argv[1]);
	}
	else
	{
		LOG(glog, INFO, "please supply a filename\n");
		exit(EXIT_FAILURE);
	}

	FILE *output = fopen(outfilename, "w");
	uint8_t *file_buffer;
	int64_t file_size = get_file_length(filename);
	read_file_to_buffer(filename, &file_buffer);

	uint32_t index = 0;
	uint32_t window_start = 0;
	match next;	
    memset(histogram, 0, SYMBOLS);
    histogram[256] = 1; // block end symbol

	while(index < file_size){
		LOG(glog, DEBUG_3, "search buffer %.*s window buffer %.*s\n", SEARCH_SIZE, file_buffer + index, index - window_start, file_buffer + window_start);			
		next = find_matches(file_buffer, window_start, index);	
		if(next.length > 0)
		{
			index += next.length;
            histogram[find_code(next.length)]++;
			// just to illustrate the length, distance pair
			fprintf(output, "<%d,%d>",next.distance, next.length);
		}
		else
		{
			index++;
            histogram[next.literal]++;
			fprintf(output, "%c",next.literal);
		}		

		if(index >= WINDOW_SIZE)
		{
			window_start = index - WINDOW_SIZE;
		}
	}
	fclose(output);

    for(int i = 0; i < SYMBOLS; i++)
    {
        if(histogram[i] > 0)
        LOG(glog, DEBUG_2, "symbol %d value %d\n", i, histogram[i]);
    }
    
    gen_huffman(histogram);

	return 0;
}
