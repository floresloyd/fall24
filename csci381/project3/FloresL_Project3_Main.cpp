#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Enhancement {
    public:
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
        int thrVal;

        // Dynamically allocated 2D arrays
        int** mirrorFmAry;
        int** avgAry;
        int** medianAry;
        int** GaussAry;
        int** thrAry;
        int** mask2DAry;

        // 1D arrays
        int mask1DAry[9];
        int neighbor1DAry[9];

        int maskWeight;

        Enhancement(int rows, int cols, int maskRows, int maskCols, int threshold)
            : numRows(rows), numCols(cols), maskRows(maskRows), maskCols(maskCols), thrVal(threshold), newMin(9999), newMax(0) {
            mirrorFmAry = allocate2DArray(numRows + 2, numCols + 2);
            avgAry = allocate2DArray(numRows + 2, numCols + 2);
            medianAry = allocate2DArray(numRows + 2, numCols + 2);
            GaussAry = allocate2DArray(numRows + 2, numCols + 2);
            thrAry = allocate2DArray(numRows + 2, numCols + 2);
            mask2DAry = allocate2DArray(maskRows, maskCols); 

            for (int i = 0; i < 9; i++) {
                mask1DAry[i] = 0;
                neighbor1DAry[i] = 0;
            }
        }

        ~Enhancement() {
            deallocate2DArray(mirrorFmAry, numRows + 2);
            deallocate2DArray(avgAry, numRows + 2);
            deallocate2DArray(medianAry, numRows + 2);
            deallocate2DArray(GaussAry, numRows + 2);
            deallocate2DArray(thrAry, numRows + 2);
            deallocate2DArray(mask2DAry, maskRows);
        }

        int** allocate2DArray(int rows, int cols) {
            int** array = new int*[rows];
            for (int i = 0; i < rows; i++) {
                array[i] = new int[cols];
                for (int j = 0; j < cols; j++) {
                    array[i][j] = 0;
                }
            }
            return array;
        }

        void deallocate2DArray(int** array, int rows) {
            for (int i = 0; i < rows; i++) {
                delete[] array[i];
            }
            delete[] array;
        }

        void binThreshold(int** inAry, int** outAry, int numRows, int numCols, int thrVal, ofstream& logFile) {
            logFile << "*** Entering binThreshold method ***" << endl;

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (inAry[i][j] > thrVal) {
                        outAry[i][j] = 1;
                    } else {
                        outAry[i][j] = 0;
                    }
                }
            }

            logFile << "*** Exiting binThreshold method ***" << endl;
        }

        void prettyPrint(int** imgAry, int rows, int cols, ofstream& logFile) {
            logFile << "*** Enter PrettyPrint() ***" << endl;
            logFile << rows << " " << cols << " " << minVal << " " << maxVal << "\n";

            string maxValStr = to_string(maxVal);
            int Width = maxValStr.length();

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    logFile << imgAry[i][j] << " ";
                }
                logFile << "\n";
            }
            logFile << "*** Leaving PrettyPrint() ***" << endl << endl;
        }

        void mirrorFraming(int** mirrorFmAry, int numRows, int numCols, ofstream& logFile) {
            logFile << "*** Entering mirrorFraming() ***" << endl;

            for (int j = 1; j <= numCols; j++) {
                mirrorFmAry[0][j] = mirrorFmAry[1][j];
                mirrorFmAry[numRows + 1][j] = mirrorFmAry[numRows][j];
            }

            for (int i = 1; i <= numRows; i++) {
                mirrorFmAry[i][0] = mirrorFmAry[i][1];
                mirrorFmAry[i][numCols + 1] = mirrorFmAry[i][numCols];
            }

            mirrorFmAry[0][0] = mirrorFmAry[1][1];
            mirrorFmAry[0][numCols + 1] = mirrorFmAry[1][numCols];
            mirrorFmAry[numRows + 1][0] = mirrorFmAry[numRows][1];
            mirrorFmAry[numRows + 1][numCols + 1] = mirrorFmAry[numRows][numCols];

            logFile << "Mirror Framing With Corners Complete!" << endl;
            logFile << "*** Exiting mirrorFraming() ***";
        }

        void loadImage(string inputFile, int** mirrorFmAry, int numRows, int numCols, ofstream& logFile) {
            logFile << "*** Entering loadImage ***" << endl;

            ifstream inFile(inputFile);
            if (!inFile) {
                logFile << "Error: Cannot open input file!" << endl;
                cerr << "Error: Cannot open input file!" << endl;
                return;
            }

            int imgHeader1, imgHeader2, imgHeader3, imgHeader4;
            inFile >> imgHeader1 >> imgHeader2 >> imgHeader3 >> imgHeader4;
            logFile << "Header: " << imgHeader1 << " " << imgHeader2 << " " << imgHeader3 << " " << imgHeader4 << endl;

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    inFile >> mirrorFmAry[i][j];
                    logFile << "mirrorFmAry[" << i << "][" << j << "] = " << mirrorFmAry[i][j] << endl;
                }
            }

            logFile << "*** Leaving loadImage() method ***" << endl;
            inFile.close();
        }

        int loadMask(string maskFile, int** mask2DAry, int maskRows, int maskCols, ofstream& logFile) {
            logFile << "*** Entering loadMask() ***" << endl;

            ifstream mskFile(maskFile);
            if (!mskFile) {
                logFile << "Error: Cannot open mask file!" << endl;
                return -1;
            }

            int maskHeader1, maskHeader2;
            mskFile >> maskHeader1 >> maskHeader2;
            logFile << "Mask Header: " << maskHeader1 << " " << maskHeader2 << endl;

            int maskWeight = 0;
            for (int i = 0; i < maskRows; i++) {
                for (int j = 0; j < maskCols; j++) {
                    mskFile >> mask2DAry[i][j];
                    maskWeight += mask2DAry[i][j];
                    logFile << "mask2DAry[" << i << "][" << j << "] = " << mask2DAry[i][j] << endl;
                }
            }

            logFile << "Mask weight is: " << maskWeight << endl;
            logFile << "*** Exiting loadMask() ***" << endl;
            return maskWeight;
        }

        void loadMask2Dto1D(int** mask2DAry, int maskRows, int maskCols, int mask1DAry[], ofstream& logFile) {
            logFile << "*** Entering loadMask2Dto1D() ***" << endl;

            int index = 0;

            for (int i = 0; i < maskRows; i++) {
                for (int j = 0; j < maskCols; j++) {
                    mask1DAry[index] = mask2DAry[i][j]; 
                    logFile << "mask1DAry[" << index << "] = mask2DAry[" << i << "][" << j << "] = " << mask2DAry[i][j] << endl;
                    index++; 
                }
            }
            logFile << "*** Exiting loadMask2Dto1D() ***" << endl;
        }

        void loadNeighbor2Dto1D(int** mirrorFrmAry, int i, int j, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering loadNeighbor2Dto1D() ***" << endl;

            int index = 0;

            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    neighbor1DAry[index] = mirrorFmAry[r][c]; 
                    logFile << "neighbor1DAry[" << index << "] = mirrorFmAry[" << r << "][" << c << "] = " << mirrorFmAry[r][c] << endl;
                    index++;
                }
            }
            logFile << "*** Exiting loadNeighbor2Dto1D() ***" << endl;
        }

        void sort(int neighborAry[], ofstream& logFile) {
            logFile << "*** Entering sort() ***" << endl;

            for (int i = 0; i < 9 - 1; i++) {
                for (int j = 0; j < 9 - i - 1; j++) {
                    if (neighborAry[j] > neighborAry[j + 1]) {
                        swap(neighborAry[j], neighborAry[j + 1]);
                    }
                }
            }
            logFile << "*** Exiting sort() ***" << endl;
        }

        void computeMedian(int** mirrorFmAry, int numRows, int numCols, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering computeMedian() ***" << endl;

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile);
                    logFile << "Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    sort(neighbor1DAry, logFile);
                    logFile << "Below is the sorted Array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    medianAry[i][j] = neighbor1DAry[4];

                    if (this->newMin > medianAry[i][j]) {
                        this->newMin = medianAry[i][j];
                    }
                    if (this->newMax < medianAry[i][j]) {
                        this->newMax = medianAry[i][j];
                    }
                }
            }

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Exiting computeMedian() ***" << endl;
        }

        void computeMean(int** mirrorFmAry, int numRows, int numCols, int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering computeMean() ***" << endl;

            int mean = 0;

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile);
                    logFile << "Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    mean = 0;
                    for (int k = 0; k <= 8; k++) {
                        mean += neighbor1DAry[k];
                    }
                    mean = mean / 9;

                    logFile << "The mean for mirrorFmAry[" << i << "][" << j << "] is " << mean << endl;

                    avgAry[i][j] = mean;

                    if (this->newMin > avgAry[i][j]) {
                        this->newMin = avgAry[i][j];
                    }
                    if (this->newMax < avgAry[i][j]) {
                        this->newMax = avgAry[i][j];
                    }
                }
            }

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Exiting computeMean() ***" << endl;
        }

        void computeGauss(int** mirrorFmAry, int** GaussAry, int numRows, int numCols, int mask1DAry[], int maskWeight, ofstream& logFile) {
            logFile << "*** Entering computeGauss method ***" << endl;

            int neighbor1DAry[9];

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    loadNeighbor2Dto1D(mirrorFmAry, i, j, neighbor1DAry, logFile);
                    logFile << "** Below is conversion of mirrorFmAry [" << i << "][" << j << "] to 1D array" << endl;
                    print1DAry(neighbor1DAry, logFile);

                    logFile << "** Below is mask1DAry **" << endl;
                    print1DAry(mask1DAry, logFile);

                    GaussAry[i][j] = convolution(neighbor1DAry, mask1DAry, maskWeight, logFile);

                    if (this->newMin > GaussAry[i][j]) {
                        this->newMin = GaussAry[i][j];
                    }
                    if (this->newMax < GaussAry[i][j]) {
                        this->newMax = GaussAry[i][j];
                    }
                }
            }

            logFile << "New Min: " << this->newMin << ", New Max: " << this->newMax << endl;
            logFile << "*** Leaving computeGauss method ***" << endl;
        }

        int convolution(int neighbor1DAry[], int mask1DAry[], int maskWeight, ofstream& logFile) {
            logFile << "*** Entering convolution method ***" << endl;

            int result = 0;

            for (int i = 0; i < 9; i++) {
                result += neighbor1DAry[i] * mask1DAry[i];
            }

            if (maskWeight > 0) {
                result = result / maskWeight;
            } else {
                logFile << "Warning: Mask weight is zero!" << endl;
            }

            result = min(max(result, minVal), maxVal);

            logFile << "Convolution result: " << result << " (maskWeight = " << maskWeight << ")" << endl;
            logFile << "*** Leaving convolution method ***" << endl;

            return result;
        }

        void print1DAry(int neighbor1DAry[], ofstream& logFile) {
            logFile << "*** Entering print1DAry ***" << endl;
            logFile << "Array = [";

            for (int i = 0; i <= 8; i++) {
                logFile << neighbor1DAry[i] << ", ";
            }
            logFile << "]" << endl;
        }
};

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
