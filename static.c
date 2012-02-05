#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "util.h"

#define NUM_ARGS 4

/**
 * Part D: Static
 * Output random data values according to the specified parameters to the standard output.
 * Each value will be between 0 and 2^(bit-size)-1
 *
 * Command Line Variables: static <bit-size> <number of samples> <sample rate>
 * Return Values: 0 - Success; 1 - Failure (error written to stderr)
 */
int main(int argc, char* argv[])
{
	int bit_size;
	int num_samples;
	int sample_rate;
	unsigned max_sample;
	int i;
	unsigned sample;
	
	if(argc != NUM_ARGS)
	{
		fprintf(stderr, "You must enter all 3 parameters");
		return 1;
	}
	
	/*Get the parameters*/
	bit_size = atoi(argv[1]);
	if((bit_size != 8) && (bit_size != 16) && (bit_size != 32))
	{
		fprintf(stderr, "The value for bit-size must be 8, 16, or 32\n");
		return 1;
	}
	
	num_samples = atoi(argv[2]);
	if(num_samples < 0 || num_samples >= INT_MAX)
	{
		fprintf(stderr, "The value for number of samples is not a legal positive integer.\n");
		return(1);
	}
	
	sample_rate = atoi(argv[3]);
	if(sample_rate < 0 || sample_rate >= INT_MAX)
	{
		fprintf(stderr, "The value for sample rate is not a legal positive integer.\n");
		return(1);
	}
	
	/*Print header info*/
	printf("Header\n");
	printf("FREQUENCY %d\n", sample_rate);
	printf("SAMPLE %d\n", num_samples);
	printf("CHANNELS MONO\n");
	printf("SAMPLEBITS %d\n", bit_size);
	printf("EndHeader\n");
	
	/*Print the "static" samples*/
	i=0;
	srand(time(NULL));
	max_sample = (unsigned)(pow(2, bit_size))-1;
	while(i<num_samples)
	{
		sample = rand() % max_sample;
		output_sample(stdout, sample, bit_size);
		i++;
	}
	
	return 0;
}
