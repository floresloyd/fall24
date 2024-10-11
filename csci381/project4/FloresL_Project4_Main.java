import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FloresL_Project4_Main {

    class Morphology{
        // Instance variables
        private int numImgRows;
        private int numImgCols;
        private int imgMin;
        private int imgMax;
        private int numStructRows;
        private int numStructCols;
        private int structMin;
        private int structMax;
        private int rowOrigin;
        private int colOrigin;

        private int rowFrameSize;   // Half-size of structure element; It helps handle 
        private int colFrameSize;   // how much space the image should be padded when applying operations at the edges
        private int extraRows;      // Use to mirrorFrame/Pad the image
        private int extraCols;
        private int rowSize;        // Used to dynamically allocate the padded arrays
        private int colSize;

        private int[][] zeroFramedAry; // Holds the padded image to perform morphological operations. We zero frame so we preserve object edges
        private int[][] morphAry;      // Store the results of Morphology
        private int[][] tempAry;       // Buffer
        private int[][] structAry;     // Holds structuring element 

        // Constructor 
        public Morphology(
            int numImgRows, int numImgCols, int imgMin, int imgMax,
            int numStructRows, int numStructCols, int structMin, int structMax,
            int rowOrigin, int colOrigin
        ){
            this.numImgRows = numImgRows;
            this.numImgCols = numImgCols;
            this.imgMin = imgMin;
            this.imgMax = imgMax;
            this.numStructRows = numStructRows;
            this.numStructCols = numStructCols;
            this.structMin = structMin;
            this.structMax = structMax;
            this.rowOrigin = rowOrigin;
            this.colOrigin = colOrigin;

            this.rowFrameSize = numStructRows / 2;
            this.colFrameSize = numStructCols/ 2;
            this.extraRows = rowFrameSize * 2; 
            this.extraCols = colFrameSize * 2; 
            this.rowSize = numImgRows + extraRows;
            this.colSize = numImgCols + extraCols;

            this.zeroFramedAry = new int[rowSize][colSize];
            this.morphAry = new int[rowSize][colSize];
            this.tempAry = new int[rowSize][colSize];
            this.structAry = new int[numStructRows][numStructCols];
        }//end-Constructor-Morphology


        /**
         * @zero2DAry Set entire 2D Array to zero
         * @param ary 2D array to be zeroed out
         */
        public void zero2DAry(int[][] ary, int nRows, int nCols){
            for (int i = 0; i < nRows; i++){
                for (int j = 0; j < nCols; j++){
                    ary[i][j] = 0;
                }
            }
        }//end-zero2DAry

        
        /**
         * @loadImg Load imgFile to zeroFramedAry inside of frame, begins at (rowOrigin, colOrigin) which serves as padding
         * @param inFile Path to input image file
         * @param zeroFramedAry 2D Array that will hold the image
         */
        public void loadImg(String inFile, int[][] zeroFramedAry){
            try {
                // Instantiate a Buffered Reader for the file
                BufferedReader br = new BufferedReader(new FileReader(inFile));
                br.readLine();     // Skip the header 
                    
                // Load the image into zeroFramedAry starting at (rowOrigin, colOrigin)
                for (int i = 0; i < numImgRows; i++) {

                    String[] imageRow = br.readLine().trim().split("\\s+"); // Take each row 
                    
                    for (int j = 0; j < numImgCols; j++) {
                        // Place image data into zeroFramedAry, starting from rowOrigin and colOrigin
                        zeroFramedAry[rowOrigin + i][colOrigin + j] = Integer.parseInt(imageRow[j]); 
                    }
                }
                // Close the BufferedReader after use
                br.close();
            }catch (IOException e) {
                System.err.println("Error: " + e.getMessage());
            }
        }//end-loadImg


        /**
         * @loadStruct load structFile into sturctAry
         * @param structFile File containing the structuring Object
         * @param structAry Array that will hold structuring Object
         */

        public void loadStruct(String structFile, int[][] structAry){
            try {
                BufferedReader br = new BufferedReader(new FileReader(structFile));
                br.readLine();  // Skip header
                br.readLine();  // Skip 2nd header

                for (int i = 0; i < numStructRows; i++){
                    String structRow[] = br.readLine().trim().split("\\s+");
                    for (int j = 0; j < numStructCols; j++){
                        structAry[i][j] = Integer.parseInt(structRow[j]);
                    }
                }
                br.close();
            } catch(IOException e){
                System.err.println("Error: " + e.getMessage());
            }
        }//end-loadStruct


        /**
         * @computeDilation Performs Dilation input ary and stores it in output ary. Does it one pixel at a time.
         * @param zeroFramedAry contains the image with zero frame. We zero frame to not mess up image border
         * @param morphAry will hold the output of morphology
         */
        public void computeDilation(int[][] zeroFramedAry, int[][] morphAry){
            for (int i = rowOrigin; i < rowOrigin + numImgRows; i++) {
                for (int j = colOrigin; j < colOrigin + numImgCols; j++) {
                    if (zeroFramedAry[i][j] > 0){
                        onePixelDilation(i, j, zeroFramedAry, morphAry);
                    }
                }
            }
        }//end-computeDilation


        /**
         * @computeErosion Performs Erosion on input ary and stores it in outputAry
         * @param zeroFramedAry
         * @param morphAry
         */
        public void computeErosion(int[][] zeroFramedAry, int[][] morphAry){
            for (int i = rowOrigin; i < rowOrigin + numImgRows; i++){
                for (int j = colOrigin; j < colOrigin + numImgCols; j++){
                    onePixelErosion(i, j, zeroFramedAry, morphAry);
                }
            }
        }//end-computeErosion

        /**
         * @onePixelDilation applies dilation operation to a signle pixel in the image. 
         * @param i current row index
         * @param j current col index
         * @param zeroFramedAry input array
         * @param morphAry output array
         */
        public void onePixelDilation(int i, int j, int[][] zeroFramedAry, int[][] morphAry){
            // Offset help us position our structuring element correctly aroud P[i,j]
            int iOffset = i - rowOrigin;
            int jOffset = j - colOrigin;

            for (int rIndex = 0; rIndex < numStructRows; rIndex++){
                for (int cIndex = 0; cIndex < numStructCols; cIndex++){
                    if (structAry[rIndex][cIndex] > 0) {
                        // Dilate
                        morphAry[iOffset + rIndex][jOffset + cIndex] = 1;
                    }
                }
            }
        }//end-onePixelDilation


        /**
         * @onePixelErosion Applies erosion operation to a signle pixel in the image
         * @param i
         * @param j
         * @param zeroFramedAry
         * @param morphAry
         */
        public void onePixelErosion(int i, int j, int[][] zeroFramedAry, int[][] morphAry){
            int iOffset = i - rowOrigin; // start position
            int jOffset = j - colOrigin;    
            boolean matchFlag = true;   // Assume that current pixel matches structuring object 

            for (int rIndex = 0; rIndex < numStructRows; rIndex++){
                for (int cIndex = 0; cIndex < numStructCols; cIndex++){
                    // Erode
                    // Condition 1: Checks if current element of structuring element is part of the shape 
                    // Condition 2: Checks if corresponding pixel in the input image  P[i][j] <= 0, it means structuring element does not match the image pixel
                    if ( (structAry[rIndex][cIndex] > 0) && (zeroFramedAry[iOffset + rIndex][jOffset + cIndex]) <= 0){
                        matchFlag = false;
                        cIndex++; // move forward if no match, or if P[i][j] <= 0; it means it is noise since it does not have a pixel value
                    }
                }
            }

            if (matchFlag) {
                morphAry[i][j] = 1; // if Matchflag we keep this pixel
            }else{
                morphAry[i][j] = 0; // If not we turn it to 0
            }
        }

        /**
         * @aryToFile Output pixels inside of frame of inAry to fileOut
         * @param inAry
         * @param fileOut
         */
        public void aryToFile(int[][] inAry, BufferedWriter fileOut){
            try{
                // Write header
                fileOut.write(numImgRows + " " + numImgCols + " " + imgMin + " " + imgMax);
                fileOut.newLine();

                for (int i = rowOrigin; i < rowOrigin + numImgRows; i++){
                    for (int j = colOrigin; j < colOrigin + numImgCols; j++){
                        fileOut.write(inAry[i][j] + " ");
                    }
                    // New line after each row
                    fileOut.newLine();
                }
                // Close writer
                fileOut.flush();

            }catch(IOException e){
                System.err.println("Error in writing array to file" + e);
            }
        }//end-aryToFile


        /**
         * @binaryPrettyPrint Prints out the input array in a formatted manner 
         * @param inAry
         * @param fileOut
         */
        public void binaryPrettyPrint(int[][] inAry, BufferedWriter fileOut){
            try{
                for (int i = 0; i < rowSize; i++){
                    for (int j = 0; j < colSize; j++){
                        if (inAry[i][j] == 0){
                            fileOut.write(". ");
                        }else{
                            fileOut.write("1 ");
                        }
                    }
                    fileOut.newLine();
                }
            }catch(IOException e){
                System.err.println("Error in binaryPrettyPrint" + e.getMessage());
            }
             
        }//end-binaryPrettyPrint




        /**
         * @process1 Performs Dilation to the image and stores it into the morph array
         * @param prettyPrintFile file that will hold the human-readable output
         */
        public void process1(String prettyPrintFile){
            try{
                // Step #1 Declare output file and BufferedWriter and output file for pretty print
                String outPutFile = "dilationOutFile.txt";
                BufferedWriter bw = new BufferedWriter(new FileWriter(outPutFile));    
                BufferedWriter prettyPrintWriter = new BufferedWriter(new FileWriter(prettyPrintFile));

                // Step #2 Zero out morphAry
                zero2DAry(morphAry, rowSize, colSize);

                // Perform Dilation
                computeDilation(zeroFramedAry, morphAry);
                
                // Print to outFile
                aryToFile(morphAry, bw);

                // Pretty Print
                binaryPrettyPrint(morphAry, prettyPrintWriter);

                // Step #3 Close files
                bw.close();
                prettyPrintWriter.close();

            }catch(IOException e){
                System.err.println("Error in process 1" + e.getMessage());
            }
        }//end-process1


        public void process2(String prettyPrintFile){
            try {
                // # Step 1
                String outPutFile = "erosionOutFile.txt";
                BufferedWriter bw = new BufferedWriter(new FileWriter(outPutFile));
                BufferedWriter prettyBufferedWriter = new BufferedWriter(new FileWriter(prettyPrintFile));

                // Step 2 
                
                // Clear out morph array to save new output
                zero2DAry(morphAry, rowSize, colSize);
                
                // Perform Erosion
                computeErosion(zeroFramedAry, morphAry);
                
                // Print to outFile
                aryToFile(morphAry, bw);

                // Pretty print
                binaryPrettyPrint(morphAry, prettyBufferedWriter);

                // Step #3 Close Files
                bw.close();
                prettyBufferedWriter.close();

            }catch (IOException e){
                System.err.println("Error in Process 2: " + e.getMessage());
            }
        }//end-process2

        public void printInstanceVariables() {
            System.out.println("Image File Variables:");
            System.out.println("numImgRows: " + numImgRows);
            System.out.println("numImgCols: " + numImgCols);
            System.out.println("imgMin: " + imgMin);
            System.out.println("imgMax: " + imgMax);
            System.out.println("Structuring Element Variables:");
            System.out.println("numStructRows: " + numStructRows);
            System.out.println("numStructCols: " + numStructCols);
            System.out.println("structMin: " + structMin);
            System.out.println("structMax: " + structMax);
            System.out.println("rowOrigin: " + rowOrigin);
            System.out.println("colOrigin: " + colOrigin);
            System.out.println("Array Sizes:");
            System.out.println("zeroFramedAry size: " + zeroFramedAry.length + "x" + zeroFramedAry[0].length);
            System.out.println("structAry size: " + structAry.length + "x" + structAry[0].length);
        }


        public void print2Dary(int[][] ary, int r, int c) {
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    // Print each element in the row, formatted nicely with spaces
                    System.out.print(String.format("%1d", ary[i][j]) + " ");
                }
                // Move to the next line after each row
                System.out.println();
            }
        }
    
        


    }//end-class-Morphology

    public static void main(String[] args){
        
        // ### STEP 0: Error Handling for Inputs ###
        if (args.length != 4){
            System.err.print("Usage: java FloresL_Project4_Main <inFile> <structFile> <Process#> <prettyPrintFile>");
        }

        String inFile = args[0];
        String structFile = args[1];
        System.out.println("Step 0: Complete! inFile and StructFile Loaded...");

        // ### STEP 1: Read inFile & structFile ###
        int numImgRows = 0;
        int numImgCols = 0;
        int imgMin = 0;
        int imgMax = 0;
        int numStructRows = 0;
        int numStructCols = 0;
        int structMin = 0;
        int structMax = 0;
        int rowOrigin = 0;
        int colOrigin = 0;
        
        try {
            // Reading inFile (image file)
            BufferedReader br_inFile = new BufferedReader(new FileReader(inFile));
            String[] header_inFile = br_inFile.readLine().trim().split("\\s+");
            
            // Assigning values for inFile
            numImgRows = Integer.parseInt(header_inFile[0]);
            numImgCols = Integer.parseInt(header_inFile[1]);
            imgMin = Integer.parseInt(header_inFile[2]);
            imgMax = Integer.parseInt(header_inFile[3]);

            // Reading structFile (structuring element file); 1st Line -> numStructRows, numStructCols, structMin, structMax
            BufferedReader br_structFile = new BufferedReader(new FileReader(structFile));
            String[] header_structFile = br_structFile.readLine().trim().split("\\s+");
            
            // Assigning values for structFile
            numStructRows = Integer.parseInt(header_structFile[0]);
            numStructCols = Integer.parseInt(header_structFile[1]);
            structMin = Integer.parseInt(header_structFile[2]);
            structMax = Integer.parseInt(header_structFile[3]);

            // Reading structFile (structuring element file); 2nd Line -> rowOrigin, colOrigin
            String[] origin_structFile = br_structFile.readLine().trim().split("\\s+");
            rowOrigin = Integer.parseInt(origin_structFile[0]);
            colOrigin = Integer.parseInt(origin_structFile[1]);
            

        } catch (IOException e){
            System.err.println("Error: " + e.getMessage());
        }
        System.out.println("Step 1: Complete! inFile and StructFile headers read...");

        // Instantiate Morphology object
        Morphology morphology = new FloresL_Project4_Main().new Morphology(numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin);
        System.out.println("Morphology Object: Instantiated");

        // ### STEP 3: Zero out 2D Arrays ###
        morphology.zero2DAry(morphology.zeroFramedAry, morphology.rowSize, morphology.colSize);
        morphology.zero2DAry(morphology.morphAry, morphology.rowSize, morphology.colSize);
        morphology.zero2DAry(morphology.tempAry, morphology.rowSize, morphology.colSize);
        morphology.zero2DAry(morphology.structAry, morphology.numStructRows, morphology.numStructCols);        
        System.out.println("Step 2 & 3: Complete! 2D Arrays Zeroed Out...");

        // ### STEP 4 and 5: loadImage and loadStruct 
        morphology.loadImg(inFile, morphology.zeroFramedAry);
        System.out.println("Step 4: Complete! Image Loaded...");
        morphology.loadStruct(structFile, morphology.structAry);
        System.out.println("Step 5: Complete! Structure Element Loaded...");

        // ### STEP 6: Obtain User's choice of Process
        int userChoice = Integer.parseInt(args[2]);
        System.out.println("Step 6: Complete! User's Choice of Process: " + userChoice);

        // ### STEP 7: Running User's Choice of Process
        if (userChoice == 1){
            morphology.process1(args[3]);
            System.out.println("Process 1: Dilation Execution Complete!");
        }
        else if (userChoice == 2) {
            morphology.process2(args[3]);
            System.out.println("Process 2: Erosion Execution Complete");
        }
        else if (userChoice == 3) {
            System.out.println("Running Process 3");
        }
        else if (userChoice == 4) {
            System.out.println("Running Process 4");
        }
        else if (userChoice == 5) {
            System.out.println("Running Process 5");
        }
        else{
            throw new IllegalArgumentException("Invalid Process! The only available options are: 1, 2, 3, 4, 5"); 
        }
        
        System.err.println("END EXEC");


    }//end-main
}//end-public-class-FloresL_Project4_Main