#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#pragma GCC optimize ("Ofast,unroll-loops")

// calculate histogram
/* void calculateHistogram(unsigned short *histogram, unsigned char Y)
{
    //double Y = round(0.2126 * R + 0.7152 * G + 0.0722 * B); // vzorec na histogram
    if (Y < 153)
    {
        if (Y < 51)
        { // <0,50>
            histogram[0]++;
        }
        else if (Y < 102)
        { // <51,101>
            histogram[1]++;
        }
        else
        { // <102,152>
            histogram[2]++;
        }
    }
    else if (Y < 204)
    { // <153,203>
        histogram[3]++;
    }
    else
    { // <204,255>
        histogram[4]++;
    }
    return;
} */

unsigned short sum(unsigned short * histogram, unsigned int start, unsigned int end)
{
    unsigned short result = 0;
    for(; start <= end; start++)
    {
        result += histogram[start];
    }
    return result;
}

/* for (unsigned int w = 0, w1 = 1, w2 = 2; w < width3; w += 3, w1 += 3, w2 += 3)
    {
        fputc(row[w], fd);
        fputc(row[w1], fd);
        fputc(row[w2], fd);
        calculateHistogram(histogram, row[w], row[w1], row[w2]);
    } */

void printBorder(FILE *fd, unsigned short *histogram, unsigned char *row, unsigned short width3)
{
    for (unsigned short w = 0; w < width3;w+=3)
    {
        //calculateHistogram(histogram, 0.5 + (0.2126 * row[w] + 0.7152 * row[w+1] + 0.0722 * row[w+2]));
        histogram[(int)(0.5 + (0.2126 * row[w] + 0.7152 * row[w+1] + 0.0722 * row[w+2]))]++;
    }
    fwrite((unsigned char *)row, sizeof(unsigned char), width3, fd);
    return;
}

int main(int argc, char **argv)
{
    // time struct for tiem measurement
//    struct timespec start, stop;
//    clock_gettime(CLOCK_REALTIME, &start);

    unsigned char * tmp;
    unsigned char * row1;
    unsigned char * row2;
    unsigned char * row3;
    unsigned char * rowOut;
    unsigned short * histogram = (unsigned short *)calloc(sizeof(unsigned short), 256);
    
    unsigned short focused;
    unsigned short w;
    unsigned short width;
    unsigned short width3;

    unsigned short height;
    unsigned short h;

    size_t outtmp;
    
    FILE *source = fopen(argv[1], "rb"); // open file with the image
    FILE *output = fopen("output.ppm", "wb"); // open file for the focused image

    // read and write header
    fseek(source, 3, SEEK_CUR);
    outtmp = fscanf(source, "%hu\n%hu\n255\n", &width3, &height);
    fprintf(output, "P6\n%hu\n%hu\n255\n", width3, height);

    width3 *= 3;
    width = width3-3;
    row1 = (unsigned char *)malloc(width3 * sizeof(unsigned char));
    row2 = (unsigned char *)malloc(width3 * sizeof(unsigned char));
    row3 = (unsigned char *)malloc(width3 * sizeof(unsigned char));
    rowOut = (unsigned char *)malloc(width3 * sizeof(unsigned char));

    // read and write the border line
    // for the top border
    if ((outtmp = fread((unsigned char *)row1, sizeof(unsigned char), width3, source)) == width3)
    {
        ;
    }
    printBorder(output, histogram, row1, width3);

    // second row
    outtmp = fread((unsigned char *)row2, sizeof(unsigned char), width3, source);

    // for the insides
    for (h = 3; h <= height; h++)
    {
        // scann following row
        outtmp = fread((unsigned char *)row3, sizeof(unsigned char), width3, source);

        // convolution
//        fputc(row2[0], output);
//        fputc(row2[1], output);
//        fputc(row2[2], output);
        //calculateHistogram(histogram, 0.5 + (0.2126 * (rowOut[0] = row2[0]) + 0.7152 * (rowOut[1] = row2[1]) + 0.0722 * (rowOut[2] = row2[2])));
        histogram[(int)(0.5 + (0.2126 * (rowOut[0] = row2[0]) + 0.7152 * (rowOut[1] = row2[1]) + 0.0722 * (rowOut[2] = row2[2])))]++;
        // convolution table
        //  0 -1  0
        // -1  5 -1
        //  0 -1  0
        w = 3;
        for (; w < width;)
        {
            focused = - row2[w - 3] + 5 * row2[w]  - row2[w + 3] - row1[w] - row3[w];
            rowOut[w++] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //fputc(rowOut[w++], output);
            focused = - row2[w - 3] + 5 * row2[w] - row2[w + 3] - row1[w] - row3[w];
            rowOut[w++] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //fputc(rowOut[w++], output);
            focused = - row2[w - 3] + 5 * row2[w] - row2[w + 3] - row1[w] - row3[w];
            rowOut[w++] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //fputc(rowOut[w++], output);

            histogram[(int)(0.5 + (0.2126 * rowOut[w - 3] + 0.7152 * rowOut[w - 2] + 0.0722 * rowOut[w - 1]))]++;
            //calculateHistogram(histogram, 0.5 + (0.2126 * rowOut[w - 3] + 0.7152 * rowOut[w - 2] + 0.0722 * rowOut[w - 1]));
        }
//        fputc(row2[w++], output);
//        fputc(row2[w++], output);
//        fputc(row2[w], output);
        //calculateHistogram(histogram, 0.5 + (0.2126 * (rowOut[w] = row2[w]) + 0.7152 * (rowOut[w+1] = row2[w+1]) + 0.0722 * (rowOut[w+2] = row2[w+2])));
        histogram[(int)(0.5 + (0.2126 * (rowOut[w] = row2[w]) + 0.7152 * (rowOut[w+1] = row2[w+1]) + 0.0722 * (rowOut[w+2] = row2[w+2])))]++;
        fwrite((unsigned char *)rowOut, sizeof(unsigned char), width3, output);

        // reconnect pointers
        tmp = row1;
        row1 = row2;
        row2 = row3;
        row3 = tmp;
    }
    // for the bottom border

    printBorder(output, histogram, row2, width3);

    // free memory
    free(row1);
    free(row2);
    free(row3);
    free(rowOut);

    // close files
    fclose(source);
    fclose(output);

    // write histogram to output file
    FILE *histFile = fopen("output.txt", "w");
    fprintf(histFile, "%u %u %u %u %u", sum(histogram, 0, 50),
    sum(histogram, 51, 101),
    sum(histogram, 102, 152),
    sum(histogram, 153, 203),
    sum(histogram, 204, 255));
    fclose(histFile);
    free(histogram);

    // time measurement
//    clock_gettime(CLOCK_REALTIME, &stop);
//    double accum = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1000000.0;
//    printf("Time: %.6lf ms\n", accum);

    return 0;
}