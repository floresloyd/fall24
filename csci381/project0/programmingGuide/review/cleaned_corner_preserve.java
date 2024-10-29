import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

/**
 * The Enhancement class provides methods for loading, processing,
 * and enhancing images using mirror framing, averaging, and corner-preserve averaging techniques.
 */
class Enhancement {
    /** Array representing various convolution masks for image enhancement */
    public static final int[][][] mask = {
        {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 1, 1, 0},
                {1, 1, 1, 1, 1}
        },
        {
                {1, 0, 0, 0, 0},
                {1, 1, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 0, 0, 0},
                {1, 0, 0, 0, 0}
        },
        {
                {1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
        },
        {
                {0, 0, 0, 0, 1},
                {0, 0, 0, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 0, 1, 1},
                {0, 0, 0, 0, 1}
        },
        {
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
        },
        {
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0}
        },
        {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 1, 1}
        },
        {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 1, 1, 0, 0}
        }
    };
    
    private int numRows;
    private int numCols;
    private int minVal;
    private int maxVal;
    private int[][] mirrorFramedAry;
    private int[][] avgAry;
    private int[][] CPavgAry;
    private int[] Avg_histAry;
    private int[] CPavg_histAry;

    /**
     * @return the mirror framed array for the image.
     */
    public int[][] getMirrorFramedAry() {
        return mirrorFramedAry;
    }

    /**
     * @return the average array generated by 5x5 averaging.
     */
    public int[][] getAvgAry() {
        return avgAry;
    }

    /**
     * @return the corner-preserve average array.
     */
    public int[][] getCPavgAry() {
        return CPavgAry;
    }

    /**
     * @return histogram array for averaged values.
     */
    public int[] getAvg_histAry() {
        return Avg_histAry;
    }

    /**
     * @return histogram array for corner-preserved average values.
     */
    public int[] getCPavg_histAry() {
        return CPavg_histAry;
    }

    /**
     * Loads the image data from a file into the mirror framed array.
     * @param inFile Scanner for reading the input image data.
     */
    public void loadImage(Scanner inFile) {
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                mirrorFramedAry[i + 2][j + 2] = inFile.nextInt();
            }
        }
    }

    /**
     * Mirrors the image data around the edges to create a frame.
     */
    public void mirrorFraming() {
        for (int j = 0; j < mirrorFramedAry[2].length; j++) {
            mirrorFramedAry[0][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[1][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[numRows + 2][j] = mirrorFramedAry[numRows + 1][j];
            mirrorFramedAry[numRows + 3][j] = mirrorFramedAry[numRows + 1][j];
        }
        for (int i = 0; i < mirrorFramedAry.length; i++) {
            mirrorFramedAry[i][0] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][1] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][numCols + 2] = mirrorFramedAry[i][numCols + 1];
            mirrorFramedAry[i][numCols + 3] = mirrorFramedAry[i][numCols + 1];
        }
    }

    /**
     * Initializes the class variables based on the input file.
     * @param inFile Scanner to read initial image metadata.
     * @throws IOException
     */
    public void init(Scanner inFile) throws IOException {
        numRows = inFile.nextInt();
        numCols = inFile.nextInt();
        minVal = inFile.nextInt();
        maxVal = inFile.nextInt();
        mirrorFramedAry = new int[numRows + 4][numCols + 4];
        avgAry = new int[numRows + 4][numCols + 4];
        CPavgAry = new int[numRows + 4][numCols + 4];
    }

    /**
     * Prints a 2D array to the console with formatting.
     * @param array The 2D array to print.
     */
    public void print2DArray(int[][] array) {
        for (int[] ints : array) {
            for (int anInt : ints) {
                if (anInt < 10) {
                    System.out.print(anInt + "  ");
                } else {
                    System.out.print(anInt + " ");
                }
            }
            System.out.println();
        }
    }

    /**
     * Performs 5x5 convolution on the mirror framed array at position (i, j).
     * @param i Row position.
     * @param j Column position.
     * @param mask The mask array for convolution.
     * @param deBugFile BufferedWriter for debug output.
     * @return Result of convolution.
     * @throws IOException
     */
    public int convolution5x5(int i, int j, int[][] mask, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering convolution5x5 method\n");
        int result = 0;
        int r = -2;
        int c = -2;
        while (r <= 2) {
            c = -2;
            while (c <= 2) {
                result = result + mask[r + 2][c + 2] * mirrorFramedAry[i + r][j + c];
                c++;
            }
            r++;
        }
        deBugFile.write("Leaving convolution5x5 with result: " + result + "\n");
        return result;
    }

    /**
     * Calculates the 5x5 average at position (i, j) on the mirror framed array.
     * @param i Row position.
     * @param j Column position.
     * @return Average result.
     */
    public int avg5x5(int i, int j) {
        int result = 0;
        int r = -2;
        int c = -2;
        while (r <= 2) {
            c = -2;
            while (c <= 2) {
                result += mirrorFramedAry[i + r][j + c];
                c++;
            }
            r++;
        }
        return result / 25;
    }

    /**
     * Computes 5x5 average on the entire array and records max/min values.
     * @param deBugFile BufferedWriter for debug output.
     * @throws IOException
     */
    public void computeAvg5x5(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering computeAvg5x5\n");
        int newMax = mirrorFramedAry[2][2];
        int newMin = mirrorFramedAry[2][2];
        for (int i = 2; i < numRows + 2; i++) {
            for (int j = 2; j < numCols + 2; j++) {
                avgAry[i][j] = avg5x5(i, j);
                newMax = Math.max(newMax, avgAry[i][j]);
                newMin = Math.min(newMin, avgAry[i][j]);
            }
        }
        maxVal = newMax;
        minVal = newMin;
        deBugFile.write("computeAvg5x5 newMax=" + maxVal + " newMin=" + minVal + "\n");
    }

    /**
     * Computes histogram for the input array.
     * @param inAry Input 2D array.
     * @param histAry Output histogram array.
     * @param deBugFile BufferedWriter for debug output.
     * @throws IOException
     */
    public void computeHist(int[][] inAry, int[] histAry, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering computeHist\n");
        for (int i = 2; i < numRows + 2; i++) {
            for (int j = 2; j < numCols + 2; j++) {
                histAry[inAry[i][j]]++;
            }
        }
    }

    /**
     * Displays histogram data by writing to an output file.
     * @param histAry The histogram array.
     * @param outFile BufferedWriter for output file.
     * @throws IOException
     */
    public void dispHist(int[] histAry, BufferedWriter outFile) throws IOException {
        outFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        for (int i = 0; i <= maxVal; i++) {
            outFile.write(i + " (" + histAry[i] + "):");
            for (int j = 0; j < histAry[i]; j++) {
                outFile.write("+");
            }
            outFile.write("\n");
        }
    }

    /**
     * Writes histogram to a file in a simplified format.
     * @param histAry The histogram array.
     * @param histFile BufferedWriter for histogram file.
     * @param deBugFile BufferedWriter for debug file.
     * @throws IOException
     */
    public void printHist(int[] histAry, BufferedWriter histFile, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering printHist\n");
        histFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        for (int i = 0; i <= maxVal; i++) {
            histFile.write(i + " " + histAry[i] + "\n");
        }
    }

    /**
     * Calculates corner-preserve averaging across the array.
     * @param deBugFile BufferedWriter for debug output.
     * @throws IOException
     */
    public void cornerPreserveAvg(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering cornerPreserveAvg\n");
        int newMax = mirrorFramedAry[2][2];
        int newMin = mirrorFramedAry[2][2];
        for (int i = 2; i < numRows + 2; i++) {
            for (int j = 2; j < numCols + 2; j++) {
                int minAvg = mirrorFramedAry[i][j];
                int minDiff = Integer.MAX_VALUE;
                for (int maskIndex = 0; maskIndex < 8; maskIndex++) {
                    int result = convolution5x5(i, j, mask[maskIndex], deBugFile) / 9;
                    int diff = Math.abs(result - mirrorFramedAry[i][j]);
                    if (diff < minDiff) {
                        minDiff = diff;
                        minAvg = result;
                    }
                }
                CPavgAry[i][j] = minAvg;
                newMax = Math.max(newMax, minAvg);
                newMin = Math.min(newMin, minAvg);
            }
        }
        maxVal = newMax;
        minVal = newMin;
    }

    /**
     * Reformats and outputs the image array to a file.
     * @param inAry Input 2D array.
     * @param fileOut BufferedWriter for output file.
     * @throws IOException
     */
    public void imgReformat(int[][] inAry, BufferedWriter fileOut) throws IOException {
        fileOut.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        String str = Integer.toString(maxVal);
        int width = str.length();
        for (int i = 2; i < numRows + 2; i++) {
            for (int j = 2; j < numCols + 2; j++) {
                str = Integer.toString(inAry[i][j]);
                fileOut.write(str);
                int ww = str.length();
                while (ww < width) {
                    fileOut.write(" ");
                    ww++;
                }
            }
            fileOut.write("\n");
        }
    }
}

/**
 * Main class to execute image processing with various enhancement options.
 */
public class cleaned_corner_preserve {
    public static void main(String[] args) throws IOException {
        Scanner inFile = new Scanner(new FileReader("test_input.txt"));
        int opChoice = 2;
        BufferedWriter outFile1 = new BufferedWriter(new FileWriter("output1_3.txt"));
        BufferedWriter outFile2 = new BufferedWriter(new FileWriter("output2_3.txt"));
        BufferedWriter outFile3 = new BufferedWriter(new FileWriter("output3_3.txt"));
        BufferedWriter deBugFile = new BufferedWriter(new FileWriter("debug_3.txt"));

        Enhancement img = new Enhancement();
        img.init(inFile);
        img.loadImage(inFile);
        img.mirrorFraming();
        outFile1.write("Image reformatted mirrorFramedAry\n");
        img.imgReformat(img.getMirrorFramedAry(), outFile1);

        if (opChoice == 1) {
            img.computeAvg5x5(deBugFile);
            outFile1.write("Result of 5x5 averaging on the input image\n");
            img.imgReformat(img.getAvgAry(), outFile1);
            img.imgReformat(img.getAvgAry(), outFile2);
            img.computeHist(img.getAvgAry(), img.getAvg_histAry(), deBugFile);
            img.dispHist(img.getAvg_histAry(), outFile1);
            img.printHist(img.getAvg_histAry(), outFile3, deBugFile);
        } else if (opChoice == 2) {
            img.cornerPreserveAvg(deBugFile);
            outFile1.write("Result of corner-preserve averaging\n");
            img.imgReformat(img.getCPavgAry(), outFile1);
            img.imgReformat(img.getCPavgAry(), outFile2);
            img.computeHist(img.getCPavgAry(), img.getCPavg_histAry(), deBugFile);
            img.dispHist(img.getCPavg_histAry(), outFile1);
            img.printHist(img.getCPavg_histAry(), outFile3, deBugFile);
        }

        // Close all files
        inFile.close();
        outFile1.close();
        outFile2.close();
        outFile3.close();
        deBugFile.close();
    }
}


/**
 * 
compile: javac cleaned_corner_preserve.java
run    : java cleaned_corner_preserve test_input.txt 2 outFile.txt reformattedOutFile.txt histogram.txt debug.txt

 */


 /**
  This Java code implements an image enhancement class that provides methods for performing mirror framing, 
  averaging, and corner-preserve averaging on an input image. The objective is to enhance images by applying 
  different convolution masks and averaging techniques, specifically useful for smoothing images while 
  preserving edge details.

  1. Class Structure
The main class, Enhancement, includes various methods to load, process, and enhance an 
image using a mirror frame and averaging techniques.
The cleaned_corner_preserve class, containing the main method, 
acts as the driver, orchestrating file I/O and calling methods based on user choices for processing

2. Image Loading and Initialization
    - init(Scanner inFile): Initializes the image properties like number of rows 
    (numRows), columns (numCols), minimum (minVal), and maximum (maxVal) values from the file. 
    It also initializes arrays for storing the image data and results.

    - loadImage(Scanner inFile): Loads the image data into a mirror-framed array (mirrorFramedAry), 
     offsetting the array by a two-pixel boundary for applying edge operations easily.

3. Mirror Framing
    - mirrorFraming(): This function "extends" the image by mirroring pixels at the boundary. 
        This technique is commonly used in image processing to handle border effects, 
        allowing convolution operations to apply at the edges without introducing artifacts.

4. Averaging and Corner-Preserve Averaging
    - The class provides two primary enhancement techniques:
    5x5 Averaging (computeAvg5x5): This method averages the values in a 5x5 grid around each pixel, 
    helping to smooth the image by replacing each pixel value with the average of its neighbors.
    
    - Corner-Preserve Averaging (cornerPreserveAvg): This approach aims to smooth the image while preserving edges.
     It applies different convolution masks to each pixel and selects the mask output with 
     the smallest difference from the original pixel value.


5. Convolution Operation
    convolution5x5: The method takes a 5x5 mask and applies it over the image data centered at a given pixel. 
    This function calculates a weighted sum using the provided mask, enhancing specific features of the image based on mask patterns.



6. Histograms and Formatting
    - Histogram Calculation (computeHist): Computes the histogram for the resulting image, counting the frequency of each intensity value.
    - Histogram Display (dispHist and printHist): Displays the histogram in both a visual format (using + signs for frequencies) and a simplified textual format.
    - Image Reformatting (imgReformat): Reformats and writes the processed image array to an output file, displaying the image in a readable matrix format.
 

================ MAIN =======================
Program Execution Flow
The main method drives the program based on opChoice:
    If opChoice is set to 1, it performs a 5x5 average.
    If opChoice is set to 2, it performs corner-preserve averaging.

    ** It initializes the Enhancement object, loads the image, applies mirror framing, and then performs the chosen enhancement.
        Finally, it reformats and outputs the processed image, histogram data, and debugging information.


File Input and Output
    Input: The program reads the image data from test_input.txt.
    Output: Writes formatted arrays and histograms to separate output files (output1_3.txt, output2_3.txt, etc.).
            Writes debug information to debug_3.txt.




Purpose and Applications
The purpose of this code is to enhance digital images by smoothing them while preserving details, 
    especially at edges. This technique is useful in preprocessing images for applications like:

1. Noise reduction in images for better analysis.
2. Medical imaging, where edge details (such as in blood vessel analysis) need to be preserved.
3. Preparing images for further processing in machine vision tasks where smoothing without losing essential details is crucial.
    */