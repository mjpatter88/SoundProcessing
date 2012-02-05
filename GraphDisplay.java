import javax.swing.JPanel;
import javax.swing.JComponent;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.Color;

/**
 * This class extends JPanel and represents one of the two graphs that are part of the GUI for
 * Part I.  This same class is used for both the fourier values and the samples values.  It
 * displays a graph similar to a bar graph of the given samples.  It also automatically scales
 * the data to fit in the window and mostly fill it.
 */
public class GraphDisplay extends JPanel
{

	private double[] samples;
	private int window_size;
	
	/**
	 * Constructs a new GraphDisplay with the given window_size.  This refers to the number
	 * of samples to be displayed, not the physical size of this JPanel.
	 */
	public GraphDisplay(int p_window_size)
	{
		window_size = p_window_size;
	}
	
	/**
	 * Resets this display with a new set of samples.
	 */
	public void updateSamples(double[] newSamples)
	{
		samples = newSamples;
		repaint();
	}
	
	/**
	 * Overrides the default paintComponent method.  This draws the custom graphs that were
	 * described in the comments about this class.
	 * Note: Some of this seems hard-coded and some of the values are somewhat arebitrary.
	 * This is a result of quite a bit of testing and trial and error.  At times I had to
	 * add constants to get things to line up right even though it seems like they should
	 * have lined up without them.
	 */
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D)g;
		
		setBackground(Color.WHITE);
		
		// width divided by window size +1 to make things line up right
		double barWidth = (getWidth() / window_size) + 1;		
		double barHeight = 0.0f;
		double barYpos = getHeight() - barHeight;
		double barXpos = 0.0f;
		
		//Find the max value and scale accordingly
		double max_sample = samples[1];
		
		for(int i=2; i<window_size; i++)
		{
			if(samples[i] > max_sample)
			{
				max_sample = samples[i];
			}
		}
		
		// -5 to leave a little padding towards the top of the graph
		double scaleFactor = max_sample / (getHeight() - 5);
		
		//Draw the bar graph. Skip the first element since it's the fourier of 0. 
		for(int i=1; i<window_size; i++)
		{
			barHeight = samples[i] / scaleFactor;
			barYpos = getHeight() - barHeight;
			barXpos = (i-1) * (barWidth);
			
			g2.setPaint(Color.BLACK);
			Rectangle2D rect = new Rectangle2D.Double(barXpos, barYpos, barWidth, barHeight);
			g2.draw(rect);
			
			g2.setPaint(Color.LIGHT_GRAY);
			g2.fill(new Rectangle2D.Double(barXpos+1, barYpos+1, barWidth-1, barHeight-1));
		}
		
	}
}
