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
        int trueNumCC;  // True value of the connected component
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

            this.CCproperty = new Property[trueNumCC + 1];
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
        
        public void connected8(int[][] zeroFramedAry, int[] EQtable, BufferedWriter prettyPrintFile, BufferedWriter logFile){
            try {
                // Step 0
                logFile.write("Entering connected8 method \n");
                // Step 1
                connected8Pass1(zeroFramedAry, EQtable);
                prettyPrintFile.write("After connected8 pass1, newLabel = " + newLabel + "\n");
                logFile.write("After connected8 pass1, newLabel = " + newLabel + "\n");
                prettyDotPrint(zeroFramedAry, prettyPrintFile);
                prettyPrintFile.write("Equivalency Table after: Pass 1 (indexing starts from 1) \n");
                printEQTable(newLabel, prettyPrintFile);

                
                prettyPrintFile.flush();
                logFile.flush();
            } catch(IOException e) {
                System.err.println("ERROR :" + e.getMessage());
            }
        }

        // public void connected8Pass1(int[][] zeroFramedAry, int newLabel, int[] EQtable) {
        //     // Scan L - R
        //     for (int i = 1; i < zeroFramedAry.length - 1; i++) {
        //         // Scan T - B
        //         for (int j = 1; j < zeroFramedAry[0].length - 1; j++) {
        //             if (zeroFramedAry[i][j] > 0) {
        //                 // a b c
        //                 // d x
        //                 int a = zeroFramedAry[i - 1][j - 1];
        //                 int b = zeroFramedAry[i - 1][j];
        //                 int c = zeroFramedAry[i - 1][j + 1];
        //                 int d = zeroFramedAry[i][j - 1];
        
        //                 // Case 1: All neighbors are zero
        //                 if (a == 0 && b == 0 && c == 0 && d == 0) {
        //                     newLabel++;
        //                     zeroFramedAry[i][j] = newLabel;
        //                 }
        //                 // Case 2: Some neighbors have the same non-zero label
        //                 // P(i, j) is part of a connected component
        //                 else if ((a > 0 || b > 0 || c > 0 || d > 0) && 
        //                          (a == b || a == c || a == d || b == c || b == d || c == d)) {
        //                     int label = Math.max(Math.max(a, b), Math.max(c, d)); // Take any non-zero label
        //                     zeroFramedAry[i][j] = label;
        //                 }
        //                 // Case 3: At least two neighbors have different labels
        //                 // This means that P(i, j) is boundaried by two or more other connected components
        //                 else {
        //                     int[] neighbors = {a, b, c, d};
        //                     int minLabel = Integer.MAX_VALUE;
        
        //                     // Find the minimum non-zero label
   
        //                     for (int neighbor : neighbors) {
        //                         if (neighbor > 0 && neighbor < minLabel) {
        //                             minLabel = neighbor;
        //                         }
        //                     }
        
        //                     // We want the minimum label to ensure consistency and we want to use the minimum 
        //                     // amount of labels.
        //                     zeroFramedAry[i][j] = minLabel;
        
        //                     // Update the EQ table for non-min labels
        //                     // We want to make sure that the labels that are not the same but are connected
        //                     // are kept track of so later on we can update the them.
        //                     for (int neighbor : neighbors) {
        //                         if (neighbor > 0 && neighbor != minLabel) {
        //                             EQtable[neighbor] = minLabel;
        //                         }
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }

        public void connected8Pass1(int[][] zeroFramedAry, int[] EQTable) {
            for (int i = 1; i < zeroFramedAry.length - 1; i++) {
                for (int j = 1; j < zeroFramedAry[0].length - 1; j++) {
                    if (zeroFramedAry[i][j] > 0) {
                        int a = zeroFramedAry[i - 1][j - 1];
                        int b = zeroFramedAry[i - 1][j];
                        int c = zeroFramedAry[i - 1][j + 1];
                        int d = zeroFramedAry[i][j - 1];
        
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
                            prettyFile.write(". ");
                        } else {
                            prettyFile.write(inAry[i][j] + " ");
                        }
                    }
                    prettyFile.newLine();
                }
                prettyFile.newLine();
                prettyFile.flush();
            } catch(IOException e) {
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }
        
        public void printEQTable(int newLabel, BufferedWriter prettyFile) {
            try {
                for (int i = 0; i <= newLabel; i++) {
                    prettyFile.write(i + " ");
                }
                prettyFile.write("\n");
        
                for (int i = 0; i <= newLabel; i++) {
                    prettyFile.write(EQtable[i] + " ");
                }
                prettyFile.write("\n");
                prettyFile.flush();
            } catch(IOException e) {
                System.err.println("Error in printEQTable: " + e.getMessage());
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
        
        // STEP 3 & 

    try (
        BufferedWriter prettyWriter = new BufferedWriter(new FileWriter(prettyFile));
        BufferedWriter logWriter = new BufferedWriter(new FileWriter(logFile))) {
    
        if (connectedness == 4) {
                System.out.println("4 Connect");
        }
        if (connectedness == 8) {
            prettyWriter.write(numRow + " " + numCol + " " + min + " " + max + "\n");
            ccLabel.prettyDotPrint(ccLabel.zeroFramedAry, prettyWriter);
            ccLabel.connected8(ccLabel.zeroFramedAry, ccLabel.EQtable, prettyWriter, logWriter);
        }
    // Handle other connectedness cases as needed

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

