#include "input_lib.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int check_input(int argc, char *argv[]);
int check_file_info(FileInfoPtr input_info[], int num_files);
void mix_files(FileInfoPtr input_info[], FILE **files, int num_files);

/**
 * Part E: Mix
 * This program takes a list of files and a list of relative gains.  It mixes them together
 * and then scales them so the maximum value output is 0.9*(max possible).
 * 
 * Command Line Variables: mix sound1 2.0 sound2 1.5 sound3 3.0 sound4 4.0
 * Return Values: 0 - Success; 1 - Failure (error written to stderr)
 */
int main(int argc, char *argv[])
{
	int num_files;
	int inp_error;
	int i;
	FileInfoPtr *input_info;		/*Pointer to pointer to file_info struct*/
	FILE **files;
	
	if((inp_error = check_input(argc, argv)) != 0)
	{
		return inp_error;
	}
	
	num_files = (argc-1)/2;	/*-1 gets rid of command input, 2 args per file*/	
	input_info = (FileInfoPtr*)malloc(num_files * sizeof(FileInfoPtr));
	for(i=0; i<num_files; i++)
	{
		input_info[i] = (FileInfoPtr)malloc(sizeof(FileInfo));
	}	
	files = (FILE**)malloc(num_files * sizeof(FILE*));	
	
	/*Open all the files and set the relative gains in the file_info structs*/
	for(i=0; i<num_files; i++)
	{
		strncpy(input_info[i]->file_name, argv[1 +2*i], MAX_FILE_NAME_LEN);
		if((files[i] = fopen(argv[1 + 2*i], "r")) == NULL)
		{
			fprintf(stderr, "Cannot open: %s\n", argv[1 +2*i]);
			/*Free all the needed memory*/
			for(i=0; i<num_files; i++)
			{
				free(input_info[i]);
			}
			free(input_info);
			free(files);
			return 1;
		}
		input_info[i]->rel_gain = atof(argv[2 +2*i]);
	}
	
	/*call handle_input on all files, make sure no errors occur*/
	for(i=0; i<num_files; i++)
	{
		if((inp_error = handle_input(files[i], input_info[i], NONE)) != 0)
		{
			/*Free all the needed memory*/
			for(i=0; i<num_files; i++)
			{
				free(input_info[i]);
			}
			free(input_info);
			free(files);
			return inp_error;
		}
	}
	
	/*using the info in the file_info structs, make sure everything is legal*/
	if((inp_error = check_file_info(input_info, num_files)) != 0)
	{
		/*Free all the needed memory*/
		for(i=0; i<num_files; i++)
		{
			free(input_info[i]);
		}
		free(input_info);
		free(files);
		return inp_error;	
	}
	
	/*Print out the header info (should be able to use any one of the structs)*/
	print_header(input_info[0]->frequency, input_info[0]->num_samples,
					input_info[0]->mono_or_stereo, input_info[0]->bit_size);
	
	/*Reset the file pointers to the beginning of the  data in the files*/
	for(i=0; i<num_files; i++)
	{
		/*Reset the file pointers to the beginning of the files*/
		rewind(files[i]);
		
		/*Call parse_header on each file to get the file pointers to the data*/
		parse_header(files[i], input_info[i], NONE);
	}
	
	/*Call the mix_files function in this file with the file pointers and the relative gains
	this function should call the "read_sample" and "output_sample" functions in util.h*/
	mix_files(input_info, files, num_files);
	
	/*Close all the files*/
	for(i=0; i<num_files; i++)
	{
		fclose(files[i]);
	}
	/*Free all the needed memory*/
	for(i=0; i<num_files; i++)
	{
		free(input_info[i]);
	}
	free(input_info);
	free(files);
	
	return 0;
}

/**
 * Makes sure that the command line arguments are legal.  Returns 0 if they are legal and 1 if
 * they are not.
 */
int check_input(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "You must provide at least one file and gain.\n");
		return 1;
	}
	if((argc-1)%2 != 0)
	{
		fprintf(stderr, "You must provide an even number of parameters.\n");
		return 1;
	}
	return 0;
}

/**
 * Makes sure that all the files are valid and can be mixed together.  Returns 0 if they are
 * legal and 1 if they are not.
 */
int check_file_info(FileInfoPtr input_info[], int num_files)
{
	int i;
	int m_s;
	int bit_size;
	int sample_rate;
	int num_samples;
	
	/*Use the properties of the first file to check that all the other files are the same*/
	m_s = input_info[0]->mono_or_stereo;
	bit_size = input_info[0]->bit_size;
	sample_rate = input_info[0]->frequency;
	num_samples = input_info[0]->num_samples;
	
	/*Loop through each of the remaining files*/
	for(i=1; i<num_files; i++)
	{
		/*Check that all files are either MONO or STEREO, not a mix of both*/
		if(input_info[i]->mono_or_stereo != m_s)
		{
			fprintf(stderr, "All files must be either MONO or STEREO.\n");
			return 1;
		}
		
		/*Check that all files have the same sample frequency*/
		if(input_info[i]->frequency != sample_rate)
		{
			fprintf(stderr, "All input files must have the same sample rate.\n");
			return 1;
		}
		
		/*Check that all files have the same bit-size*/
		if(input_info[i]->bit_size != bit_size)
		{
			fprintf(stderr, "All files must have the same bit-size.\n");
			return 1;
		}
		
		/*Check that all files have the same number of samples*/
		if(input_info[i]->num_samples != num_samples)
		{
			fprintf(stderr, "All files must have the same number of samples.\n");
			return 1;
		}
	}
	return 0;
}

/**
 * Does the actual mixing of the files as described in the description of this whole program.
 * Outputs the final values to standard out.
 * Note: Jim said that it was ok to ignore overflow in this case, so that's what I've done.
 */
void mix_files(FileInfoPtr input_info[], FILE **files, int num_files)
{
	unsigned num_samples = input_info[0]->num_samples;
	unsigned *samples;
	unsigned i;
	unsigned j;
	unsigned cur_sample;
	unsigned to_add;
	unsigned max_sample;
	double scale_factor;
	
	samples = (unsigned*)malloc(num_samples * sizeof(unsigned));
	
	/*Fill the array with the weighted sums*/
	for(i=0; i<num_samples; i++)
	{
		cur_sample = 0;
		for(j=0; j<num_files; j++)
		{
			read_sample(files[j], input_info[j]->bit_size, &to_add);
			cur_sample += (int)(to_add*input_info[j]->rel_gain);
		}
		samples[i] = cur_sample;
	}
	
	/*Find the max and use this to find the scaling factor*/
	max_sample = 0;
	for(i=0; i<num_samples; i++)
	{
		if(max_sample < samples[i])
		{
			max_sample = samples[i];
		}
	}
	scale_factor = ((pow(2, input_info[0]->bit_size))-1) / (double)max_sample;
	scale_factor = scale_factor * 0.9;
	
	/*Scale the whole array*/
	for(i=0; i<num_samples; i++)
	{
		samples[i] = (unsigned)(samples[i] * scale_factor);
		
		/*Output the value*/
		output_sample(stdout, samples[i], input_info[0]->bit_size);
	}
	
	/*Release the memory used for the array*/
	free(samples);
	return;
}
