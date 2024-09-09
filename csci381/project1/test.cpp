#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <limits>  // Add this header for std::numeric_limits


class Image {
public:
    int numRows, numCols, minVal, maxVal;
    int** imgAry;
    int* histAry;
    int thrVal;

    // Constructor to initialize image and histogram arrays
    Image(int rows, int cols, int min, int max) {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;

        // Allocate memory for the image array and initialize it
        imgAry = new int* [numRows];
        for (int i = 0; i < numRows; i++) {
            imgAry[i] = new int[numCols];
        }

        // Allocate memory for the histogram array and initialize it
        histAry = new int[maxVal + 1]();
    }

    // Destructor to free dynamically allocated memory
    ~Image() {
        for (int i = 0; i < numRows; i++) {
            delete[] imgAry[i];
        }
        delete[] imgAry;
        delete[] histAry;
    }

    // Function to load the image from a file
    void loadImage(const std::string& inFile) {
        std::ifstream file(inFile);
        if (!file.is_open()) {
            std::cerr << "Error opening input file." << std::endl;
            exit(1);
        }

        // Skip the header (already read)
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                file >> imgAry[i][j];
            }
        }

        file.close();
    }

    // Function to compute the histogram
    void computeHist(std::ofstream& logFile) {
        logFile << "Entering computeHist()" << std::endl;

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                int val = imgAry[i][j];
                if (val < minVal || val > maxVal) {
                    logFile << "imgAry[" << i << "][" << j << "] value is not within minVal and maxVal" << std::endl;
                    exit(1);
                }
                histAry[val]++;
            }
        }

        logFile << "Leaving computeHist()" << std::endl;
    }

    // Function to print histogram to histCountFile
    void printHist(std::ofstream& histCountFile, std::ofstream& logFile) {
        histCountFile << numRows << " " << numCols << " " << minVal << " " << maxVal << std::endl;
        for (int i = 0; i <= maxVal; i++) {
            histCountFile << i << " " << histAry[i] << std::endl;
        }

        logFile << "Histogram printed to histCountFile" << std::endl;
    }

    // Function to display histogram as a graph
    void dispHist(std::ofstream& histGraphFile, std::ofstream& logFile) {
        histGraphFile << numRows << " " << numCols << " " << minVal << " " << maxVal << std::endl;
        for (int i = 0; i <= maxVal; i++) {
            histGraphFile << i << " (" << histAry[i] << "): ";
            for (int j = 0; j < histAry[i]; j++) {
                histGraphFile << "+";
            }
            histGraphFile << std::endl;
        }

        logFile << "Histogram graph displayed in histGraphFile" << std::endl;
    }

    // Function to perform binary thresholding
    void binaryThreshold(std::ofstream& binThrFile, std::ofstream& logFile) {
        logFile << "Entering binaryThreshold()" << std::endl;
        logFile << "The result of the binary thresholding using " << thrVal << " as threshold value" << std::endl;

        binThrFile << numRows << " " << numCols << " 0 1" << std::endl;
        logFile << numRows << " " << numCols << " 0 1" << std::endl;

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (imgAry[i][j] >= thrVal) {
                    binThrFile << "1 ";
                    logFile << "1 ";
                } else {
                    binThrFile << "0 ";
                    logFile << "0 ";
                }
            }
            binThrFile << std::endl;
            logFile << std::endl;
        }

        logFile << "Leaving binaryThreshold()" << std::endl;
    }

    // Function to pretty print the image
    void PrettyPrint(std::ofstream& logFile) {
        logFile << "Entering PrettyPrint()" << std::endl;
        logFile << numRows << " " << numCols << " " << minVal << " " << maxVal << std::endl;

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                logFile << std::setw(3) << imgAry[i][j] << " ";
            }
            logFile << std::endl;
        }

        logFile << "Leaving PrettyPrint()" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <inFile> <threshold> <histCountFile> <histGraphFile> <binThrFile> <logFile>" << std::endl;
        return 1;
    }

    // Open the files
    std::ifstream inFile(argv[1]);
    std::ofstream histCountFile(argv[3]);
    std::ofstream histGraphFile(argv[4]);
    std::ofstream binThrFile(argv[5]);
    std::ofstream logFile(argv[6]);

    if (!inFile.is_open() || !histCountFile.is_open() || !histGraphFile.is_open() || !binThrFile.is_open() || !logFile.is_open()) {
        std::cerr << "Error opening one of the files." << std::endl;
        return 1;
    }

    // Read image header
    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    // Create an Image object
    Image img(numRows, numCols, minVal, maxVal);
    img.loadImage(argv[1]);

    // Pretty print the image
    img.PrettyPrint(logFile);

    // Compute the histogram
    img.computeHist(logFile);

    // Print the histogram to histCountFile
    img.printHist(histCountFile, logFile);

    // Display the histogram in graph format
    img.dispHist(histGraphFile, logFile);

    // Set the threshold value and perform binary thresholding
    img.thrVal = std::atoi(argv[2]);
    img.binaryThreshold(binThrFile, logFile);

    // Close all files
    inFile.close();
    histCountFile.close();
    histGraphFile.close();
    binThrFile.close();
    logFile.close();

    return 0;
}
