#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
#include <climits>  // For INT_MAX
#include <iomanip>
#include <cmath>

using namespace std;

class docImage {
    public:
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        // Arrays
        int** imgAry; // 2D[numRows + 2][numCols + 2] -- holds image
        int* HPP;     // 1D[numRows] to store horizontal projection (L-R) | HPP[i] is the sum of all pixels on imgAry row_i
        int* VPP;     // 1D[numCols] to store vertical projection (T->B)  | VPP[i] os tje si, of all pixels on imgAry col_j

// Constructor
docImage(int row, int col, int min, int max)
    :
    numRows(row),
    numCols(col),
    minVal(min),
    maxVal(max)
    {
        // Instantiate Arrays
        imgAry = new int*[numRows + 2];
        HPP = new int[numRows + 2];  // Added +2 to match imgAry dimensions
        VPP = new int[numCols + 2];  // Added +2 to match imgAry dimensions

        // Make 2D array and initialize all elements to 0
        for (int i = 0; i < numRows + 2; i++) {
            imgAry[i] = new int[numCols + 2];
            // Initialize row to 0
            for (int j = 0; j < numCols + 2; j++) {
                imgAry[i][j] = 0;
            }
        }

        // Initialize HPP and VPP arrays to 0
        for (int i = 0; i < numRows + 2; i++) {
            HPP[i] = 0;
        }
        for (int i = 0; i < numCols + 2; i++) {
            VPP[i] = 0;
        }
    }//end-docImage-constructor

    ~docImage(){
        for (int i = 0; i < numRows + 2; i ++){
            // Delete Columns
            delete[] imgAry[i];
        }
        // Delete Reference to object
        delete[] imgAry;
        delete[] HPP;
        delete[] VPP;
    }//end-deconstructor-docImage


    void loadImage(string inputFile, int** mirrorFmAry, int numRows, int numCols, ofstream& logFile) {
        logFile << "*** Entering loadImage ***" << endl;

        ifstream inFile(inputFile);
        if (!inFile) {
            logFile << "Error: Cannot open input file!" << endl;
            cerr << "Error: Cannot open input file!" << endl;
            return;
        }

        // Skip the header since we already read it in main
        int dummy;
        inFile >> dummy >> dummy >> dummy >> dummy;

        // Initialize all array elements to 0
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                mirrorFmAry[i][j] = 0;
            }
        }

        // Read input and populate matrix
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                inFile >> mirrorFmAry[i][j];
            }
        }

        logFile << "*** Leaving loadImage() method ***" << endl;
        inFile.close();
    }


    void prettyDotPrint(int** ary, ostream& outFile) {
        for (int i = 0; i <= numRows; i++) {
            for (int j = 0; j <= numCols; j++) {
                if(ary[i][j] == 0) {
                    outFile << setw(4) << ".";  // 4 spaces for dots
                } else {
                    outFile << setw(4) << ary[i][j];  // 4 spaces for numbers
                }
            }
            outFile << endl;
        }
    }//end-prettyDotPrint()


    void computePP(int** imgAry, ostream& logFile) {
        logFile << "*** Entering computePP() method ***" << endl;
        
        // Step 1: Scan imgAry and compute both projections
        for (int i = 1; i <= numRows; i++) {
            int rowSum = 0;
            for (int j = 1; j <= numCols; j++) {
                rowSum += imgAry[i][j];      // Add to horizontal projection
                VPP[j] += imgAry[i][j];      // Add to vertical projection
            }
            HPP[i] = rowSum;
        }
        
        logFile << "*** Leaving computePP() method ***" << endl;
    }// end-void-computePP()

    void printPP(int* pp, ostream& outFile) {
        // Step 1: Determine if it's HPP or VPP based on array length
        int length = (pp == HPP) ? numRows : numCols;
        
        // Print all values
        for (int i = 1; i <= length; i++) {
            outFile << pp[i] << endl;
        }
    }//end-void-printPP()


};//end-class-docImage

int main(int argc, char* argv[]){
    // Step 0
    
    // Inputs
    ifstream inFile(argv[1]);
    // Outputs
    ofstream outFile(argv[2]);
    ofstream logFile(argv[3]);

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    cout << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

    docImage docImage(numRows, numCols, minVal, maxVal);

    // Step 1 
    docImage.loadImage(argv[1], docImage.imgAry, numRows, numCols, logFile);
    outFile << "Below is the input image" << endl;
    docImage.prettyDotPrint(docImage.imgAry, outFile);   

    // Step 2 
    docImage.computePP(docImage.imgAry, logFile);

    // Step 3 
    outFile << "Below is HPP" << endl;
    docImage.printPP(docImage.HPP, outFile);
    outFile << "Below is VPP" << endl;
    docImage.printPP(docImage.VPP, outFile);


    // Step 4: Close all Files 
    inFile.close();
    outFile.close();
    logFile.close();

}//end-main


/**
 * TO RUN: 

1.  compile   :    
g++ FloresL_Q2_Main.cpp -o FloresL_Q2_Main

2.  run      :    
./FloresL_Q2_Main.exe Q2_data.txt outFile.txt logFile.txt

Arguments:
(Input)
args[1] - inFile -- binary img w/ header 
(Output)
args[2] - outFile 
args[3] - logFile

*/



