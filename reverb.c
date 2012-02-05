#include <stdio.h>
#include <stdlib.h>
#include "input_lib.h"
#include "util.h"

#define MILLSEC_TO_SEC .001

void add_to_queue(unsigned *queue, unsigned value, int *beg_in, int *end_in, int size);
void pop_queue(unsigned *queue, unsigned *value_out, int *beg_in, int *end_in, int size);
void add_echo(unsigned *queue, unsigned echo, int index);
void print_queue(unsigned *queue, int size);

/*
 * Part K: Reverb
 * This program reads a sound sample file from standard input, adds the specified reverb
 * effects, and then outputs the resulting sound to standard output.
 *
 * Command Line Variables: "reverb delay1 attenuation1 delay2 attenuation2"
 *			delays are in microseconds, and attenuations are percents from 0 to 100
 * Return Values: 0 - Success; 1 - Failure
 */
int main(int argc, char *argv[])
{
	/*Delays are in microseconds, attenuations are percents from 0 to 100*/
	int delay1 = 0;
	int delay2 = 0;
	int percent1 = 0;
	int percent2 = 0;
	int max_delay;
	int bytes_per_sample, bytes_read, num_bytes, num_samples;
	int end_of_file = 0;
	int filling_queue = 1;
	int beg_in, end_in;
	unsigned value, value_out;
	unsigned *samples;
	FileInfoPtr file_info;
	int err_no, echo_index;
	int num_echoes = 1;
	
	
	if(argc != 3 && argc != 5)
	{
		fprintf(stderr, "You must specify either 2 or 4 parameters.\n");
		return 1;
	}
	
	file_info = (FileInfoPtr)malloc(sizeof(FileInfo));
	/*Parse the header and keep the info*/
	if((err_no = parse_header(stdin, file_info, REVERB)) != 0)
	{
		free(file_info);
		return err_no;
	}
	
	/*Get values from the input parameters*/
	/*Figure out how many samples are delayed (convert from ms to num samples)*/
	delay1 = MILLSEC_TO_SEC*(file_info->frequency)*atoi(argv[1]);
	percent1 = atoi(argv[2]);
	if(argc == 5)
	{
		delay2 = MILLSEC_TO_SEC*(file_info->frequency)*atoi(argv[3]);
		percent2 = atoi(argv[4]);
		num_echoes = 2;
	}
	/*If it's a stereo file there are two values for each sample, so we double all delays*/
	if(file_info->mono_or_stereo == STEREO)
	{
		delay1 = delay1 * 2;
		delay2 = delay2 * 2;
	}
	max_delay = delay1;
	if(delay2 > delay1)
	{
		max_delay = delay2;
	}
	
	/*Check that percent1 and percent2 are between 0 and 100.  Assuming 100 percent is max.*/
	if(percent1 < 0 || percent1 > 100 || percent2 < 0 || percent2 > 100)
	{
		fprintf(stderr, "Each perent parameter input must be between 0 and 100");
		free(file_info);
		return 1;
	}
	
	/*Create the "queue" of size max_delay*/
	samples = (unsigned*)malloc(max_delay * sizeof(unsigned));
	beg_in = max_delay;	/*Should point one past the last element*/
	end_in = max_delay - 1;	/*Should be index of last element*/
	
	/*Loop through stdin, similar to the input_lib funtion.  Fill the queue up first.
	Then, output the value popped off the queue and add the percent*that value back into
	the queue with the proper delay.  Do this until input is done.*/
	bytes_per_sample = file_info->bit_size / 8;
	while(end_of_file == 0)
	{
		if((bytes_read = read_sample(stdin, file_info->bit_size, &value)) != 
															bytes_per_sample)
		{
			end_of_file = 1;
		}
		
		if(filling_queue == 1)
		{	
			add_to_queue(samples, value, &beg_in, &end_in, max_delay);
			if(beg_in == 0)
			{
				filling_queue = 0;
			}
		}
		else
		{
			pop_queue(samples, &value_out, &beg_in, &end_in, max_delay);
			output_sample(stdout, value_out, file_info->bit_size);
			add_to_queue(samples, value, &beg_in, &end_in, max_delay);
			
			
			if(num_echoes == 2)
			{
				/*(+1 since we already popped/added)*/
				echo_index = (end_in+1) - delay2;
				if(echo_index < 0)
				{
					/*max_delay is the length of the queue */
					echo_index = echo_index + max_delay;
				}
				add_echo(samples, (value_out*percent2)/100, echo_index);
			}
			echo_index = (end_in+1) - delay1;
			if(echo_index < 0)
			{
				echo_index = echo_index + max_delay;
			}
			add_echo(samples, (value_out*percent1)/100, echo_index);
		}
		num_bytes = num_bytes + bytes_read;
	}
	
	/*Although input is done, we still need to output what remains in the queue.  Loop through
	as before, but no input is taken and only echoes are added to the queue.*/
	while(beg_in != end_in)
	{
		pop_queue(samples, &value_out, &beg_in, &end_in, max_delay);
		output_sample(stdout, value_out, file_info->bit_size);
		if(num_echoes == 2)
		{
			echo_index = (end_in+1) - delay2;
			if(echo_index < 0)
			{
				echo_index = echo_index + max_delay;
			}
			add_echo(samples, (value_out*percent2/100), echo_index);
		}
		echo_index = (end_in+1) - delay1;
		if(echo_index < 0)
		{
			echo_index = echo_index + max_delay;
		}
		add_echo(samples, (value_out*percent1/100), echo_index);
	}
	
	/*Check that numb of bytes in file was even if stereo, and also equal to the amount 
	specified in the header.*/
	num_samples = num_bytes / bytes_per_sample;
	if(file_info->mono_or_stereo == STEREO)
	{
		if(num_samples % 2 != 0)
		{
			fprintf(stderr, "Stereo files must have an even number of samples");
			free(file_info);
			free(samples);
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
		free(file_info);
		free(samples);
		return 1;
	}
	
	/*Free the allocated memory.*/
	free(samples);
	free(file_info);
	return 0;
}

/**
 * Add the item into the queue.
 */
void add_to_queue(unsigned *queue, unsigned value, int *beg_in, int *end_in, int size)
{
	/*Since I will be the only one using this function, I won't do as much error checking.*/
	*beg_in = *beg_in - 1;
	if(*beg_in < 0)
	{
		*beg_in = size-1;
	}
	queue[*beg_in] = value;
}

/**
 * Remove the last item from the queue and return it.
 */
void pop_queue(unsigned *queue, unsigned *value_out, int *beg_in, int *end_in, int size)
{
	/*Since I will be the only one using this function, I won't do as much error checking.*/
	*value_out = queue[*end_in];
	*end_in = *end_in - 1;
	if(*end_in < 0)
	{
		*end_in = size-1;
	}	
}

/**
 * Add the echo to the queue at the specified index.
 */
void add_echo(unsigned *queue, unsigned echo, int index)
{
	if(index < 0)
	{
		fprintf(stderr, "Index cannot be a negative number.\n");
		return;
	}
	queue[index] = queue[index] + echo;
	return;
}

/**
 * Function to print out the items in the queue.  Won't be used in final version, but should
 * be helpful with debugging and testing.
 */
void print_queue(unsigned *queue, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		printf("%d,", queue[i]);
	}
	printf("\n");
}
