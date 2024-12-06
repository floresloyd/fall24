#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
#include <climits>  // For INT_MAX
#include <iomanip>
#include <cmath>

using namespace std;

class Edge {
    public:
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int threshold;
        // Arrays
        int** MFAry; // 2D[numRows + 2][numCols + 2] 
        int** edgeAry; // 2D[numRows + 2][numCols + 2]
        int** binAry; // 2D[numRows + 2][numCols + 2]

    // Constructor
    Edge(int row, int col, int min, int max, int thr)
        :
        numRows(row),
        numCols(col),
        minVal(min),
        maxVal(max),
        threshold(thr)
        {
            // Instantiate Arrays
            MFAry = new int*[numRows + 2];
            edgeAry = new int*[numRows + 2];
            binAry = new int*[numRows + 2];

            // Make 2d
            for (int i = 0; i < numRows + 2; i++){
                MFAry[i] = new int[numCols + 2];
                edgeAry[i] = new int[numCols + 2];
                binAry[i] = new int[numCols + 2];
            }
        }//end-constructor-Edge

    ~Edge(){
        for (int i = 0; i < numRows + 2; i ++){
            // Delete Columns
            delete[] MFAry[i];
            delete[] edgeAry[i];
            delete[] binAry[i];
        }
        // Delete Reference to object
        delete[] MFAry;
        delete[] edgeAry;
        delete[] binAry;
    }//end-deconstructor-Edge


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
                //logFile << "mirrorFmAry[" << i << "][" << j << "] = " << mirrorFmAry[i][j] << endl;
            }
        }

        logFile << "*** Leaving loadImage() method ***" << endl;
        inFile.close();
    }//end-LoadImage


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


    void binThreshold(int** inputAry, int** binAry) {
        for (int i = 0; i <= numRows; i++) {
            for (int j = 0; j <= numCols; j++) {
                if (inputAry[i][j] <= threshold) {
                    binAry[i][j] = 0;
                } else {
                    binAry[i][j] = 1;
                }
            }
        }
    }//end-binThreshold



    void prettyPrint(int** ary, ostream& outFile) {
        for (int i = 0; i <= numRows; i++) {
            for (int j = 0; j <= numCols; j++) {
                if(ary[i][j] == 0) {
                    outFile << setw(4) << " ";  
                } else {
                    outFile << setw(4) << ary[i][j];  
                }
            }
            outFile << endl;
        }
    }//end-prettyPrint()

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

    void printAll(int** ary, ostream& outFile){
        for (int i = 0; i <= numRows; i++){
            for (int j = 0; j <= numCols; j++){
                outFile << ary[i][j] <<" ";    
            }
            outFile << endl;
        }
    }//end-prettyDotPrint()
    
    void gradientEdgedetector(int** MFAry, int** edgeAry, ofstream& logFile) {
        logFile << "*** Entering Gradient Edge detector ***" << endl;

        // step 1: Scan MFAry Left to right and Top to bottom
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
            
                int x = MFAry[i][j];        // Get the current pixel value (x)
                int a = MFAry[i][j + 1];    // Get the right neighbor (a)
                int b = MFAry[i + 1][j];    // Get the below neighbor (b)
                
                // Calculate gradient using the formula: sqrt((x-a)^2 + (x-b)^2)
                int diffA = x - a;
                int diffB = x - b;
                edgeAry[i][j] = static_cast<int>(sqrt(diffA * diffA + diffB * diffB));
            }
        }

        // Set the frame of edgeAry to 0
        for (int j = 0; j <= numCols + 1; j++) {
            edgeAry[0][j] = 0;
            edgeAry[numRows + 1][j] = 0;
        }
        for (int i = 0; i <= numRows + 1; i++) {
            edgeAry[i][0] = 0;
            edgeAry[i][numCols + 1] = 0;
        }

        logFile << "*** Leaving Gradient Edge detector ***" << endl;
    }

};//end-class-Edge

int main(int argc, char* argv[]){
    // Step 0 
    // Inputs
    ifstream inFile(argv[1]);
    int thrVal = atoi(argv[2]);
    // Outputs
    ofstream outFile(argv[3]);
    ofstream logFile(argv[4]);

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    cout << numRows << " " << numCols << " " << minVal << " " << maxVal << " " << "Thr: " << thrVal;

    Edge edge(numRows, numCols, minVal, maxVal, thrVal);
    // Step 1 
    edge.loadImage(argv[1], edge.MFAry, numRows, numCols, logFile);
    // Step 2 
    edge.mirrorFraming(edge.MFAry, numRows, numCols, logFile);
    // Step 3 
    outFile << " Below is mirror framed input image" << endl;
    edge.prettyPrint(edge.MFAry, outFile);
    // Step 4 
    edge.gradientEdgedetector(edge.MFAry, edge.edgeAry, logFile);
    outFile << "Below is the edgeAry , the result of gradient edge detector" << endl;
    edge.prettyPrint(edge.edgeAry, outFile);
    // Step 5 
    edge.binThreshold(edge.edgeAry, edge.binAry);
    outFile << " Below is the binary threshold of the edge with thr = " << thrVal << endl;
    edge.prettyDotPrint(edge.binAry, outFile);

    // Step 6: Close all Files 
    inFile.close();
    outFile.close();
    logFile.close();

}//end-main


/**
 * TO RUN: 

1.  compile:    g++ FloresL_Q1_Main.cpp -o FloresL_Q1_Main

2.  run    :    ./FloresL_Q1_Main.exe Q1_data.txt 25 outFile.txt logFile.txt

Arguments:
(Input)
args[1] - inFile
args[2] - thrVal = 25
(Output)
args[3] - outFile 
args[4] - logFile

*/

