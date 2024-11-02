#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
#include <climits>  // For INT_MAX
#include <algorithm>

using namespace std;


class connect4 {
    public: 
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int newLabel;
        int minLabel;
        int maxLabel;
        // 2D Arrays
        int** ZFAry; 
        int* EQTable;

        connect4(int rows, int cols, int min, int max)
            :
            numRows(rows),
            numCols(cols),
            minVal(min),
            maxVal(max),
            newLabel(0){
        
                // Instantiate ZFAry
                ZFAry = new int*[numRows + 2];
                for (int i = 0; i < numRows + 2; i++){
                    ZFAry[i] = new int[numCols + 2];
                }
                cout << "Instantiated 2D ZF Ary" << endl;

                for (int i = 0; i < numRows + 2; i++){
                    for (int j = 0; j < numCols + 2; j++){
                        ZFAry[i][j] = 0;
                    }
                }
                cout << "ZEROED 2D ZF Ary" << endl;
                
                // Instantiate EQ Table and allocate values
                EQTable = new int[(numRows * numCols) / 4];
                for (int i = 0; i < (numRows * numCols) / 4; i++){
                    EQTable[i] = i;
                }
                cout << "Instantiated 1D EQTable" << endl;
            }//end-constructor

        void loadImage(ifstream &inFile, int** ZFAry) {
            cout << "Loading Image ..." << endl; 

            // Error Handle
            if (!inFile) { 
                cerr << "Error: Cannot open input file!" << endl;
                return;
            }


            // Load image data into ZFAry (assuming image data starts from row 1, col 1)
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    inFile >> ZFAry[i][j];
                }
            }
            cout << "Image Loaded ..." << endl;
        }//end-loadImage()

        void prettyPrint(int** ary, ostream &outFile){
            for (int i = 0; i <= numRows; i++){
                for (int j = 0; j <= numCols; j++){
                    
                    if(ary[i][j] == 0){
                        outFile << ". ";
                    }
                    else{
                        outFile << ary[i][j] <<" ";
                    }
                }
                outFile << endl;
            }
        }//end-prettyPrint()

        void prettyPrint2(int** ary, ofstream& logFile) {
            logFile << numRows << " " << numCols << " " << minVal << " " << maxVal << "\n";

            string maxValStr = to_string(maxVal);
            int Width = maxValStr.length();

            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    string valStr = to_string(ary[i][j]);
                    logFile << ary[i][j] << "      ";
                }
                logFile << "\n";
            }

        }// end prettyPrint()

        void printEQTable(int* EQTable, ofstream &outFile){
            for (int i = 0; i <= newLabel; i++){
                outFile << i << "        " << EQTable[i] << endl; 
            }
        }

        void connect4Pass1(int** ZFAry, int& newLabel, int* EQTable, ofstream &outFile){
            int myCase;
            outFile << "Running connect4Pass1 ..." << endl;
            for (int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){
                    if (ZFAry[i][j] > 0){
                        myCase = whichCase(ZFAry, i, j);

                        if (myCase == 1){
                            newLabel++; // Increment newLabel before using it
                            ZFAry[i][j] = newLabel;
                        }
                        if (myCase == 2){
                            ZFAry[i][j] = minLabel;
                        }
                        if (myCase == 3){
                            ZFAry[i][j] = minLabel;
                            EQTable[maxLabel] = minLabel;
                        }
                    }
                }
            }
        }

        int whichCase(int** ZFAry, int i, int j){
            int b = ZFAry[i - 1][j]; // ABOVE
            int d = ZFAry[i][j - 1]; // LEFT 

            if (b == 0 && d == 0){
                return 1;  
            }
            else if (b > 0 && d == 0){
                minLabel = b;
                return 2;  
            }
            else if (d > 0 && b == 0){
                minLabel = d;
                return 2;  
            }
            else if (b > 0 && d > 0){
                if (b == d){
                    minLabel = b;
                    return 2;
                } else {
                    minLabel = min(b, d);
                    maxLabel = max(b, d);
                    return 3;  
                }
            }
            return 1; 
        }

        
        void print2D(){
            cout << endl;
            cout << "Header: " << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            for (int i = 0; i < numRows + 2; i++){
                for (int j = 0; j < numCols + 2; j++){
                    cout << ZFAry[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        void print1D(){
            cout << endl;
            for (int i = 0; i < (numRows * numCols) / 4; i++){
                cout << EQTable[i] << " "; 
            }
           
            cout << endl;
        }
        
}; //end-connect4

int main(int argc, char* argv[]){
    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);
    
    // Step 0 
    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;
    connect4 c4 (numRows, numCols, minVal, maxVal);
    c4.print2D();
    cout << endl;
    c4.print1D();
    cout << endl;

    // Step 1 
    c4.newLabel = 0;

    // Step 2 
    c4.loadImage(inFile, c4.ZFAry);
    c4.print2D(); 
    outFile << " *** Below is the input data before connect4Pass1" << endl;
    c4.prettyPrint(c4.ZFAry, outFile);

    // Step 3
    outFile << endl; 
    c4.connect4Pass1(c4.ZFAry, c4.newLabel, c4.EQTable, outFile);
    outFile << endl;

    // Step 4
    outFile << " *** Below is the input data after connect4Pass1" << endl;
    c4.prettyPrint(c4.ZFAry, outFile);

    // Step 5 
    outFile << endl;
    outFile << "*** Below is the EQTable" << endl;
    c4.printEQTable(c4.EQTable, outFile);

    // Step 6 
    inFile.close();
    outFile.close();
}


/**
 COMPILE: g++ FloresL_Q2_Main.cpp -o FloresL_Q2_Main
 RUN    : ./FloresL_Q2_Main Q2_data1.txt outFile.txt
 * 
 */