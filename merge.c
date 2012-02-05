#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_lib.h"
#include "util.h"

#define MILLSEC_TO_SEC .001

void print_delay(int pause, int bit_size);
void print_merge_header(int sample_freq, int mono_stereo, int bit_size);

/**
 * This program accepts any number of files and merges them together.  Since this program is
 * only meant to be used as a helper to the gendtmf2.sh shell script, I'm assuming that all 
 * files have the same sample rate, bit-size, and channels.
 */
int main(int argc, char *argv[])
{
	int i, num_files, inp_error;
	unsigned value;
    int pause = atoi(argv[1]);
    
    FileInfoPtr *input_info;		/*Pointer to pointer to file_info struct*/
	FILE **files;
    
    if(argc < 3)
    {
    		fprintf(stderr, "You must supply the pause duration ");
    		fprintf(stderr, "and at least 1 file name.\n");
    		return 1;
    }
    
    num_files = (argc - 2);	/*-2 gets rid of command input and pause length*/	
	input_info = (FileInfoPtr*)malloc(num_files * sizeof(FileInfoPtr));
	for(i=0; i<num_files; i++)
	{
		input_info[i] = (FileInfoPtr)malloc(sizeof(FileInfo));
	}	
	files = (FILE**)malloc(num_files * sizeof(FILE*));
    /*Open all the files and parse each header to get the file pointer to the desired point*/
	for(i=0; i<num_files; i++)
	{
		strncpy(input_info[i]->file_name, argv[2 + i], MAX_FILE_NAME_LEN);
		if((files[i] = fopen(argv[2 + i], "r")) == NULL)
		{
			fprintf(stderr, "Cannot open: %s\n", argv[2 + i]);
			/*Free all memory*/
			for(i=0; i<num_files; i++)
			{
				free(input_info[i]);
			}
			free(input_info);
			free(files);
			return 1;
		}
		if((inp_error = parse_header(files[i], input_info[i], NONE)) != 0)
		{
			/*Free all memory*/
			for(i=0; i<num_files; i++)
			{
				free(input_info[i]);
			}
			free(input_info);
			free(files);
			return inp_error;
		}
	}
    
    /*Print the header from the first file, since we know they will all have the same info*/
    print_merge_header(input_info[0]->frequency, input_info[0]->mono_or_stereo, 
    					input_info[0]->bit_size);
	
	/*Convert the "pause" variable to samples rather than ms*/
	pause = pause*MILLSEC_TO_SEC*(input_info[0]->frequency);
	    					
    	/*Print the data from the first file*/
    	while((value = getc(files[0]))!= EOF)
    	{
    		putc(value, stdout);
    	}
    	
    	/*Loop through the remaining files, print the delay first, then the file.*/
    	/*Already handled the first file*/
    for(i=1; i<num_files; i++)
    {
    		print_delay(pause, input_info[i]->bit_size);
    		
    		/*Print the data from the file*/
    		while((value = getc(files[i]))!= EOF)
    		{
    			putc(value, stdout);
    		}
    }
    
    /*Close all files*/
    for(i=0; i<num_files; i++)
	{
		fclose(files[i]);
	}
    
    /*Free all memory*/
	for(i=0; i<num_files; i++)
	{
		free(input_info[i]);
	}
	free(input_info);
	free(files);
    
    return 0;
}

/*
 * Prints the header information.  This is specific to this merge program since the number of
 * samples will not be put in the header.
 */
void print_merge_header(int sample_freq, int mono_stereo, int bit_size)
{
	printf("Header\n");
	printf("FREQUENCY %d\n", sample_freq);
	if(mono_stereo == MONO)
	{
		printf("CHANNELS MONO\n");
	}
	else
	{
		printf("CHANNELS STEREO\n");
	}
	printf("SAMPLEBITS %d\n", bit_size);
	printf("EndHeader\n");
}

/*
 * Prints a string of 0's for the delay in between files
 */
void print_delay(int pause, int bit_size)
{
	int i;
	for(i=0; i<pause; i++)
	{
		output_sample(stdout, 0, bit_size);
	}
}
