#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

#define MONO 0
#define STEREO 1

/**
 * Writes the data out to the file.  Appropriately handles the 3 possible bit-sizes.
 * Since this will only be called by me, I know that bit_size will only be the three values
 * that I am handling.
 */
void output_sample(FILE *out, unsigned data, int bit_size);

/**
 * Reads one sample from a file and puts it in the integer pointed to by value.  
 * The size of the sample is dependent on the bit size.
 * Returns how many bytes were read.
 */
int read_sample(FILE *inp, int bit_size, unsigned *value);

/**
 * Prints the header information provided
 */
void print_header(int freq, int number_of_samples, int mono_stereo, int bit_size);

#endif
