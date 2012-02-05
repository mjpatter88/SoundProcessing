import java.lang.Math;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import javax.swing.JFileChooser;
import java.io.File;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.Color;



/**
 * Part I: SoundProcessor
 * This class is the main GUI class for part I.  It allows the user to choose a file to load
 * and then displays two graphs relating to the data in that file.  It also provides a slider
 * for the user to control the starting sample.  The top graph shows the samples in the file, 
 * and the bottom graph shows the fourier transform of those samples.
 */ 
public class SoundProcessor extends JPanel implements ChangeListener,
										 			ActionListener
{
	static final int SAMPLE_MIN = 0;
	static final int window_size = 256;
	private static int SAMPLE_MAX = 100;
	private static int curSample = 0;
	private static String fileName = "";
	private static JFrame topFrame;
	private static JButton button;
	private static JLabel sliderLabel;
	private static JLabel samplesLabel;
	private static JLabel fourierLabel;
	private static JSlider startingSampleSlider;
	private static SoundInfo soundInfo;
	
	private static GraphDisplay fourierDisplay;
	private static GraphDisplay sampleDisplay;
		
	/**
	 * Creates a new SoundProcessor.  Originally it just displays a button allowing the user
	 * to load a file of sound samples.
	 */
	public SoundProcessor()
	{	
		setSize(800, 600);
		//Create and add the button
		button = new JButton("Load sample");
		button.setMnemonic(KeyEvent.VK_D);
		button.setActionCommand("load");
		button.addActionListener(this);
		button.setLocation(300, 500);
		button.setSize(200, 50);
		
		add(button);
		validate();
	}
	
	
	/**
	 * Doesn't do much other than telling Swing/Java to create our GUI.  This code comes
	 * almost directly from the example given in class.
	 */
	public static void main(String[] args)
	{
		javax.swing.SwingUtilities.invokeLater( new Runnable()
			{
				public void run()
				{
					createAndShowGui();
				}
			});
	}
	
	/**
	 * Creates a new outer frame and create a new SoundProcessor and add it to the frame.
	 */
	private static void createAndShowGui()
	{
		topFrame = new JFrame("Sound Processor");
		topFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		topFrame.setSize(800, 600);
		
		SoundProcessor sp = new SoundProcessor();
		sp.setLayout(null);
		sp.setVisible(true);
		
		topFrame.getContentPane().add(sp);
		//topFrame.add(sp);
		topFrame.setVisible(true);
	}
	
	/**
	 * Handles the action when the JSlider is moved. It purposefully only updates the display
	 * when the slider is released after it has changed.  I chose to do it this way because
	 * I think it makes more sense to let the user place the slider and then update the 
	 * display.  Also, with large files the display might get sluggish if I tried to update it
	 * as it was moving.
	 */
	public void stateChanged(ChangeEvent e)
	{
		JSlider source = (JSlider)e.getSource();
		if(!source.getValueIsAdjusting())
		{
			curSample = source.getValue();
			/*Update the SoundInfo object with the new starting sample.*/
			soundInfo.recalc_info(fileName, curSample);
			/*Update each of the two displays with the new SoundInfo object*/
			sampleDisplay.updateSamples(soundInfo.get_sample_vals());
			fourierDisplay.updateSamples(soundInfo.get_fourier_vals());
		}
	}
	
	/**
	 * Handles the action when the "load" button has been pressed.  It uses a JFileChooser
	 * to allow the user to pick a file.  It then goes through and creates all the parts of
	 * the gui if they don't already exist.  It then updates each component.
	 */
	public void actionPerformed(ActionEvent e)
	{
		if("load".equals(e.getActionCommand()))
		{	
			JFileChooser open = new JFileChooser();
			open.setVisible(true);
			int retVal = open.showOpenDialog(topFrame);
			if(retVal != JFileChooser.APPROVE_OPTION)
			{
				return;
			}
			File file = open.getSelectedFile();
			fileName = file.getAbsolutePath();
			
			//Set the position of the button to the upper left corner
			button.setLocation(0, 5);
			
			//Create the slider if it doesn't exist
			if(startingSampleSlider == null)
			{
				startingSampleSlider = new JSlider(JSlider.HORIZONTAL, 
								SAMPLE_MIN, SAMPLE_MAX, curSample);
				startingSampleSlider.addChangeListener(this);
				startingSampleSlider.setVisible(true);
				startingSampleSlider.setLocation(210, 25);
				startingSampleSlider.setSize(580, 50);
				startingSampleSlider.setMajorTickSpacing(10);
				//startingSampleSlider.setMinorTickSpacing(1);
				startingSampleSlider.setPaintTicks(true);
				startingSampleSlider.setPaintLabels(true);
				add(startingSampleSlider);
				
				//Also add the label
				sliderLabel = new JLabel("Starting Sample (percent of total samples)");
				sliderLabel.setLocation(370, 5);
				sliderLabel.setSize(300, 25);
				sliderLabel.setVisible(true);
				add(sliderLabel);
			}
			
			//Create the soundInfo if it doesn't exist
			if(soundInfo == null)
			{
				soundInfo = new SoundInfo(fileName, 0);				
			}
			else
			{
				soundInfo.recalc_info(fileName, curSample);
			}
			
			//Create the sampleDisplay if it doesn't exist
			if(sampleDisplay == null)
			{
				sampleDisplay = new GraphDisplay(window_size);
				sampleDisplay.setVisible(true);
				sampleDisplay.setLocation(20, 100);
				sampleDisplay.setSize(760, 200);
				add(sampleDisplay);
				
				//Also add the label
				samplesLabel = new JLabel("Sound Samples");
				samplesLabel.setLocation(0, 75);
				samplesLabel.setSize(150, 25);
				samplesLabel.setVisible(true);
				add(samplesLabel);
			}
			//Update
			sampleDisplay.updateSamples(soundInfo.get_sample_vals());
			
			//Create the fourierDisplay if it doesn't exist, otherwise update
			if(fourierDisplay == null)
			{
				fourierDisplay = new GraphDisplay(window_size / 2);
				fourierDisplay.setVisible(true);
				fourierDisplay.setLocation(20, 350);
				fourierDisplay.setSize(760, 200);
				add(fourierDisplay);
				
				//Also add the label
				fourierLabel = new JLabel("Fourier Transform");
				fourierLabel.setLocation(0, 325);
				fourierLabel.setSize(150, 25);
				fourierLabel.setVisible(true);
				add(fourierLabel);
			}
			//Update
			fourierDisplay.updateSamples(soundInfo.get_fourier_vals());
			
			//So that the changes will appear.
			validate();
			
		}
	}	
}