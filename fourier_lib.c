#include "FHighLow.h"
#include "SoundInfo.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "input_lib.h"
#include "util.h"
#include "fourier.h" 

#define MAX_FILE_NAME_LENGTH 100

/**
 * This file defines the jni functions that are called from JAVA in parts H and I.  In most 
 * cases these functions just handle the inputs and return values and call another c function
 * to do most of the real processing.  This was done for the sake of code resuse since the
 * c functions doing the processing need to be called from other c functions sometime also. 
 * Note: The two get_sample functions and the two calc_fourier functions are very similar.
 * I would have liked to only make one of each, but since it has to match the jni
 * signature exactly I was not able to.  However, they both call the same c function which
 * does all the real work anyway.
 */


/**
 * This functions gets the "number" of samples from the file specified by "fileName".  The
 * "start" parameter is a percent from 0 to 100 that determines where in the file the samples
 * are read from.
 */
JNIEXPORT jintArray JNICALL Java_FHighLow_get_1samples(JNIEnv *env, jclass cls, 
				jint number, jint start, jstring fileName)
{
	jintArray samples;
	const char *file_path;
	int *result;
	
	samples = (*env)->NewIntArray(env, number+1);
	
	file_path = (*env)->GetStringUTFChars(env, fileName, 0);
	
	/*Call the c function that does all the real work*/
	result = get_samples(number, start, file_path);
	
	(*env)->ReleaseStringUTFChars(env, fileName, file_path);
	
	(*env)->SetIntArrayRegion(env, samples, 0, number+1, result);
	
	return samples;
}

/**
 * This function calculates the fourier transform of the given samples with the provided k 
 * value.  It is returned as a 2-element array in which the first element is the real part 
 * and the second is the imaginary part.
 */
JNIEXPORT jdoubleArray JNICALL Java_FHighLow_calc_1fourier(JNIEnv *env, 
				jclass cls, jintArray samples, jint window_size, jint k)
{
	double* result;
	jdoubleArray to_ret;
	
	jint *elements = (*env)->GetIntArrayElements(env, samples, 0);
	
	/*Call the c function that does all the real work*/
	result = calc_fourier(elements, window_size, k);
	
	to_ret = (*env)->NewDoubleArray(env, 2);
	
	(*env)->ReleaseIntArrayElements(env, samples, elements, 0);
	
	(*env)->SetDoubleArrayRegion(env, to_ret, 0, 2, result);
	
	return to_ret;
}


/*Functions for the second Java Class using jni.*/

/**
 * This function calculates the fourier transform of the given samples with the provided k 
 * value.  It is returned as a 2-element array in which the first element is the real part 
 * and the second is the imaginary part.
 */
JNIEXPORT jintArray JNICALL Java_SoundInfo_get_1samples(JNIEnv *env, jclass cls, 
				jint number, jint start, jstring fileName)
{
	jintArray samples;
	const char *file_path;
	int *result;
	
	samples = (*env)->NewIntArray(env, number+1);
	
	file_path = (*env)->GetStringUTFChars(env, fileName, 0);
	
	/*Call the c function that does all the real work*/
	result = get_samples(number, start, file_path);
	
	(*env)->ReleaseStringUTFChars(env, fileName, file_path);
	
	(*env)->SetIntArrayRegion(env, samples, 0, number+1, result);

	return samples;
}

/**
 * This functions gets the "number" of samples from the file specified by "fileName".  The
 * "start" parameter is a percent from 0 to 100 that determines where in the file the samples
 * are read from.
 */
JNIEXPORT jdoubleArray JNICALL Java_SoundInfo_calc_1fourier(JNIEnv *env, 
				jclass cls, jintArray samples, jint window_size, jint k)
{
	double* result;
	jdoubleArray to_ret;
	
	jint *elements = (*env)->GetIntArrayElements(env, samples, 0);
	
	/*Call the c function that does all the real work*/
	result = calc_fourier(elements, window_size, k);
	
	to_ret = (*env)->NewDoubleArray(env, 2);
	
	(*env)->ReleaseIntArrayElements(env, samples, elements, 0);
	
	(*env)->SetDoubleArrayRegion(env, to_ret, 0, 2, result);
	
	return to_ret;
}


