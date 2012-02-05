#include "input_lib.h"

/**
 * Part C: Split
 * This program reads in a mono input stream from standard input and outputs a stereo stream
 * by duplicating the mono channel into both stereo samples.
 *
 * Command Line Variables: None
 * Return Values: 0 - Success; 1 - Failure (error written to stderr)
 */
int main(int argc, char* argv[])
{
	int ret_val;
	
	/*The "handle_input" function does all the real processing.*/
	ret_val = handle_input(stdin, NULL, SPLIT);
	
	return ret_val;
}
