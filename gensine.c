#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*limits.h needed for using INT_MAX*/
#include <limits.h>
#include "sine_lib.h"
#include "util.h"


#define NUM_ARGS 7
/*Define the index of each argument from the command line*/
#define IND_MON_STER 0
#define IND_FREQ 1
#define IND_BIT_SIZE 2
#define IND_SAMP_RATE 3
#define IND_AMP 4
#define IND_DUR 5

int load_check_args(char *inp[], SinePropPtr sin_prop_ptr);

/**
 * Output a sine wave according to the specified parameters to the standard output.
 * 
 * gensine <MONO | STEREO> <frequency> <bit-size> <sample rate> <amplitude> <duration>
 */
int main(int argc, char* argv[])
{
	int args_error;
	int samples_per_cycle;
	int number_of_samples;
	
	SinePropPtr spptr;
	
	/*handle the command line arguments*/
	if(argc != NUM_ARGS)
	{
		fprintf(stderr, "You must enter all 6 parameters.\n");
		return 1;
	}
	
	spptr = (SinePropPtr)malloc(sizeof(SineProp));
	
	/*Load and check the command like arguments. (argv+1) to skip the first arg*/
	if((args_error = load_check_args((argv+1), spptr)) != 0)
	{
		free(spptr);
		return 1;	/*An error has occurred.*/
	}
	
	samples_per_cycle = (spptr->sample_rate) / (spptr->frequency);
	number_of_samples = (spptr->sample_rate) * (spptr->duration);
	/*Print the header output*/
	print_header(spptr->sample_rate, number_of_samples, 
				spptr->mono_or_stereo, spptr->bit_size);
	
	/*Generate and print the wave*/
	print_wave(spptr);
	
	free(spptr);
	
	return 0;
}

/**
 * Checks that the command line arguments are all legal.
 * Returns 0 if they are all legal, and 1 if they are not.
 * Returns 1 and prints to stderr if any of the arguments are illegal
 */
int load_check_args(char* inp[], SinePropPtr sin_prop_ptr)
{	
	/*Check that the first arg is "MONO" or "STEREO"*/
	if(strncmp(inp[IND_MON_STER], "MONO", 4)==0 && strlen(inp[IND_MON_STER])==4)
	{
		sin_prop_ptr->mono_or_stereo = MONO;	/*0 for MONO*/
	}
	else if(strncmp(inp[IND_MON_STER], "STEREO", 6)==0 && strlen(inp[IND_MON_STER])==6)
	{
		sin_prop_ptr->mono_or_stereo = STEREO; /*1 for STEREO*/
	}
	else
	{
		fprintf(stderr, "The first command line argument must be \"MONO\" or \"STEREO\".\n");
		return 1;
	}
	
	/*Check that the freq and sample rate are both legal integer values*/
	sin_prop_ptr->frequency = atoi(inp[IND_FREQ]);
	if(sin_prop_ptr->frequency < 0 || sin_prop_ptr->frequency >= INT_MAX)
	{
		fprintf(stderr, "The value for frequency is not a legal positive integer.\n");
		return(1);
	}
	sin_prop_ptr->sample_rate = atoi(inp[IND_SAMP_RATE]);
	if(sin_prop_ptr->sample_rate < 0 || sin_prop_ptr->sample_rate >= INT_MAX)
	{
		fprintf(stderr, "The value for sample rate is not a legal positive integer.\n");
		return 1;
	}
	
	/*Check that the frequency is not more than twice the sampling rate*/
	if(sin_prop_ptr->frequency > (sin_prop_ptr->sample_rate / 2))
	{
		fprintf(stderr, "The value for frequency can't be more than twice the sample rate.\n");
		return 1;
	}
	
	/*Check that the bitsize is either 8, 16, or 32 bits*/
	sin_prop_ptr->bit_size = atoi(inp[IND_BIT_SIZE]);
	if((sin_prop_ptr->bit_size != 8) && (sin_prop_ptr->bit_size != 16) && 
												(sin_prop_ptr->bit_size != 32))
	{
		fprintf(stderr, "The value for bit-size must be 8, 16, or 32\n");
		return 1;
	}
	
	/*Check that the amplitude is > 0 and < (2^bitsize)-1*/
	sin_prop_ptr->amplitude = atoi(inp[IND_AMP]);
	if((sin_prop_ptr->amplitude < 0) || 
			(sin_prop_ptr->amplitude > (unsigned)(pow(2, sin_prop_ptr->bit_size))-1))
	{
		fprintf(stderr, "The value for amplitude must be > 0 and < 2^bit-size\n");
		return 1;
	}
	
	/*Check that the duration is > 0*/
	sin_prop_ptr->duration = atof(inp[IND_DUR]);
	if((sin_prop_ptr->duration < 0))
	{
		fprintf(stderr, "The value for duration is not a positive number.\n");
		return 1;
	}
	
	/*If all these checks are passed, then the cmd line arguments are valid*/
	return 0;
}
