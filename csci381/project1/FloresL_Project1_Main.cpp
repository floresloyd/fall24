#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Image {
    public:
        int numRows;   
        int numCols;
        int minVal;
        int maxVal;
        int* histAry;  // Dynamic 1D Integer array, size of maxVal + 1;
        int** imgAry;  // Dynamic 2D Integer array size: (numRows x numCols);
        int thrVal;   
        
        // Default constructor to initialize variables
        Image() : numRows(0), numCols(0), minVal(0), maxVal(0), histAry(nullptr), imgAry(nullptr), thrVal(0) {}

        // New constructor to initialize variables and allocate memory for imgAry and histAry
        Image(int rows, int cols, int maxVal) : numRows(rows), numCols(cols), minVal(0), maxVal(maxVal) {
            // Allocate memory for imgAry
            imgAry = new int*[numRows];
            for (int i = 0; i < numRows; i++) {
                imgAry[i] = new int[numCols];
            }

            // Allocate memory for histAry and initialize to 0
            histAry = new int[maxVal + 1];
            for (int i = 0; i <= maxVal; i++) {
                histAry[i] = 0;
            }
        }

        // Destructor to deallocate memory
        ~Image() {
            if (imgAry != nullptr) {
                for (int i = 0; i < numRows; i++) {
                    delete[] imgAry[i];
                }
                delete[] imgAry;
            }
            if (histAry != nullptr) {
                delete[] histAry;
            }
        }

        // Function to load image 
        void loadImage(ifstream& inFile, ofstream& logFile){
            logFile << "Entering loadImage() \n";
            
            // Reading from the input file into the already allocated imgAry
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    inFile >> imgAry[i][j];
                    //logFile << "imgAry[" << i << "][" << j << "] = " << imgAry[i][j] << "\n"; // Log pixel values     // LOGFILE TURNED OFF TO REDUCE LOG OUTPUT
                }
            }

            logFile << "Image loaded. \n";
        } // end void loadImage()

        // Function to compute histogram
        void computeHist(ofstream& logFile){
            logFile << "Entering computeHist()\n";

            // count pixel intensities
            for (int i = 0; i < this->numRows; i++) {
                for (int j = 0; j < this->numCols; j++) {
                    int val = imgAry[i][j];
    
                    // Error handle 
                    if (val < this->minVal || val > this->maxVal) {
                        //logFile << "imgAry[" << i << "][" << j << "] value is not within minVal and maxVal\n";      // LOGFILE TURNED OFF TO REDUCE LOG OUTPUT 
                        exit(1);
                    }
                    
                    histAry[val]++;
                    // logFile << "histAry[" << val << "] = " << histAry[val] << "\n"; // Log histogram values       // LOGFILE TURNED OFF TO REDUCE LOG OUTPUT 
                }
            }
                        
            logFile << "Leaving computeHist()\n";
        }// end void computeHist()

        // Print the histogram counts
        void printHist(ofstream& histCountFile, ofstream& logFile) {
            logFile << "Entering printHist() \n";
            histCountFile << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";

            for (int i = 0; i <= maxVal; i++) {
                histCountFile << i << " " << histAry[i] << "\n";
                //logFile << "histAry[" << i << "] = " << histAry[i] << "\n"; // Log counts                        // LOGFILE TURNED OFF TO REDUCE LOG OUTPUT
            }
            
            logFile << "Leaving printHist() \n";
        } // end printHist()

        // Display histogram graphically
        void dispHist(ofstream& histGraphFile, ofstream& logFile) { 
            logFile << "Entering dispHist() \n";
            histGraphFile << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";

            for (int i = 0; i <= maxVal; i++) {
                histGraphFile << i << " (" << histAry[i] << "): ";
                for (int j = 0; j < histAry[i]; j++) {
                    histGraphFile << "+";
                }
                histGraphFile << "\n";
                //logFile << "histAry[" << i << "] = " << histAry[i] << " printed as: " << string(histAry[i], '+') << "\n"; // Log graph // LOGFILE TURNED OF TO REDUCE LOG OUTPUT
            }

            logFile << "Leaving dispHist() \n";
        } // end dispHist()

        // Binary thresholding with more logging
        void binaryThreshold(ofstream& binThrFile, ofstream& logFile) {
            logFile << "Entering binaryThreshold() \n";
            logFile << "Threshold value: " << thrVal << "\n";

            binThrFile << numRows << " " << numCols << " 0 1 \n";
            logFile << numRows << " " << numCols << " 0 1 \n";

            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    if (imgAry[i][j] >= thrVal) {
                        binThrFile << "1 ";
                        logFile << "1 "; // Log threshold result
                    } else {
                        binThrFile << "0 ";
                        logFile << "0 "; // Log threshold result
                    }
                }
                binThrFile << "\n";
                logFile << "\n";
            }

            logFile << "Leaving binaryThreshold() \n";
        } // end binary binaryThreshold()

        // Pretty print function with detailed logging
        void prettyPrint(ofstream& logFile) {
            logFile << "Enter PrettyPrint()\n";
            logFile << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";

            string maxValStr = to_string(maxVal);
            int Width = maxValStr.length();

            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    string valStr = to_string(imgAry[i][j]);
                    logFile << imgAry[i][j] << " ";
                }
                logFile << "\n";
            }

            logFile << "Leaving PrettyPrint()\n";
        }// end prettyPrint()
};

int main(int argc, char* argv[]){
    // Step 0: Check if the number of arguments is correct
    if (argc != 7) {
        cerr << "Wrong number of arguments";
        return 1; 
    }

    // Open Input and output files
    ifstream inFile(argv[1]);        // Input file
    ofstream histCountFile(argv[3]); // Histogram count output file
    ofstream histGraphFile(argv[4]); // Histogram graph output file
    ofstream binThrFile(argv[5]);    // Binary threshold file 
    ofstream logFile(argv[6]);       // Log file

    // Check if all files are opened successfully 
    if (!inFile || !histCountFile || !histGraphFile || !binThrFile || !logFile){
        cerr << "Error: Could not open one or more files. \n";
        return 1;
    }
    logFile << "All Files opened successfully. \n";

    // Step 1: Read the image header (rows, cols, minVal, maxVal) from inFile
    int numRows, numCols, minVal, maxVal;
    if (!(inFile >> numRows >> numCols >> minVal >> maxVal)) {
        cerr << "Error: Failed to read image header.\n";
        return 1;
    }

    // Step 2: Create image object and dynamically allocate space for imgAry
    Image img(numRows, numCols, maxVal);   // Initialize imgAry and histAry in the constructor
    img.minVal = minVal;  // Set minVal to class member

    // Step 3: Load the image from infile into imgAry
    img.loadImage(inFile, logFile);   // Load image data into imgAry

    // Step 4: Pretty print the image
    img.prettyPrint(logFile);         // Pretty print the image

    // Step 5: Compute histogram of image
    img.computeHist(logFile);         // Compute histogram

    // Step 6: Print histogram counts to histCountFile
    img.printHist(histCountFile, logFile);  // Print histogram
    
    // Step 7: Display histogram to histGraphFile
    img.dispHist(histGraphFile, logFile);   // Display histogram
   
    // Step 8: Get threshold value from argv[2] and log it
    img.thrVal = atoi(argv[2]);  // Assign threshold value to class member
    logFile << "The threshold value = " << img.thrVal << "\n";

    // Step 9: Perform BinaryThresholding
    img.binaryThreshold(binThrFile, logFile);  // Perform binary thresholding

    // Step 10: Close all files
    inFile.close();
    histCountFile.close();
    histGraphFile.close();
    binThrFile.close();
    logFile.close();

    return 0;
} // end main

/*
NOTES:
    input: 
        1. inFile 1/2       [1]
        2. threshold        [2]

    output:
        1. histCountFile    [3]
        2. histGraphFile    [4]
        3. binThrFile       [5]
        4. logFile          [6]
 */
