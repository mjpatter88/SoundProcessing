import java.lang.Integer;

/**
 * This class holds information about a sound file that is needed for Part I.  It uses
 * jni to call c functions to get samples from a file and also calculate the fourier transform
 * of those samples.
 */
public class SoundInfo
{
	private final static int window_size = 256;	
	
	private String fileName = "";		//Should never be "" other than when first created
	private int firstSample = -1;		//Should never be -1 other than when first created
	private int[] sample_vals;
	private double[] sample_vals_fl;
	private double[] fourier_vals;	
	
	static native int[] get_samples(int number, int start, String fileName);
	static native double[] calc_fourier(int[] samples, int windowSize, int k);
	static
	{
		String curPath = new java.io.File(".").getAbsolutePath();
		String libPath = curPath + java.io.File.separator + "fourier_lib.so";
		System.load(libPath);
	}
	
	/**
	 * Constructs a new SoundInfo object from the given file.
	 */
	public SoundInfo(String pFileName, int pFirstSample)
	{
		fourier_vals = new double[window_size/2];
		sample_vals_fl = new double[window_size];
		recalc_info(pFileName, pFirstSample);
	}
	
	/**
	 * Returns the array containing the samples from the file
	 */
	public double[] get_sample_vals()
	{
		return sample_vals_fl;
	}
	
	/**
	 * Returns the array containing the fourier transform of the samples.
	 */
	public double[] get_fourier_vals()
	{
		return fourier_vals;
	}
	
	/**
	 * Do the work to get samples from the file and calculate the fourier transform of them.
	 * Uses jni to call c functions to do this.
	 */
	public void recalc_info(String pFileName, int pFirstSample)
	{	
		if(!(fileName.equals(pFileName)) || (firstSample != pFirstSample))
		{
			fileName = pFileName;
			firstSample = pFirstSample;
			
			//First get samples and the file frequency
			sample_vals = get_samples(window_size, pFirstSample, fileName);
			//We need to store them as doubles also so we can return them
			for(int i=0; i<window_size; i++)
			{
				//This is the best way I came up with to handle overflow.  Java doesn't
				//have unsigned ints, so we use doubles and manually correct for overflow.
				double curVal = sample_vals[i];
				double offset = Integer.MAX_VALUE;
				offset += Integer.MAX_VALUE;
				
				if(curVal < 0)
				{
					curVal += offset;
				}
				sample_vals_fl[i] = curVal;
			}
			
			//Calculate the fourier transforms
			double[] complex = new double[2];
			for(int k=0; k<(int)(0.5*(window_size-1)); k++)
			{
				complex = calc_fourier(sample_vals, window_size, k);
				fourier_vals[k] = Math.sqrt((Math.pow(complex[0], 2) + Math.pow(complex[1], 2)));
			}
		}	
		//If the file and the starting sample are the same, there's no reason to update anything
	}
}