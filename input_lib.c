#include "input_lib.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

#define MAX_LINE_LENGTH 200

/**
 * Handles the input from a given file.  Information about the file is stored in the struct
 * pointed to by "file_info".  If this information is not needed by the caller, it can safely
 * be passed as NULL.  The options argument allows this function to be used by the "split",
 * "combine", and "reverb" programs also.
 * options : 0-none; 1-split; 2-combine
 */
int handle_input(FILE *inp, FileInfoPtr file_info, int options)
{
	int delete_struct = 0;
	
	if(file_info == NULL)
	{
		file_info = (FileInfoPtr)malloc(sizeof(FileInfo));
		/*We want to free this memory before returning to the caller.*/
		delete_struct = 1;
	}
	
	if(parse_header(inp, file_info, options) != 0)
	{
		if(delete_struct != 0)
		{
			free(file_info);
		}
		return 1;
	}
	if(parse_file(inp, file_info, options) != 0)
	{
		if(delete_struct != 0)
		{
			free(file_info);
		}
		return 1;
	}
	
	if(delete_struct != 0)
	{
		free(file_info);
	}
	return 0;
}

/**
 * Parses through the header of the input.  It firsts looks for the keyword "header" which
 * is case insensitive.  Once found, it looks through the following fields for parameters.
 * Once "endheader" is found it returns, and leaves the file pointer pointing to the first
 * byte of data in the file.  Returns 0 if no format errors are found and 1 if they are.
 */
int parse_header(FILE *inp, FileInfoPtr file_info, int options)
{
	char next_char;
	char next_line[MAX_LINE_LENGTH];
	char next_line_cpy[MAX_LINE_LENGTH];
	char *header = "HEADER";
	int end_header_found = 0;
	int freq_set = 0;
	int bit_size_set = 0;
	int cur_index = 0;
	char *channels;
	char *next_keyword;
	
	/*Look for "header" keyword.  We must do this character by character because we don't
	know how many bytes will come before the word "header"*/
	while((next_char = getc(inp)) != EOF && cur_index < 6)
	{
		if(next_char == header[cur_index] || next_char == (header[cur_index] + 32))
		{
			cur_index++;
		}
		else
		{
			cur_index = 0;	/*not part of string, so restart*/
		}
		
		/*If SPLIT or COMBINE or REVERB we want to keep all header info*/
		if(options == SPLIT || options == COMBINE || options == REVERB)
		{
			putc(next_char, stdout);
		}
	}
	if(cur_index != 6)
	{
		fprintf(stderr, "The keyword \"header\" was not found in the file.\n");
		return 1;
	}
	/*If SPLIT or COMBINE, we need to print a newline after HEADER*/
	if(options == SPLIT || options == COMBINE || options == REVERB)
	{
		putc('\n', stdout);
	}
	
	/*By default channels is set to MONO*/
	file_info->mono_or_stereo = MONO;
	/*By default number of samples is 0*/
	file_info->num_samples = 0;
	
	/*Parse the rest of the header*/
	while(fgets(next_line, MAX_LINE_LENGTH, inp) != NULL)
	{
		strncpy(next_line_cpy, next_line, MAX_LINE_LENGTH);
		next_keyword = strtok(next_line, " ");
		/*remove newline*/
		if(next_line[strlen(next_line)-1]=='\n')
		{
			next_line[strlen(next_line)-1] = '\0';
		}
		
		/*If SPLIT or COMBINE or REVERB then we need to print out the header values*/
		if((options == SPLIT || options == COMBINE || options == REVERB) && 
								strncasecmp(next_keyword, "CHANNELS", 8) != 0)
		{
			printf("%s", next_line_cpy);
		}
		
		if(strncasecmp(next_line, "ENDHEADER", 9) == 0)
		{
			end_header_found = 1;
			break;
		}
		if(strncasecmp(next_keyword, "FREQUENCY", 9) == 0 &&
							strlen(next_keyword) == strlen("FREQUENCY"))
		{
			file_info->frequency = atoi(strtok(NULL, " "));
			freq_set = 1;
		}
		else if(strncasecmp(next_keyword, "SAMPLE", 6) == 0 &&
							strlen(next_keyword) == strlen("SAMPLE"))
		{
			file_info->num_samples = atoi(strtok(NULL, " "));
		}
		else if(strncasecmp(next_keyword, "SAMPLEBITS", 10) == 0 &&
							strlen(next_keyword) == strlen("SAMPLEBITS"))
		{
			file_info->bit_size = atoi(strtok(NULL, " "));
			bit_size_set = 1;
		}
		else if(strncasecmp(next_keyword, "CHANNELS", 8) == 0 &&
							strlen(next_keyword) == strlen("CHANNELS"))
		{
			channels = strtok(NULL, " ");
			if(strncmp(channels, "MONO", 4) == 0)
			{
				file_info->mono_or_stereo = MONO; 
			}
			else if(strncmp(channels, "STEREO", 5) == 0)
			{
				file_info->mono_or_stereo = STEREO;
			}
			else
			{
				fprintf(stderr, "Illegal value for \"CHANNELS\" in the header.\n");
				fprintf(stderr, "CHANNELS: %s\n", channels);
				return 1;
			}
			
			/*Print out the needed header info for split, combine and reverb (channels)*/
			if(options == SPLIT)
			{
				printf("CHANNELS STEREO\n");
			}
			else if(options == COMBINE)
			{
				printf("CHANNELS MONO\n");
			}
			else if(options == REVERB)
			{
				printf("%s", next_line_cpy);
			}
		}
		else
		{
			fprintf(stderr, "Illegal field in the header: \"%s\"\n", next_line);
			return 1;
		}
		
	}
	
	/*Check that all the necessary information was found in the header and is legal*/
	if(end_header_found == 0)
	{
		fprintf(stderr, "The keyword \"endheader\" was not found in the file.\n");
		return 1;
	}
	if(freq_set == 0)
	{
		fprintf(stderr, "FREQUENCY must be specified in the header.\n");
		return 1;
	}
	if(bit_size_set == 0)
	{
		fprintf(stderr, "SAMPLEBITS must be specified in the header.\n");
		return 1;
	}
	if(file_info->frequency < 0 || file_info->frequency >= INT_MAX)
	{
		fprintf(stderr, "The value for frequency is not a legal integer.\n");
		return 1;
	}
	if((file_info->bit_size != 8) && (file_info->bit_size != 16) && 
												(file_info->bit_size != 32))
	{
		fprintf(stderr, "The value for bit-size must be 8, 16, or 32\n");
		return 1;
	}
	return 0;
}

/**
 * Parses through the data portion of the input.  Behavior is very dependent on the options
 * argument.  The error checking at the end of the file is done in all situations.
 * If options = NONE (0): We just make sure the format is valid.
 * If options = SPLIT (1): Data must be MONO.  Outputs the stream in stereo format by 
 * duplicating the mono stream into stereo.
 * If options = COMBINE (2): Data must be STEREO.  Outputs a mono stream by averaging the two
 * stereo channels.
 */
int parse_file(FILE *inp, FileInfoPtr file_info, int options)
{
	unsigned value;
	unsigned value2;
	int bytes_per_sample;
	int num_bytes = 0;
	int num_samples = 0;
	int end_of_file = 0;
	
	int bytes_read = 0;
	int bytes_read2 = 0;
	
	/*Preference to work in bytes rather than bits*/
	bytes_per_sample = file_info->bit_size / 8;
	
	if(options == SPLIT)
	{
		if(file_info->mono_or_stereo != MONO)
		{
			fprintf(stderr, "The \"split\" program can only be run when a mono stream is input.\n");
			return 1;
		}
		while(end_of_file == 0)
		{
			if((bytes_read = read_sample(inp, file_info->bit_size, &value)) != 
																bytes_per_sample)
			{
				end_of_file = 1;
			}
			else
			{
				output_sample(stdout, value, file_info->bit_size);
				output_sample(stdout, value, file_info->bit_size);
			}
			num_bytes = num_bytes + bytes_read;
		}
	}
	else if(options == COMBINE)
	{
		if(file_info->mono_or_stereo != STEREO)
		{
			fprintf(stderr, "The \"combine\" program can only be run when a stereo stream is input.\n");
			return 1;
		}
		while(end_of_file == 0)
		{
			if((bytes_read = read_sample(inp, file_info->bit_size, &value)) != 
																bytes_per_sample)
			{
				end_of_file = 1;
			}
			else
			{
				if((bytes_read2 = read_sample(inp, file_info->bit_size, &value2)) != 
																bytes_per_sample)
				{
					end_of_file = 1;
				}
				else
				{
					output_sample(stdout, (int)(value+value2)/2, file_info->bit_size);
				}
				num_bytes = num_bytes + bytes_read2;
			}
			num_bytes = num_bytes + bytes_read;
		}
	}
	else		/*Just parse the file making sure format is valid*/
	{
		while((value = getc(inp)) != EOF)
		{
			num_bytes++;
		}
	}
	
	if(num_bytes % bytes_per_sample !=0)
	{
		fprintf(stderr, "The number of bytes of data must be evenly divisible by the bytes per sample.\n");
		return 1;
	}
	num_samples = num_bytes / bytes_per_sample;
	if(file_info->mono_or_stereo == STEREO)
	{
		if(num_samples % 2 != 0)
		{
			fprintf(stderr, "Stereo files must have an even number of samples");
			return 1;
		}
		/*Each sample consists of a pair when the channel type is stereo*/
		num_samples = num_samples / 2;
	}
	if(file_info->num_samples != 0 && num_samples != file_info->num_samples)
	{
		fprintf(stderr, "The number of samples in the file does not match the value in the header.");
		fprintf(stderr, "Header: %d\nFile: %d", 
					file_info->num_samples, num_samples);
		return 1;
	}
	/*If it hasn't already been set from the header, set this value now.*/
	file_info->num_samples = num_samples;
	return 0;
}
