#ifndef FOURIER_H_
#define FOURIER_H_

#include "input_lib.h"
/**
 * These functions are used with the "fourier_lib.c" file and the "dtmf.c" files.  I needed
 * to create a new file with these functions in them so both the jni functions and the pure
 * c functions were able to resuse much of the same code.
 */


/**
 * Gets the number of samples from the file specified by fileName starting at start and returns
 * them.  Note: start is the starting percent, from 0 to 100, not the starting sample number.
 */
int* get_samples(int number, int start, const char* fileName);

/**
 * Calculates the fourier transform of the samples provided for the k value provided.
 */
double* calc_fourier(int* samples, int window_size, int k);

/**
 * Verifies that the input is valid.
 */
int check_input(FileInfoPtr file_info);

/**
 * Gets the data samples from standard in.  I was unable to reuse the code from the "get_samples"
 * function above because this gets the samples from standard in and not a file.  It can't
 * move around the file in the same way, so it always returns the first "number" of samples.
 */
unsigned* get_samples_stdin(int number, int bit_size);

#endif
