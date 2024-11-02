#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

struct property {
    int label;
    int numPixels;
    int minR;
    int minC;
    int maxR;
    int maxC;
};

class ccLabel {
  public:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int newLabel;
    int trueNumCC;
    int newMin;
    int newMax;
    int **zeroFramedAry;
    int NonZeroNeighborAry[5];
    int *EQAry;
    char option;
    property *CCproperty;

    ccLabel(std::ifstream &inFile, char option) {
        load(inFile);
        // initialize eqary with its index
        EQAry = new int[(numRows * numCols) / 4];
        for (int i = 0; i < (numRows * numCols) / 4; i++) {
            EQAry[i] = i;
        }
        this->option = option;
        newLabel = 0;
    }

    void load(std::ifstream &inFile) {
        // load the header
        inFile >> numRows >> numCols >> minVal >> maxVal;
        // allocate memory
        zeroFramedAry = new int *[numRows + 2];
        for (int i = 0; i < numRows + 2; i++) {
            zeroFramedAry[i] = new int[numCols + 2];
        }

        zero2D(zeroFramedAry);
        // print2D();

        // load the data
        // but leave 1 more row and column on each side
        int temp = 0;
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                inFile >> temp;
                zeroFramedAry[i][j] = temp;
            }
        }
        // print2D();
    }

    void zero2D(int **ary) {
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                ary[i][j] = 0;
            }
        }
    }

    // vood negative1D(int *ary){
    // }

    void connect4Pass1()  // int **zeroFramedAry, int &newLabel, int *EQAry)
    {
        // L -> R  T -> B
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                int temp = zeroFramedAry[i][j];
                if (zeroFramedAry[i][j] > 0) {
                    //    a
                    // b  x;
                    int a = zeroFramedAry[i - 1][j];
                    int b = zeroFramedAry[i][j - 1];
                    int size = 2;
                    int array[size];
                    array[0] = a;
                    array[1] = b;
                    std::sort(array, array + size);

                    int min1 = INT32_MAX;
                    int min2 = INT32_MAX;

                    for (int i = 0; i < size; i++) {
                        if (array[i] == 0) {
                            continue;
                        }
                        if (min1 == INT32_MAX) {
                            min1 = array[i];
                            min2 = array[i];
                        }
                        if (array[i] > min2) {
                            min2 = array[i];
                            break;
                        }
                    }

                    // case 1
                    if (array[0] == 0 && array[0] == array[1]) {
                        newLabel++;
                        zeroFramedAry[i][j] = newLabel;
                    }
                    // case 2
                    else if (min1 == min2) {
                        zeroFramedAry[i][j] = min1;
                    }
                    // case 3
                    else if (min1 != min2) {
                        zeroFramedAry[i][j] = min1;
                        EQAry[min2] = min1;
                    }
                }
            }
        }
    }

    void connect8Pass1()  // int **zeroFramedAry, int &newLabel, int *EQAry)
    {
        // L -> R  T -> B
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                // int temp = zeroFramedAry[i][j];

                if (zeroFramedAry[i][j] > 0) {
                    // a b c
                    // d x
                    int size = 4;
                    int tempAry[size];
                    tempAry[0] = zeroFramedAry[i - 1][j - 1];
                    tempAry[1] = zeroFramedAry[i - 1][j];
                    tempAry[2] = zeroFramedAry[i - 1][j + 1];
                    tempAry[3] = zeroFramedAry[i][j - 1];

                    std::sort(tempAry, tempAry + size);
                    int t = tempAry[size - 1];

                    int minIndex1 = -1;
                    int minIndex2 = -1;

                    for (int i = 0; i < size; i++) {
                        if (tempAry[i] == 0) {
                            continue;
                        }
                        minIndex1 = i;
                        break;
                    }

                    for (int i = minIndex1 + 1; i < size; i++) {
                        if (minIndex1 == -1) {
                            break;
                        }
                        if (tempAry[i] > tempAry[minIndex1]) {
                            minIndex2 = i;
                            break;
                        }
                    }

                    // case 1
                    // minIndex1 == -1 means all elements in tempAry are 0
                    if (minIndex1 == -1) {
                        newLabel++;
                        zeroFramedAry[i][j] = newLabel;
                    }
                    // case 2
                    // minIndex1 == minIndex2 means in tempAry there are some
                    // same elements
                    else if (minIndex1 == minIndex2) {
                        zeroFramedAry[i][j] = tempAry[minIndex1];
                    }
                    // case 3
                    // minIndex1 != minIndex2 means in tempAry there are some
                    // different none zeroelements
                    else if (minIndex1 != minIndex2) {
                        zeroFramedAry[i][j] = tempAry[minIndex1];
                        for (int i = minIndex1 + 1; i < size; i++) {
                            EQAry[tempAry[i]] = tempAry[minIndex1];
                        }
                    }
                }
            }
        }
    }

    void connect8Pass2() {
        // B -> T L -> R
        for (int i = numRows; i >= 1; i--) {
            for (int j = numCols; j >= 1; j--) {
                if (zeroFramedAry[i][j] > 0) {
                    //   x a
                    // b c d
                    int size = 5;
                    int tempAry[size];

                    int a = zeroFramedAry[i][j + 1];
                    int b = zeroFramedAry[i + 1][j - 1];
                    int c = zeroFramedAry[i + 1][j];
                    int d = zeroFramedAry[i + 1][j + 1];
                    int x = zeroFramedAry[i][j];

                    tempAry[0] = a;
                    tempAry[1] = b;
                    tempAry[2] = d;
                    tempAry[3] = c;
                    tempAry[4] = x;

                    std::sort(tempAry, tempAry + size);
                    int t = tempAry[size - 1];

                    int minIndex1 = -1;
                    int minIndex2 = -1;

                    for (int i = 0; i < size; i++) {
                        if (tempAry[i] == 0) {
                            continue;
                        }
                        minIndex1 = i;
                        break;
                    }

                    // case 1
                    if (a == 0 && b == 0 && c == 0 && d == 0) {
                        continue;
                    }

                    if (tempAry[minIndex1] != x) {
                        // case 2&3
                        // tempAry[minIndex] is none zero min of x, a, b, c, d
                        zeroFramedAry[i][j] = tempAry[minIndex1];
                        for (int i = minIndex1; i < size; i++) {
                            // update all none-zero elements to min label
                            EQAry[tempAry[i]] = tempAry[minIndex1];
                        }
                    }
                }
            }
        }
    }

    void connect4Pass2() {
        // B -> T L -> R
        for (int i = numRows; i >= 1; i--) {
            for (int j = numCols; j >= 1; j--) {
                if (zeroFramedAry[i][j] > 0) {
                    // x a
                    // b
                    int a = zeroFramedAry[i][j + 1];
                    int b = zeroFramedAry[i + 1][j];
                    int x = zeroFramedAry[i][j];

                    int tempAry[3] = {x, a, b};
                    std::sort(tempAry, tempAry + 3);

                    int minIndex = -1;

                    for (int i = 0; i < 3; i++) {
                        if (tempAry[i] == 0) {
                            continue;
                        }
                        minIndex = i;
                        break;
                    }

                    // case 1
                    if (a == 0 && a == b) {
                        continue;
                    }

                    // case 2&3
                    // tempAry[minIndex] is the min of a,b,x
                    if (tempAry[minIndex] != x) {
                        zeroFramedAry[i][j] = tempAry[minIndex];
                        for (int i = minIndex; i < 3; i++) {
                            // update all none-zero elements to min label
                            EQAry[tempAry[i]] = tempAry[minIndex];
                        }
                    }
                }
            }
        }
    }

    int manageEQAry() {
        int relabel = 0;
        int index = 1;

        while (index <= newLabel) {
            if (index != EQAry[index]) {
                EQAry[index] = EQAry[EQAry[index]];
            } else {
                relabel++;
                EQAry[index] = relabel;
            }
            index++;
        }
        return relabel;
    }

    void connectPass3(std::ofstream &deBugFile) {
        deBugFile << "entering connectPas3 method" << std::endl;
        CCproperty = new property[trueNumCC + 1];

        for (int i = 1; i <= trueNumCC; i++) {
            CCproperty[i].label = i;
            CCproperty[i].numPixels = 0;
            CCproperty[i].minR = numRows;
            CCproperty[i].maxR = 0;
            CCproperty[i].minC = numCols;
            CCproperty[i].maxC = 0;
        }

        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedAry[i][j] > 0) {
                    zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]];
                    int k = zeroFramedAry[i][j];

                    CCproperty[k].numPixels++;
                    if (i < CCproperty[k].minR) {
                        CCproperty[k].minR = i - 1;
                    }
                    if (i > CCproperty[k].maxR) {
                        CCproperty[k].maxR = i - 1;
                    }
                    if (j < CCproperty[k].minC) {
                        CCproperty[k].minC = j - 1;
                    }
                    if (j > CCproperty[k].maxC) {
                        CCproperty[k].maxC = j - 1;
                    }
                }
            }
        }
        deBugFile << "leaving connectPas3 method" << std::endl;
    }

    void connect4(std::ofstream &RFprettyPrintFile, std::ofstream &deBugFile) {
        deBugFile << "entering connected4 method" << std::endl;
        // pass1
        connect4Pass1();
        deBugFile << "After connected4 pass1, newLabel = " << newLabel
                  << std::endl;
        RFprettyPrintFile << "Result of: Pass 1" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass1 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // pass2
        connect4Pass2();
        deBugFile << "After connected4 pass2, newLabel = " << newLabel
                  << std::endl;
        RFprettyPrintFile << "Result of: Pass 2" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass2 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // EQ Table Management
        trueNumCC = manageEQAry();
        RFprettyPrintFile << "Equivalency Table after: EQ Table Management "
                             "(indexing starts from 1)"
                          << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);
        deBugFile << "In connected4, after manage EQAry, trueNumCC = "
                  << trueNumCC << std::endl;

        // pass3
        connectPass3(deBugFile);
        RFprettyPrintFile << "Result of: Pass 3" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass3 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        RFprettyPrintFile << "Number of Connected Components = " << trueNumCC
                          << std::endl;

        deBugFile << "Leaving connected4 method" << std::endl;
    }

    void connect8(std::ofstream &RFprettyPrintFile, std::ofstream &deBugFile) {
        deBugFile << "entering connected8 method" << std::endl;
        // pass1
        connect8Pass1();
        deBugFile << "After connected8 pass1, newLabel = " << newLabel
                  << std::endl;
        RFprettyPrintFile << "Result of: Pass 1" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass1 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // pass2
        connect8Pass2();
        deBugFile << "After connected8 pass2, newLabel = " << newLabel
                  << std::endl;
        RFprettyPrintFile << "Result of: Pass 2" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass2 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // EQ Table Management
        trueNumCC = manageEQAry();
        RFprettyPrintFile << "Equivalency Table after: EQ Table Management "
                             "(indexing starts from 1)"
                          << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);
        deBugFile << "In connected8, after manage EQAry, trueNumCC = "
                  << trueNumCC << std::endl;

        // pass3
        connectPass3(deBugFile);
        RFprettyPrintFile << "Result of: Pass 3" << std::endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile
            << "Equivalency Table after: Pass3 indexing starts from 1"
            << std::endl;
        printEQAry(newLabel, RFprettyPrintFile);

        RFprettyPrintFile << "Number of Connected Components = " << trueNumCC
                          << std::endl;

        deBugFile << "Leaving connected8 method" << std::endl;
    }

    void printEQAry(int newLabel, std::ofstream &outFile) {
        // index
        for (int i = 1; i <= newLabel; i++) {
            if (i < 10) {
                outFile << i << "  ";
            } else {
                outFile << i << " ";
            }
        }
        outFile << std::endl;

        // value
        for (int i = 1; i <= newLabel; i++) {
            if (EQAry[i] < 10) {
                outFile << EQAry[i] << "  ";
            } else {
                outFile << EQAry[i] << " ";
            }
        }
        outFile << std::endl;
        outFile << std::endl;
    }

    void printEQ() {
        int size = (numRows * numCols) / 4;
        for (int i = 0; i < size - 1; i++) {
            std::cout << EQAry[i] << " ";
        }
        std::cout << std::endl;
    }

    void print2D() {
        std::cout << numRows << " " << numCols << " " << minVal << " " << maxVal
                  << std::endl;
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                if (zeroFramedAry[i][j] > 9) {
                    std::cout << zeroFramedAry[i][j] << " ";
                } else {
                    std::cout << zeroFramedAry[i][j] << "  ";
                }
            }
            std::cout << std::endl;
        }
    }

    void conversion() {
        // in zeroFramedAry
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedAry[i][j] == 0) {
                    zeroFramedAry[i][j] = 1;
                } else {
                    zeroFramedAry[i][j] = 0;
                }
            }
        }
    }

    void imgReformat(int **Ary, std::ofstream &outFile, bool withZero = false) {
        outFile.fill(' ');
        int fieldWidth = std::to_string(newLabel).length() + 1;
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (Ary[i][j] == 0) {
                    if (withZero) {
                        outFile << std::setw(fieldWidth) << std::left << "0";
                    } else {
                        outFile << std::setw(fieldWidth) << std::left << ".";
                    }
                } else {
                    if (Ary[i][j] > 9) {
                        outFile << std::setw(fieldWidth) << std::left
                                << Ary[i][j];
                    } else {
                        outFile << std::setw(fieldWidth) << std::left
                                << Ary[i][j];
                    }
                }
            }
            outFile << std::endl;
        }
        outFile << std::endl;
    }

    void printCCproperty(std::ofstream &outFile) {
        outFile << numRows << " " << numCols << " " << minVal << " " << maxVal
                << std::endl;
        outFile << trueNumCC << std::endl;
        for (int i = 1; i <= trueNumCC; i++) {
            outFile << CCproperty[i].label << std::endl;
            outFile << CCproperty[i].numPixels << std::endl;
            outFile << CCproperty[i].minR << " " << CCproperty[i].minC
                    << std::endl;
            outFile << CCproperty[i].maxR << " " << CCproperty[i].maxC
                    << std::endl;
        }
    }

    void drawBoxes(std::ofstream &deBugFile) {
        deBugFile << "entering drawBoxes method" << std::endl;
        for (int i = 1; i <= trueNumCC; i++) {
            int minRow = CCproperty[i].minR + 1;
            int minCol = CCproperty[i].minC + 1;
            int maxRow = CCproperty[i].maxR + 1;
            int maxCol = CCproperty[i].maxC + 1;
            int label = CCproperty[i].label;
            // top line and bottom line
            for (int j = minCol; j <= maxCol; j++) {
                zeroFramedAry[minRow][j] = label;
                zeroFramedAry[maxRow][j] = label;
            }
            // left line and right line
            for (int j = minRow; j <= maxRow; j++) {
                zeroFramedAry[j][minCol] = label;
                zeroFramedAry[j][maxCol] = label;
            }
        }
        deBugFile << "leaving drawBoxes method" << std::endl;
    }
};

int main(int argc, char const *argv[]) {
    std::ifstream inFile(argv[1]);
    int Connectness = atoi(argv[2]);
    char option = *argv[3];
    std::ofstream RFprettyPrintFile(argv[4]);
    std::ofstream labelFile(argv[5]);
    std::ofstream propertyFile(argv[6]);
    std::ofstream deBugFile(argv[7]);

    // initialize the img
    ccLabel ccLabel(inFile, option);

    if (option == 'y' || option == 'Y') {
        ccLabel.conversion();
    }

    if (Connectness == 4) {
        ccLabel.connect4(RFprettyPrintFile, deBugFile);
    } else if (Connectness == 8) {
        ccLabel.connect8(RFprettyPrintFile, deBugFile);
    }

    labelFile << ccLabel.numRows << " " << ccLabel.numCols << " "
              << ccLabel.minVal << " " << ccLabel.maxVal << std::endl;
    ccLabel.imgReformat(ccLabel.zeroFramedAry, labelFile, true);

    ccLabel.printCCproperty(propertyFile);

    ccLabel.drawBoxes(deBugFile);
    RFprettyPrintFile << "\nAfter drawBoxes" << std::endl;
    ccLabel.imgReformat(ccLabel.zeroFramedAry, RFprettyPrintFile);

    return 0;
}
