#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Enhancement {
    // Member variables
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
    int thrVal; // Threshold value

    // Dynamically allocated 2D arrays
    int** mirrorFmAry;
    int** avgAry;
    int** medianAry;
    int** GaussAry;
    int** thrAry;
    int** mask2DAry;

    // 1D arrays
    int mask1DAry[9]; // 1D array for the Gaussian mask
    int neighbor1DAry[9]; // 1D array for a pixel's 3x3 neighbors

    // Other variables
    int maskWeight; // Total value of the mask

    // Constructor
    Enhancement(int rows, int cols, int maskRows, int maskCols, int threshold)
        :
        // Assign Params to member variables  
        numRows(rows), 
        numCols(cols), 
        maskRows(maskRows), 
        maskCols(maskCols), 
        thrVal(threshold), 
        newMin(9999), 
        newMax(0) {

            // Dynamically instantiate 2D-Arrays
            mirrorFmAry = allocate2DArray(numRows + 2, numCols + 2);
            avgAry = allocate2DArray(numRows + 2, numCols + 2);
            medianAry = allocate2DArray(numRows + 2, numCols + 2);
            GaussAry = allocate2DArray(numRows + 2, numCols + 2);
            thrAry = allocate2DArray(numRows + 2, numCols + 2);

            // Init masak1DAry and neighbor1DAry to all 0's
            for (int i = 0; i < 9; i++){
                mask1DAry[i] = 0;
                neighbor1DAry[i] = 0;
            } // end-for-loop
    }; // End-Enhancement-Constructor

    // Destructor to deallocate the 2D arrays
    ~Enhancement() {
        deallocate2DArray(mirrorFmAry, numRows + 2);
        deallocate2DArray(avgAry, numRows + 2);
        deallocate2DArray(medianAry, numRows + 2);
        deallocate2DArray(GaussAry, numRows + 2);
        deallocate2DArray(thrAry, numRows + 2);
        deallocate2DArray(mask2DAry, maskRows);
    }

    private:
    // Helper function to dynamically allocate a 2D array
    int** allocate2DArray(int rows, int cols) {
        int** array = new int*[rows];
        for (int i = 0; i < rows; i++) {
            array[i] = new int[cols];
            for (int j = 0; j < cols; j++) {
                array[i][j] = 0; // Initialize to 0
            }
        }
        return array;
    } // end-allocate2DArray

    // Helper function to deallocate a 2D array
    void deallocate2DArray(int** array, int rows) {
        for (int i = 0; i < rows; i++) {
            delete[] array[i];
        }
        delete[] array;
    } // end-deallocate2DArray

    // binary threshold to classify 1 or 0 based on thrVal
    void binThreshold(int** inAry, int** outAry, int numRows, int numCols, int thrVal, ofstream& logFile){
        logFile << "**  Entering binThreshold method" << endl;
        
        // Loop through all pixels in the input array
        for (int i = 0; i <= numRows; i++){
            for (int j = 0; i <= numCols; j++){
                // If value is greater than threshold: Set output to 1
                if (inAry[i][j] > thrVal){
                    outAry[i][j] = 1;
                } else{
                    outAry = 0;
                } // end if-else
            } // end-j-loop
        } // end-i-loop
        logFile << "** Exiting binThreshold method" << endl;
    } // end-binThreshold-method

    // Pretty print function with detailed logging
    void prettyPrint(int** imgAry, int rows, int cols, ofstream& logFile) {
        logFile << "Enter PrettyPrint()\n";
        logFile << rows << " " << cols << " " << minVal << " " << maxVal << "\n";

        string maxValStr = to_string(maxVal);
        int Width = maxValStr.length();

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                string valStr = to_string(imgAry[i][j]);
                logFile << imgAry[i][j] << " ";
            } // end-j-loop
            logFile << "\n";
        } // end-i-loop
        logFile << "Leaving PrettyPrint()\n";
    }// end prettyPrint()

    // mirrorFraming 
    // CHECKPOINT
    void mirrorFraming(ofstream& logFile){

    }

}; // End-Enhancement-Class


int main(){

}