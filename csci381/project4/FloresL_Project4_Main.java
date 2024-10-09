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

        private int[][] zeroFramedAry; // Holds the padded image
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

        


    }//end-class-Morphology

    public static void main(String[] args){
        
        // ### STEP 0: Error Handling for Inputs ###
        if (args.length != 4){
            System.err.print("Usage: java FloresL_Project4_Main <inFile> <structFile> <Process#> <prettyPrintFile>");
        }

        String inFile = args[0];
        String structFile = args[1];

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

        Morphology morphology = new FloresL_Project4_Main().new Morphology(numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin);
        morphology.printInstanceVariables();

        // ### Dynamically Allocate Arrays ###
        


    }//end-main
}//end-public-class-FloresL_Project4_Main