#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "helpers.h"

void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            //Take the gray value as the average of the value of each of the layers for a given position
            float average = (image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0;
            int roundedAverage = round(average);
            // Apply the grayscale value to all three RGB components
            image[i][j].rgbtBlue = roundedAverage;
            image[i][j].rgbtGreen = roundedAverage;
            image[i][j].rgbtRed = roundedAverage;
        }
    }
    return;
}

void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        int j = 0;
        int m = width - 1;
        while (j < m)
        {
            // Swap pixels on opposite sides of the image horizontally
            RGBTRIPLE temp;
            temp = image[i][j];
            image[i][j] = image[i][m];
            image[i][m] = temp;
            j++;
            m--;
        }
    }
    return;
}

void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*temp)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (temp == NULL)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int count = 0;
            float sum_red = 0.0;
            float sum_green = 0.0;
            float sum_blue = 0.0;

            // Sum the values of the pixel and its neighbors
            for (int k = i - 1; k < i + 2; k++)
            {
                for (int l = j - 1; l < j + 2; l++)
                {

                    // Takes all non border cells within a 3x3 matrix centered in current position
                    if (k >= 0 && k < height && l >= 0 && l < width)
                    {
                        sum_red += image[k][l].rgbtRed;
                        sum_green += image[k][l].rgbtGreen;
                        sum_blue += image[k][l].rgbtBlue;
                        ++count;
                    }

                }
            }

            // Computes the average (note that not all positions have the same amount of cells around)
            temp[i][j].rgbtRed = round(sum_red / count);
            temp[i][j].rgbtGreen = round(sum_green / count);
            temp[i][j].rgbtBlue = round(sum_blue / count);
        }
    }

    memcpy(image, temp, height * width * sizeof(RGBTRIPLE));

    free(temp);

    return;
}

void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*temp)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (temp == NULL)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Initialise Sobel masks in horizontal and vertical directions
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int gxr = 0, gyr = 0;
            int gxg = 0, gyg = 0;
            int gxb = 0, gyb = 0;

            // Calculate gradient magnitudes for each color channel
            for (int k = i - 1; k <= i + 1; k++)
            {
                for (int l = j - 1; l <= j + 1; l++)
                {
                    if (k >= 0 && k < height && l >= 0 && l < width)
                    {
                        int weightX = gx[k - i + 1][l - j + 1];
                        int weightY = gy[k - i + 1][l - j + 1];

                        RGBTRIPLE pixel = image[k][l];
                        gxr += pixel.rgbtRed * weightX;
                        gyr += pixel.rgbtRed * weightY;
                        gxg += pixel.rgbtGreen * weightX;
                        gyg += pixel.rgbtGreen * weightY;
                        gxb += pixel.rgbtBlue * weightX;
                        gyb += pixel.rgbtBlue * weightY;
                    }
                }
            }

            // Compute the final color values by combining the gradients and clamping
            temp[i][j].rgbtRed = fmin(round(sqrt(gxr * gxr + gyr * gyr)), 255);
            temp[i][j].rgbtGreen = fmin(round(sqrt(gxg * gxg + gyg * gyg)), 255);
            temp[i][j].rgbtBlue = fmin(round(sqrt(gxb * gxb + gyb * gyb)), 255);
        }
    }

    
    memcpy(image, temp, height * width * sizeof(RGBTRIPLE));

    free(temp);

    return;
}
