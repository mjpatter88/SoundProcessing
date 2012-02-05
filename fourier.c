#include "fourier.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "input_lib.h"

/**
 * Used with parts H, and I
 * This function gets the number of samples from the file specified by fileName starting at 
 * start and returns them.  
 * Note: start is the starting percent, from 0 to 100, not the starting sample number.
 */
int* get_samples(int number, int start, const char* fileName)
{
	unsigned value;
	int i, end_of_file, bytes_per_sample;
	int bytes_read = 0;
	int* samples;
	FileInfoPtr input_info;
	FILE* inp_file;
	int opened_file = 0;
	unsigned starting_sample;
	
	/*Plus 1 is to create the last element which will hold the sample rate*/
	samples = (int*)malloc(sizeof(int)*(number + 1));
	
	/*Get the file for input.  If the string passed is "stdin" then it's handled as standard input*/
	if(strncmp(fileName, "stdin", 5) == 0)
	{
		inp_file = stdin;
	}
	else
	{
		inp_file = fopen(fileName, "r");
		if(inp_file == NULL)
		{
			fprintf(stderr, "Could not open file %s.\n", fileName);
			return NULL;
		}
		opened_file = 1;
	}
	
	/*Parse the header from the input*/
	input_info = (FileInfoPtr)malloc(sizeof(FileInfo));
	parse_header(inp_file, input_info, NONE);
	
	/*Since the starting sample is coming in as a percent, we need to know the number of samples*/
	/*If we don't get this from the header, we need to parse the file to get it, unless number is 0*/
	if((start != 0) && (input_info->num_samples == 0))
	{
		parse_file(inp_file, input_info, NONE);
		starting_sample = (start * input_info->num_samples) / 100;
		/*Make sure there are still enough samples left in the file*/
		if(input_info->num_samples - starting_sample < number)
		{
			starting_sample = input_info->num_samples - number;
		}
		rewind(inp_file);
		parse_header(inp_file, input_info, NONE);
	}
	else
	{
		if(start == 0)
		{
			starting_sample = 0;
		}
		else
		{
			starting_sample = (start * input_info->num_samples) / 100;
			/*Make sure there are still enough samples left in the file*/
			if(input_info->num_samples - starting_sample < number)
			{
				starting_sample = input_info->num_samples - number;
			}
		}
	}
	
	/*Make sure the info provided in the header describes a legal file for this program
	We can't really return -1, since we are returning an array, but at least we will
	print out an error message.*/
	check_input(input_info);
	
	/*Read the number of samples into the array, or until the file ends*/
	end_of_file = 0;
	bytes_per_sample = input_info->bit_size / 8;
	i=0;
	while(end_of_file == 0)
	{
		if((bytes_read = read_sample(inp_file, input_info->bit_size, &value)) != 
															bytes_per_sample)
		{
			end_of_file = 1;
		}
		else
		{
			/*Only add to the array if we are far enough along in the file.*/
			if(i >= starting_sample)
			{
				samples[i - starting_sample] = value;
			}
		}
		
		i++;
		
		if(i >= (number + starting_sample))
		{
			/*We only want to read in "number" of samples even if the input is longer*/
			end_of_file = 1;
		}
	}
	
	/*If i is not equal to the number of samples we wanted to read, 
	then the file must have been shorter so we print an error*/
	if(i != (number + starting_sample))
	{
		fprintf(stderr, "The file did not contain enough samples. %d\n", i);
	}
	samples[number] = input_info->frequency;
	
	if(opened_file == 1)
	{
		fclose(inp_file);
	}
	
	return samples;

}

/**
 * Used with parts H, I and J.
 * Calculates the fourier transform of the samples provided for the k value provided.  Returns
 * the real and imaginary results in an array with two elements.
 */
double* calc_fourier(int* samples, int window_size, int k)
{
	double a, b;
	int n;
	double* result = (double*)malloc(sizeof(double)*2);
	
	a=0;
	b=0;
	for(n=0; n<window_size; n++)
	{
		a = a + ((unsigned)samples[n])*(cos((2.0*M_PI*n*k)/window_size));
		b = b + ((unsigned)samples[n])*(sin((2.0*M_PI*n*k)/window_size));
	}
	
	result[0] = a;
	result[1] = b; 
	
	return result;
}


/**
 * Check that the sound sample is mono.
 */
int check_input(FileInfoPtr file_info)
{
	if(file_info->mono_or_stereo != MONO)
	{
		fprintf(stderr, "This program only handles MONO samples.\n");
		return 1;
	}
	return 0;
}

/**
 * Used with part J.
 * Gets the data samples from standard in.  I was unable to reuse the code from the "get_samples"
 * function above because this gets the samples from standard in and not a file.  It can't
 * move around the file in the same way, so it always returns the first "number" of samples.
 */
unsigned* get_samples_stdin(int number, int bit_size)
{
	int end_of_file;
	int bytes_per_sample;
	unsigned value;
	unsigned *samples;
	int i, bytes_read;
	
	/*Plus 1 is to create the last element which will hold the sample rate*/
	samples = (unsigned*)malloc(sizeof(int)*(number + 1));
	
	/*Read the number of samples into the buffer, or until the file ends*/
	end_of_file = 0;
	bytes_per_sample = bit_size / 8;
	i=0;
	while(end_of_file == 0)
	{
		if((bytes_read = read_sample(stdin, bit_size, &value)) != 
															bytes_per_sample)
		{
			end_of_file = 1;
		}
		else
		{
			samples[i] = value;
		}
		
		i++;
		
		if(i >= (number))
		{
			/*We only want to read in "number" of samples even if the input is longer*/
			end_of_file = 1;
		}
	}
	
	/*If i is not equal to the number of samples we wanted to read, 
	then the file must have been shorter so we print an error*/
	if(i != (number))
	{
		free(samples);
		return NULL;
	}
	/*Memory can't be freed since we are returning the pointer to it, but whoever calls
	this function will free it when they are done with it.*/
	return samples;
}

