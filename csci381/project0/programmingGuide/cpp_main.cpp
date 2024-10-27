#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
#include <climits>  // For INT_MAX

using namespace std;

// INSTANTIATING A CLASS + CONSTRUCTOR && DECONSTRUCTOR 

class distanceSkeleton {
    public:
        // Declare Instance Variables
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int newMinVal;      // Holds value after first pass
        int newMaxVal;      // Holds value after second pass
        // Arrays
        int** ZFAry;        // 2D Integer Array size [numRows + 2][numCols + 2]; ZeroFramed Array that holds the image
        int** skeletonAry;  // 2D         Array size [numRows + 2][numCols + 2]
        int distanceChoice; 

        // Constructor
        distanceSkeleton(int rows, int cols, int min, int max, int option)
            :
            numRows(rows),
            numCols(cols),
            minVal(min),
            maxVal(max),
            distanceChoice(option),
            newMinVal(INT_MAX),
            newMaxVal(0) {
                // Instantiate them as 1D first
                ZFAry = new int*[numRows + 2];
                skeletonAry = new int*[numRows + 2];

                // Make it 2D By appending Columns
                for (int i = 0; i < numRows + 2; i++){
                    ZFAry[i] = new int[numCols + 2];
                    skeletonAry[i] = new int[numCols + 2];
                }
        }//end-distanceSkeleton-constructor

        // Deconstructor
        ~distanceSkeleton(){
            for (int i = 0; i < numRows + 2; i ++){
                // Delete Columns
                delete[] ZFAry[i];
                delete[] skeletonAry[i];
            }
            // Delete Reference to object
            delete[] ZFAry;
            delete[] skeletonAry;
        }//end Deconstructor
} 


// READING INPUT 

int main(int argc, char* argv[]){

    //==================================== Error Handle & Load Arguments ====================================
    int numCommandLineArgs;
    if (argc != numCommandLineArgs){
        cerr << "Usage: " << "<inFile> <distanceChoice> <prettyPrintFile> <skeletonFile> <deCompressedFile> <logFile>";
        return -1;
    }

    //==================================== READING INPUT ====================================
    // Reading Integer Input
    int integerInput = atoi(argv[2]);

    // Reading input file
    ifstream inFile(argv[1]);
    int numRows, numCols, minVal, maxVal; 
    // Actually reading
    infile >> numRows >> numCols >> minVal >> maxVal

    // Continuous Loading to Image
    void loadImage(ifstream &inFile, int** ZFAry) {
        cout << "Loading Image ..." << endl; 

        // Error Handle
        if (!inFile) { 
            cerr << "Error: Cannot open input file!" << endl;
            return;
        }

        cout << "Header: " << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

        // Load image data into ZFAry (assuming image data starts from row 1, col 1)
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                inFile >> ZFAry[i][j];
            }
        }
        cout << "Image Loaded ..." << endl;
    }//end-loadImage()


    //==================================== WRITING OUTPUT ====================================
    ofstream prettyPrintFile(argv[3]);
    // Actually writing one line 
    prettyPrintFile << "PRINTING TO PRETTY PRINT FILE";

    // Continous Writing
    void prettyPrint(int** ary, ostream& outFile){
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
}