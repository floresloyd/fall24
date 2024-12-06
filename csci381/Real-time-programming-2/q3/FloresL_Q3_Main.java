import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FloresL_Q3_Main {
    class chainCode{

        // Represents a point in a 2d image
        static class Point {
            int row;
            int col;   

            // Constructor
            Point(int r, int c){
                this.row = r;
                this.col = c;
            }
        }//end-static-class-Point

        // Represents  the properties of a cc in the image 
        static class CCproperty {
            int label;
            int numPixels;
            int minRow;
            int minCol;
            int maxRow;
            int maxCol;
            
            CCproperty(int label, int numpixel, int minrow, int mincol, int maxrow, int maxcol){
                this.label = label;
                this.numPixels = numpixel;
                this.minRow = minrow;
                this.minCol = mincol;
                this.maxRow = maxrow;
                this.minCol = maxcol;
            }
        }//end-static-class-CCproperty

        int numCC;          // Number of connected components in the image. From propFile
        CCproperty cc;      // 
        int numRows;        // number of rows from image
        int numCols;        // number of cols from image
        int minVal;         // min pixel value from image
        int maxVal;         // max pixel value from image

        int[][] imgAry;         // 2d array holding the image
        int[][] boundaryAry;    // 2d array to hold reconstructed image using chain code
        int[][] CCary;          // 2d array used to isolate and process each cc for chain code
        Point[] coordOffset;    // 2d array of fofsets representing the 8 possible directions [HARD CODED]
        int[] zeroTable;        // 1d array lookup table to determine the direction of last zero during chain code [HARD CODED]
        Point startP;           // starting point of a cc's boundary
        Point currentP;         // current point being processed
        Point nextP;            // next point to visit 
        
        int lastQ;               // last direction scanned (0-7) during chain code
        int nextDir;            // direction to scan during chain code traversal
        int PchainDir;          // direction (0-7) from currentP->nextP

        chainCode(int numRows, int numCols, int minVal, int maxVal){
                this.numRows = numRows;
                this.numCols = numCols;
                this.minVal = minVal;
                this.maxVal = maxVal;

                // Initialize arrays
                this.imgAry = new int[numRows + 2][numCols + 2]; // Zero frame
                this.boundaryAry = new int[numRows + 2][numCols + 2];
                this.CCary = new int[numRows + 2][numCols + 2];

                // Zero-frame the arrays
                zeroFrameArray(this.imgAry);
                zeroFrameArray(this.boundaryAry);
                zeroFrameArray(this.CCary);

                // 8 possible directions  (hard-coded)
                /**
                0: 0: (0, +1)    → Right
                1: (-1, +1)     ↗ Up-Right
                2: (-1, 0)      ↑ Up
                3: (-1, -1)     ↖ Up-Left
                4: (0, -1)      ← Left
                5: (+1, -1)     ↙ Down-Left
                6: (+1, 0)      ↓ Down
                7: (+1, +1)     ↘ Down-Right
                 */

                this.coordOffset = new Point[] {
                    new Point(0, 1),    // 0: Right
                    new Point(-1, 1),     // 1: Up-Right
                    new Point(-1, 0),     // 2: Up
                    new Point(-1, -1),      // 3: Up-Left
                    new Point(0, -1),     // 4: Left
                    new Point(1, -1),     // 5: Down-Left
                    new Point(1, 0),    // 6: Down
                    new Point(1, 1)     // 7: Down-Right
                };

                // Initialize zeroTable (hard-coded)
                // This is where we start after we jump
                this.zeroTable = new int[] {6, 0, 0, 2, 2, 4, 4, 6};

                // Initialize Points w Placeholder values
                this.startP = new Point(0, 0); 
                this.currentP = new Point(0, 0);
                this.nextP = new Point(0, 0);

                // Initialize other variables
                this.lastQ = 0;
                this.nextDir = 0;
                this.PchainDir = 0;

                // Default CCproperty initialization
                this.numCC = 0; 
        }//end-chainCode constructor 
        

        public void clearCCAry() {
            for (int i = 0; i < CCary.length; i++) {
                for (int j = 0; j < CCary[0].length; j++) {
                    CCary[i][j] = 0; // Set each element to zero
                }
            }
        }//end-clearCCAry



        public void getChainCode(CCproperty CC, int[][] CCAry, BufferedWriter chainCodeFile, BufferedWriter logFile, BufferedWriter zeroBoundaryFile) {
            try {
                logFile.write("Entering getChainCode method\n");
        
                // Write the header to the chainCodeFile
                chainCodeFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        
                // Scan the CCAry for the starting point
                for (int i = 1; i < CCAry.length - 1; i++) {
                    for (int j = 1; j < CCAry[0].length - 1; j++) {
                        if (CCAry[i][j] == CC.label) { // Found starting point
                            chainCodeFile.write(i + " " + j + " " + CC.label + "\n");
                            
                            // Record the zero pixel before the start point
                            // CHANGE #1 RECORD ALL ZERO POSITIONS 
                            int lastZeroRow = i + coordOffset[lastQ].row;
                            int lastZeroCol = j + coordOffset[lastQ].col;
                            zeroBoundaryFile.write(lastZeroRow + " " + lastZeroCol + "\n");
        
                            startP.row = i;
                            startP.col = j;
                            currentP.row = i;
                            currentP.col = j;
                            lastQ = 4;
        
                            do {
                                nextDir = (lastQ + 1) % 8; // Start from next direction
                                PchainDir = findNextP(currentP, CCAry, CC, nextDir, logFile, zeroBoundaryFile);
        
                                if (PchainDir == -1) {
                                    logFile.write("No further valid points found. Stopping chain code.\n");
                                    break; // Exit if no valid neighbor is found
                                }
        
                                chainCodeFile.write(PchainDir + " ");
        
                                // Update the next point
                                nextP.row = currentP.row + coordOffset[PchainDir].row;
                                nextP.col = currentP.col + coordOffset[PchainDir].col;
        
                                // Update currentP and lastQ
                                currentP.row = nextP.row;
                                currentP.col = nextP.col;
                                lastQ = PchainDir == 0 ? zeroTable[7] : zeroTable[PchainDir - 1];
        
                                logFile.write("lastQ=" + lastQ + ", currentP=(" + currentP.row + "," + currentP.col + "), " +
                                            "nextP=(" + nextP.row + "," + nextP.col + ")\n");
        
                            } while (!(currentP.row == startP.row && currentP.col == startP.col)); // Loop until we return to the start
        
                            chainCodeFile.write("\n");
                            chainCodeFile.flush();
                            zeroBoundaryFile.flush();
                            logFile.write("Leaving getChainCode method\n");
                            return;
                        }
                    }
                }
        
            } catch (IOException e) {
                System.err.println("Error in getChainCode: " + e.getMessage());
            }
        }
        

        public int findNextP(Point currentP, int[][] CCAry, CCproperty CC, int lastQ, BufferedWriter logFile, BufferedWriter zeroBoundaryFile) {
            try {
                logFile.write("Entering findNextP method\n");
        
                int index = lastQ;
                boolean found = false;
        
                // Iterate over all directions
                do {
                    int iRow = currentP.row + coordOffset[index].row;
                    int jCol = currentP.col + coordOffset[index].col;
        
                    // Check bounds
                    if (iRow >= 0 && iRow < CCAry.length && jCol >= 0 && jCol < CCAry[0].length) {
                        // Change 2: RECORD ON ZERO BOUNDARY FILE while looking for  next object pixel
                        if (CCAry[iRow][jCol] == 0) {
                            // Record zero pixel position
                            zeroBoundaryFile.write(iRow + " " + jCol + "\n");
                        } else if (CCAry[iRow][jCol] == CC.label) {
                            found = true;
                            logFile.write("In findNextP: index=" + index + ", iRow=" + iRow + ", jCol=" + jCol +
                                        ", chainDir=" + index + ", found=" + found + ", CCAry[" + iRow + "][" + jCol + "]=" + CCAry[iRow][jCol] + "\n");
                            return index; // Return the valid direction
                        }
                    }
        
                    index = (index + 1) % 8; // Move to the next direction
        
                } while (index != lastQ); // Stop if we scanned all directions
        
                // No valid direction found
                logFile.write("No valid direction found. Exiting findNextP.\n");
                return -1;
        
            } catch (IOException e) {
                System.err.println("Error in findNextP: " + e.getMessage());
            }
            return -1; // Fallback in case of an error
        }
    
        
        

        //  isolates one connected component (CC) from the image
        // It copies only the pixels belonging to a specific label (ccLabel) from the full image (imgAry) 
        // into a new array (CCAry). All other pixels are ignored.
        public void loadCCAry(int ccLabel) {
            for (int i = 1; i < imgAry.length - 1; i++) { // Skip the zero-frame
                for (int j = 1; j < imgAry[0].length - 1; j++) {
                    if (imgAry[i][j] == ccLabel) {
                        CCary[i][j] = ccLabel; // Copy matching label to CCary
                    } else {
                        CCary[i][j] = 0; // Set non-matching elements to 0
                    }
                }
            }
        }//end-loadCCAry
        

        private void zeroFrameArray(int[][] array) {
            int rows = array.length;
            int cols = array[0].length;
    
            // Zero the first and last rows
            for (int col = 0; col < cols; col++) {
                array[0][col] = 0;
                array[rows - 1][col] = 0;
            }
    
            // Zero the first and last columns
            for (int row = 0; row < rows; row++) {
                array[row][0] = 0;
                array[row][cols - 1] = 0;
            }
        }//end-zeroFrameArray


        public void loadImg(String inFile, int[][] zeroFramedAry) {
            try {
                // STEP 0
                // Instantiate a Buffered Reader for the file
                BufferedReader br = new BufferedReader(new FileReader(inFile));
                String[] header = br.readLine().trim().split("\\s+");     // Read header
                
                // Load the image into zeroFramedAry
                for (int i = 1; i < zeroFramedAry.length - 1; i++) {
                    String[] imageRow = br.readLine().trim().split("\\s+");
                    
                    for (int j = 1; j < zeroFramedAry[0].length - 1; j++) {
                        // Place image data into zeroFramedAry, adjusting indexing for imageRow
                        zeroFramedAry[i][j] = Integer.parseInt(imageRow[j - 1]); 
                    }
                }
                // Close the BufferedReader after use
                br.close();
            } catch (IOException e) {
                System.err.println("Error: " + e.getMessage());
            }
        }//end-loadImg



        public void prettyDotPrint(int[][] inAry, BufferedWriter prettyFile) {
            try {
                for (int i = 1; i < inAry.length - 1; i++) {
                    for (int j = 1; j < inAry[0].length - 1; j++) {
                        if (inAry[i][j] == 0) {
                            prettyFile.write(".  ");  // '.' and a fixed width of 2 for alignment
                        } else {
                            prettyFile.write(String.format("%-3d", inAry[i][j]));  // Pad numbers to width of 2
                        }
                    }
                    prettyFile.newLine();
                }
                prettyFile.newLine();
                prettyFile.flush();
            } catch (IOException e) {
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }//end-prettyDotPrint


        public void prettyPrint(int[][] inAry, BufferedWriter prettyFile) {
            try {
                for (int i = 1; i < inAry.length - 1; i++) {
                    for (int j = 1; j < inAry[0].length - 1; j++) {
                        if (inAry[i][j] == 0) {
                            prettyFile.write("   ");  // ' ' and a fixed width of 2 for alignment
                        } else {
                            prettyFile.write(String.format("%-3d", inAry[i][j]));  // Pad numbers to width of 2
                        }
                    }
                    prettyFile.newLine();
                }
                prettyFile.newLine();
                prettyFile.flush();
            } catch (IOException e) {
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }//end-prettyPrint

        public void constructBoundary(int[][] boundaryAry, BufferedReader chainCodeFile, BufferedReader zeroBoundaryFile) {
            try {
                String line;
                Point currentP = null;
                int label = -1;
        
                // First pass: Process chain code and construct boundary
                while ((line = chainCodeFile.readLine()) != null) {
                    String[] tokens = line.trim().split("\\s+");
        
                    if (tokens.length == 4) {
                        // Header
                        int numRows = Integer.parseInt(tokens[0]);
                        int numCols = Integer.parseInt(tokens[1]);
                        int minVal = Integer.parseInt(tokens[2]);
                        int maxVal = Integer.parseInt(tokens[3]);
                        System.out.println("Processing header: " + numRows + " " + numCols + " " + minVal + " " + maxVal);
        
                    } else if (tokens.length == 3) {
                        // Starting point
                        int startRow = Integer.parseInt(tokens[0]);
                        int startCol = Integer.parseInt(tokens[1]);
                        label = Integer.parseInt(tokens[2]);
        
                        currentP = new Point(startRow, startCol);
                        boundaryAry[startRow][startCol] = label;
                        System.out.println("Starting new component at: (" + startRow + ", " + startCol + ") with label " + label);
        
                    } else {
                        // Process chain directions
                        for (String dir : tokens) {
                            if (dir.trim().isEmpty()) continue;
                            
                            int chainDir = Integer.parseInt(dir);
                            if (chainDir < 0 || chainDir >= coordOffset.length) {
                                System.err.println("Invalid chainDir: " + chainDir);
                                continue;
                            }
        
                            currentP.row += coordOffset[chainDir].row;
                            currentP.col += coordOffset[chainDir].col;
                            boundaryAry[currentP.row][currentP.col] = label;
                        }
                    }
                }
                
                // CHANGE 3: Second pass: Mark zero pixels with numCC+1
                zeroBoundaryFile.mark(100000); // Mark the start of file to allow reset
                while ((line = zeroBoundaryFile.readLine()) != null) {
                    String[] coords = line.trim().split("\\s+");
                    if (coords.length == 2) {
                        int row = Integer.parseInt(coords[0]);
                        int col = Integer.parseInt(coords[1]);
                        if (boundaryAry[row][col] == 0) { // Only mark if it's still zero
                            boundaryAry[row][col] = numCC + 1;
                        }
                    }
                }
        
            } catch (IOException e) {
                System.err.println("Error in constructBoundary: " + e.getMessage());
            } catch (Exception e) {
                System.err.println("Unexpected error: " + e.getMessage());
            }
        }
        
        

        public void aryToFile(int[][] inAry, BufferedWriter fileOut, int numRows, int numCols, int minVal, int maxVal) {
            try {
                // Step 1: Write the image header
                fileOut.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        
                // Step 2: Write the pixel values including the frame
                for (int i = 0; i < inAry.length; i++) { // Include the zero frame
                    for (int j = 0; j < inAry[0].length; j++) {
                        fileOut.write(inAry[i][j] + " "); // Write pixel value followed by a space
                    }
                    fileOut.newLine(); // Move to the next line after each row
                }
        
                // Step 3: Flush the output to ensure data is written
                fileOut.flush();
        
            } catch (IOException e) {
                System.err.println("Error in AryToFile: " + e.getMessage());
            }
        }//end-arryToFile
        
    }//end-class-chainCode

    public static void main(String[] args) {
        // Step 0 
        String labelFile = args[0];
        String propFile = args[1];
        String outFile = args[2];
        String chainCodeFile = args[3];
        String boundaryFile = args[4];
        String logFile = args[5];
        String zeroBoundaryFile = args[6];
        
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
    
        int propRows;
        int propCols;
        int propMin;
        int propMax;
    
        try {
            // Read Input Header for input img
            BufferedReader br = new BufferedReader(new FileReader(labelFile));
            String inputHeader[] = br.readLine().trim().split("\\s+");
            // Instantiate chainCode Object
            numRows = Integer.parseInt(inputHeader[0]);
            numCols = Integer.parseInt(inputHeader[1]);
            minVal = Integer.parseInt(inputHeader[2]);
            maxVal = Integer.parseInt(inputHeader[3]);
            chainCode chainCode = new FloresL_Q3_Main().new chainCode(numRows, numCols, minVal, maxVal);
            
            // Read prop file
            BufferedReader prop_br = new BufferedReader(new FileReader(propFile));
            String propInputHeader[] = prop_br.readLine().trim().split("\\s+");
            propRows = Integer.parseInt(propInputHeader[0]);
            propCols = Integer.parseInt(propInputHeader[1]);
            propMin = Integer.parseInt(propInputHeader[2]);
            propMax = Integer.parseInt(propInputHeader[3]);
            propInputHeader = prop_br.readLine().trim().split("\\s+");
            chainCode.numCC = Integer.parseInt(propInputHeader[0]);
    
            // Instantiate Writer objects
            BufferedWriter outFileWriter = new BufferedWriter(new FileWriter(outFile));
            BufferedWriter chainCodeWriter = new BufferedWriter(new FileWriter(chainCodeFile));
            BufferedWriter boundaryWriter = new BufferedWriter(new FileWriter(boundaryFile));
            BufferedWriter logFileWriter = new BufferedWriter(new FileWriter(logFile));
            BufferedWriter zeroBoundaryWriter = new BufferedWriter(new FileWriter(zeroBoundaryFile));
            
            chainCode.loadImg(labelFile, chainCode.imgAry);
            outFileWriter.write("Below is the loaded imgAry of input labelFile \n");
            chainCode.prettyDotPrint(chainCode.imgAry, outFileWriter);
            
            // Verify that things are loaded
            System.out.println("Label File Header: " + numRows + " " + numCols + " " + minVal + " " + maxVal);
            System.out.println("Prop File Header: " + propRows + " " + propCols + " " + propMin + " " + propMax);
            System.out.println("Number of Connected Components: " + chainCode.numCC);
    
            // Step 2: Read Connected Component Properties
            for (int i = 0; i < chainCode.numCC; i++) {
                // Read CC label
                int label = Integer.parseInt(prop_br.readLine().trim());
                // Read number of pixels
                int numPixels = Integer.parseInt(prop_br.readLine().trim());
                // Read upper-left corner coordinates (minRow, minCol)
                String[] upperLeft = prop_br.readLine().trim().split("\\s+");
                int minRow = Integer.parseInt(upperLeft[0]);
                int minCol = Integer.parseInt(upperLeft[1]);
                // Read lower-right corner coordinates (maxRow, maxCol)
                String[] lowerRight = prop_br.readLine().trim().split("\\s+");
                int maxRow = Integer.parseInt(lowerRight[0]);
                int maxCol = Integer.parseInt(lowerRight[1]);
    
                // Create a CCproperty object and assign it to the chainCode object
                chainCode.cc = new chainCode.CCproperty(label, numPixels, minRow, minCol, maxRow, maxCol);
    
                // Debugging output
                logFileWriter.write("Below is the loaded CCAry of connected component label " + label + "\n"); 
                logFileWriter.write("CC " + (i + 1) + ": label=" + label + ", pixels=" + numPixels +
                        ", minRow=" + minRow + ", minCol=" + minCol + ", maxRow=" + maxRow + ", maxCol=" + maxCol + "\n");
                
                // Step 3 - Clear previous cc's data
                chainCode.clearCCAry();
            
                // Step 4 - read next cc's data
                chainCode.loadCCAry(label);
                outFileWriter.write("Connected Component Label = " + label + "\n");
                chainCode.prettyDotPrint(chainCode.CCary, outFileWriter);
    
                // Step 5 
                chainCode.getChainCode(chainCode.cc, chainCode.CCary, chainCodeWriter, logFileWriter, zeroBoundaryWriter);
            }//end-processing
    
            // Step 7
            chainCodeWriter.flush();
            chainCodeWriter.close();
            zeroBoundaryWriter.flush();
            zeroBoundaryWriter.close();
    
            // Step 8 
            BufferedReader chainCodeReader = new BufferedReader(new FileReader(chainCodeFile));
            BufferedReader zeroBoundaryReader = new BufferedReader(new FileReader(zeroBoundaryFile));
    
            // Step 9
            chainCode.constructBoundary(chainCode.boundaryAry, chainCodeReader, zeroBoundaryReader);
            outFileWriter.write("** Below is the objects boundaries of the input label image. \n");
            chainCode.prettyDotPrint(chainCode.boundaryAry, outFileWriter);
            boundaryWriter.write("** Below is the objects boundaries of the input label image. \n");
            chainCode.aryToFile(chainCode.boundaryAry, boundaryWriter, numRows, numCols, minVal, maxVal);
    
            // Step 10
            logFileWriter.flush(); // Flush log data to file
            br.close();
            prop_br.close();
            outFileWriter.close();
            boundaryWriter.close();
            logFileWriter.close();
            chainCodeReader.close();
            zeroBoundaryReader.close();
    
        } catch (IOException e){
            System.err.println("Error: " + e.getMessage());
        }
    }//end-main

}//end-FloresL_Project8_Main

/**
 COMPILE: javac FloresL_Q3_Main.java
 RUN    : java FloresL_Q3_Main.java img1CC.txt img1Property.txt outFile.txt chainCodeFile.txt boundaryFile.txt logFile.txt zeroBoundaryFile.txt

 args[0] - image file with header // labelfile
 args[1] - connectedComponent properties with format 
 args[2] - outfile1
 args[3] - chainCodeFile
 args[4] - boundaryFile
 args[5] - logFile
 args[6] - zeroBoundaryFile

 //


 Prop File

20 31 0 1       -- Header
1               -- # of connected components
1               -- Label
119             -- number of pixels
2 9             -- r,c of upper left
18 21           -- r,c of lower right 

 */



 /** 
GOAL:

modify chain code so that objects in the boundary file will be surrounded by marked zero-pixels  and output it to the zeroBoundaryFile.txt
should look like this 

2 15 // zero pixel at row 2 and column 15 
3 14 // zero pixel row 3 and column 14 
1 15// zero pixel row 3 and column 15

methods to modify
getChainCode():
record on zeroBoundaryFile the row and column position of lastZero which is the pixel before startrow and startcol

findNextP():
record on zeroboundaryfile the row and column position of each zero-pixel while currentP is looking for the next object pixel (== label)

constructBoundary():
after you use chaincodeFile to plot the boundary of objects you plot all zero pixels at the position recorded in zeroBoundaryFile using numCC+1 as the marker ie turn zero to numcc+1

  */