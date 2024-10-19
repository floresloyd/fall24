#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
using namespace std;


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
            distanceChoice(option) {
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


        /**
         * @brief This method sets all the values of input array to zero.
         * 
         * @param array input array
         */
        void setZero(int** array){
            cout << "setZero() called ..." << endl;

            for (int i = 0; i < numRows + 2; i++){
                for (int j = 0; j < numCols + 2; j++){
                    array[i][j] = 0;
                }
            }
            
            cout << "setZero() Execution Complete ..." << endl;
        }//end-setZero


        /**
         * @brief loads inFile image into Zero Framed Array
         * 
         * @param inputFile 
         * @param ZFAry  Zero Framed Array
         */

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


        void print2DArray(int** ary){
            for (int i = 0; i < numRows + 2; i++){
                for (int j = 0; j < numCols + 2; j++){
                    cout << ary[i][j] << " ";
                }
                cout << endl;
            }
        }//end-print2DArray()

    };//end-distanceSkeleton


int main(int argc, char* argv[]){

    // Step 0: Error Handle & Load Arguments
    if (argc != 7){
        cerr << "Usage: " << "<inFile> <distanceChoice> <prettyPrintFile> <skeletonFile> <deCompressedFile> <logFile>";
        return -1;
    }

    // Input 
    ifstream inFile(argv[1]);
    int distanceChoice = atoi(argv[2]);
    // Output
    ofstream prettyPrintFile(argv[3]);
    ofstream skeletonFile(argv[4]);
    ofstream deCompressedFile(argv[5]);
    ofstream logFile(argv[6]);
    // Declare Variables
    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;
    
    // Step 1: Instantiate distanceSkeleton and set values of ZFAry and SkeletonAry to 0.
    distanceSkeleton ds(numRows, numCols, minVal, maxVal, distanceChoice);
    cout << "distanceSkeleton instantiated ..." << endl; 
    ds.setZero(ds.ZFAry);
    cout << "ZFAry values are set to 0 ..." << endl;
    ds.setZero(ds.skeletonAry);
    cout << "skeletonAry values are set to 0 ..." << endl;

    ds.print2DArray(ds.ZFAry);
    cout << endl;

    // Step  2: Load Image
    ds.loadImage(inFile, ds.ZFAry);
    ds.print2DArray(ds.ZFAry);
    
    // Step 3: TODO!
    // CONTINUE CODING FROM HERE! 

    // Step 12: Close all Files
    inFile.close();
    prettyPrintFile.close();
    skeletonFile.close();
    deCompressedFile.close();
    logFile.close();

    return 0;
}//end-main