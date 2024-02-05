# Image filter

Image filter is a simple implementation of a program in C to apply filters to 24-bit uncompressed images in Microsoft's BMP 4.0 format.

The program allows applying grayscale filters, reflection, blur, and edge highlighting to the selected image.

It shows the basics of opening, reading and processing operations on binary files. Since this is a quick and purely educational implementation, it has many limitations, and is certainly not entirely efficient. My plan is to continue advancing in the implementation and add new functionalities as I find the time to do it.

## Running

Use `gcc -o filter src/filter.c src/helpers.c -lm` to compile, and then `./filter [flag] infile outfile` to apply a filter to the desired image. The available flags are:

-  **Grayscale (-g):** Takes an image and converts it to black and white, by averaging the values of red, green and blue for each pixel
- **Reflection (-r):** Pixels on the left side of the image are placed on the right. Resulting image is what you would get by placing the original image in front of a mirror
- **Blur (-b):** Creates a blurring effect. It uses box blur with a 3x3 grid, so the effect is not likely to be very noticeable in large images
- **Edges (-e):** Highlights the boundaries between objects, through the application of the Sobel operator

**Example usage:** *Highlight the edges of infile.bmp file and save the result to outfile.bmp*

```shell
./filter -e infile.bmp outfile.bmp
```

> About image conversion: You can use tools such as [imagemagick](https://imagemagick.org/index.php) to convert the images to the appropriate format. With this tool, you can use the following command to get the image in the correct format `convert infile.jpg -type TrueColor -depth 24 outfile.bmp`
