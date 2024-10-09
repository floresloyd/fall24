#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
using namespace std;

class Enhancement {
    public:
        // Declare Instance Variables
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int maskRows;
        int maskCols;
        int maskMin;
        int maskMax;
        int newMin;
        int newMax;
        int thrVal;

        // Dynamically allocated 2D arrays
        int** mirrorFmAry;
        int** avgAry;
        int** medianAry;
        int** GaussAry;
        int** thrAry;
        int** mask2DAry;

        // 1D arrays
        int mask1DAry[9];
        int neighbor1DAry[9];

        int maskWeight;

        Enhancement(int rows, int cols, int maskRows, int maskCols, int threshold)
            : 
            numRows(rows), 
            numCols(cols), 
            maskRows(maskRows), 
            maskCols(maskCols), 
            thrVal(threshold), 
            newMin(9999), 
            newMax(0) {
                // Instantiate dynamic arrays with + 2 for mirror framing by calling a helper function
                mirrorFmAry = allocate2DArray(numRows + 2, numCols + 2);
                avgAry = allocate2DArray(numRows + 2, numCols + 2);
                medianAry = allocate2DArray(numRows + 2, numCols + 2);
                GaussAry = allocate2DArray(numRows + 2, numCols + 2);
                thrAry = allocate2DArray(numRows + 2, numCols + 2);
                mask2DAry = allocate2DArray(maskRows, maskCols); 

                // Make the Dynamic arrays only have zeroes
                for (int i = 0; i < 9; i++) {
                    mask1DAry[i] = 0;
                    neighbor1DAry[i] = 0;
                }
        }//End-Enhancement()-constructor

        ~Enhancement() {
            deallocate2DArray(mirrorFmAry, numRows + 2);
            deallocate2DArray(avgAry, numRows + 2);
            deallocate2DArray(medianAry, numRows + 2);
            deallocate2DArray(GaussAry, numRows + 2);
            deallocate2DArray(thrAry, numRows + 2);
            deallocate2DArray(mask2DAry, maskRows);
        }//End-Ehancement-deconstructor

        int** allocate2DArray(int rows, int cols) {
            int** array = new int*[rows];
            for (int i = 0; i < rows; i++) {
                array[i] = new int[cols];
                for (int j = 0; j < cols; j++) {
                    array[i][j] = 0;
                }//j
            }//i
            return array;
        }//end-allocate2DArray()

        void deallocate2DArray(int** array, int rows) {
            for (int i = 0; i < rows; i++) {
                delete[] array[i];
            }
            delete[] array;
        }//end-deallocate2DArray()

        /**
         * @binThreshold
         * This method takes in a matrix and performs Binary Thresholding
         * 
         * @param inAry the original input matrix
         * @param outAry the binary threshold modified matrix
         * @param numRows the number of rows for the input matrix
         * @param numCols the number of cols for the input matrix
         * @param thrVal threshold value to perform binary thresholding on
         * @param logFile where the log will be displayed 
         */
        void binThreshold(int** inAry, int** outAry, int numRows, int numCols, int thrVal, ofstream& logFile) {
            logFile << "*** Entering binThreshold method ***" << endl;

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (inAry[i][j] > thrVal) {
                        outAry[i][j] = 1;
                    } else {
                        outAry[i][j] = 0;
                    }//end-if-else
                }//j
            }//i

            logFile << "*** Exiting binThreshold method ***" << endl;
        }//end-binThreshold()

        /**
         * @prettyPrint 
         * this method takes in a 2D Matrix and prints it out with even spacing
         * 
         * @param imgAry The 2D matrix to be pretty printed
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param logFile where the log will be displayed          
         */
        void prettyPrint(int** imgAry, int rows, int cols, ofstream& logFile) {
            logFile << "*** Enter PrettyPrint() ***" << endl;
            logFile << rows << " " << cols << " " << minVal << " " << maxVal << "\n";

            string maxValStr = to_string(maxVal);
            int Width = maxValStr.length();

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    logFile << " " << imgAry[i][j] << " ";
                }//j
                logFile << "\n";
            }//i
            logFile << "*** Leaving PrettyPrint() ***" << endl << endl;
        }//end-prettyPrint()

        /**
         * @mirrorFraming 
         * This method takes in a 2D matrix and populates the frame with pixel values 
         * 
         * @param mirrorFmAry Input 2D Matrix
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param logFile where the log will be displayed          
         */
        void mirrorFraming(int** mirrorFmAry, int numRows, int numCols, ofstream& logFile) {
            logFile << "*** Entering mirrorFraming() ***" << endl;

            // Populate top most & bottom most row or the frame | mirrorFmAry[0][j] & mirrorFmAry[numRows + 1][j]
            for (int j = 1; j <= numCols; j++) {
                mirrorFmAry[0][j] = mirrorFmAry[1][j];
                mirrorFmAry[numRows + 1][j] = mirrorFmAry[numRows][j];
            }

            // Populate left most & right most column of the frame | mirrorFmAry[i][0] & mirrorFmAry[i][numCols + 1]
            for (int i = 1; i <= numRows; i++) {
                mirrorFmAry[i][0] = mirrorFmAry[i][1];
                mirrorFmAry[i][numCols + 1] = mirrorFmAry[i][numCols];
            }

            // Populate corners 
            mirrorFmAry[0][0] = mirrorFmAry[1][1];
            mirrorFmAry[0][numCols + 1] = mirrorFmAry[1][numCols];
            mirrorFmAry[numRows + 1][0] = mirrorFmAry[numRows][1];
            mirrorFmAry[numRows + 1][numCols + 1] = mirrorFmAry[numRows][numCols];

            logFile << "Mirror Framing With Corners Complete!" << endl;
            logFile << "*** Exiting mirrorFraming() ***";
        } //end-mirrorFraming

        /**
         * @loadImage
         * This method takes the inputFile and loads it into a 2D matrix that can be manipulated
         * 
         * @param inputFile File name of input image
         * @param mirrorFmAry 2D matrix to store pixel values of the input image
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param logFile where the log will be displayed  
         */
        void loadImage(string inputFile, int** mirrorFmAry, int numRows, int numCols, ofstream& logFile) {
            logFile << "*** Entering loadImage ***" << endl;

            // Error Handle when loading in file
            ifstream inFile(inputFile);
            if (!inFile) {
                logFile << "Error: Cannot open input file!" << endl;
                cerr << "Error: Cannot open input file!" << endl;
                return;
            }

            // Read the header, which includes numRows, numCols, minVal, maxVal
            int imgHeader1, imgHeader2, imgHeader3, imgHeader4;
            inFile >> imgHeader1 >> imgHeader2 >> imgHeader3 >> imgHeader4;
            numRows = imgHeader1;
            numCols = imgHeader2;
            minVal = imgHeader3; // Set minVal from the header
            maxVal = imgHeader4; // Set maxVal from the header

            logFile << "Header: " << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

            // Read input and populate matrix
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    inFile >> mirrorFmAry[i][j];
                    logFile << "mirrorFmAry[" << i << "][" << j << "] = " << mirrorFmAry[i][j] << endl;
                }
            }

            logFile << "*** Leaving loadImage() method ***" << endl;
            inFile.close();
        }//end-LoadImage

        /**
         * @loadMask 
         * This method loads the Gaussian Mask and saves it into a 2D matrix that can be manipulated 
         * @param maskFile File name of the Gaussian 
         * @param mirrorFmAry 2D matrix to store pixel values of the input image
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param logFile where the log will be displayed  
         */
        int loadMask(string maskFile, int** mask2DAry, int maskRows, int maskCols, ofstream& logFile) {
            logFile << "*** Entering loadMask() ***" << endl;

            // Error Handle when loading Mask File
            ifstream mskFile(maskFile);
            if (!mskFile) {
                logFile << "Error: Cannot open mask file!" << endl;
                return -1;
            }

            // Load Headers
            int maskHeader1, maskHeader2;
            mskFile >> maskHeader1 >> maskHeader2;
            logFile << "Mask Header: " << maskHeader1 << " " << maskHeader2 << endl;

            //  Iterate over the Gaussian Mask to find the total value and weight 
            int maskWeight = 0;
            for (int i = 0; i < maskRows; i++) {
                for (int j = 0; j < maskCols; j++) {
                    mskFile >> mask2DAry[i][j];
                    maskWeight += mask2DAry[i][j];
                    logFile << "mask2DAry[" << i << "][" << j << "] = " << mask2DAry[i][j] << endl;
                }//j
            }//i

            logFile << "Mask weight is: " << maskWeight << endl;
            logFile << "*** Exiting loadMask() ***" << endl;
            return maskWeight;
        }//end-loadMask()


        /**
         * @loadMask2Dto1D 
         * This method Reduces the matrix from 2D to 1D, which is used for the Gaussian Mask
         * @param mask2DAry 2D Gaussian Mask
         * @param maskRows number of rows for the gaussian mask
         * @param maskCols number of cols for the gaussian mask
         * @param mask1DAry This array will hold the Gaussian mask values in 1D space
         * @param logFile where the log will be displayed  
         */
        void loadMask2Dto1D(int** mask2DAry, int maskRows, int maskCols, int mask1DAry[], ofstream& logFile) {
            logFile << "*** Entering loadMask2Dto1D() ***" << endl;

            // Declare index as this will be our pointer to iterate over the 1D array
            int index = 0;

            for (int i = 0; i < maskRows; i++) {
                for (int j = 0; j < maskCols; j++) {
                    mask1DAry[index] = mask2DAry[i][j]; // Store value
                    logFile << "mask1DAry[" << index << "] = mask2DAry[" << i << "][" << j << "] = " << mask2DAry[i][j] << endl;
                    index++;                            // Move pointer to iterate 1D Array
                }//j
            }//i
            logFile << "*** Exiting loadMask2Dto1D() ***" << endl;
        }//end-loadMask2Dto1D

        /**
         * @loadNeighbor2Dto1D
         *  This method Reduces the matrix from 2D to 1D, which used when performing computations on neighborhood operations 
         * @param mirrorFrmAry 2D Matrix containing pixel values
         * @param i             Current row
         * @param i             Current col
         * @param neighbor1DAry Will hold all values within the neighborhood in 1D Space
         * @param logFile where the log will be displayed  
         */
        void loadNeighbor2Dto1D(int** mirrorFrmAry, int i, int j, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering loadNeighbor2Dto1D() ***" << endl;
            
            /// Declare index as this will be our pointer to iterate over the 1D array
            int index = 0;

            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    neighbor1DAry[index] = mirrorFmAry[r][c]; // Store value 
                    logFile << "neighbor1DAry[" << index << "] = mirrorFmAry[" << r << "][" << c << "] = " << mirrorFmAry[r][c] << endl;
                    index++;                                  // Move pointer Iterate 1D array 
                }//j
            }//i
            logFile << "*** Exiting loadNeighbor2Dto1D() ***" << endl;
        }//end-loadNeighbor2Dto1D

        /**
         * @sort
         * Sorts 1D Array using bubble sort
         * @param neighborAry Array holding to be sorted the values of a neighborhood 
         * @param logFile where the log will be displayed  
         */
        void sort(int neighborAry[], ofstream& logFile) {
            logFile << "*** Entering sort() ***" << endl;

            for (int i = 0; i < 9 - 1; i++) {
                for (int j = 0; j < 9 - i - 1; j++) {
                    if (neighborAry[j] > neighborAry[j + 1]) {
                        swap(neighborAry[j], neighborAry[j + 1]);
                    }
                }
            }
            logFile << "*** Exiting sort() ***" << endl;
        }//end-sort()

        /**
         * @computeMedian 
         * This method computes the median by Storing all the values of neighborhood in a 1D array by utilizing a helper function.
         * 
         * @param mirrorFmAry 2D Matrix containing pixel values
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param neighbor1DAry 1D array used to store neighborhood values which will be passed to the helper function
         * @param logFile where the log will be displayed  
         */
        void computeMedian(int** mirrorFmAry, int numRows, int numCols, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering computeMedian() ***" << endl;

            // Iterate over the entire matrix
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    // Load Neighborhood into an array with i,j being the center
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile);
                    logFile << "Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    // Sort to find median
                    sort(neighbor1DAry, logFile);
                    logFile << "Below is the sorted Array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    // Change i,j into the median value of neighborhood
                    medianAry[i][j] = neighbor1DAry[4];

                    if (this->newMin > avgAry[i][j]) {  
                        this->newMin = avgAry[i][j];
                    }
                    if (this->newMax < avgAry[i][j]) {
                        this->newMax = avgAry[i][j];
                    }

                }//j
            }//i

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Exiting computeMedian() ***" << endl;
        }//end-computeMedian()

        /**
         * @computeMean 
         * This method computes the mean by Storing all the values of neighborhood in a 1D array by utilizing a helper function.
         * 
         * @param mirrorFmAry 2D Matrix containing pixel values
         * @param rows The number of rows for the input matrix
         * @param cols The number of cols for the input matrix
         * @param neighbor1DAry 1D array used to store neighborhood values which will be passed to the helper function
         * @param logFile where the log will be displayed  
         */
        void computeMean(int** mirrorFmAry, int numRows, int numCols, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering computeMean() ***" << endl;
            
            int mean = 0; // Will be used to store mean values over the course of the method 

            // Iterate through each pixel as the center 
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile); // Squeeze neighborhood into 1D space with i,j as the center
                    logFile << "Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    mean = 0; 
                    for (int k = 0; k <= 8; k++) {
                        // Add all values within the neighborhood 
                        mean += neighbor1DAry[k];
                    }
                    mean = mean / 9;

                    logFile << "The mean for mirrorFmAry[" << i << "][" << j << "] is " << mean << endl;
                    
                    // Assign the mean
                    avgAry[i][j] = mean;

                    if (this->newMin > avgAry[i][j]) {
                        this->newMin = avgAry[i][j];
                    }
                    if (this->newMax < avgAry[i][j]) {
                        this->newMax = avgAry[i][j];
                    }

                }//j
            }//i

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Exiting computeMean() ***" << endl;
        }//end-computeMean()

        /**
         * @computeGauss
         * This method computes the mean with the Gaussian mask applied by Storing all the values of neighborhood in a 1D array by utilizing 2 helper functions.
         * 
         * @param mirrorFmAry 2D Matrix containing pixel values
         * @param GaussAry    2D Gaussian mask
         * @param rows        The number of rows for the input matrix
         * @param cols        The number of cols for the input matrix
         * @param mask1DAry   Will hold Gaussian Mask values after being squeezed into 1D Space
         * @param maskWeight  Hold the total weight of Gaussian mask  
         * @param logFile where the log will be displayed  
         */
        void computeGauss(int** mirrorFmAry, int** GaussAry, int numRows, int numCols, int mask1DAry[], int maskWeight, ofstream& logFile) {
            logFile << "*** Entering computeGauss method ***" << endl;
            
            // Will hold Gaussian Mask values in 1D
            int neighbor1DAry[9];

            // Iterate over each pixel value in 2D matrix
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile); // Squeeze neighborhood to 1D Space
                    logFile << "** Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    logFile << "** Below is mask1DAry **" << endl;
                    print1DAry(mask1DAry, logFile);

                    // Helper function to find Mean with Gaussian Mask applied
                    GaussAry[i][j] = convolution(neighbor1DAry, mask1DAry, maskWeight, logFile);

                    if (this->newMin > avgAry[i][j]) {
                        this->newMin = avgAry[i][j];
                    }
                    if (this->newMax < avgAry[i][j]) {
                        this->newMax = avgAry[i][j];
                    }

                }//j
            }//i

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Leaving computeGauss method ***" << endl;
        }//end-computeGauss()

        /**
         * @convolution 
         * This method applies the gaussian mask and then finds the mean. It is mainly a helper function to computeGauss
         * @param neighbor1DAry This holds the entire neighborhood in 1D Space
         * @param mask1DAry     This holds mask values in 1D Space
         * @param maskWeight    Total weight of Gaussian mask
         * @param logFile       where the log will be displayed  
         */
        int convolution(int neighbor1DAry[], int mask1DAry[], int maskWeight, ofstream& logFile) {
            logFile << "*** Entering convolution method ***" << endl;
            
            int result = 0;

            // Apply Gaussian mask and store the entire results
            for (int i = 0; i < 9; i++) {
                result += neighbor1DAry[i] * mask1DAry[i];
            }

            if (maskWeight > 0) {
                // Find Mean with Gaussian Mask weight
                result = result / maskWeight;
            } else {
                logFile << "Warning: Mask weight is zero!" << endl;
            }

            // Handles edge cases for when values have unexpected behavior
            result = min(max(result, minVal), maxVal);

            logFile << "Convolution result: " << result << " (maskWeight = " << maskWeight << ")" << endl;
            logFile << "*** Leaving convolution method ***" << endl;

            return result;
        }//end-computeGauss()
        
        /**
         * @print1DAry 
         * This method prints out the 1D array into the logfile
         * 
         * @param neighbor1DAry 1D Array to be logged
         * @param logFile where the log will be displayed  
         */
        void print1DAry(int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering print1DAry ***" << endl;
            logFile << "Array = [";

            // Iterate over 1D Array
            for (int i = 0; i <= 8; i++) {
                logFile << neighbor1DAry[i] << ", "; // Log output 
            }
            logFile << "]" << endl;
        }
};

using namespace std;

int main(int argc, char* argv[]) {
    // Step 0: Check if argc count is correct | Error Handle Args
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " <inputImageFile> <maskFile> <threshold> <avgOutFile> <medianOutFile> <gaussOutFile>. Logfile will be generated automatically" << endl;
        return -1;
    }

    // Open the input and output files
    ifstream inFile(argv[1]);
    ifstream maskFile(argv[2]);
    int thrVal = atoi(argv[3]); // threshold value from command line

    ofstream avgOutFile(argv[4]);       // Mean Filtering output file
    ofstream medianOutFile(argv[5]);    // Median Filtering output file
    ofstream gaussOutFile(argv[6]);     // Gaussian Filtering output file
    ofstream logFile("logFile.txt");    // Create LogFile

    // Check if files were successfully opened | Error Handling input
    if (!inFile || !maskFile || !avgOutFile || !medianOutFile || !gaussOutFile) {
        cerr << "Error: Unable to open one or more files." << endl;
        return -1;
    }

    // Step 1: Read Headers: numRows, numCols, minVal, maxVal from inFile
    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    // Step 1: Read Gaussian Mask Header: maskCols, maskMin, maskMax from maskFile
    int maskRows, maskCols, maskMin, maskMax;
    maskFile >> maskRows >> maskCols >> maskMin >> maskMax;

    // Step 2: Dynamically allocate all 1-D and 2-D arrays
    Enhancement enhancement(numRows, numCols, maskRows, maskCols, thrVal);

    // Step 3: Load the mask and calculate maskWeight
    int maskWeight = enhancement.loadMask(argv[2], enhancement.mask2DAry, maskRows, maskCols, logFile);
    enhancement.loadMask2Dto1D(enhancement.mask2DAry, maskRows, maskCols, enhancement.mask1DAry, logFile);

    // Step 4: Load the image
    enhancement.loadImage(argv[1], enhancement.mirrorFmAry, numRows, numCols, logFile);

    // Step 5: Perform mirror framing
    enhancement.mirrorFraming(enhancement.mirrorFmAry, numRows, numCols, logFile);

    // Step 6: MEAN FILTERING | Perform 3x3 averaging and save to avgOutFile 
    avgOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, avgOutFile);

    enhancement.computeMean(enhancement.mirrorFmAry, numRows, numCols, enhancement.neighbor1DAry, logFile);
    avgOutFile << "** Below is the 3x3 averaging of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.avgAry, numRows + 2, numCols + 2, avgOutFile);

    enhancement.binThreshold(enhancement.avgAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    avgOutFile << "** Below is the result of the binary threshold of averaging image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows + 2, numCols + 2, avgOutFile);

    // Step 7: MEDIAN FILTERING | Perform 3x3 median filtering and save to medianOutFile
    medianOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, medianOutFile);

    enhancement.computeMedian(enhancement.mirrorFmAry, numRows, numCols, enhancement.neighbor1DAry, logFile);
    medianOutFile << "** Below is the 3x3 median filter of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.medianAry, numRows + 2, numCols + 2, medianOutFile);

    enhancement.binThreshold(enhancement.medianAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    medianOutFile << "** Below is the result of the binary threshold of median filtered image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows + 2, numCols + 2, medianOutFile);

    // Step 8: GAUSSIAN FILTERING | Perform 3x3 Gaussian filtering and save to gaussOutFile
    gaussOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, gaussOutFile);

    gaussOutFile << "** Below is the mask for Gaussian filter. ***" << endl;
    enhancement.prettyPrint(enhancement.mask2DAry, maskRows, maskCols, gaussOutFile);

    enhancement.computeGauss(enhancement.mirrorFmAry, enhancement.GaussAry, numRows, numCols, enhancement.mask1DAry, maskWeight, logFile);
    gaussOutFile << "** Below is the 3x3 Gaussian filter of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.GaussAry, numRows + 2, numCols + 2, gaussOutFile);

    enhancement.binThreshold(enhancement.GaussAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    gaussOutFile << "** Below is the result of the binary threshold of Gaussian filtered image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows + 2, numCols + 2, gaussOutFile);

    // Step 9: Close all files
    inFile.close();
    maskFile.close();
    avgOutFile.close();
    medianOutFile.close();
    gaussOutFile.close();
    logFile.close();

    return 0;
}