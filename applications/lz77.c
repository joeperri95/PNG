#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "fileutils.h"

#define WINDOW_SIZE 5
#define SEARCH_SIZE 3

void find_matches(uint8_t *buffer, uint32_t start_window, uint32_t start_search, uint8_t size)
{
	char *buff = malloc(size + 1);
	buff[size] = '\0';
	char *search_buff = malloc(size + 1);
	search_buff[size] = '\0';
	strncpy(search_buff, buffer + start_search, size);

	for(int i = start_window; i < start_search; i++)
{
		strncpy(buff, buffer + i, size);
		if(strcmp(buff, search_buff) == 0)
		{
			LOG(glog, INFO, "Found a match at %d\n", i);
		}	
	}	

	free(buff);
	free(search_buff);
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

	uint8_t *file_buffer;
	int64_t file_size = get_file_length(filename);
	read_file_to_buffer(filename, &file_buffer);

	uint32_t index = 0;
	uint32_t window_start = 0;
	uint8_t search_buffer[SEARCH_SIZE + 1];
	uint8_t window_buffer[WINDOW_SIZE + 1];
	
	memset(window_buffer, 0, WINDOW_SIZE + 1);
	memset(search_buffer, 0, SEARCH_SIZE + 1);

	while(index + SEARCH_SIZE < file_size){
		index++;
		LOG(glog, DEBUG_3, "search buffer %.*s window buffer %.*s\n", SEARCH_SIZE, file_buffer + index, index - window_start, file_buffer + window_start);			
		find_matches(file_buffer, window_start, index, SEARCH_SIZE);	
		if(index >= WINDOW_SIZE)
		{
			window_start++;
		}
	}

	return 0;
}
