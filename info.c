#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_lib.h"

/**
 * Part B: Info
 * Checks if a sound file provided is in the proper format.  If it is, various pieces of
 * information about the file are output.  A filename can be provided as a command line
 * parameter, but if it is not, then the input is taken from standard input.
 *
 * Command Line Variables: file name (optional)
 * Return Values: 0 - Success; 1 - Failure (error written to stderr)
 */
int main(int argc, char *argv[])
{
	char filename[MAX_FILE_NAME_LEN];
	FILE *inp;
	FileInfoPtr info = (FileInfoPtr)malloc(sizeof(FileInfo));
	int ret_val;
	
	if(argc > 1)
	{
		strncpy(filename, argv[1], MAX_FILE_NAME_LEN);
		if((inp = fopen(filename, "r")) == NULL)
		{
			fprintf(stderr, "Cannot open: %s\n", filename);
			free(info);
			return 1;
		}
		strncpy(info->file_name, filename, MAX_FILE_NAME_LEN);
	}
	else
	{
		inp = stdin;
		strncpy(info->file_name, "stdin", 5);
	}
	/*The handle_input call does most of the work in checking data format etc.*/
	ret_val = handle_input(inp, info, NONE);
	/*Output the required information about the file if the file is valid*/
	if(ret_val == 0)
	{
		/*It was said on WebCT that we shouldn't output a filename if input is stdin.*/
		if(strncmp(info->file_name, "stdin", 5) != 0)
		{
			printf("Filename: %s\n", info->file_name);
		}
		
		printf("Frequency: %d\n", info->frequency);
		if(info->mono_or_stereo == MONO)
		{
			printf("Channels: MONO\n");
		}
		else
		{
			printf("Channels: STEREO\n");
		}
		printf("Bits per sample: %d\n", info->bit_size);
		printf("Number of samples: %d\n", info->num_samples);
	}
	
	/*Close the file if needed*/
	if(argc > 1)
	{
		fclose(inp);
	}
	free(info);
	return ret_val;
}
