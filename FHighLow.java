/**
 * Part H: FHighLow
 * This program reads a sound sample file from the standard input and outputs the values of
 * the fourier transform at the highest and lowest frequencies.  It uses jni to call the two
 * native c functions to get the samples and do the required calculations.
 */
public class FHighLow
{
	static native int[] get_samples(int number, int start, String fileName);
	static native double[] calc_fourier(int[] samples, int windowSize, int k);
	static
	{
		/*This seems to be the best way to load my native c library dynamically*/
		String curPath = new java.io.File(".").getAbsolutePath();
		String libPath = curPath + java.io.File.separator + "fourier_lib.so";
		System.load(libPath);
	}
	
	/**
	 * The main method that does what this program is supposed to do.  See description above.
	 */
	public static void main(String[] args)
	{
		int window_size = 256;
		//This function returns the array containing the correct number of samples.
		//Also, it adds the sample rate to the very end of the array.
		//So in this case it actually returns 257 numbers.  The sample rate is needed to 
		//calculate k2
		int[] samples = get_samples(window_size, 0, "stdin");
		
		int freq1 = 0;
		//This next line is really sample_rate/2
		int freq2 = samples[window_size]/2;
		
		
		int k1 = 0;
		int k2 = (int)(0.5*(window_size - 1));
		
		double[] result1 = calc_fourier(samples, window_size, k1);
		double[] result2 = calc_fourier(samples, window_size, k2);
		
		System.out.println("Freq. " + freq1 + " Real: " + result1[0] + " Imaginary: " + result1[1]);
		System.out.println("Freq. " + freq2 + " Real: " + result2[0] + " Imaginary: " + result2[1]);
		
	}
}