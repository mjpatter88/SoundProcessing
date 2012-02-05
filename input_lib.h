#ifndef INPUT_LIB_H_
#define INPUT_LIB_H_

#include <stdio.h>

#define NONE 0
#define SPLIT 1
#define COMBINE 2
#define REVERB 3

#define MONO 0
#define STEREO 1
#define MAX_FILE_NAME_LEN 100

typedef struct file_info *FileInfoPtr;

/**
 * Store the needed information about a file of sound samples.
 */
typedef struct file_info
{
	char file_name[MAX_FILE_NAME_LEN];
	int mono_or_stereo;
	int frequency;		/*Sample frequency (Sample Rate)*/
	int bit_size;
	unsigned num_samples;
	
	/*The following only used with mix*/
	double rel_gain;
} FileInfo;

/**
 * Handles the input from a given file.  Information about the file is stored in the struct
 * pointed to by "file_info".  If this information is not needed by the caller, it can safely
 * be passed as NULL.  The options argument allows this function to be used by the "split",
 * "combine", and "reverb" programs also.
 * options : 0-none; 1-split; 2-combine
 */
int handle_input(FILE *inp, FileInfoPtr file_info, int options);

/**
 * Parses through the header of the input.  It firsts looks for the keyword "header" which
 * is case insensitive.  Once found, it looks through the following fields for parameters.
 * Once "endheader" is found it returns, and leaves the file pointer pointing to the first
 * byte of data in the file.  Returns 0 if no format errors are found and 1 if they are.
 */
int parse_header(FILE *inp, FileInfoPtr file_info, int options);

/**
 * Parses through the data portion of the input.  Behavior is very dependent on the options
 * argument.  The error checking at the end of the file is done in all situations.
 * If options = NONE (0): We just make sure the format is valid.
 * If options = SPLIT (1): Data must be MONO.  Outputs the stream in stereo format by 
 * duplicating the mono stream into stereo.
 * If options = COMBINE (2): Data must be STEREO.  Outputs a mono stream by averaging the two
 * stereo channels.
 */
int parse_file(FILE *inp, FileInfoPtr file_info, int options);

#endif
