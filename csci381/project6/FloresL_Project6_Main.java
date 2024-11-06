package project6;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOError;
import java.io.IOException;

import javax.print.DocFlavor.STRING;

public class FloresL_Project6_Main {

    class Property {
        /**
         
          In the Cartesian coordinate system, any rectangular box 
          can be represented by two points: upper-left corner and 
          the lower-right of the box. Here, the two points:

            (minR minC) and (maxR maxC)
            
        the smallest rectangular box that a cc can fit in the box; 
        object pixels can be on the border of the box.  

         */
        int label;      // Component label
        int numPixels;  // total number of pixel in cc
        int minR;       // with respect to input img
        int minC;       
        int maxR;
        int maxC; 

        public Property(int label, int numPixels, int minR, int minC, int maxR, int maxC){
            this.label = label;
            this.numPixels = numPixels;
            this.minR = minR;
            this.minC = minC;
            this.maxR = maxR;
            this.maxC = maxC;
        }//end-constructor
    }//end-class-Property 

    class ccLabel {
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int newLabel;   // Init to 0 
        int trueNumCC;  // The Property array (CCproperty) should store the bounding box and pixel count for each connected component. 
                        // After manageEQTable, each unique connected component will have a final label from 1 to trueNumCC. 
                        // Each entry in Property represents one of these connected components.

        int newMin;     // Init to 0
        int newMax;     // set to trueNumCC

        // Arrays
        int[][] zeroFramedAry;      // array[numRows + 2][numCols + 2]
        int[] nonZeroNeighborAry;   // array[5] that keeps track of the neighbors 
        int[] EQtable;              // array[(numRows * numCols) / 4]
        int[] negative1D;
        Property[] CCproperty;

        public ccLabel(int numRows, int numCols, int minVal, int maxVal){
            this.numRows = numRows;
            this.numCols = numCols;
            this.minVal = minVal;
            this.maxVal = maxVal;
            this.newLabel = 0;
            this.trueNumCC = 0; 
            this.newMin = 0;
            this.maxVal = trueNumCC;
            
            this.zeroFramedAry = new int[numRows + 2][numCols + 2];
            zero2D(zeroFramedAry);
            
            this.negative1D = new int[(numRows + 2) * (numCols + 2)];
            negative1D(negative1D);

            this.nonZeroNeighborAry = new int[5];
            zero1D(nonZeroNeighborAry);

            this.EQtable = new int[(numRows * numCols) / 4];
            for (int i = 0; i < EQtable.length; i++){
                EQtable[i] = i;
            }

        }//end-constructor-ccLabel

        /**
         * @loadImg Load imgFile to zeroFramedAry inside of frame
         * @param inFile Path to input image file
         * @param zeroFramedAry 2D Array that will hold the image
         */
        public void loadImg(String inFile, int[][] zeroFramedAry) {
            try {
                // Instantiate a Buffered Reader for the file
                BufferedReader br = new BufferedReader(new FileReader(inFile));
                br.readLine();     // Skip the header 
        
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

        public void connected4(int[][] zeroFramedAry, int[] EQtable, BufferedWriter prettyPrintFile, BufferedWriter logFile) {
            try {
                // Step 0
                logFile.write("Entering connected4 method \n");
                
                // Step 1: Perform the first pass
                connected4Pass1(zeroFramedAry, EQtable);
                prettyPrintFile.write("Result of: Pass 1 \n");
                logFile.write("After connected4 pass1, newLabel = " + newLabel + "\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 1 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);
        
                // Step 2: Perform the second pass
                connected4Pass2(zeroFramedAry, EQtable);
                prettyPrintFile.write("Result of: Pass 2\n");
                logFile.write("After connected4 pass2, newLabel = " + newLabel + "\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 2 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);
        
                // Step 3: Manage the equivalency table and determine the true number of connected components
                trueNumCC = manageEQTable(EQtable, newLabel);
                prettyPrintFile.write("Equivalency Table after: EQ Table Management (indexing starts from 1)\n");
                printEQTable(newLabel, prettyPrintFile);
                newMin = 0;
                newMax = trueNumCC;
                logFile.write("In connected4, after manageEQTable, trueNumCC = " + trueNumCC + "\n");
        
                // Step 4: Run the third pass to finalize the labels
                connectPass3(zeroFramedAry, EQtable);
                prettyPrintFile.write("After connected4 pass3\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 3 (indexing starts from 1) \n");
                prettyPrintFile.write("Number of Connected Components: " + trueNumCC + "\n\n");
        
                prettyPrintFile.flush();
                logFile.flush();
            } catch(IOException e) {
                System.err.println("ERROR: " + e.getMessage());
            }
        }

        public void connected4Pass1(int[][] zeroFramedAry, int[] EQtable) {
            for (int i = 1; i < zeroFramedAry.length - 1; i++) {
                for (int j = 1; j < zeroFramedAry[0].length - 1; j++) {
                    if (zeroFramedAry[i][j] > 0) {
                        int b = zeroFramedAry[i - 1][j];  // Top neighbor
                        int d = zeroFramedAry[i][j - 1];  // Left neighbor
        
                        // Case 1: Both neighbors are zero
                        if (b == 0 && d == 0) {
                            // Assign a new label
                            newLabel++;
                            zeroFramedAry[i][j] = newLabel;
                        } else {
                            // Gather non-zero neighbors
                            int minLabel = Integer.MAX_VALUE;
                            if (b > 0) minLabel = Math.min(minLabel, b);
                            if (d > 0) minLabel = Math.min(minLabel, d);
        
                            // Assign the minimum non-zero label
                            zeroFramedAry[i][j] = minLabel;
        
                            // Update the EQ table for all non-zero neighbors with different labels
                            if (b > 0 && b != minLabel) EQtable[b] = minLabel;
                            if (d > 0 && d != minLabel) EQtable[d] = minLabel;
                        }
                    }
                }
            }
        }
        
        public void connected4Pass2(int[][] zeroFramedAry, int[] EQtable) {
            for (int i = zeroFramedAry.length - 2; i > 0; i--) {  // Bottom-to-top
                for (int j = zeroFramedAry[0].length - 2; j > 0; j--) {  // Right-to-left
                    if (zeroFramedAry[i][j] > 0) {
                        int g = zeroFramedAry[i + 1][j];  // Bottom neighbor
                        int e = zeroFramedAry[i][j + 1];  // Right neighbor
                        int currentLabel = zeroFramedAry[i][j];
        
                        // Case 1: Both neighbors are zero, do nothing
                        if (g == 0 && e == 0) {
                            continue;
                        } else {
                            // Gather non-zero neighbors to find the minimum label
                            int minLabel = Integer.MAX_VALUE;
                            if (g > 0) minLabel = Math.min(minLabel, g);
                            if (e > 0) minLabel = Math.min(minLabel, e);
        
                            // Assign the minimum label if necessary and update EQTable
                            if (currentLabel != minLabel) {
                                zeroFramedAry[i][j] = minLabel;
                                EQtable[currentLabel] = minLabel;
                            }
                        }
                    }
                }
            }
        }
        
        public void connected8(int[][] zeroFramedAry, int[] EQtable, BufferedWriter prettyPrintFile, BufferedWriter logFile){
            try {
                // Step 0
                logFile.write("Entering connected8 method \n");
                
                // Step 1
                connected8Pass1(zeroFramedAry, EQtable);
                prettyPrintFile.write("Result of: Pass 1 \n");
                logFile.write("After connected8 pass1, newLabel = " + newLabel + "\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 1 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);

                // Step 2
                connected8Pass2(zeroFramedAry, EQtable);
                prettyPrintFile.write(" Result of: Pass 2\n");
                logFile.write("After connected8 pass2, newLabel = " + newLabel + "\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 2 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);

                // Step 3
                trueNumCC = manageEQTable(EQtable, newLabel);
                prettyPrintFile.write("Equivalency Table after: EQ Table Management (indexing starts from 1)\n");
                printEQTable(newLabel, prettyPrintFile);
                newMin = 0;
                newMax = trueNumCC;
                logFile.write("In connected8, after manageEQTable, trueNumCC = " + trueNumCC + "\n");
                
                // Step 4 
                connectPass3(zeroFramedAry, EQtable);
                prettyPrintFile.write("After connected8 pass2, newLabel = " + newLabel + "\n");
                prettyPrintFile.write(" Result of: Pass 3\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 3 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);
                prettyPrintFile.write("Number of Connected Compontents: " + trueNumCC + "\n\n");
                
                prettyPrintFile.flush();
                logFile.flush();
            } catch(IOException e) {
                System.err.println("ERROR :" + e.getMessage());
            }
        }

        public void connected8Pass1(int[][] zeroFramedAry, int[] EQTable) {
            for (int i = 1; i < zeroFramedAry.length - 1; i++) {
                for (int j = 1; j < zeroFramedAry[0].length - 1; j++) {
                    if (zeroFramedAry[i][j] > 0) {
                        int a = zeroFramedAry[i - 1][j - 1];
                        int b = zeroFramedAry[i - 1][j];
                        int c = zeroFramedAry[i - 1][j + 1];
                        int d = zeroFramedAry[i][j - 1];
                        
                        // Case 1: All neighbors are zero
                        if (a == 0 && b == 0 && c == 0 && d == 0) {
                            // Case 1: Assign a new label
                            newLabel++;
                            zeroFramedAry[i][j] = newLabel;
                        } else {
                            // Gather non-zero neighbors
                            int minLabel = Integer.MAX_VALUE;
                            if (a > 0) minLabel = Math.min(minLabel, a);
                            if (b > 0) minLabel = Math.min(minLabel, b);
                            if (c > 0) minLabel = Math.min(minLabel, c);
                            if (d > 0) minLabel = Math.min(minLabel, d);
        
                            // Case 2 or Case 3: Assign the minimum non-zero label
                            // Case 2: Some neighbors have the same non-zero label
                            // P(i, j) is part of a connected component

                             // Case 3: At least two neighbors have different labels
                             // This means that P(i, j) is boundaried by two or 
                             //more other connected components
                        
                            zeroFramedAry[i][j] = minLabel;
        
                            // Update the EQ table for all non-zero neighbors with different labels
                            if (a > 0 && a != minLabel) EQtable[a] = minLabel;
                            if (b > 0 && b != minLabel) EQtable[b] = minLabel;
                            if (c > 0 && c != minLabel) EQtable[c] = minLabel;
                            if (d > 0 && d != minLabel) EQtable[d] = minLabel;
                        }
                    }
                }
            }
        }

        public void connected8Pass2(int[][] zeroFramedAry, int[] EQtable) {
            for (int i = zeroFramedAry.length - 2; i > 0; i--) {  // Bottom-to-top
                for (int j = zeroFramedAry[0].length - 2; j > 0; j--) {  // Right-to-left
                    if (zeroFramedAry[i][j] > 0) {
                        int e = zeroFramedAry[i][j + 1];
                        int f = zeroFramedAry[i + 1][j - 1];
                        int g = zeroFramedAry[i + 1][j];
                        int h = zeroFramedAry[i + 1][j + 1];
                        int currentLabel = zeroFramedAry[i][j];
                        
                        // Case 1: All neighbors are zero, do nothing
                        if (e == 0 && f == 0 && g == 0 && h == 0) {
                            continue;
                        } else {
                            // Gather non-zero neighbors to find the minimum label
                            int minLabel = Integer.MAX_VALUE;
                            if (e > 0) minLabel = Math.min(minLabel, e);
                            if (f > 0) minLabel = Math.min(minLabel, f);
                            if (g > 0) minLabel = Math.min(minLabel, g);
                            if (h > 0) minLabel = Math.min(minLabel, h);
        
                            // Case 2: All non-zero neighbors have the same label as P(i, j)
                            // Do nothing as labels are consistent
                            
                            // Case 3: Some neighbors have different labels than P(i, j)
                            // Assign the minimum label and update EQTable if necessary
                            if (currentLabel != minLabel) {
                                zeroFramedAry[i][j] = minLabel;
                                // Update EQ table to reflect equivalences
                                EQtable[currentLabel] = minLabel;
                            }
                        }
                    }
                }
            }
        }

        //  Creates and populates the CCproperty array with the bounding box and pixel count for each connected component based on final labels.
        public void connectPass3(int[][] zeroFramedAry, int[] EQtable) {
            // Initialize the CCproperty array based on the true number of components
            CCproperty = new Property[trueNumCC + 1];
            
            // Initialize Property objects for each component
            for (int i = 1; i <= trueNumCC; i++) {
                CCproperty[i] = new Property(i, 0, numRows, numCols, 0, 0);  // Set minR/minC to bounds, not MAX_VALUE
            }
            
            // Iterate over zeroFramedAry within the actual image area only
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    int label = zeroFramedAry[i][j];
                    if (label > 0) {
                        // Update zeroFramedAry with the final label from EQtable
                        int finalLabel = EQtable[label];
                        zeroFramedAry[i][j] = finalLabel;
                        
                        // Update the Property for the connected component
                        Property prop = CCproperty[finalLabel];
                        prop.numPixels++;
                        
                        // Update bounding box coordinates within actual image bounds
                        prop.minR = Math.min(prop.minR, i - 1);  // Adjusting for 0-based indexing
                        prop.minC = Math.min(prop.minC, j - 1);
                        prop.maxR = Math.max(prop.maxR, i - 1);
                        prop.maxC = Math.max(prop.maxC, j - 1);
                    }
                }
            }
        }

        
        


        // method processes the equivalence table (EQtable) 
        // after the two passes of connected component labeling 
        // to assign final labels to each connected component. 
        
        public int manageEQTable(int[] EQtable, int newLabel) {
            int realLabel = 0;
            
            for (int i = 1; i <= newLabel; i++) {
                // Path compression to ensure each index points to its root label
                if (EQtable[i] != i) {
                    EQtable[i] = EQtable[EQtable[i]];  // Path compression
                } else {
                    realLabel++;
                    EQtable[i] = realLabel;  // Assign final compact label
                }
            }
        
            return realLabel; // Return the number of unique components
        }
        
        
        

        /**
         * @prettyDotPrint Prints out the input array in a formatted manner 
         * @param inAry
         * @param prettyPrintFile
         */
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
        }
        
        
        public void printEQTable(int newLabel, BufferedWriter prettyFile) {
            try {
                // Print indices with a fixed width
                for (int i = 1; i <= newLabel; i++) {
                    prettyFile.write(String.format("%-3d", i));
                }
                prettyFile.write("\n");
        
                // Print values in EQ table with the same fixed width
                for (int i = 1; i <= newLabel; i++) {
                    prettyFile.write(String.format("%-3d", EQtable[i]));
                }
                prettyFile.write("\n\n");
                prettyFile.flush();
            } catch (IOException e) {
                System.err.println("Error in printEQTable: " + e.getMessage());
            }
        }
        


        public void printCCproperty(BufferedWriter propertyFile) {
            try {
                // Print header information for image size and min/max values
                propertyFile.write(numRows + " " + numCols + " " + newMin + " " + newMax + "\n");
                propertyFile.write(trueNumCC + "\n");

                // Print each component's label, pixel count, and bounding box
                for (int i = 1; i <= trueNumCC; i++) {
                    Property prop = CCproperty[i];
                    propertyFile.write(prop.label + "\n");
                    propertyFile.write(prop.numPixels + "\n");
                    propertyFile.write(prop.minR + " " + prop.minC + "\n");
                    propertyFile.write(prop.maxR + " " + prop.maxC + "\n");
                }

                propertyFile.flush();
            } catch (IOException e) {
                System.err.println("Error in printCCproperty: " + e.getMessage());
            }
        }

        public void drawBoxes(int[][] zeroFramedAry, Property[] CCproperty, int trueNumCC, BufferedWriter logFile) {
            try {
                // Step 0: Log entry
                logFile.write("Entering drawBoxes method\n");
        
                // Step 1: Initialize index to 1
                int index = 1;
        
                // Step 5: Loop through each connected component
                while (index <= trueNumCC) {
                    // Step 2: Extract bounding box properties for the current component
                    int minRow = CCproperty[index].minR + 1;
                    int minCol = CCproperty[index].minC + 1;
                    int maxRow = CCproperty[index].maxR + 1;
                    int maxCol = CCproperty[index].maxC + 1;
                    int label = CCproperty[index].label;
        
                    // Step 3: Draw the box
                    // Draw top border
                    for (int j = minCol; j <= maxCol; j++) {
                        zeroFramedAry[minRow][j] = label;
                    }
        
                    // Draw bottom border
                    for (int j = minCol; j <= maxCol; j++) {
                        zeroFramedAry[maxRow][j] = label;
                    }
        
                    // Draw left border
                    for (int i = minRow; i <= maxRow; i++) {
                        zeroFramedAry[i][minCol] = label;
                    }
        
                    // Draw right border
                    for (int i = minRow; i <= maxRow; i++) {
                        zeroFramedAry[i][maxCol] = label;
                    }
        
                    // Step 4: Move to the next component
                    index++;
                }
        
                // Step 6: Log exit
                logFile.write("Leaving drawBoxes method\n");
                logFile.flush();
            } catch (IOException e) {
                System.err.println("Error in drawBoxes: " + e.getMessage());
            }
        }
                 
        
        /**
         * Zeroes out all values of input array
         * @param arr
         */
        public void zero2D(int[][] arr){
            for(int i = 1; i < arr.length; i++){
                for(int j = 1; j < arr[0].length; j++){
                    arr[i][j] = 0;
                }
            }
        }
        
        /*
         * Sets all the values of an array to 0
         */
        public void zero1D(int[] arr){
            for(int i = 0; i < arr.length; i++){
                arr[i] = 0;
            }
        }

        /*
         * Sets all the values of an array to -1
         */
        public void negative1D(int[] arr){
            for(int i = 0; i < arr.length; i++){
                arr[i] = -1;
            }
        }

        public void print2D(int[][] arr){
            for(int i = 0; i < arr.length; i++){
                for(int j = 0; j < arr[0].length; j++){
                    System.out.print(arr[i][j] + " ");
                }
                System.out.println();
            }
            System.out.println();
        }

        public void print1d(int[] arr){
            for(int i = 0; i < arr.length; i++){
                System.out.print(arr[i] + " ");
            }
            System.out.println();
        }        
    }

    public static void main(String[] args) {
        // STEP 0: Open Files via args
        String inFile = args[0];
        int connectedness = Integer.parseInt(args[1]);
        String prettyFile = args[2];
        String labelFile = args[3];
        String propertyFile = args[4];
        String logFile = args[5];

        int numRow = 0;
        int numCol = 0;
        int min = 0;
        int max = 0;

        try{
            BufferedReader br = new BufferedReader(new FileReader(inFile));
            String inputHeader[] = br.readLine().trim().split("\\s+");
            
            numRow = Integer.parseInt(inputHeader[0]);
            numCol = Integer.parseInt(inputHeader[1]);
            min = Integer.parseInt(inputHeader[2]);
            max = Integer.parseInt(inputHeader[3]);
            System.out.println("HEADER: " + numRow + " " + numCol + " " + min + " " + max);
            br.close();
        }catch(IOException e){
            System.err.println("ERROR :" + e.getMessage());
        }

        // STEP 1 
        ccLabel ccLabel = new FloresL_Project6_Main().new ccLabel(numRow, numCol, min, max);
        System.out.println("ZEROFRM");
        ccLabel.print2D(ccLabel.zeroFramedAry);
        System.out.println("NONZERONEIGHBOR");
        ccLabel.print1d(ccLabel.nonZeroNeighborAry);
        System.out.println("EQTABLE");
        ccLabel.print1d(ccLabel.EQtable);

        // STEP 2 
        ccLabel.loadImg(inFile, ccLabel.zeroFramedAry);
        System.out.println("LOADED ZEROFRM: ");
        ccLabel.print2D(ccLabel.zeroFramedAry);
        
        // STEP 3 & 4

        try (BufferedWriter prettyWriter = new BufferedWriter(new FileWriter(prettyFile));
            BufferedWriter logWriter = new BufferedWriter(new FileWriter(logFile));
            BufferedWriter labelWriter = new BufferedWriter(new FileWriter(labelFile));
            BufferedWriter propertyWriter = new BufferedWriter(new FileWriter(propertyFile));){
        
            if (connectedness == 4) {
                ccLabel.connected4(ccLabel.zeroFramedAry, ccLabel.EQtable, prettyWriter, logWriter);
            }
            if (connectedness == 8) {
                // prettyWriter.write("Input Image: \n");
                // prettyWriter.write(numRow + " " + numCol + " " + min + " " + max + "\n");
                // ccLabel.prettyDotPrint(ccLabel.zeroFramedAry, prettyWriter);
                ccLabel.connected8(ccLabel.zeroFramedAry, ccLabel.EQtable, prettyWriter, logWriter);
            }

            // Step 5
            labelWriter.write(ccLabel.numRows + " " + ccLabel.numCols + " " + ccLabel.newMin + " " + ccLabel.newMax + "\n");
            // Step 6
            ccLabel.prettyDotPrint(ccLabel.zeroFramedAry, labelWriter);
            // Step 7 
            ccLabel.printCCproperty(propertyWriter);
            // Step 8 
            ccLabel.drawBoxes(ccLabel.zeroFramedAry, ccLabel.CCproperty, ccLabel.trueNumCC, logWriter);
            // Step 9
            prettyWriter.write("Bounding Boxes \n");
            ccLabel.prettyDotPrint(ccLabel.zeroFramedAry, prettyWriter);

        }catch(IOException e) {
            System.err.println("ERROR: " + e.getMessage());
        }
    }

}//end-FloresL_Project6_Main


/**
 COMPILE: javac FloresL_Project6_Main.java
 RUN    : java FloresL_Project6_Main.java data1.txt 4 prettyPrintFile.txt labelFile.txt propertyFile.txt logFile.txt
 
 args[0] - input file
 args[1] - connected algorithm; 4-connect or 8-connect
 args[2] - prettyPrintFile
 args[3] - labelFile to store connected components of pass 3
 args[4] - store-connected component properties
 args[5] - logFile
 
 */

