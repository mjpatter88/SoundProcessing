#include "sine_lib.h"
#include "util.h"
#include <math.h>
#include <stdio.h>

/**
 * Print the wave described by "sin_prop_ptr"
 */
void print_wave(SinePropPtr sin_prop_ptr)
{
	double samples_per_cycle;
	int number_of_samples;
	double cur_radian = 0.0;
	int i=0;
	unsigned result;
	
	samples_per_cycle = (double)(sin_prop_ptr->sample_rate) / (sin_prop_ptr->frequency);
	number_of_samples = (sin_prop_ptr->sample_rate) * (sin_prop_ptr->duration);
	
	/*Do the math to calculate the values for the described sine wave*/
	for(i=0; i<number_of_samples; i++)
	{
		result = (unsigned)((sin(cur_radian))*(sin_prop_ptr->amplitude/2) + 
													(sin_prop_ptr->amplitude/2));
		if(sin_prop_ptr->mono_or_stereo == STEREO)
		{
			/*Print out two channels of the same amplitude*/
			output_sample(stdout, result, sin_prop_ptr->bit_size);
		}
		output_sample(stdout, result, sin_prop_ptr->bit_size);
		cur_radian += 2*M_PI / (samples_per_cycle);
	}
	
	return;
}
