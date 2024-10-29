import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

class Enhancement {
    public static final int[][][] mask = {
            {
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0},
                    {0, 0, 1, 0, 0},
                    {0, 1, 1, 1, 0},
                    {1, 1, 1, 1, 1}
            },
            {
                    {1, 0, 0, 0, 0},
                    {1, 1, 0, 0, 0},
                    {1, 1, 1, 0, 0},
                    {1, 1, 0, 0, 0},
                    {1, 0, 0, 0, 0}
            },
            {
                    {1, 1, 1, 1, 1},
                    {0, 1, 1, 1, 0},
                    {0, 0, 1, 0, 0},
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0}
            },
            {
                    {0, 0, 0, 0, 1},
                    {0, 0, 0, 1, 1},
                    {0, 0, 1, 1, 1},
                    {0, 0, 0, 1, 1},
                    {0, 0, 0, 0, 1}
            },
            {
                    {1, 1, 1, 0, 0},
                    {1, 1, 1, 0, 0},
                    {1, 1, 1, 0, 0},
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0}
            },
            {
                    {0, 0, 1, 1, 1},
                    {0, 0, 1, 1, 1},
                    {0, 0, 1, 1, 1},
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0}
            },
            {
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0},
                    {0, 0, 1, 1, 1},
                    {0, 0, 1, 1, 1},
                    {0, 0, 1, 1, 1}
            },
            {
                    {0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0},
                    {1, 1, 1, 0, 0},
                    {1, 1, 1, 0, 0},
                    {1, 1, 1, 0, 0}
            }
    };
    private int numRows;
    private int numCols;
    private int minVal;
    private int maxVal;
    private int[][] mirrorFramedAry;
    private int[][] avgAry;
    private int[][] CPavgAry;
    private int[] Avg_histAry;
    private int[] CPavg_histAry;

    public int[][] getMirrorFramedAry() {
        return mirrorFramedAry;
    }

    public int[][] getAvgAry() {
        return avgAry;
    }

    public int[][] getCPavgAry() {
        return CPavgAry;
    }

    public int[] getAvg_histAry() {
        return Avg_histAry;
    }

    public int[] getCPavg_histAry() {
        return CPavg_histAry;
    }

    public void loadImage(Scanner inFile) {
        // start with [2][2]
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                mirrorFramedAry[i + 2][j + 2] = inFile.nextInt();
            }
        }
    }

    public void mirrorFraming() {
        // copy row
        for (int j = 0; j < mirrorFramedAry[2].length; j++) {
            mirrorFramedAry[0][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[1][j] = mirrorFramedAry[2][j];
            //and copy numRows +1 of mirrorFramedAry to both numRows+2 and numRows+3
            mirrorFramedAry[numRows + 2][j] = mirrorFramedAry[numRows + 1][j];
            mirrorFramedAry[numRows + 3][j] = mirrorFramedAry[numRows + 1][j];
        }
        // copy column
        for (int i = 0; i < mirrorFramedAry.length; i++) {
            mirrorFramedAry[i][0] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][1] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][numCols + 2] = mirrorFramedAry[i][numCols + 1];
            mirrorFramedAry[i][numCols + 3] = mirrorFramedAry[i][numCols + 1];
        }
        //print2DArray(mirrorFramedAry);
    }

    public void init(Scanner inFile) throws IOException {
        // Get the header info and init the arrays
        numRows = inFile.nextInt();
        numCols = inFile.nextInt();
        minVal = inFile.nextInt();
        maxVal = inFile.nextInt();
        //System.out.printf("numRows=%d numCols=%d minV=%d maxV=%d\n",numRows,numCols,minVal,maxVal);
        mirrorFramedAry = new int[numRows + 4][numCols + 4];
        avgAry = new int[numRows + 4][numCols + 4];
        CPavgAry = new int[numRows + 4][numCols + 4];
//        Avg_histAry = new int[maxVal + 1];
//        CPavg_histAry = new int[maxVal + 1];
    }

    public void print2DArray(int[][] array) {
        for (int[] ints : array) {
            for (int anInt : ints) {
                if (anInt < 10) {
                    System.out.print(anInt + "  ");
                } else {
                    System.out.print(anInt + " ");
                }

            }
            System.out.println();
        }
    }

    public int convolution5x5(int i, int j, int[][] mask, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering convolution5x5 method\n");
        int result = 0;
        int r = -2;
        int c = -2;
        while (r <= 2) {
            c = -2;
            while (c <= 2) {
                result = result + mask[r + 2][c + 2] * mirrorFramedAry[i + r][j + c];
                c++;
            }
            r++;
        }
        deBugFile.write("Before leaving convolution5x5 method(): i=" + i +
                "; j=" + j +
                "; result=" + result + "\n");
        return result;
    }

    public int avg5x5(int i, int j) {
        int result = 0;
        int r = -2;
        int c = -2;
        while (r <= 2) {
            c = -2;
            while (c <= 2) {
                result = result + mirrorFramedAry[i + r][j + c];
                //System.out.print(mirrorFramedAry[i + r][j + c] + "  ");
                c++;
            }
            r++;
            //System.out.println();
        }
        return result / 25;
    }

    public void computeAvg5x5(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering computeAvg5x5 method\n");
        int newMax = mirrorFramedAry[2][2];
        int newMin = mirrorFramedAry[2][2];
        int i = 2;
        while (i < (numRows + 2)) {
            int j = 2;
            while (j < (numCols + 2)) {
                avgAry[i][j] = avg5x5(i, j);
                if (newMax < avgAry[i][j]) {
                    newMax = avgAry[i][j];
                }
                if (newMin > avgAry[i][j]) {
                    newMin = avgAry[i][j];
                }
                j++;
            }
            i++;
        }
        maxVal = newMax;
        minVal = newMin;
        deBugFile.write("In computAvg5x5 newMax=" + maxVal + " newMin=" + newMin + "\n");
        deBugFile.write("Leaving computeAvg5x5 method\n");
        Avg_histAry = new int[maxVal + 1];
        CPavg_histAry = new int[maxVal + 1];
    }

    public void computeHist(int[][] inAry, int[] histAry, BufferedWriter deBugFile) throws IOException {
        //Avg_histAry = new int[maxVal + 1];
        //CPavg_histAry = new int[maxVal + 1];
        deBugFile.write("entering computeHist method ()\n");
        int i = 2;
        while (i < (numRows + 2)) {
            int j = 2;
            while (j < (numCols + 2)) {
                histAry[inAry[i][j]]++;
                j++;
            }
            i++;
        }
        deBugFile.write("Leaving computeHist method\n");
    }

    public void dispHist(int[] histAry, BufferedWriter outFile) throws IOException {
        // print histogram header
        outFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        for (int i = 0; i < maxVal + 1; i++) {
            outFile.write(i + " (" + histAry[i] + "):");
            for (int j = 0; j < histAry[i]; j++) {
                outFile.write("+");
            }
            outFile.write("\n");
        }
    }

    public void printHist(int[] histAry, BufferedWriter histFile, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("entering printHist method\n");
        histFile.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        int index = 0;//step 1: index= 0;
        while (index <= maxVal) {
            //step 2:histFile =print index, a space, histAry[index] //per text-line
            histFile.write(index + " " + histAry[index] + "\n");
            index++;
        }
        deBugFile.write("Leaving printHist method\n");
    }

    public void cornerPreserveAvg(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("entering cornerPreserveAvg() method\n");
        //Step 0: deBugFile = “entering cornerPreserveAvg() method”
        int newMax = mirrorFramedAry[2][2];
        int newMin = mirrorFramedAry[2][2];
        //Step 1: i = 2
        int i = 2;
        //Step 2:
        while (i < numRows + 2) {
            int j = 2;
            while (j < (numCols + 2)) {
                int maskIndex = 0;
                int minAvg = mirrorFramedAry[i][j];
                int minDiff = 9999;
                while (maskIndex < 8) {
                    int result = convolution5x5(i, j, mask[maskIndex], deBugFile) / 9;
                    int diff = Math.abs(result - mirrorFramedAry[i][j]);
                    if (diff < minDiff) {
                        minDiff = diff;
                        minAvg = result;
                    }
                    maskIndex++;
                }
                CPavgAry[i][j] = minAvg;
                if (newMax < minAvg) {
                    newMax = minAvg;
                }
                if (newMin > minAvg) {
                    newMin = minAvg;
                }
                j++;
            }
            i++;
        }
        maxVal = newMax;
        minVal = newMin;
        deBugFile.write("In cornerPreserveAvg(): newMax=" + maxVal + " newMin=" + minVal + "\n");
        deBugFile.write("Leaving cornerPreserveAvg () method\n");
        Avg_histAry = new int[maxVal + 1];
        CPavg_histAry = new int[maxVal + 1];
    }

    public void imgReformat(int[][] inAry, BufferedWriter fileOut) throws IOException {
        //fileOut = output numRows, numCols, minVal, maxVal
        fileOut.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
        String str = Integer.toString(maxVal);
        int Width = str.length();
        int i = 2;
        while (i < (numRows + 2)) {
            int j = 2;
            while (j < (numCols + 2)) {
                //Step 5: fileOut= inAry[i][j];
                str = Integer.toString(inAry[i][j]);
                fileOut.write(str);
                //System.out.print(str);
                int WW = str.length();
                while (WW <= Width) {
                    fileOut.write(" ");
                    //System.out.print(" ");
                    WW++;
                }
                j++;
            }
            i++;
            fileOut.write("\n");
            //System.out.println();
        }
    }
}

public class SunR_Project2_Main {
    public static void main(String[] args) throws IOException {

//        Scanner inFile = new Scanner(new FileReader(args[0]));
//        int opChoice = Integer.parseInt(args[1]);
//        BufferedWriter outFile1 = new BufferedWriter(new FileWriter(args[2]));
//        BufferedWriter outFile2 = new BufferedWriter(new FileWriter(args[3]));
//        BufferedWriter outFile3 = new BufferedWriter(new FileWriter(args[4]));
//        BufferedWriter deBugFile = new BufferedWriter(new FileWriter(args[5]));

        Scanner inFile = new Scanner(new FileReader("test_input.txt"));
        int opChoice = 2;
        BufferedWriter outFile1 = new BufferedWriter(new FileWriter("output1_3.txt"));
        BufferedWriter outFile2 = new BufferedWriter(new FileWriter("output2_3.txt"));
        BufferedWriter outFile3 = new BufferedWriter(new FileWriter("output3_3.txt"));
        BufferedWriter deBugFile = new BufferedWriter(new FileWriter("debug_3.txt"));

        Enhancement img = new Enhancement();
        img.init(inFile);
        img.loadImage(inFile);
        img.mirrorFraming();
        outFile1.write("Below is the image reformatted mirrorFramedAry\n");
        img.imgReformat(img.getMirrorFramedAry(), outFile1);
        if (opChoice == 1) {
            img.computeAvg5x5(deBugFile);
            outFile1.write("Below is the reformatted image of the result of 5x5 averaging on the input image\n");
            img.imgReformat(img.getAvgAry(), outFile1);
            //outFile2  output numRows, numCols, minVal, maxVal
            //outFile2  output avgAry to outFile2
            img.imgReformat(img.getAvgAry(), outFile2);
            img.computeHist(img.getAvgAry(), img.getAvg_histAry(), deBugFile);
            img.dispHist(img.getAvg_histAry(), outFile1);
            //outFile3  output numRows, numCols, minVal, maxVal
            //printHist (Avg_histAry, outFile3, deBugFile)
            img.printHist(img.getAvg_histAry(), outFile3, deBugFile);
        } else if (opChoice == 2) {
            img.cornerPreserveAvg(deBugFile);
            outFile1.write("Below is reformatted image of the result of corner-preserve averaging on the input image\n");
            img.imgReformat(img.getCPavgAry(), outFile1);
            // print reformatted CPavgAry to outFile1
//            outFile2  output numRows, numCols, minVal, maxVal
//            outFile2  output CPavgAry to outFile2
            img.imgReformat(img.getCPavgAry(), outFile2);
            img.computeHist(img.getCPavgAry(), img.getCPavg_histAry(), deBugFile);
            img.dispHist(img.getCPavg_histAry(), outFile1);
            img.printHist(img.getCPavg_histAry(), outFile3, deBugFile);
        }
        // Close all files
        inFile.close();
        outFile1.close();
        outFile2.close();
        outFile3.close();
        deBugFile.close();
    }
}
