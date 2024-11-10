import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOError;
import java.io.IOException;
import java.nio.channels.Pipe.SourceChannel;

import javax.print.DocFlavor.STRING;

public class FloresL_Project7_Main {
    class Thinning {
        int numRows;
        int numCols;
        int minVal;
        int maxVal;
        int changeCount;    // Number of object pixels we changed to 0. 
        int cycleCount;     // 
        int[][] aryOne;     // arr[numRows + 2][numCols + 2] = 0; used to check those three conditions of thinning 
        int[][] aryTwo;     // arr[numRows + 2][numCols + 2] = 0; storing intermediate results after each side of thinning


        public Thinning(int rows, int cols, int min, int max){
            this.numRows = rows;
            this.numCols = cols;
            this.minVal = min;
            this.maxVal = max; 
            this.changeCount = 0;
            this.cycleCount = 0; 

            // Arrays
            this.aryOne = new int[numRows + 2][numCols + 2];
            this.aryTwo = new int[numRows + 2][numCols + 2];

            // Instantiate to Zero
            for (int i = 0; i < aryOne.length; i++){
                for (int j = 0; j < aryOne[0].length; j++){
                    aryOne[i][j] = 0;
                    aryTwo[i][j] = 0;
                }
            }
        }//end-public-Thinning

        public void thinning(int[][] aryOne, int[][] aryTwo, BufferedWriter logWriter){
            try{
                // Step 0
                logWriter.write("Entering thinning() before thinning 4 sides, aryOne is below: \n ");
                prettyDotPrint(aryOne, logWriter);
                
                // Step 1 
                northThinning(aryOne, aryTwo, logWriter);
                logWriter.write("After northThinning; aryTwo is below: \n");
                prettyDotPrint(aryTwo, logWriter);
                copyArys(aryOne, aryTwo);

                // Step 2
                southThinning(aryOne, aryTwo, logWriter);
                logWriter.write("After southThinning; aryTwo is below: \n");
                prettyDotPrint(aryTwo, logWriter);
                copyArys(aryOne, aryTwo);

                // Step 3
                westThinning(aryOne, aryTwo, logWriter);
                logWriter.write("After westThinning; aryTwo is below: \n");
                prettyDotPrint(aryTwo, logWriter);
                copyArys(aryOne, aryTwo);

                // Step 4
                eastThinning(aryOne, aryTwo, logWriter);
                logWriter.write("After eastThinning; aryTwo is below: \n");
                prettyDotPrint(aryTwo, logWriter);
                copyArys(aryOne, aryTwo);

                // Step 5 
                logWriter.write("Leaving thinning() before thinning 4 sides, aryOne is below: \n");
                prettyDotPrint(aryTwo, logWriter);
            
            }catch(IOException e){
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }//end-void-thinning

        public void northThinning(int[][] aryOne, int[][] aryTwo, BufferedWriter logWriter){
            try{
                logWriter.write("Entering northThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
                int nonZeroCount = 0;
                boolean flag;
                for (int i = 1; i < numRows + 1; i++){
                    for (int j = 1; j < numCols + 1; j++){

                        // Check p[i][j] is an object pixel and its north neighbor is zero
                        if (aryOne[i][j] > 0 && aryOne[i - 1][j] == 0){
                            nonZeroCount = countNonZeroNeighbors(aryOne, i, j);
                            flag = checkConnector(aryOne, i, j);

                            if (nonZeroCount >= 4 && flag == false){
                                aryTwo[i][j] = 0;
                                changeCount ++;
                            }
                            else {
                                aryTwo[i][j] = aryOne[i][j];
                            }

                        }
                    }
                }
                logWriter.write("Leaving northThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
            }catch(IOException e){
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }

        public void southThinning(int[][] aryOne, int[][] aryTwo, BufferedWriter logWriter){
            try{
                logWriter.write("Entering southThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
                int nonZeroCount = 0;
                boolean flag;
                for (int i = 1; i < numRows + 1; i++){
                    for (int j = 1; j < numCols + 1; j++){

                        // Check p[i][j] is an object pixel and its south neighbor is zero
                        if (aryOne[i][j] > 0 && aryOne[i + 1][j] == 0){
                            nonZeroCount = countNonZeroNeighbors(aryOne, i, j);
                            flag = checkConnector(aryOne, i, j);

                            if (nonZeroCount >= 4 && flag == false){
                                aryTwo[i][j] = 0;
                                changeCount ++;
                            }
                            else {
                                aryTwo[i][j] = aryOne[i][j];
                            }

                        }
                    }
                }
                logWriter.write("Leaving southThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
            }catch(IOException e){
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }

        public void westThinning(int[][] aryOne, int[][] aryTwo, BufferedWriter logWriter){
            try{
                logWriter.write("Entering westThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
                int nonZeroCount = 0;
                boolean flag;
                for (int i = 1; i < numRows + 1; i++){
                    for (int j = 1; j < numCols + 1; j++){

                        // Check p[i][j] is an object pixel and its west neighbor is zero
                        if (aryOne[i][j] > 0 && aryOne[i][j - 1] == 0){
                            nonZeroCount = countNonZeroNeighbors(aryOne, i, j);
                            flag = checkConnector(aryOne, i, j);

                            if (nonZeroCount >= 4 && flag == false){
                                aryTwo[i][j] = 0;
                                changeCount ++;
                            }
                            else {
                                aryTwo[i][j] = aryOne[i][j];
                            }

                        }
                    }
                }
                logWriter.write("Leaving westThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
            }catch(IOException e){
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }

        public void eastThinning(int[][] aryOne, int[][] aryTwo, BufferedWriter logWriter){
            try{
                logWriter.write("Entering eastThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
                int nonZeroCount = 0;
                boolean flag;
                for (int i = 1; i < numRows + 1; i++){
                    for (int j = 1; j < numCols + 1; j++){

                        // Check p[i][j] is an object pixel and its west neighbor is zero
                        if (aryOne[i][j] > 0 && aryOne[i][j + 1] == 0){
                            nonZeroCount = countNonZeroNeighbors(aryOne, i, j);
                            flag = checkConnector(aryOne, i, j);

                            if (nonZeroCount >= 4 && flag == false){
                                aryTwo[i][j] = 0;
                                changeCount ++;
                            }
                            else {
                                aryTwo[i][j] = aryOne[i][j];
                            }

                        }
                    }
                }
                logWriter.write("Leaving eastThinning (); cycleCount = " + cycleCount + "; changeCount = " + changeCount + "\n");
            }catch(IOException e){
                System.err.println("Error in prettyDotPrint: " + e.getMessage());
            }
        }

        // Counts the number of non-zero values within a 3x3 neighborhood, excluding aryOne[i][j]
        public int countNonZeroNeighbors(int[][] aryOne, int i, int j) {
            int nonZeroCount = 0;
            for (int x = i - 1; x <= i + 1; x++) {
                for (int y = j - 1; y <= j + 1; y++) {
                    // Count non-zero neighbors, excluding the center pixel (i, j)
                    if (aryOne[x][y] > 0 && (x != i || y != j)) {
                        nonZeroCount++;
                    }
                }
            }
            return nonZeroCount;
        }

        // Determine if a given 3x3 neighborhood around a pixel matches on of the six predefined patterns
        // match = true; match with at least one of the six congifurations; Current pixel is a connector pixel and cannot be removed 
        // match = false; no match; this pixel is not critical and can be removed 
        public boolean checkConnector(int[][] aryOne, int i, int j) {
            // Define the six configurations as 3x3 matrices
            int[][][] configurations = {
                {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}, // Configuration 1
                {{0, 0, 0}, {1, 1, 1}, {0, 1, 0}}, // Configuration 2
                {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}}, // Configuration 3
                {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}}, // Configuration 4
                {{0, 1, 0}, {0, 1, 1}, {0, 0, 1}}, // Configuration 5
                {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}  // Configuration 6
            };
        
            // Check if any configuration matches the 3x3 neighborhood around (i, j)
            for (int k = 0; k < 6; k++) { // Iterate over each configuration
                boolean match = true;
                for (int x = 0; x < 3; x++) { // Iterate over rows of the 3x3 neighborhood
                    for (int y = 0; y < 3; y++) { // Iterate over columns of the 3x3 neighborhood
                        // Compute the actual coordinates in aryOne based on (i, j) as the center
                        int row = i + x - 1;
                        int col = j + y - 1;
        
                        // Skip checking positions marked as 'x' (can be represented by -1 if necessary)
                        if (configurations[k][x][y] != -1 && configurations[k][x][y] != aryOne[row][col]) {
                            match = false;
                            break;
                        }
                    }
                    if (!match) break;
                }
                if (match) return true; // Return true if any configuration matches
            }
            return false; // No configuration matched
        }


        // Copies all values from sourceArr to destArr
        public void copyArys(int[][] destArr, int[][] sourceArr) {
            for (int i = 0; i < destArr.length; i++) {
                for (int j = 0; j < destArr[0].length; j++) {
                    destArr[i][j] = sourceArr[i][j]; // Copy each element from sourceArr to destArr
                }
            }
        }

        
        public void loadImg(String inFile, int[][] zeroFramedAry, BufferedWriter outWriter) {
            try {
                // STEP 0
                // Instantiate a Buffered Reader for the file
                BufferedReader br = new BufferedReader(new FileReader(inFile));
                String[] header = br.readLine().trim().split("\\s+");     // Read header
                outWriter.write("Input Image Header \n");
                outWriter.write(header[0] + " " + header[1] + " " + header[2] + " " + header[3] + "\n");
        
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
    }//end-class-Thinning

    public static void main(String[] args) {
        System.out.println("Code Running ....");
        // Instantiate Variables
        String inFile = args[0];
        String outFile = args[1];
        String logFile = args[2];
        int numRow = 0;
        int numCol = 0;
        int minVal = 0; 
        int maxVal = 0;

        
        try{
            long startTime = System.nanoTime();
            // Read Input
            BufferedReader br = new BufferedReader(new FileReader(inFile));
            String inputHeader[] = br.readLine().trim().split("\\s+");
            
            numRow = Integer.parseInt(inputHeader[0]);
            numCol = Integer.parseInt(inputHeader[1]);
            minVal = Integer.parseInt(inputHeader[2]);
            maxVal = Integer.parseInt(inputHeader[3]);

            // Instantiate thinning class and output files 
            Thinning thinning = new FloresL_Project7_Main().new Thinning(numRow, numCol, minVal, maxVal);
            BufferedWriter outWriter = new BufferedWriter(new FileWriter(outFile));
            BufferedWriter logWriter = new BufferedWriter(new FileWriter(logFile));
            
            // Step 1
            thinning.loadImg(inFile, thinning.aryOne, outWriter);
            // Step 2 
            outWriter.write("In main(), before thinning, changeCount = " + thinning.changeCount + "; cycleCount = " + thinning.cycleCount + "\n");
            thinning.prettyDotPrint(thinning.aryOne, outWriter);
            
            // Step 3 - 6: Apply iterative thinning until there are no more changes 
            while (true) {
                thinning.changeCount = 0;   // prevent foreverloop
                // Step 3: Apply thinning
                thinning.thinning(thinning.aryOne, thinning.aryTwo, logWriter);
    
                // Step 4: Increment cycle count
                thinning.cycleCount++;
    
                // Step 5: Print iteration progress to outFile
                outWriter.write("In main(), inside iteration; changeCount = " + thinning.changeCount + "; cycleCount = " + thinning.cycleCount + "\n");
                thinning.prettyDotPrint(thinning.aryOne, outWriter);
    
                // Step 6: Check if further iterations are needed
                if (thinning.changeCount <= 0) {
                    // changeCount > 0: There are still pixels that can be thinned meaning iterations are still necessary to continue to reducing the structure
                    // changeCount <= 0 indicates that no more pixels can be thinned 
                    break;
                }
            }

            // Step 7: 
            outWriter.write("In main(), the final skeleton, changeCount = " + thinning.changeCount + "; cycleCount = " + thinning.cycleCount + "\n");
            thinning.prettyDotPrint(thinning.aryOne, outWriter); 
            
            // Step 8 
            long endTime = System.nanoTime();
            br.close();
            outWriter.flush();
            logWriter.flush();
            outWriter.close();
            logWriter.close();
            long duration = (endTime - startTime);  // duration in nanoseconds
            double milliseconds = duration / 1e6;   // convert to milliseconds
            System.out.println("Execution time: " + duration + " nanoseconds");
            System.out.println("Execution time: " + milliseconds + " milliseconds");
            System.out.println("Execution Complete!");
        }catch(IOException e){
            System.err.println("ERROR :" + e.getMessage());
        }

    }//end-main
}//end-FloresL_Project7_Main


/**
 COMPILE: javac FloresL_Project7_Main.java
 RUN    : java FloresL_Project7_Main.java data1.txt outFile.txt logFile.txt

 args[0] - input image
 args[1] - output file
 args[2] - logfile 
 */