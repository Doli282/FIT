#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void calculateHistogram(unsigned int *histogram, unsigned char R, unsigned char G, unsigned char B)
{
    // calculate histogram
    double Y = round(0.2126 * R + 0.7152 * G + 0.0722 * B); // vzorec na histogram
    //printf("Y = %lf  (R =%d,G=%d,B=%d) in interval: ", Y, R, G, B);
    if (Y < 153.0)
    {
        if (Y < 51.0)
        { // <0,50>
            histogram[0]++;
            //printf("0\n");
        }
        else if (Y < 102.0)
        { // <51,101>
            histogram[1]++;
            //printf("1\n");
        }
        else
        { // <102,152>
            histogram[2]++;
            //printf("2\n");
        }
    }
    else if (Y < 204.0)
    { // <153,203>
        histogram[3]++;
        //printf("3\n");
    }
    else
    { // <204,255>
        histogram[4]++;
        //printf("4\n");
    }
    return;
}

void printBorder(FILE *fd, unsigned int *histogram, unsigned char *row, unsigned int width3)
{
    for (unsigned int w = 0; w < width3;)
    {
        fputc(row[w++], fd);
        fputc(row[w++], fd);
        fputc(row[w++], fd);
        calculateHistogram(histogram, row[w - 3], row[w - 2], row[w - 1]);
    }
    return;
}

int main(int argc, char **argv)
{
    // time struct for tiem measurement
//    struct timespec start, stop;
//    clock_gettime(CLOCK_REALTIME, &start);

    // histogram
    // double Y;
    unsigned int *histogram = (unsigned int *)calloc(sizeof(*histogram), 5);
    // unsigned int h1 = 0; // <0,50>
    // unsigned int h2 = 0; // <51,101>
    // unsigned int h3 = 0; // <102,152>
    // unsigned int h4 = 0; // <153,203>
    // unsigned int h5 = 0; // <204,255>

    unsigned int height;
    unsigned int width;
    int tmp2;
    size_t outtmp;
    // tmp2 = printf("%s\n", argv[1]);
    FILE *source = fopen(argv[1], "rb"); // open file with the image
    // FILE *source = fopen("./test-10x8/test.ppm", "rb"); // open file with the image
    FILE *output = fopen("output.ppm", "wb"); // open file for the focused image

    /* if (source)
    {
        tmp2 = printf("source opened\n");
    }
    if (output)
    {
        tmp2 = printf("output opened\n");
    } */

    // read and write header
    char tmp1[4];
    tmp2 = fscanf(source, "%s\n%u\n%u\n%d\n", tmp1, &width, &height, &tmp2);
    tmp2 = fprintf(output, "P6\n%u\n%u\n255\n", width, height);
    // tmp2 = fprintf(stdin, "P6\n%u\n%u\n255\n", width, height);

    unsigned int width3 = 3 * width;
    unsigned char *tmp;
    unsigned char *row1 = (unsigned char *)malloc(width3 * sizeof(*row1));
    unsigned char *row2 = (unsigned char *)malloc(width3 * sizeof(*row2));
    unsigned char *row3 = (unsigned char *)malloc(width3 * sizeof(*row3));
    unsigned char *rowOut = (unsigned char *)malloc(width3 * sizeof(*rowOut));
    /* if (!row1 || !row2 || !row3 || !rowOut)
    {
        printf("UNsuccessfully allocated\n");
    } */

    // read and write the border line
    // for the top border
    if ((outtmp = fread((unsigned char *)row1, sizeof(*row1), width3, source)) == width3)
    {
        ;
    }
    printBorder(output, histogram, row1, width3);
    //printf("top border printed\n");
    // outtmp = fwrite(row1, sizeof(*row1), width3, output);
    // printf("successfully written\n");

    // second row
    outtmp = fread((unsigned char *)row2, sizeof(*row2), width3, source);
    // outtmp = fwrite(row2, sizeof(*row2), width3, output);
    //printf("successfully read\n");

    // for the insides
    for (unsigned int h = 3; h <= height; h++)
    {
        // scann following row
        outtmp = fread((unsigned char *)row3, sizeof(*row3), width3, source);
        // outtmp = fwrite(row3, sizeof(*row3), width3, output);
        //printf("successfully read in loop\n");

        // convolution
        fputc(row2[0], output);
        fputc(row2[1], output);
        fputc(row2[2], output);
        calculateHistogram(histogram, row2[0], row2[1], row2[2]);
        // convolution table
        //  0 -1  0
        // -1  5 -1
        //  0 -1  0
        unsigned int w = 3;
        unsigned int focused;
        for (; w < (width3 - 3);)
        {
            focused = 5 * row2[w] - row2[w - 3] - row2[w + 3] - row1[w] - row3[w];
            rowOut[w] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //printf("center(%d), left(%d), right(%d), up(%d), bottom(%d) -> %d\n,", row2[w], row2[w - 3], row2[w + 3], row1[w], row3[w], rowOut[w]);
            fputc(rowOut[w++], output);
            focused = 5 * row2[w] - row2[w - 3] - row2[w + 3] - row1[w] - row3[w];
            rowOut[w] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //printf("center(%d), left(%d), right(%d), up(%d), bottom(%d) -> %d\n,", row2[w], row2[w - 3], row2[w + 3], row1[w], row3[w], rowOut[w]);
            fputc(rowOut[w++], output);
            focused = 5 * row2[w] - row2[w - 3] - row2[w + 3] - row1[w] - row3[w];
            rowOut[w] = (focused > 255) ? ((focused > 2000) ? 0 : 255) : focused;
            //printf("center(%d), left(%d), right(%d), up(%d), bottom(%d) -> %d\n,", row2[w], row2[w - 3], row2[w + 3], row1[w], row3[w], rowOut[w]);
            fputc(rowOut[w++], output);

            calculateHistogram(histogram, rowOut[w - 3], rowOut[w - 2], rowOut[w - 1]);
        }
        fputc(row2[w++], output);
        fputc(row2[w++], output);
        fputc(row2[w], output);
        calculateHistogram(histogram, row2[w - 2], row2[w - 1], row2[w]);

        // reconnect pointers
        tmp = row1;
        row1 = row2;
        row2 = row3;
        row3 = tmp;
    }
    // for the bottom border
    // outtmp = fread((unsigned char * ) row3, sizeof(*row3), width3, source);
    // outtmp = fwrite(row3, sizeof(*row3), width3, output);
    // printf("successfully read for the last time\n");

    printBorder(output, histogram, row2, width3);
    //printf("bottom border printed\n");

    // free memory
    free(row1);
    free(row2);
    free(row3);
    free(rowOut);
    //printf("successfully freed\n");

    // close files
    fclose(source);
    //printf("successfully closed\n");
    fclose(output);
    //printf("successfully closed\n");

    // write histogram to output file
    FILE *histFile = fopen("output.txt", "w");
    tmp2 = fprintf(histFile, "%u %u %u %u %u", histogram[0], histogram[1], histogram[2], histogram[3], histogram[4]);
    fclose(histFile);
    free(histogram);
    //printf("successfully created histogram\n");

    // time measurement
//    clock_gettime(CLOCK_REALTIME, &stop);
//    double accum = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1000000.0;
//    printf("Time: %.6lf ms\n", accum);

    return 0;
}