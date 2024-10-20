#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for atoi
#include <climits>  // For INT_MAX

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


        /**
         * @brief Performs 2 passes of distance transform to prepare for local maxima operation
         * @param ZFARy zero framed array containing image
         * @param distanceChoice Distance transformation Algorithm of choice
         * @param logFile output file 
         */
        void distanceTransform(int** ZFAry, int distanceChoice, ostream& prettyPrintFile, ostream& logFile ){
            logFile << "*** Entering distanceTransform() method ... ***" << endl;

            string distanceChoiceDescription;
            if (distanceChoice == 8){
                distanceChoiceDescription = " : 8 Connected Distance Transform";
            }
            else if (distanceChoice == 4){
                distanceChoiceDescription = " : City-block Distance Transform";
            }

            // Pass 1
            distancePass1(ZFAry, distanceChoice, logFile);
            prettyPrintFile << "Header: " << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;    
            prettyPrintFile << "1st Pass transform with choice = " << distanceChoice << distanceChoiceDescription << endl;
            logFile << "1st Pass transform with choice = " << distanceChoice << distanceChoiceDescription << endl;
            prettyPrint(ZFAry, prettyPrintFile);

            // Pass 2 
            distancePass2(ZFAry, distanceChoice, logFile);
            prettyPrintFile << "Header: " << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;    
            prettyPrintFile << "2nd Pass transform with choice = " << distanceChoice << distanceChoiceDescription << endl;
            logFile << "2nd Pass transform with choice = " << distanceChoice << distanceChoiceDescription << endl;
            prettyPrint(ZFAry, prettyPrintFile);
            logFile << "*** Leaving distanceTransform() Method ... ***" << endl;
            logFile << "newMinVal: " << newMinVal << " newMaxVal: " << newMaxVal << endl;           
        }//end-distanceTransform()


        /**
         * @brief Performs First pass of distance transform that looks at upper right corner of neighborhood
         */
        void distancePass1(int** ZFAry, int distanceChoice, ostream& logFile){
            logFile << "*** Entering distancePass1() ***" << endl;
            int a, b, c, d;

            newMinVal = INT_MAX;
            newMaxVal = 0;

            for (int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){

                    if (ZFAry[i][j] > 0){

                        // Get neighbors 
                        a = ZFAry[i - 1][j - 1];
                        b = ZFAry[i - 1][j];
                        c = ZFAry[i - 1][j + 1];
                        d = ZFAry[i][j - 1];

                        if (distanceChoice == 8){       // 8-Connected
                            ZFAry[i][j] = min(min(a, b), min(c, d)) + 1;
                            //cout << a << ", " << b << ", " << c << ", " << d << ", min = " << ZFAry[i][j] << endl;
                        }
                        else if (distanceChoice == 4){  // City Block
                            ZFAry[i][j] = min( min(a + 2, b + 1), min(c + 2, d + 1));                        
                        }

                        if (ZFAry[i][j] > 0) { // Only consider non-zero values
                            if (ZFAry[i][j] < newMinVal) {
                                newMinVal = ZFAry[i][j];
                            }
                            if (ZFAry[i][j] > newMaxVal) {
                                newMaxVal = ZFAry[i][j] ;
                            }
                        } // end-newMin_and_newMax_tracker

                    }//end-if
                }//end-j-loop
            }//end-i-loop
        }//end-distancePass1()


        /**
         * @brief Performs Second pass of distance transform that looks at lower left of neighborhood
         */
        void distancePass2(int** ZFAry, int distanceChoice, ostream& logFile){
            logFile << "*** Entering distancePass2() ***" << endl;
            
            int e, f, g, h;
            newMinVal = INT_MAX;
            newMaxVal = 0;

            for (int i = numRows; i >= 1; i--){
                for (int j = numCols; j >= 1; j--){

                    if (ZFAry[i][j] > 0){

                        // Get neighbors 
                        e = ZFAry[i][j + 1];
                        f = ZFAry[i + 1][j - 1];
                        g = ZFAry[i + 1][j];
                        h = ZFAry[i + 1][j + 1];

                        if (distanceChoice == 8){       // 8-Connected
                            ZFAry[i][j] = min( min( min(e + 1, f + 1), min(g + 1, h + 1)), ZFAry[i][j]);
                            //cout << e << ", " << f << ", " << g << ", " << h << ", min = " << ZFAry[i][j] << endl;
                        }
                        else if (distanceChoice == 4){  // City Block
                            ZFAry[i][j] = min( min( min(e + 1, f + 2), min(g + 1, h + 2)), ZFAry[i][j]);
                        }


                        if (ZFAry[i][j] > 0) { // Only consider non-zero values
                            if (ZFAry[i][j] < newMinVal) {
                                newMinVal = ZFAry[i][j];
                            }
                            if (ZFAry[i][j] > newMaxVal) {
                                newMaxVal = ZFAry[i][j];
                            }
                        } // end-newMin_and_newMax_tracker
                       
                    }//end-if
                }//end-j-loop
            }//end-i-loop
        }//end-distancePass2()


        /**
         * brief 
         */
        void compression(int** ZFAry, int distanceChoice, int** skeletonAry, ostream& skeletonFile, ostream& prettyPrintFile, ostream& logFile){
            logFile << "*** Entering compression() method ***" << endl;
            
            for( int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){
                    computeLocalMaxima(ZFAry, i, j, skeletonAry, distanceChoice);
                }
            }

            extractSkeleton(skeletonAry, skeletonFile, logFile);
            prettyPrintFile << "Extracted Skeleton Array " << endl;
            prettyPrintFile << "Header: " << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;    
            prettyPrint(skeletonAry, prettyPrintFile);

            logFile <<"*** Leaving compression() method ***" << endl;
        }//end-compression


        /**
         * @brief Assigns values to skeletonAry
         * @param ZFAry zero framed array containing image
         * @param skeletonARy will hold the skeleton of the image
         * @param distanceChoice choice of distance transform algorithm 
         */
        void computeLocalMaxima(int** ZFAry, int i, int j, int** skeletonAry, int distanceChoice){
            if (isLocalMaxima(ZFAry, i, j, distanceChoice)){
                skeletonAry[i][j] = ZFAry[i][j];
            }
            else{
                skeletonAry[i][j] = 0;
            }
        }//end-computeLcocalMaxima()


        /**
         * @brief Helper function to computeLocalMaxima. 
         * This method Checks its neighborhood (depending on dt-algorithm) 
         * and returns a boolean if p[i][j] is the local max
         *  
         */
        bool isLocalMaxima(int** ZFAry, int i, int j, int distanceChoice){
            if (distanceChoice == 4){
                int b = ZFAry[i - 1][j]; 
                int d = ZFAry[i][j - 1];
                int pij = ZFAry[i][j];
                int e = ZFAry[i][j + 1];
                int g = ZFAry[i + 1][j];

                int neighborhoodMax = max( max(b, d), max(e, g));

                if (pij >= neighborhoodMax){
                    return true;
                }
                else{
                    return false;
                }
            }//end-4Connect

            if (distanceChoice == 8){
                int a = ZFAry[i - 1][j - 1];
                int b = ZFAry[i - 1][j];
                int c = ZFAry[i - 1][j + 1]; 
                int d = ZFAry[i][j - 1];
                int pij = ZFAry[i][j];
                int e = ZFAry[i][j + 1];
                int f = ZFAry[i + 1][j - 1];
                int g = ZFAry[i + 1][j];
                int h = ZFAry[i + 1][j + 1];
                
                int neighborhoodMax = max(max(max(a, b), max(c, d)), max(max(e, f), max(g, h)));

                if (pij >= neighborhoodMax){
                    return true;
                }
                else{
                    return false;
                }
            }//end-8Connect
        }//end-isLocalMaxima()
         

         /**
          * @brief compresses the 2D skeleton into triplets
          */
        void extractSkeleton(int** skeletonAry, ostream& skeletonFile, ostream& logFile ){
            newMinVal = INT_MAX;
            newMaxVal = 0;

            for (int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){
                    if (skeletonAry[i][j] > 0){
                        skeletonFile << i << " " << j << " " << skeletonAry[i][j] << endl;
                    }


                    if (ZFAry[i][j] > 0) { // Only consider non-zero values
                        if (ZFAry[i][j] < newMinVal) {
                            newMinVal = ZFAry[i][j];
                        }
                        if (ZFAry[i][j] > newMaxVal) {
                            newMaxVal = ZFAry[i][j];
                        }
                    } // end-newMin_and_newMax_tracker
            
        
                }
            }
            logFile << "Skeleton Extracted ..." << endl;
        }//end-extractSkeleton()


        /**
         *@brief reads skeletonFile and stores in it ZFAry
         */
        void loadSkeleton(ifstream& skeletonFile, int** ZFAry, ostream& logFile){
            newMinVal = INT_MAX;
            newMaxVal = 0;

            logFile << "*** Entering LoadingSkeleton() ***" << endl;
            int row, col, val;

            // Keep reading while there are still lines
            while(skeletonFile >> row >> col >> val){
                ZFAry[row][col] = val;
                logFile << "Loaded: " << row << " " << col << " " << val << endl;
                if (val > 0) { // Only consider non-zero values
                    if (val < newMinVal) {
                        newMinVal = val;
                    }
                    if (val > newMaxVal) {
                        newMaxVal = val;
                }
            } // end-newMin_and_newMax_tracker
            } 
            logFile << "*** Skeleton loaded ... ***" << endl;
        }//end-loadSkeleton()

        /**
         * @brief Pretty prints a 2D Array into the outfile
         * 
         * @param ary 2D input array
         * @param outFile output file
         */
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


        /**
         * 
         *  
         */
        void deCompression(int** ZFAry, int distanceChoice, ostream& prettyPrintFile, ostream& logFile){
            logFile << "*** Entering deCompression() ***" << endl;
            expansionPass1(ZFAry, distanceChoice, logFile);
            prettyPrintFile << "1st pass Expansion with choice = " << distanceChoice << distanceChoice << endl;
            prettyPrintFile << "Header: " << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;    
            prettyPrint(ZFAry, prettyPrintFile);
            expansionPass2(ZFAry, distanceChoice, logFile);
            prettyPrintFile << "2nd pass Expansion with choice = " << distanceChoice << distanceChoice << endl;
            prettyPrintFile << "Header: " << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;    
            prettyPrint(ZFAry, prettyPrintFile);
            logFile << "*** Leaving deCompression() ***" << endl;

        }//end-deCompression


        void expansionPass1(int** ZFAry, int distanceChoice, ostream& logFile){
            logFile << "expansionPass1 ..." << endl;
            int a, b, c, d, e, f, g, h, neighborhoodMax;
            newMinVal = INT_MAX;
            newMaxVal = 0;

            for (int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){

                    int pij = ZFAry[i][j];  // Get the current pixel value

                    // Get values of the neighborhood 
                    a = ZFAry[i - 1][j - 1];
                    b = ZFAry[i - 1][j];
                    c = ZFAry[i - 1][j + 1];
                    d = ZFAry[i][j - 1];
                    e = ZFAry[i][j + 1];
                    f = ZFAry[i + 1][j - 1];
                    g = ZFAry[i + 1][j];
                    h = ZFAry[i + 1][j + 1];

                    // Calculate neighborhood max
                    if (distanceChoice == 8){
                        neighborhoodMax = max(max(max(a, b), max(c, d)), max(max(e, f), max(g, h)));
                    }
                    else if (distanceChoice == 4){
                        neighborhoodMax = max(max(b, d), max(e, g));  // Only 4-connected neighbors
                    }

                    // Expand based on the neighborhood values
                    if (pij < neighborhoodMax - 1){
                        ZFAry[i][j] = neighborhoodMax - 1;  // Propagate the max value, reducing by 1
                    }


                    if (ZFAry[i][j] > 0) { // Only consider non-zero values
                        if (ZFAry[i][j] < newMinVal) {
                            newMinVal = ZFAry[i][j];
                        }
                        if (ZFAry[i][j] > newMaxVal) {
                            newMaxVal = ZFAry[i][j];
                        }
                    } // end-newMin_and_newMax_tracker
                    
                }
            }
        }//end-expansionPass1


        void expansionPass2(int** ZFAry, int distanceChoice, ostream& logFile){
            logFile << "expansionPass2 ..." << endl;
            int a, b, c, d, e, f, g, h, neighborhoodMax;
            newMinVal = INT_MAX;
            newMaxVal = 0;


            for (int i = numRows; i >= 1; i--){
                for (int j = numCols; j >= 1; j--){

                    int pij = ZFAry[i][j];  // Get the current pixel value

                    // Get values of the neighborhood 
                    a = ZFAry[i - 1][j - 1];
                    b = ZFAry[i - 1][j];
                    c = ZFAry[i - 1][j + 1];
                    d = ZFAry[i][j - 1];
                    e = ZFAry[i][j + 1];
                    f = ZFAry[i + 1][j - 1];
                    g = ZFAry[i + 1][j];
                    h = ZFAry[i + 1][j + 1];

                    // Calculate neighborhood max
                    if (distanceChoice == 8){
                        neighborhoodMax = max(max(max(a, b), max(c, d)), max(max(e, f), max(g, h)));
                    }
                    else if (distanceChoice == 4){
                        neighborhoodMax = max(max(b, d), max(e, g));  // Only 4-connected neighbors
                    }

                    // Expand based on the neighborhood values
                    if (pij < neighborhoodMax - 1){
                        ZFAry[i][j] = neighborhoodMax - 1;  // Propagate the max value, reducing by 1
                    }


                    if (ZFAry[i][j] > 0) { // Only consider non-zero values
                        if (ZFAry[i][j] < newMinVal) {
                            newMinVal = ZFAry[i][j];
                        }
                        if (ZFAry[i][j] > newMaxVal) {
                            newMaxVal = ZFAry[i][j];
                        }
                    } // end-newMin_and_newMax_tracker
                    

                }
            }
        }//end-expansionPass1


        void binThreshold(int** ZFAry, ostream& deCompressedFile){
            for (int i = 1; i <= numRows; i++){
                for (int j = 1; j <= numCols; j++){
                    if (ZFAry[i][j] >= 1){
                        deCompressedFile << 1 << " ";
                    }
                    else{
                        deCompressedFile << 0 << " ";
                    }
                }
                deCompressedFile << endl;
            }
        }


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

    if (atoi(argv[2]) != 4 && atoi(argv[2]) != 8 ){
        cerr << "Argument Error: argv[2] Can only be: (4) City Block; (8) 8-Connected" << endl;
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

    // Step  2: Load Image
    prettyPrintFile << "Header: " << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    ds.loadImage(inFile, ds.ZFAry);
    logFile << "*** Below is the input image ***" << endl;
    ds.prettyPrint(ds.ZFAry, logFile);
    prettyPrintFile << "*** Below is the input image ***" << endl;
    ds.prettyPrint(ds.ZFAry, prettyPrintFile);

    
    // Step 3: Run Distance Transform
    ds.distanceTransform(ds.ZFAry, distanceChoice, prettyPrintFile, logFile);
    cout << "distanceTransform() executed ..." << endl;

    // Step 4: Compression
    ds.compression(ds.ZFAry, distanceChoice, ds.skeletonAry, skeletonFile, prettyPrintFile, logFile);
    cout << "compression() Executed ..." << endl;

    // Step 5: Close Skeleton File because it was loaded as an output file 
    skeletonFile.close();
    cout << "Skeleton File closed ..." << endl;
    logFile << "Skeleton File closed ..." << endl;

    // Step 6: Reopen Skeleton File to read from it 
    ifstream skeletonFileInput(argv[4]);
    cout << "Skeleton File read ..."  << endl;
    logFile << "Skeleton File read ..."  << endl;

    // Step 7 : Set ZF Array to zero to load in values of skeletonFile
    ds.setZero(ds.ZFAry);
    cout << "ZF array Zeroed out ..." << endl;
    logFile << "ZF array Zeroed out ..." << endl;
    
    // Step 8 : Load SkeletonFile to ZF Array
    ds.loadSkeleton(skeletonFileInput, ds.ZFAry, logFile);
    prettyPrintFile << "*** Below is the loaded skeleton with choice = " << distanceChoice << " ***" << endl;
    prettyPrintFile << "Header: " << numRows << " " << numCols << " " << ds.newMinVal << " " << ds.newMaxVal << endl;            
    ds.prettyPrint(ds.ZFAry, prettyPrintFile); 
    
    // Step 9 : Perform Decompression
    ds.deCompression(ds.ZFAry, distanceChoice, prettyPrintFile, logFile);
    cout << "decompression() Executed... " << endl;

    // Step 10: Print header of Output FIle deCompressedFile
    deCompressedFile << numRows << " " << numCols << " " << 0 << " " << 1 << endl;
    logFile << "*** deCompressed Header Printed ***" << endl;

    // Step 11: 
    ds.binThreshold(ds.ZFAry, deCompressedFile);
    logFile << "ZFArray printed onto deConpressedFIle";

    // Step 12: Close all Files
    inFile.close();
    prettyPrintFile.close();
    skeletonFileInput.close();
    deCompressedFile.close();
    logFile.close();

    return 0;
}//end-main