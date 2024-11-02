#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std; // Added to avoid prefixing std::

struct property {
    int label;      // Label of the connected component
    int numPixels;  // Number of pixels in the component
    int minR;       // Minimum row index
    int minC;       // Minimum column index
    int maxR;       // Maximum row index
    int maxC;       // Maximum column index
};

class ccLabel {
  public:
    int numRows;               // Number of rows in the image
    int numCols;               // Number of columns in the image
    int minVal;                // Minimum pixel value
    int maxVal;                // Maximum pixel value
    int newLabel;              // Counter for new labels
    int trueNumCC;             // True number of connected components
    int newMin;                // New minimum pixel value
    int newMax;                // New maximum pixel value
    int **zeroFramedAry;       // 2D array with zero frame
    int NonZeroNeighborAry[5]; // Array to hold non-zero neighbors
    int *EQAry;                // Equivalence array for labels
    char option;               // Option for processing
    property *CCproperty;      // Array of properties for each component

    // Constructor to initialize the object and load image data
    ccLabel(ifstream &inFile, char option) {
        load(inFile); // Load image data from file

        // Initialize EQAry with its index
        EQAry = new int[(numRows * numCols) / 4];
        for (int i = 0; i < (numRows * numCols) / 4; i++) {
            EQAry[i] = i;
        }
        this->option = option; // Set the processing option
        newLabel = 0;          // Initialize label counter
    }

    // Method to load image data from file
    void load(ifstream &inFile) {
        // Load the header information
        inFile >> numRows >> numCols >> minVal >> maxVal;

        // Allocate memory for zeroFramedAry with an extra border
        zeroFramedAry = new int *[numRows + 2];
        for (int i = 0; i < numRows + 2; i++) {
            zeroFramedAry[i] = new int[numCols + 2];
        }

        zero2D(zeroFramedAry); // Initialize the array with zeros

        // Load the image data into zeroFramedAry
        int temp = 0;
        for (int i = 1; i <= numRows; i++) {     // Start from 1 to leave a border
            for (int j = 1; j <= numCols; j++) { // Start from 1 to leave a border
                inFile >> temp;
                zeroFramedAry[i][j] = temp;
            }
        }
    }

    // Method to initialize a 2D array with zeros
    void zero2D(int **ary) {
        for (int i = 0; i < numRows + 2; i++) {     // Include the border
            for (int j = 0; j < numCols + 2; j++) { // Include the border
                ary[i][j] = 0; // Set each element to zero
            }
        }
    }

    // First pass for 4-connected component labeling
    void connect4Pass1() {
        // Traverse the image from top-left to bottom-right
        for (int i = 1; i <= numRows; i++) {     // Exclude the border
            for (int j = 1; j <= numCols; j++) { // Exclude the border
                int temp = zeroFramedAry[i][j];
                if (zeroFramedAry[i][j] > 0) { // If current pixel is foreground
                    // Get the neighbors
                    int a = zeroFramedAry[i - 1][j]; // Pixel above
                    int b = zeroFramedAry[i][j - 1]; // Pixel to the left
                    int size = 2;
                    int array[size];
                    array[0] = a;
                    array[1] = b;
                    sort(array, array + size); // Sort the neighbor labels (from= array, to= array + size / end)

                    int min1 = INT32_MAX; // Initialize min1
                    int min2 = INT32_MAX; // Initialize min2

                    // Find the smallest and second smallest non-zero labels
                    for (int k = 0; k < size; k++) {
                        if (array[k] == 0) {
                            continue; // Skip zero values
                        }
                        if (min1 == INT32_MAX) {
                            min1 = array[k];    // First iteration set to k
                            min2 = array[k];
                        }
                        if (array[k] > min2) {
                            min2 = array[k];    // Find the second mind which will be in index 1 
                            break;
                        }
                    }// Loop only runs twice 

                    // Case 1: Both neighbors are zero
                    if (array[0] == 0 && array[0] == array[1]) {
                        newLabel++;                  // Increment label counter
                        zeroFramedAry[i][j] = newLabel; // Assign new label
                    }
                    // Case 2: Neighbors have the same label
                    else if (min1 == min2) {
                        zeroFramedAry[i][j] = min1; // Assign existing label
                    }
                    // Case 3: Neighbors have different labels
                    else if (min1 != min2) {
                        zeroFramedAry[i][j] = min1; // Assign smallest label
                        EQAry[min2] = min1;         // Update equivalence array
                    }
                }
            }
        }
    }

    // Second pass for 4-connected component labeling
    void connect4Pass2() {
        // Traverse the image from bottom-right to top-left
        for (int i = numRows; i >= 1; i--) {       // Exclude the border
            for (int j = numCols; j >= 1; j--) {   // Exclude the border
                if (zeroFramedAry[i][j] > 0) { // If current pixel is foreground
                    // Get the neighbors
                    int a = zeroFramedAry[i][j + 1]; // Right
                    int b = zeroFramedAry[i + 1][j]; // Below
                    int x = zeroFramedAry[i][j];     // Current pixel

                    int tempAry[3] = {x, a, b};
                    sort(tempAry, tempAry + 3); // Sort the labels

                    int minIndex = -1;

                    // Find the smallest non-zero label
                    for (int k = 0; k < 3; k++) {
                        if (tempAry[k] == 0) {
                            continue; // Skip zero values
                        }
                        minIndex = k; // Find smallest value
                        break;
                    }

                    // Case 1: Both neighbors are zero
                    if (a == 0 && b == 0) {
                        continue; // No action needed
                    }

                    // Case 2: Neighbors have the same label – we simply use this label for the current pixel.
                    // Case 3: Neighbors have different labels – we assign the smallest label to the current pixel and update the equivalence array to reflect this relationship.
                    // Update the label and equivalence array
                    if (tempAry[minIndex] != x) { // CASE 2 && 3
                        zeroFramedAry[i][j] = tempAry[minIndex]; // Assign smallest label
                        for (int k = minIndex; k < 3; k++) {
                            EQAry[tempAry[k]] = tempAry[minIndex]; // Update equivalence array
                        }
                    }
                }
            }
        }
    }

    // First pass for 8-connected component labeling
    void connect8Pass1() {
        // Traverse the image from top-left to bottom-right
        for (int i = 1; i <= numRows; i++) {     // Exclude the border
            for (int j = 1; j <= numCols; j++) { // Exclude the border
                if (zeroFramedAry[i][j] > 0) { // If current pixel is foreground
                    // Get the neighbors
                    int size = 4;
                    int tempAry[size];
                    tempAry[0] = zeroFramedAry[i - 1][j - 1]; // Diagonal upper-left
                    tempAry[1] = zeroFramedAry[i - 1][j];     // Above
                    tempAry[2] = zeroFramedAry[i - 1][j + 1]; // Diagonal upper-right
                    tempAry[3] = zeroFramedAry[i][j - 1];     // Left

                    sort(tempAry, tempAry + size); // Sort the neighbor labels

                    int minIndex1 = -1;
                    int minIndex2 = -1;

                    // Find the smallest and second smallest non-zero labels
                    for (int k = 0; k < size; k++) {
                        if (tempAry[k] == 0) {
                            continue; // Skip zero values
                        }
                        minIndex1 = k;
                        break;
                    }

                    for (int k = minIndex1 + 1; k < size; k++) {
                        if (minIndex1 == -1) {
                            break; // No non-zero labels found
                        }
                        if (tempAry[k] > tempAry[minIndex1]) {
                            minIndex2 = k;
                            break;
                        }
                    }

                    // Case 1: All neighbors are zero
                    if (minIndex1 == -1) {
                        newLabel++;                  // Increment label counter
                        zeroFramedAry[i][j] = newLabel; // Assign new label
                    }
                    // Case 2: Neighbors have the same label
                    else if (minIndex1 == minIndex2) {
                        zeroFramedAry[i][j] = tempAry[minIndex1]; // Assign existing label
                    }
                    // Case 3: Neighbors have different labels
                    else if (minIndex1 != minIndex2) {
                        zeroFramedAry[i][j] = tempAry[minIndex1]; // Assign smallest label
                        for (int k = minIndex1 + 1; k < size; k++) {
                            EQAry[tempAry[k]] = tempAry[minIndex1]; // Update equivalence array
                        }
                    }
                }
            }
        }
    }

    // Second pass for 8-connected component labeling
    void connect8Pass2() {
        // Traverse the image from bottom-right to top-left
        for (int i = numRows; i >= 1; i--) {       // Exclude the border
            for (int j = numCols; j >= 1; j--) {   // Exclude the border
                if (zeroFramedAry[i][j] > 0) { // If current pixel is foreground
                    // Get the neighbors
                    int size = 5;
                    int tempAry[size];

                    int a = zeroFramedAry[i][j + 1];     // Right
                    int b = zeroFramedAry[i + 1][j - 1]; // Diagonal lower-left
                    int c = zeroFramedAry[i + 1][j];     // Below
                    int d = zeroFramedAry[i + 1][j + 1]; // Diagonal lower-right
                    int x = zeroFramedAry[i][j];         // Current pixel

                    tempAry[0] = a;
                    tempAry[1] = b;
                    tempAry[2] = d;
                    tempAry[3] = c;
                    tempAry[4] = x;

                    sort(tempAry, tempAry + size); // Sort the labels

                    int minIndex1 = -1;

                    // Find the smallest non-zero label
                    for (int k = 0; k < size; k++) {
                        if (tempAry[k] == 0) {
                            continue; // Skip zero values
                        }
                        minIndex1 = k;
                        break;
                    }

                    // Case 1: All neighbors are zero
                    if (a == 0 && b == 0 && c == 0 && d == 0) {
                        continue; // No action needed
                    }

                    // Update the label and equivalence array
                    if (tempAry[minIndex1] != x) {
                        zeroFramedAry[i][j] = tempAry[minIndex1]; // Assign smallest label
                        for (int k = minIndex1; k < size; k++) {
                            EQAry[tempAry[k]] = tempAry[minIndex1]; // Update equivalence array
                        }
                    }
                }
            }
        }
    }


    // Method to manage the equivalence array and relabel components
    // Returns the number of true components 
    int manageEQAry() {
        int relabel = 0;
        int index = 1;

        // Update equivalence labels
        while (index <= newLabel) {
            if (index != EQAry[index]) {            //  indicates that this label points to another label, implying an equivalence relationship.
                EQAry[index] = EQAry[EQAry[index]]; // Path compression; Point directly to the root label of its equivalence class.
            } else {
                relabel++;                          // Increment unique component count
                EQAry[index] = relabel;             // Assign new label
            }
            index++;
        }
        return relabel; // Return the true number of connected components
    }

    // Third pass to assign final labels and compute properties
    void connectPass3(ofstream &deBugFile) {
        deBugFile << "entering connectPass3 method" << endl;
        CCproperty = new property[trueNumCC + 1]; // Allocate properties array

        // Initialize properties for each component
        for (int i = 1; i <= trueNumCC; i++) {
            CCproperty[i].label = i;
            CCproperty[i].numPixels = 0;
            CCproperty[i].minR = numRows;
            CCproperty[i].maxR = 0;
            CCproperty[i].minC = numCols;
            CCproperty[i].maxC = 0;
        }

        // Traverse the image to update labels and properties
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedAry[i][j] > 0) {
                    zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]]; // Update label
                    int k = zeroFramedAry[i][j];

                    CCproperty[k].numPixels++; // Increment pixel count
                    // Update bounding box coordinates
                    if (i < CCproperty[k].minR) {
                        CCproperty[k].minR = i - 1; // Adjust for border
                    }
                    if (i > CCproperty[k].maxR) {
                        CCproperty[k].maxR = i - 1; // Adjust for border
                    }
                    if (j < CCproperty[k].minC) {
                        CCproperty[k].minC = j - 1; // Adjust for border
                    }
                    if (j > CCproperty[k].maxC) {
                        CCproperty[k].maxC = j - 1; // Adjust for border
                    }
                }
            }
        }
        deBugFile << "leaving connectPass3 method" << endl;
    }

    // Method to perform 4-connected component labeling
    void connect4(ofstream &RFprettyPrintFile, ofstream &deBugFile) {
        deBugFile << "entering connected4 method" << endl;
        // Pass 1
        connect4Pass1();
        deBugFile << "After connected4 pass1, newLabel = " << newLabel << endl;
        RFprettyPrintFile << "Result of: Pass 1" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass1 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // Pass 2
        connect4Pass2();
        deBugFile << "After connected4 pass2, newLabel = " << newLabel << endl;
        RFprettyPrintFile << "Result of: Pass 2" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass2 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // Equivalence Table Management
        trueNumCC = manageEQAry();
        RFprettyPrintFile << "Equivalency Table after: EQ Table Management (indexing starts from 1)" << endl;
        printEQAry(newLabel, RFprettyPrintFile);
        deBugFile << "In connected4, after manage EQAry, trueNumCC = " << trueNumCC << endl;

        // Pass 3
        connectPass3(deBugFile);
        RFprettyPrintFile << "Result of: Pass 3" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass3 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        RFprettyPrintFile << "Number of Connected Components = " << trueNumCC << endl;

        deBugFile << "Leaving connected4 method" << endl;
    }

    // Method to perform 8-connected component labeling
    void connect8(ofstream &RFprettyPrintFile, ofstream &deBugFile) {
        deBugFile << "entering connected8 method" << endl;
        // Pass 1
        connect8Pass1();
        deBugFile << "After connected8 pass1, newLabel = " << newLabel << endl;
        RFprettyPrintFile << "Result of: Pass 1" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass1 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // Pass 2
        connect8Pass2();
        deBugFile << "After connected8 pass2, newLabel = " << newLabel << endl;
        RFprettyPrintFile << "Result of: Pass 2" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass2 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        // Equivalence Table Management
        trueNumCC = manageEQAry();
        RFprettyPrintFile << "Equivalency Table after: EQ Table Management (indexing starts from 1)" << endl;
        printEQAry(newLabel, RFprettyPrintFile);
        deBugFile << "In connected8, after manage EQAry, trueNumCC = " << trueNumCC << endl;

        // Pass 3
        connectPass3(deBugFile);
        RFprettyPrintFile << "Result of: Pass 3" << endl;
        imgReformat(zeroFramedAry, RFprettyPrintFile);
        RFprettyPrintFile << "Equivalency Table after: Pass3 indexing starts from 1" << endl;
        printEQAry(newLabel, RFprettyPrintFile);

        RFprettyPrintFile << "Number of Connected Components = " << trueNumCC << endl;

        deBugFile << "Leaving connected8 method" << endl;
    }

    // Method to print the equivalence array to a file
    void printEQAry(int newLabel, ofstream &outFile) {
        // Print indices
        for (int i = 1; i <= newLabel; i++) {
            if (i < 10) {
                outFile << i << "  ";
            } else {
                outFile << i << " ";
            }
        }
        outFile << endl;

        // Print values
        for (int i = 1; i <= newLabel; i++) {
            if (EQAry[i] < 10) {
                outFile << EQAry[i] << "  ";
            } else {
                outFile << EQAry[i] << " ";
            }
        }
        outFile << endl;
        outFile << endl;
    }

    // Method to print the equivalence array to console
    void printEQ() {
        int size = (numRows * numCols) / 4;
        for (int i = 0; i < size - 1; i++) {
            cout << EQAry[i] << " ";
        }
        cout << endl;
    }

    // Method to print the 2D array to console
    void print2D() {
        cout << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
        for (int i = 0; i < numRows + 2; i++) {     // Include the border
            for (int j = 0; j < numCols + 2; j++) { // Include the border
                if (zeroFramedAry[i][j] > 9) {
                    cout << zeroFramedAry[i][j] << " ";
                } else {
                    cout << zeroFramedAry[i][j] << "  ";
                }
            }
            cout << endl;
        }
    }

    // Method to convert the image (invert foreground and background)
    void conversion() {
        // In zeroFramedAry, swap 0s and 1s
        for (int i = 1; i <= numRows; i++) {     // Exclude the border
            for (int j = 1; j <= numCols; j++) { // Exclude the border
                if (zeroFramedAry[i][j] == 0) {
                    zeroFramedAry[i][j] = 1;
                } else {
                    zeroFramedAry[i][j] = 0;
                }
            }
        }
    }

    // Method to reformat the image and output to a file
    // Part of pretty printing 
    void imgReformat(int **Ary, ofstream &outFile, bool withZero = false) {
        outFile.fill(' ');
        int fieldWidth = to_string(newLabel).length() + 1; // Determine field width
        for (int i = 1; i <= numRows; i++) {     // Exclude the border
            for (int j = 1; j <= numCols; j++) { // Exclude the border
                if (Ary[i][j] == 0) {
                    if (withZero) {
                        outFile << setw(fieldWidth) << left << "0"; // Output zero
                    } else {
                        outFile << setw(fieldWidth) << left << "."; // Output dot for background
                    }
                } else {
                    outFile << setw(fieldWidth) << left << Ary[i][j]; // Output label
                }
            }
            outFile << endl;
        }
        outFile << endl;
    }

    // Method to print connected component properties to a file
    void printCCproperty(ofstream &outFile) {
        outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
        outFile << trueNumCC << endl;
        for (int i = 1; i <= trueNumCC; i++) {
            outFile << CCproperty[i].label << endl;
            outFile << CCproperty[i].numPixels << endl;
            outFile << CCproperty[i].minR << " " << CCproperty[i].minC << endl;
            outFile << CCproperty[i].maxR << " " << CCproperty[i].maxC << endl;
        }
    }

    // Method to draw bounding boxes around connected components
    void drawBoxes(ofstream &deBugFile) {
        deBugFile << "entering drawBoxes method" << endl;
        for (int i = 1; i <= trueNumCC; i++) {
            int minRow = CCproperty[i].minR + 1; // Adjust for border
            int minCol = CCproperty[i].minC + 1; // Adjust for border
            int maxRow = CCproperty[i].maxR + 1; // Adjust for border
            int maxCol = CCproperty[i].maxC + 1; // Adjust for border
            int label = CCproperty[i].label;
            // Draw top and bottom lines
            for (int j = minCol; j <= maxCol; j++) {
                zeroFramedAry[minRow][j] = label;
                zeroFramedAry[maxRow][j] = label;
            }
            // Draw left and right lines
            for (int j = minRow; j <= maxRow; j++) {
                zeroFramedAry[j][minCol] = label;
                zeroFramedAry[j][maxCol] = label;
            }
        }
        deBugFile << "leaving drawBoxes method" << endl;
    }
};

int main(int argc, char const *argv[]) {
    ifstream inFile(argv[1]);           // Input file stream
    int Connectness = atoi(argv[2]);    // 4 or 8 connectedness
    char option = *argv[3];             // Option for conversion
    ofstream RFprettyPrintFile(argv[4]); // Output file for pretty print
    ofstream labelFile(argv[5]);         // Output file for labels
    ofstream propertyFile(argv[6]);      // Output file for properties
    ofstream deBugFile(argv[7]);         // Debug file

    // Initialize the ccLabel object
    ccLabel ccLabel(inFile, option);

    // Perform conversion if option is 'y' or 'Y'
    if (option == 'y' || option == 'Y') {
        ccLabel.conversion();
    }

    // Perform connected component labeling based on connectedness
    if (Connectness == 4) {
        ccLabel.connect4(RFprettyPrintFile, deBugFile);
    } else if (Connectness == 8) {
        ccLabel.connect8(RFprettyPrintFile, deBugFile);
    }

    // Output the labeled image
    labelFile << ccLabel.numRows << " " << ccLabel.numCols << " "
              << ccLabel.minVal << " " << ccLabel.maxVal << endl;
    ccLabel.imgReformat(ccLabel.zeroFramedAry, labelFile, true);

    // Output connected component properties
    ccLabel.printCCproperty(propertyFile);

    // Draw bounding boxes around components
    ccLabel.drawBoxes(deBugFile);
    RFprettyPrintFile << "\nAfter drawBoxes" << endl;
    ccLabel.imgReformat(ccLabel.zeroFramedAry, RFprettyPrintFile);

    return 0;
}


/**
 * COMMAND LINE ARGS 
argv[1] - Input image file name (string)
argv[2] - Connectedness (integer: 4 or 8)
argv[3] - Option for inversion ('y' or 'Y' to invert, any other character to skip)
argv[4] - Pretty print output file name (string)
argv[5] - Labeled image output file name (string)
argv[6] - Properties output file name (string)
argv[7] - Debug output file name (string)
COMPILE -> g++ cleaned_project_4.cpp  -o cleared_proj4
RUN     -> ./cleared_proj4  img1.txt 4 Y prettyPrint.txt labeledOutFile.txt propertiesOutFile.txt debugFile.txt
 */

/**
 * Processing Steps

1. Reading the Input Image:

- The program reads the header to get the image dimensions and pixel value range.
- It allocates a 2D array (zeroFramedAry) with an extra border (padding) around the image to simplify boundary conditions during processing.
- It loads the pixel values into zeroFramedAry, starting from index [1][1] to account for the border.

2. Optionally Inverting the Image:
- If the user specifies 'y' or 'Y' for inversion (argv[3]), the program inverts the image by swapping 0s and 1s.
- This is useful if the foreground and background are reversed in the input image.

3. Connected Component Labeling:
- The program performs CCL using either 4-connected or 8-connected methods based on the user's choice (argv[2]).
    1) First Pass:
        - It scans the image from top-left to bottom-right.
        - It assigns labels to pixels and records equivalence relationships when different labels are found to be connected.

    2) Second Pass:
        - It scans the image from bottom-right to top-left.
        - It resolves label conflicts by updating labels based on the equivalence relationships.

    3) Equivalence Table Management:
        - It processes the equivalence array (EQAry) to assign final labels to each connected component.

    4) Third Pass:
        - It updates the image with the final labels.
        - It computes properties for each connected component (label, pixel count, bounding box coordinates).


4. Outputting Results:
    - Pretty Print File (argv[4]):
        ~ Contains the intermediate and final labeled images after each pass.
        ~ Shows the equivalence table at each stage.

    - Labeled Image File (argv[5]):
        ~ Contains the final labeled image in a format similar to the input.
    
    - Properties File (argv[6]):
        ~ Lists the properties of each connected component (label, pixel count, bounding box).

    - Debug File (argv[7]):
        ~ Logs detailed debugging information during processing.

5. Drawing Bounding Boxes:
    - The program can draw bounding boxes around each connected component in the image by modifying zeroFramedAry based on the computed properties.
 */


/**
 * Equivalence Table
1. First Pass (connect4Pass1 or connect8Pass1): Initial Labeling
  In Pass 1, the algorithm scans each pixel in the image (top-left to bottom-right) and assigns preliminary labels:
    - If a pixel has non-zero neighbors (foreground), it compares labels of neighboring pixels to determine the smallest label to assign.
    - If a conflict arises (i.e., neighboring labels are different but non-zero), the smallest label is assigned to the current pixel, and the equivalence table is updated to reflect that the two labels are equivalent.
    EX: EQAry[b] = a; // This means label `b` is equivalent to label `a`.


2. Second Pass (connect4Pass2 or connect8Pass2): Correcting Labels Using Equivalences
    - In Pass 2, the image is scanned in reverse order (bottom-right to top-left). For each pixel, if its label does not match the smallest label in its neighborhood (according to EQAry), it is updated to the smallest label, thereby "correcting" the label inconsistencies left over from Pass 1.
    - During this pass, the equivalence table is used but not modified. Instead, it helps ensure that all pixels belonging to the same connected component adopt the smallest label in their equivalence class.

    EX: For a pixel labeled b with a smaller neighboring label a, Pass 2 uses EQAry to ensure the pixel’s label reflects the smallest equivalent label.
        If EQAry[b] points to a, Pass 2 will relabel the pixel as a, ensuring consistency.


3. Equivalence Management (manageEQAry): Resolving Final Labels
    - After Pass 2, manageEQAry is called to resolve all labels in EQAry, compressing paths and assigning unique, sequential labels for each connected component.
    - Path Compression: Each label in EQAry is updated to point directly to the root label of its equivalence class. This step ensures that any label referring to the same connected component now points to a single representative label.
    EX: If EQAry contains entries like [1, 1, 1, 2, 1, ...], where 2 points to 1, manageEQAry will make every entry point directly to 1, ensuring consistency.

4. Third Pass (connectPass3): Applying Final Labels and Computing Component Properties

    - Pass 3 uses the now-resolved EQAry to finalize the labels on the image. Each pixel’s label is updated according to the final values in EQAry, ensuring all pixels within the same connected component have the same label.
    - During this pass, CCproperty is populated with details about each connected component (like pixel count and bounding box dimensions), using the final labels determined by EQAry.

 */


                //     if (ZFAry[i][j] > 0){
                //         int a = ZFAry[i - 1][j];
                //         int b = ZFAry[i][j - 1];
                //         int array[2];
                //         array[0] = a;
                //         array[1] = b;
                //         sort(array, array + 2);

                //         int min
                //     }
                // }