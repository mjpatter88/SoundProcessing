#include "util.h"

/**
 * Reads one sample from a file and puts it in the integer pointed to by value.  
 * The size of the sample is dependent on the bit size.
 * Returns how many bytes were read.
 */
int read_sample(FILE *inp, int bit_size, unsigned *value)
{
	int i=0;
	int shift_amt = 0;
	int byte_size;
	unsigned next_byte;
	*value = 0;
	byte_size = bit_size/8;
	
	while(i<byte_size)
	{
		next_byte = getc(inp);
		if(next_byte == EOF)
		{
			break;
		}
		else
		{
			*value = *value | (next_byte<<shift_amt); 
		}
		i++;
		shift_amt = shift_amt + 8;
	}
	return i;
}

/**
 * Writes the data out to the file.  Appropriately handles the 3 possible bit-sizes.
 * Since this will only be called by me, I know that bit_size will only be the three values
 * that I am handling.
 */
void output_sample(FILE *out, unsigned data, int bit_size)
{
	/*If the bit size is 8, no special treatment is needed*/
	if(bit_size == 8)
	{
		putc(data, out);
	}
	
	/*If the bit size is 16, the first char is bits 0-7, second char is bits 8-15*/
	if(bit_size == 16)
	{
		putc(data, out);
		putc(data>>8, out);
	}
	
	/*If the bit size is 32, 1st char = 0-7, 2nd char = 8-15, 3rd char = 16-23, 4th char = 24-31*/
	if(bit_size == 32)
	{
		putc(data, out);
		putc(data>>8, out);
		putc(data>>16, out);
		putc(data>>24, out);
	}
}

/**
 * Prints the header information provided
 */
void print_header(int sample_freq, int number_of_samples, int mono_stereo, int bit_size)
{
	printf("Header\n");
	printf("FREQUENCY %d\n", sample_freq);
	printf("SAMPLE %d\n", number_of_samples);
	if(mono_stereo == MONO)
	{
		printf("CHANNELS MONO\n");
	}
	else
	{
		printf("CHANNELS STEREO\n");
	}
	printf("SAMPLEBITS %d\n", bit_size);
	printf("EndHeader\n");
}

