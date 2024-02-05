// Parse arguments and get the filter
char parse_arguments(int argc, char *argv[]);

// Open input and output files
void open_files(char *infile_name, char *outfile_name, FILE **inptr, FILE **outptr);

// Validate BMP headers
void validate_bitmap_headers(char *infile_name, FILE *inptr, FILE *outptr, BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi);

// Apply the selected filter
void apply_filter(char filter, int height, int width, RGBTRIPLE(*image)[width]);

// Frees any allocated memory and closes open files
void cleanup(FILE *inptr, FILE *outptr, RGBTRIPLE(*image)[], int status_code);