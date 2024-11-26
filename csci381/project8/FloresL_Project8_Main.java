package project8;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOError;
import java.io.IOException;
import java.nio.channels.Pipe.SourceChannel;

public class FloresL_Project8_Main {
    class chainCode{

        static class Point {
            int row;
            int col;   

            // Constructor
            Point(int r, int c){
                this.row = r;
                this.col = c;
            }
        }//end-static-class-Point

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

        int numCC;
        CCproperty cc;
        int numRows;
        int numCols;
        int minVal;
        int maxVal;

        int[][] imgAry;
        int[][] boundaryAry;
        int[][] CCary;
        Point[] coordOffset;
        int[] zeroTable;
        Point startP;
        Point currentP;
        Point nextP;
        
        int lastQ;
        int nextDir;
        int PchainDir;

    }//end-class-chainCode

}

/**
 COMPILE: javac FloresL_Project8_Main.java
 RUN    : java FloresL_Project8_Main.java img1CC.txt img1Property.txt outFile.txt chainCodeFile.txt boundaryFile.txt logFile.txt

 args[0] - image file with header
 args[1] - connectedComponent properties with format 
 args[2] - outfile1
 args[3] - chainCodeFile
 args[4] - boundaryFile
 args[5] - logFile
 */