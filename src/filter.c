#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "filter.h"

#define FILTERS "begr"

#define FILE_TYPE 0x4d42
#define BM_BPP_NUMBER 24
#define BM_COMPRESSION 0

int main(int argc, char *argv[])
{
    char filter = parse_arguments(argc, argv);

    // Remember filenames
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];
    FILE *inptr = NULL;
    FILE *outptr = NULL;
    open_files(infile, outfile, &inptr, &outptr);

    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    validate_bitmap_headers(infile, inptr, outptr, &bf, &bi);

    // Get image's dimensions
    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        cleanup(inptr, outptr, NULL, 7);
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }


    apply_filter(filter, height, width, image);
    

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    cleanup(inptr, outptr, image, 0);
}

/**
 * Parses the command line arguments to determine the filter to be applied and ensures
 * that the correct number of arguments are provided.
 * 
 * @param argc The number of command line arguments passed to the program.
 * @param argv An array of pointers to strings representing the command line arguments.
 * @return A character representing the filter to be applied.
 */
char parse_arguments(int argc, char *argv[])
{
    // Get filer flag and check validity
    char filter = getopt(argc, argv, FILTERS);
    if (filter == '?') {
        printf("Invalid filter.\n");
        cleanup(NULL, NULL, NULL, 1);
    }

    // Ensure only one filter
    if (getopt(argc, argv, FILTERS) != -1) {
        printf("Only one filter allowed.\n");
        cleanup(NULL, NULL, NULL, 2);
    }

    // Ensure proper usage
    if (argc != optind + 2) {
        printf("Usage: ./filter [flag] infile outfile\n");
        cleanup(NULL, NULL, NULL, 3);
    }

    return filter;
}

/**
 * Opens the input and output files specified by the file names.
 * 
 * @param infile_name Pointer to a string containing the name of the input file to be opened.
 * @param outfile_name Pointer to a string containing the name of the output file to be opened.
 * @param inptr Pointer to a FILE pointer where the opened input file handle will be stored.
 * @param outptr Pointer to a FILE pointer where the opened output file handle will be stored.
 */
void open_files(char *infile_name, char *outfile_name, FILE **inptr, FILE **outptr)
{
    // Open input file
    *inptr = fopen(infile_name, "rb");
    if (*inptr == NULL) {
        printf("Could not open %s.\n", infile_name);
        cleanup(NULL, NULL, NULL, 4);
    }

    // Open output file
    *outptr = fopen(outfile_name, "w");
    if (*outptr == NULL) {
        printf("Could not create %s.\n", outfile_name);
        cleanup(*inptr, NULL, NULL, 5);
    }

    return;
}

/**
 * Reads and validates the BITMAPFILEHEADER and BITMAPINFOHEADER from the input file.
 * Ensures that the file is a 24-bit uncompressed BMP 4.0.
 * 
 * @param infile_name Pointer to a string containing the name of the input file to be opened.
 * @param inptr Pointer to the input file from which the headers are read.
 * @param outptr Pointer to the output file.
 * @param bf Pointer to a BITMAPFILEHEADER structure where the file header will be stored.
 * @param bi Pointer to a BITMAPINFOHEADER structure where the info header will be stored.
 */
void validate_bitmap_headers(char *infile_name, FILE *inptr, FILE *outptr, BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi)
{
    // Read infile's BITMAPFILEHEADER
    if (fread(bf, sizeof(*bf), 1, inptr) != 1) {
	    printf("Could not read file header from %s.\n", infile_name);
        cleanup(inptr, outptr, NULL, 6);
    }

    // Read infile's BITMAPINFOHEADER
    if(fread(bi, sizeof(*bi), 1, inptr) != 1) {
	    printf("Could not read info header from %s.\n", infile_name);
	    cleanup(inptr, outptr, NULL, 7);
    }

    // Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf->bfType != FILE_TYPE || bi->biBitCount != BM_BPP_NUMBER || bi->biCompression != BM_COMPRESSION) {
	    printf("Unsupported file format.\n");
	    cleanup(inptr, outptr, NULL, 8);
    }

    return;
}

/**
 * Applies the specified filter to the image based on the filter character provided.
 * The function modifies the image in place according to the filter selected.
 * 
 * @param filter A character indicating which filter to apply. Valid options are:
 *               'b' for blur, 'e' for edges, 'g' for grayscale, and 'r' for reflect.
 * @param height The height of the image in pixels.
 * @param width The width of the image in pixels.
 * @param image A pointer to the image array where the filter will be applied.
 */
void apply_filter(char filter, int height, int width, RGBTRIPLE(*image)[width])
{
    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Edges
        case 'e':
            edges(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflect
        case 'r':
            reflect(height, width, image);
            break;
    }

    return;
}


/**
 * Frees allocated memory and closes open files.
 * 
 * @param inptr Pointer to the input file. If NULL, no action is taken.
 * @param outptr Pointer to the output file. If NULL, no action is taken.
 * @param image Pointer to the dynamically allocated image memory. If NULL, no action is taken.
 * @param status_code The exit code to terminate the program with.
 */
void cleanup(FILE *inptr, FILE *outptr, RGBTRIPLE(*image)[], int status_code)
{
    // Free memory for image
    if (image != NULL) {
        free(image);
    }

    // Close files
    if (inptr != NULL) {
        fclose(inptr);
    }

    if (outptr != NULL) {
        fclose(outptr);
    }

    exit(status_code);   
}
