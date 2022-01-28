#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "fileutils.h"

#define WINDOW_SIZE 32767 

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

	while(index < file_size){
		next = find_matches(file_buffer, window_start, index);	
		if(next.length > 0)
		{
			index += next.length;
			// just to illustrate the length, distance pair
			fprintf(output, "<%d,%d>",next.distance, next.length);
		}
		else
		{
			index++;
			fprintf(output, "%c",next.literal);
		}		

		if(index >= WINDOW_SIZE)
		{
			window_start = index - WINDOW_SIZE;
		}
	}
	fclose(output);
	return 0;
}
