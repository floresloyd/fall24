#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 7) {
        cerr << "Usage: " << argv[0] << " <inputFile> <maskFile> <threshold> <avgOutFile> <medianOutFile> <gaussOutFile>" << endl;
        return 1;
    }

    ifstream inFile(argv[1]), maskFile(argv[2]);
    ofstream avgOutFile(argv[4]), medianOutFile(argv[5]), gaussOutFile(argv[6]), logFile("log.txt");

    if (!inFile.is_open()) {
        cerr << "Error opening input file: " << argv[1] << endl;
        return 1;
    }
    if (!maskFile.is_open()) {
        cerr << "Error opening mask file: " << argv[2] << endl;
        return 1;
    }
    if (!avgOutFile.is_open() || !medianOutFile.is_open() || !gaussOutFile.is_open()) {
        cerr << "Error opening one of the output files!" << endl;
        return 1;
    }

    int thrVal = atoi(argv[3]);

    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;

    int maskRows, maskCols, maskMin, maskMax;
    maskFile >> maskRows >> maskCols >> maskMin >> maskMax;

    Enhancement enhancement(numRows, numCols, maskRows, maskCols, thrVal);

    enhancement.maskWeight = enhancement.loadMask(argv[2], enhancement.mask2DAry, maskRows, maskCols, logFile);
    enhancement.loadMask2Dto1D(enhancement.mask2DAry, maskRows, maskCols, enhancement.mask1DAry, logFile);

    enhancement.loadImage(argv[1], enhancement.mirrorFmAry, numRows, numCols, logFile);

    enhancement.mirrorFraming(enhancement.mirrorFmAry, numRows, numCols, logFile);

    avgOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, avgOutFile);

    enhancement.computeMean(enhancement.mirrorFmAry, numRows, numCols, enhancement.neighbor1DAry, logFile);
    avgOutFile << "** Below is the 3x3 averaging of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.avgAry, numRows, numCols, avgOutFile);

    enhancement.binThreshold(enhancement.avgAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    avgOutFile << "** Below is the result of the binary threshold of averaging image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows, numCols, avgOutFile);

    medianOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, medianOutFile);

    enhancement.computeMedian(enhancement.mirrorFmAry, numRows, numCols, enhancement.neighbor1DAry, logFile);
    medianOutFile << "** Below is the 3x3 median filter of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.medianAry, numRows, numCols, medianOutFile);

    enhancement.binThreshold(enhancement.medianAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    medianOutFile << "** Below is the result of the binary threshold of median filtered image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows, numCols, medianOutFile);

    gaussOutFile << "** Below is the mirror framed input image. ***" << endl;
    enhancement.prettyPrint(enhancement.mirrorFmAry, numRows + 2, numCols + 2, gaussOutFile);

    gaussOutFile << "** Below is the mask for Gaussian filter. ***" << endl;
    enhancement.prettyPrint(enhancement.mask2DAry, maskRows, maskCols, gaussOutFile);

    enhancement.computeGauss(enhancement.mirrorFmAry, enhancement.GaussAry, numRows, numCols, enhancement.mask1DAry, enhancement.maskWeight, logFile);
    gaussOutFile << "** Below is the 3x3 Gaussian filter of the input image. ***" << endl;
    enhancement.prettyPrint(enhancement.GaussAry, numRows, numCols, gaussOutFile);

    enhancement.binThreshold(enhancement.GaussAry, enhancement.thrAry, numRows, numCols, thrVal, logFile);
    gaussOutFile << "** Below is the result of the binary threshold of Gaussian filtered image. ***" << endl;
    enhancement.prettyPrint(enhancement.thrAry, numRows, numCols, gaussOutFile);

    inFile.close();
    maskFile.close();
    avgOutFile.close();
    medianOutFile.close();
    gaussOutFile.close();
    logFile.close();

    return 0;
}