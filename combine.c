#include "input_lib.h"

/**
 * Part C: Combine
 * This program reads a stereo input stream from standard input and outputs a mono stream by 
 * combining the two stereo channels into one by averaging the left and the right channels
 * to form the mono output stream.  Outputs this mono stream to the standard output.
 *
 * Command Line Variables: None
 * Return Values: 0 - Success; 1 - A problem occurred. (error written to stderr)
 */
 int main(int argc, char* argv[])
 {
 	int ret_val;
 	
 	/*The "handle_input" function does all the real processing.*/
 	ret_val = handle_input(stdin, NULL, COMBINE);
 	
 	return ret_val;
 }
