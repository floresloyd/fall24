package programmingGuide;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class java_main {
//=================================== INSTANTIATING A CLASS ===================================================
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
    }// end-morphology-class


    public static void main(String[] args){
// ================================================== ERROR HANDLING ===========================================
    if (args.length != 4){
        System.err.print("Usage: java FloresL_Project4_Main <inFile> <structFile> <Process#> <prettyPrintFile>");
    }
// ================================================== READING INPUT ===========================================
    // GETTING FILEPATHS AND STORING ARGS
    String inFile = args[0];        // Input Argument from pos 0
    String structFile = args[1];    // Input argument from pos 1
    System.out.println("Step 0: Complete! inFile and StructFile Loaded...");
    
    // READING ONE LINE
    try{    

            // VARS TO READ
            int numImgRows; int numImgCols; int imgMin; int imgMax;
            // INSTANTIATING READERS 
            BufferedReader br_inFile = new BufferedReader(new FileReader(inFile));
            // SPLIT LINE 
            String[] header_inFile = br_inFile.readLine().trim().split("\\s+");
            // READ 
            numImgRows = Integer.parseInt(header_inFile[0]);
            numImgCols = Integer.parseInt(header_inFile[1]);
            imgMin = Integer.parseInt(header_inFile[2]);
            imgMax = Integer.parseInt(header_inFile[3]);
    } catch (IOException e){
        System.err.println("ERRORL " + e.getMessage());
    }
    
    // CONTINOUOS READING
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

    // ================================================== WRITING OUTPUT ===========================================
    // WRITING ONE LINE 
    BufferedWriter prettyPrintWriter = new BufferedWriter(new FileWriter(prettyPrintFile, append));
    prettyPrintWriter.write("Original Image: \n");

    // CONTINOUS PRINTING 
    public void binaryImagePrettyPrint(int[][] inAry, BufferedWriter fileOut){
        try {
            fileOut.write(numImgRows + " " + numImgCols + " " + imgMin + " " + imgMax);
            fileOut.newLine();
            
            // Iterate over the actual image dimensions, starting from the rowOrigin and colOrigin
            for (int i = rowOrigin; i < rowOrigin + numImgRows; i++) {
                for (int j = colOrigin; j < colOrigin + numImgCols; j++) {
                    if (inAry[i][j] == 0) {
                        fileOut.write(". ");
                    } else {
                        fileOut.write("1 ");
                    }
                }
                fileOut.newLine();  // New line after each row
            }
            fileOut.flush();
        } catch (IOException e) {
            System.err.println("Error in binaryImagePrettyPrint: " + e.getMessage());
        }
    }//end-binaryPrettyPrint
    
    } // end main class
}//end main class
