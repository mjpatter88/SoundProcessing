#ifndef SINE_LIB_H_
#define SINE_LIB_H_

#define MONO 0
#define STEREO 1

typedef struct sine_properties *SinePropPtr;

/**
 * Hold the needed information about a sine wave
 */
typedef struct sine_properties
{
	int mono_or_stereo;	/*0 for MONO, 1 for STEREO*/
	int frequency;
	int bit_size;
	int sample_rate;
	unsigned amplitude;
	double duration;
} SineProp;

/**
 * Print the wave described by the "sin_prop_ptr" structure.
 */
void print_wave(SinePropPtr sin_prop_ptr);

#endif
