import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FloresL_Project2_Main {

    class BiMeans {
        // Image Header
        private int numRows;
        private int numCols;
        private int minVal;
        private int maxVal;

        // Bi-Gaussian method threshold value
        private int BiGaussThrVal;      //  Auto-selected threshold value by the Bi-Gaussian method

        // Histogram attributes
        private int histHeight;         // The largest hist[i] in the input histogram
        private int maxHeight = 0;      // The largest hist[i] within a given range of the histogram

        // Arrays
        private int[] histAry;          // Stores histogram
        private int[] GaussAry;         // Modified Gaussian curve Values
        private int[] bestFitGaussAry;  // Best biGaussian Curves
        private char[][] GaussGraph;    // 2-D array for visualizing the best-fit Gaussian curves
        private char[][] gapGraph;      // 2-D array for visualizing gaps between histogram and Gaussians
        
        // Constructor to initialize and allocate arrays
        public BiMeans(int numRows, int numCols, int minVal, int maxVal) {
            this.numRows = numRows;
            this.numCols = numCols;
            this.minVal = minVal;
            this.maxVal = maxVal;

            // Initialize arrays with the appropriate sizes
            histAry = new int[maxVal + 1];
            GaussAry = new int[maxVal + 1];
            bestFitGaussAry = new int[maxVal + 1];
        } // end BiMeans Constructor


        // loads histogram from inputfile 
        public int loadHist(String inFile, String logFileName) {
            int maxHistVal = 0;
            try (BufferedReader br = new BufferedReader(new FileReader(inFile));
                BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {

                logFile.write("Entering loadHist()\n");

                // Read the first line to get the image header information
                String[] header = br.readLine().trim().split("\\s+");

                // Store header values
                numRows = Integer.parseInt(header[0]);
                numCols = Integer.parseInt(header[1]);
                minVal = Integer.parseInt(header[2]);
                maxVal = Integer.parseInt(header[3]);

                // Re-initialize the arrays if necessary based on updated maxVal
                histAry = new int[maxVal + 1];
                GaussAry = new int[maxVal + 1];
                bestFitGaussAry = new int[maxVal + 1];

                // Read and load histogram data into histAry
                String line;
                while ((line = br.readLine()) != null) {
                    String[] parts = line.trim().split("\\s+");
                    int index = Integer.parseInt(parts[0]);
                    int value = Integer.parseInt(parts[1]);

                    // Check if index is within bounds
                    if (index >= 0 && index <= maxVal) {
                        histAry[index] = value;

                        // Keep track of the maximum histogram value
                        if (value > maxHistVal) {
                            maxHistVal = value;
                        }
                    } else {
                        logFile.write("Error: Index " + index + " out of bounds\n");
                        System.err.println("Error: Index " + index + " out of bounds");
                    }
                } // end-while loop

                histHeight = maxHistVal; // Update histHeight based on loaded data

                // Initialize the 2D arrays with blank spaces based on histHeight
                GaussGraph = new char[maxVal + 1][histHeight + 1];
                gapGraph = new char[maxVal + 1][histHeight + 1];
                setBlanks(GaussGraph, logFileName);
                setBlanks(gapGraph, logFileName);

                logFile.write("Leaving loadHist()\n");

            } catch (IOException e) {
                System.err.println("Error reading the file: " + e.getMessage());
            } // end try-catch

            return maxHistVal;
        } // end loadHist Method
        

         // Method to print the histogram counts to a file
        public void printHist(String histCountFileName, String logFileName) {
            try (BufferedWriter histCountFile = new BufferedWriter(new FileWriter(histCountFileName));
                BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {

                logFile.write("Entering printHist()\n");
                histCountFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");

                for (int i = 0; i <= maxVal; i++) {
                    histCountFile.write(i + " " + histAry[i] + "\n");
                    // Uncomment the next line to log each histogram entry
                    // logFile.write("histAry[" + i + "] = " + histAry[i] + "\n");
                }

                logFile.write("Leaving printHist()\n");

            } catch (IOException e) {
                System.err.println("Error writing to file: " + e.getMessage());
            }
        } // end printHist method


        // Method to display the histogram graphically
        public void dispHist(String histGraphFileName, String logFileName) {
            try (BufferedWriter histGraphFile = new BufferedWriter(new FileWriter(histGraphFileName));
                BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {

                logFile.write("Entering dispHist()\n");
                histGraphFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");

                for (int i = 0; i <= maxVal; i++) {
                    histGraphFile.write(i + " (" + histAry[i] + "): ");
                    for (int j = 0; j < histAry[i]; j++) {
                        histGraphFile.write("+");
                    }
                    histGraphFile.write("\n");
                    // Uncomment the next line to log each histogram entry graph
                    // logFile.write("histAry[" + i + "] = " + histAry[i] + " printed as: " + "+".repeat(histAry[i]) + "\n");
                }

                logFile.write("Leaving dispHist()\n");

            } catch (IOException e) {
                System.err.println("Error writing to file: " + e.getMessage());
            }
        } // end dispHist method


        // Method to copy contents from ary1 to ary2 
        public void copyArys(int[] ary1, int[] ary2, String logFileName) {
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering copyArys()\n");

                if (ary1.length != ary2.length) {
                    logFile.write("Error: Arrays have different lengths\n");
                    throw new IllegalArgumentException("Arrays must have the same length");
                }
                
                // Perform copy
                for (int i = 0; i < ary1.length; i++) {
                    ary2[i] = ary1[i];
                }

                logFile.write("Leaving copyArys()\n");
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            }
        } // end copyArys method


        // Method to set all elements of a 1D array to zero
        public void setZero(int[] ary, String logFileName) {
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering setZero()\n");

                // Perform zero assignment to all positions 
                for (int i = 0; i < ary.length; i++) {
                    ary[i] = 0;
                }

                logFile.write("Leaving setZero()\n");
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            }
        } // end setZero method


        // Method to set all elements of a 2D char array to blanks 
        public void setBlanks(char[][] graph, String logFileName) {
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering setBlanks()\n");

                // Method to set all positions to blanks
                for (int i = 0; i < graph.length; i++) {
                    for (int j = 0; j < graph[i].length; j++) {
                        graph[i][j] = ' ';
                    }
                }

                logFile.write("Leaving setBlanks()\n");
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            }
        } // erd setBlanks method

        //This method determines the optimal threshold that splits the histogram into two Gaussian curves,
        //* fitting the data in the best possible way. It iterates through potential threshold values, 
        // calculating the fit for each one using the `fitGauss` method. 
        public int biGaussian(int[] histAry, int[] GaussAry, int maxHeight, int minVal, int maxVal, char[][] Graph, String logFileName) {
            double sum1, sum2, total, minSumDiff;
            int offSet = (maxVal - minVal) / 10;
            int dividePt = offSet;
            int bestThr = dividePt;
            minSumDiff = Double.MAX_VALUE; // Use Double.MAX_VALUE for initialization
        
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering biGaussian method\n");
        
                // Loop through possible threshold values
                while (dividePt < (maxVal - offSet)) {
                    // Reset GaussAry in each iteration
                    setZero(GaussAry, logFileName);
        
                    // Get the sum for the first Gaussian curve
                    sum1 = fitGauss(0, dividePt, histAry, GaussAry, maxHeight, Graph, logFileName);
                    // Get the sum for the second Gaussian curve
                    sum2 = fitGauss(dividePt, maxVal, histAry, GaussAry, maxHeight, Graph, logFileName);
                    total = sum1 + sum2;
        
                    // Check if we found a better threshold
                    if (total < minSumDiff) {
                        minSumDiff = total;
                        bestThr = dividePt;
                        // Ensure this copies correctly
                        System.arraycopy(GaussAry, 0, bestFitGaussAry, 0, GaussAry.length);
                    }
        
                    // Logging current iteration details
                    logFile.write("In biGaussian (): dividePt = " + dividePt + ", sum1= " + sum1 + ", sum2= " + sum2 + ", total= " + total + ", minSumDiff = " + minSumDiff + " and bestThr= " + bestThr + "\n");
        
                    dividePt++;
                } // end while-loop
        
                logFile.write("Leaving biGaussian method, minSumDiff = " + minSumDiff + " bestThr is " + bestThr + "\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            } // end try-catch
        
            return bestThr;
        } // end biGaussian method
        

        


        //  This method computes the Gaussian curve fitting for a specific segment of the histogram.
        public double fitGauss(int leftIndex, int rightIndex, int[] histAry, int[] GaussAry, int maxHeight, char[][] Graph, String logFileName) {
            double sum = 0.0;
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering fitGauss method\n");
        
                // Calculate the mean and variance for the given segment
                double mean = computeMean(leftIndex, rightIndex, maxHeight, histAry, logFileName);
                double var = computeVar(leftIndex, rightIndex, mean, histAry, logFileName);
        
                // Loop through the range to compute the Gaussian values
                for (int index = leftIndex; index <= rightIndex; index++) {
                    double Gval = modifiedGauss(index, mean, var, maxHeight, logFileName);
                    sum += Math.abs(Gval - (double) histAry[index]);
                    GaussAry[index] = (int) Gval;  // Store Gaussian value into GaussAry
                }
        
                // Log the result of the fitting process
                logFile.write("Leaving fitGauss method, sum is: " + sum + "\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            }
        
            return sum;  // Return the sum of absolute differences
        } // end fitGauss method
        
        
        // calculates the weighted mean of pixel intensities within a specified range of the histogram and logs the process.
        public double computeMean(int leftIndex, int rightIndex, int maxHeight, int[] histAry, String logFileName) {
            int sum = 0;
            int numPixels = 0;
        
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering computeMean method\n");
        
                // Loop through the specified range to calculate the sum and number of pixels
                for (int index = leftIndex; index <= rightIndex; index++) {
                    sum += histAry[index] * index;
                    numPixels += histAry[index];
        
                    // Update maxHeight if the current value is greater
                    if (histAry[index] > maxHeight) {
                        maxHeight = histAry[index];
                    }
                }
        
                // Calculate the mean
                double result = (double) sum / (double) numPixels;
        
                // Log the results
                logFile.write("Leaving computeMean method, maxHeight = " + maxHeight + " result = " + result + "\n");
        
                return result;  // Return the computed mean
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
                return 0;  // Return 0 if there's an error
            }
        } // end computeMean method 


        //  calculates the variance of pixel intensities within a specified range of the histogram and logs the process.
        public double computeVar(int leftIndex, int rightIndex, double mean, int[] histAry, String logFileName) {
            double sum = 0.0;
            int numPixels = 0;
        
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering computeVar() method\n");
        
                // Loop through the specified range to calculate the sum for variance
                for (int index = leftIndex; index <= rightIndex; index++) {
                    sum += histAry[index] * Math.pow((index - mean), 2);
                    numPixels += histAry[index];
                }
        
                // Calculate the variance
                double result = sum / (double) numPixels;
        
                // Log the result
                logFile.write("Leaving computeVar method, returning result = " + result + "\n");
        
                return result;  // Return the computed variance
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
                return 0;  // Return 0 if there's an error
            } // end try-catch
        } // end computeVar method 
        

        //  computes the modified Gaussian value for a given x, using the mean, variance, and maximum height of the histogram.
        public double modifiedGauss(int x, double mean, double var, int maxHeight, String logFileName) {
            double result = maxHeight * Math.exp(-Math.pow((x - mean), 2) / (2 * var));
        
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering modifiedGauss method\n");
                logFile.write("Computed modified Gaussian value for x = " + x + " with mean = " + mean + ", variance = " + var + ", maxHeight = " + maxHeight + "\n");
                logFile.write("Resulting Gaussian value = " + result + "\n");
                logFile.write("Leaving modifiedGauss method\n");
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            } // end try-catch
        
            return result;
        } // end modifiedGauss method
        
        
        public void printBestFitGauss(int[] bestFitGaussAry, String GaussFileName, String logFileName) {
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true));
                 BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
        
                logFile.write("Entering printBestFitGauss method\n");
        
                // Output the bestFitGaussAry to the GaussFile
                for (int i = 0; i <= maxVal; i++) {
                    GaussFile.write(i + " " + bestFitGaussAry[i] + "\n");
                }
        
                logFile.write("Leaving printBestFitGauss method\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to file: " + e.getMessage());
            } // end try-catch
        } // end printBestFitGauss method        


        // plots the best-fit Gaussian array onto a 2D graph using '*' characters and logs the process.
        public void plotGaussGraph(int[] bestFitGaussAry, char[][] GaussGraph, String logFileName) {
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering plotGaussGraph() method\n");
        
                // Clear the GaussGraph by setting all elements to blanks
                setBlanks(GaussGraph, logFileName);
        
                // Loop through the bestFitGaussAry to plot the graph
                for (int index = 0; index <= maxVal; index++) {
                    int height = bestFitGaussAry[index];
                    for (int i = 0; i < height; i++) {
                        if (i < GaussGraph[index].length) {
                            GaussGraph[index][GaussGraph[index].length - 1 - i] = '*';  // Plot '*' for each value in reverse order
                        }
                    }
                }
        
                logFile.write("Leaving plotGaussGraph() method\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            } // end try-catch
        } // end plotGaussGraph method
        


        //  outputs the GaussGraph to a specified file, including the image header
        public void dispGaussGraph(char[][] GaussGraph, String GaussFileName, String logFileName) {
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true));
                 BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
        
                logFile.write("Entering dispGaussGraph() method\n");
        
                // Write the image header to the file
                GaussFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        
                // Output the GaussGraph to the GaussFile
                for (int i = 0; i < GaussGraph[0].length; i++) {
                    for (int j = 0; j < GaussGraph.length; j++) {
                        GaussFile.write(GaussGraph[j][i]); // Swap the indices to match expected output format
                    }
                    GaussFile.write("\n"); // Newline after each row
                }
        
                logFile.write("Leaving dispGaussGraph() method\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to file: " + e.getMessage());
            }
        } // end dispGaussGraph method        


        // plots the gaps between the histogram and the best-fitted Gaussian curves onto a 2D graph using '@' characters
        public void plotGapGraph(int[] histAry, int[] bestFitGaussAry, char[][] gapGraph, String logFileName) {
            try (BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
                logFile.write("Entering plotGapGraph() method\n");
        
                // Clear the gapGraph by setting all elements to blanks
                setBlanks(gapGraph, logFileName);
        
                // Loop through the range to plot the gaps
                for (int index = 0; index <= maxVal; index++) {
                    int end1, end2;
        
                    if (bestFitGaussAry[index] <= histAry[index]) {
                        end1 = bestFitGaussAry[index];
                        end2 = histAry[index];
                    } else {
                        end1 = histAry[index];
                        end2 = bestFitGaussAry[index];
                    }
        
                    // Ensure i is within bounds of gapGraph's second dimension
                    for (int i = 0; i < end2; i++) {
                        if (i >= end1 && i < gapGraph[index].length) {
                            gapGraph[index][gapGraph[index].length - 1 - i] = '@'; // Plot '@' for the gap in reverse order
                        }
                    }
                }
        
                logFile.write("Leaving plotGapGraph() method\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to log file: " + e.getMessage());
            }
        } // end plotGapGraph method
        

        // outputs the gapGraph to a specified file, including the image header, and logs the process.
        public void dispGapGraph(char[][] gapGraph, String GaussFileName, String logFileName) {
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true));
                 BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName, true))) {
        
                logFile.write("Entering dispGapGraph() method\n");
        
                // Write the image header to the file
                GaussFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        
                // Output the gapGraph to the GaussFile
                for (int i = 0; i < gapGraph[0].length; i++) {
                    for (int j = 0; j < gapGraph.length; j++) {
                        GaussFile.write(gapGraph[j][i]); // Swap the indices to match expected output format
                    }
                    GaussFile.write("\n"); // Newline after each row
                }
        
                logFile.write("Leaving dispGapGraph() method\n");
        
            } catch (IOException e) {
                System.err.println("Error writing to file: " + e.getMessage());
            }
        } // end dispGapGraph method        
    } // end class BiMeans

    public static void main(String[] args) {
        // Step 0: Check if argc count is correct and each file can be opened.
        if (args.length != 4) {
            System.err.println("Usage: java FloresL_Project2_Main <inFile> <histFile> <GaussFile> <logFile>");
            return;
        }
    
        String inFile = args[0];
        String histFileName = args[1]; // This will be used now
        String GaussFileName = args[2];
        String logFileName = args[3];
    
        try {
            // Load histogram data to get image header values first
            BufferedReader br = new BufferedReader(new FileReader(inFile));
            String[] header = br.readLine().trim().split("\\s+");
            int numRows = Integer.parseInt(header[0]);
            int numCols = Integer.parseInt(header[1]);
            int minVal = Integer.parseInt(header[2]);
            int maxVal = Integer.parseInt(header[3]);
            br.close();
    
            // Step 1: Initialize and read input data
            BiMeans biMeans = new FloresL_Project2_Main().new BiMeans(numRows, numCols, minVal, maxVal);
    
            // Load histogram data
            biMeans.histHeight = biMeans.loadHist(inFile, logFileName);
    
            // Output the histogram data in numeric format (i)
            biMeans.printHist(histFileName, logFileName);
    
            // Display the histogram data in 2-D format (ii)
            biMeans.dispHist(histFileName, logFileName);
    
            // Step 3: Perform Bi-Gaussian thresholding
            biMeans.BiGaussThrVal = biMeans.biGaussian(biMeans.histAry, biMeans.GaussAry, biMeans.histHeight, biMeans.minVal, biMeans.maxVal, biMeans.GaussGraph, logFileName);
    
            // Write the selected threshold value to the GaussFile
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true))) {
                GaussFile.write("** The selected threshold value is " + biMeans.BiGaussThrVal + "\n");
                GaussFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
            }
    
            // Step 4: Output the best-fit Gaussian array in numerical format
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true))) {
                GaussFile.write("** Below is the bestFitGaussAry array **\n");
            }
            biMeans.printBestFitGauss(biMeans.bestFitGaussAry, GaussFileName, logFileName);
    
            // Step 5: Plot and display the best-fit Gaussians
            biMeans.plotGaussGraph(biMeans.bestFitGaussAry, biMeans.GaussGraph, logFileName);
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true))) {
                GaussFile.write("** Above is the 2-D display of the best fitted Gaussians (with *) **\n");
                biMeans.dispGaussGraph(biMeans.GaussGraph, GaussFileName, logFileName);
            }
    
            // Step 6: Plot and display the gaps between the histogram and the best-fitted Gaussians
            biMeans.plotGapGraph(biMeans.histAry, biMeans.bestFitGaussAry, biMeans.gapGraph, logFileName);
            try (BufferedWriter GaussFile = new BufferedWriter(new FileWriter(GaussFileName, true))) {
                GaussFile.write("** Above displays the gaps between the histogram and the best fitted Gaussians (with @) **\n");
                biMeans.dispGapGraph(biMeans.gapGraph, GaussFileName, logFileName);
            }
    
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
        }
    } // end main        
} // End FloresL_Project2_Main.java class
