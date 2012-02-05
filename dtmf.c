#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "input_lib.h"
#include "fourier.h"

#define hz697 0
#define hz770 1
#define hz852 2
#define hz941 3
#define hz1209 4
#define hz1336 5
#define hz1477 6
#define hz1633 7
#define INTERRUPTION 8

void get_fourier(unsigned *samples, int *freq_present, int window_size, int sample_rate);
int is_present(unsigned* samples, int window_size, int frequency, int sample_rate);
void calc_tones(int *freq_present, int *button_pressed);
void print_button(int button_index);
void add_press(int *freq_present, int *button_pressed);

/**
 * Part J: dtmf
 * This program reads a sample file from standard input and outputs the sequence of dtmf 
 * buttons pushed to generate the sample data.
 *
 * Command Line Variables: None
 * Return Values: 0 - Success; 1 - Failure (error written to stderr) 
 */
int main(int argc, char *argv[])
{
	int err_no;
	unsigned *samples;
	int samples_in_20ms;
	FileInfoPtr input_info;
	
	/*Arrays to hold which frequencies were detected and which buttons have been pressed*/
	int freq_present[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int button_pressed[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	/*Allocate space for the file_info struct and parse header*/
	input_info = (FileInfoPtr)malloc(sizeof(FileInfo));
	err_no = parse_header(stdin, input_info, NONE);
	if(err_no != 0)
	{
		free(input_info);
		return err_no;
	}
	
	/*I decided to use a 20ms window and to move this many samples each time.  This allows me
	to say that if a tone is received in two adjacent windows, then it counts as a button
	press.  If there is no button press detected in a window, then it's long enough to count
	as a break between presses*/
	samples_in_20ms = (input_info->frequency * 20) / 1000;
	
	/*Get the samples from stdin for the first window*/
	samples = get_samples_stdin(samples_in_20ms, input_info->bit_size);
	
	/*Samples will return null when the end of the file is reached.*/
	while(samples != NULL)
	{
		/*Figure out which frequency components are present*/
		get_fourier(samples, freq_present, samples_in_20ms, input_info->frequency);
		
		/*Using that info, figure out which buttons on the phone were pressed*/
		calc_tones(freq_present, button_pressed);
		
		/*Free the memory as we will get new space from the next get_samples_stdin call*/
		free(samples);
		
		/*Get the next set of samples and continue looping.*/
		samples = get_samples_stdin(samples_in_20ms, input_info->bit_size);
	}
	
	/*So output isn't on the same line as the next shell prompt*/
	printf("\n");
	
	free(input_info);
	return 0;	
}

/**
 * This function uses the values in the array pointed to by samples and determines if any of
 * the frequency components that we care about are present.  Any that are get incremented in
 * the array pointed to by "freq_present".  If no frequencies are present then we increment
 * the values being used for breaks in the tone.
 */
void get_fourier(unsigned *samples, int *freq_present, int window_size, int sample_rate)
{
	int i;
	int interr = 1; /*If nothing is present, then we call it an interruption*/
	
	/*These are the frequencies that we care about for dtmf*/
	int frequencies[] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
	
	/*Loop through each of the frequencies from the above array and see if it's present*/
	for(i=0; i<8; i++)
	{
		if(is_present(samples, window_size, frequencies[i], sample_rate))
		{
			freq_present[i] = 1;
			interr = 0;
		}
		else
		{
			freq_present[i] = 0;
		}
	}
	
	if(interr == 1)
	{
		freq_present[INTERRUPTION] = 1;
	}
	else
	{
		freq_present[INTERRUPTION] = 0;
	}
}

/**
 * Using the Fourier transform and the specs given in the assignment sheet about dtmf to 
 * determine if the frequency component provided is present in the array pointed to by "samples".
 * 
 * Note: after testing it seems to give the best results if I check that the fourier value
 * is 2.5x bigger than the samples 3.5% away rather than 5.  Most will work with 5, but I seem
 * to get more consisten results with 2.5.
 */
int is_present(unsigned* samples, int window_size, int frequency, int sample_rate)
{
	int k;
	int k_low, k_high;
	int k_limit_low, k_limit_high;
	int freq_low, freq_high;
	int freq_limit_low, freq_limit_high;
	double *complex;
	double result;
	double highest_fourier = 0.0;
	
	
	/*Find the highest fourier within 1.5% of the given frequency*/
	freq_low = frequency * 0.985;
	freq_high = frequency * 1.015;
	
	k_low = (freq_low * window_size) / sample_rate;
	k_high = (freq_high * window_size) / sample_rate;
	
	for(k = k_low; k<= k_high; k++)
	{
		complex = calc_fourier((int*)samples, window_size, k);
		result = sqrt((complex[0]*complex[0]) + (complex[1]*complex[1]));
		free(complex);
		if(result > highest_fourier)
		{
			highest_fourier = result;
		}
	}
	
	/*Check that it is 2.5x bigger than each fourier at 3.5% away*/
	freq_limit_low = frequency * 0.965;
	freq_limit_high = frequency * 1.035;
	
	k_limit_low = (freq_limit_low * window_size) / sample_rate;
	k_limit_high = (freq_limit_high * window_size) / sample_rate;
	
	complex = calc_fourier((int*)samples, window_size, k_limit_low);
	result = sqrt((complex[0]*complex[0]) + (complex[1]*complex[1]));
	free(complex);
	if((result * 2.5) > highest_fourier)
	{
		return 0;
	}
	
	complex = calc_fourier((int*)samples, window_size, k_limit_low);
	result = sqrt((complex[0]*complex[0]) + (complex[1]*complex[1]));
	free(complex);
	if((result * 2.5) > highest_fourier)
	{
		return 0;
	}
	
	/*Even if it passes all these checks we only want to return 1 if it's over 500*/
	if(highest_fourier > 500)
	{
		return 1;
	}
	return 0;
}

/**
 * This function handles the button presses based on which frequencies are present.  It first
 * uses the "add_press" function to increae the appropriate value in the "button_pressed" 
 * array.  Then it loops through and sees if any of the values are exactly 2.  If they are
 * it then prints that button and sets the other values to 0.  No matter how long a button
 * is held down, it will only print out once since it only prints when exactly equal to 2.
 * The value of 2 is used since our window size is 20ms and a button must be pressed for 40ms
 * according to the dtmf spec provided in the assignment.
 */
void calc_tones(int *freq_present, int *button_pressed)
{
	int delete_others = 0;
	int i, j;
	/*Go through and increment the appropriate button-pressed*/
	add_press(freq_present, button_pressed);
	
	/*Loop through, if anything == 2, then output appropriate letter/number and delete others*/
	for(i=0; i<17; i++)
	{	
		if(button_pressed[i] == 2)
		{
			print_button(i);
			delete_others = 1;
			break;
		}
	}
	
	/*If there was a button press, all the other ones get reset.*/
	if(delete_others == 1)
	{
		for(j=0; j<17; j++)
		{
			if(j != i)
			{
				button_pressed[j] = 0;
			}
		}
	}
}

/**
 * Print the value corresponding to whichever button was pressed.  If it's just a break in
 * between tones then nothing is printed. 
 */
void print_button(int button_index)
{
	char buttons[] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 
							'C', '*', '0', '#', 'D'};
	/*17 is the index for static, so nothing needs to be printed*/
	if(button_index == 16)
	{
		return;
	}
	printf("%c ", buttons[button_index]);
	return;
}

/**
 * Increments the appropriate value in the array pointed to by "button_pressed" based on
 * which frequencies are present in the array pointed to by "freq_present".  This is based
 * on the grid showing the relationships between the buttons and frequencies that was provided
 * in the assignment.
 */
void add_press(int *freq_present, int *button_pressed)
{
	if(freq_present[hz697])
	{
		if(freq_present[hz1209])
		{
			button_pressed[0]++;
		}
		else if(freq_present[hz1336])
		{
			button_pressed[1]++;
		}
		else if(freq_present[hz1477])
		{
			button_pressed[2]++;
		}
		else if(freq_present[hz1633])
		{
			button_pressed[3]++;
		}
	}
	else if(freq_present[hz770])
	{
		if(freq_present[hz1209])
		{
			button_pressed[4]++;
		}
		else if(freq_present[hz1336])
		{
			button_pressed[5]++;
		}
		else if(freq_present[hz1477])
		{
			button_pressed[6]++;
		}
		else if(freq_present[hz1633])
		{
			button_pressed[7]++;
		}
	}
	else if(freq_present[hz852])
	{
		if(freq_present[hz1209])
		{
			button_pressed[8]++;
		}
		else if(freq_present[hz1336])
		{
			button_pressed[9]++;
		}
		else if(freq_present[hz1477])
		{
			button_pressed[10]++;
		}
		else if(freq_present[hz1633])
		{
			button_pressed[11]++;
		}
	}
	else if(freq_present[hz941])
	{
		if(freq_present[hz1209])
		{
			button_pressed[12]++;
		}
		else if(freq_present[hz1336])
		{
			button_pressed[13]++;
		}
		else if(freq_present[hz1477])
		{
			button_pressed[14]++;
		}
		else if(freq_present[hz1633])
		{
			button_pressed[15]++;
		}
	}
	else if (freq_present[INTERRUPTION])
	{
		/*This isn't really a button but is used to signal a break between buttons*/
		/*We increment it by two because a break only needs to be 20ms long but a button 
		must be 40ms*/
		button_pressed[16] += 2;
	}
}
