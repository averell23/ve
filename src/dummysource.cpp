/***************************************************************************
 *   Copyright (C) 2003 by Daniel Hahn,,,                                  *
 *   daniel@81pc04                                                         *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
#include "dummysource.h"

DummySource::DummySource()
 : VideoSource()
{
    bool status = true;
    char filename[1024];
    status = true; 
    
    for ( int i = 0 ; i < NUM_IMAGES ; i++ ) {
	sprintf(filename, "images/img%04d.ppm.bmp", 1000 + i);
	if (!loadBMP(filename, &images[i])) { status = false; }
	if (images[i] == NULL) { cout << "FATAL" << endl; }
    }

    
    if ( !status ) {
	std::cerr << "Error: Could not load images. This may be fatal." << std::endl;
    } else {
	width = images[0]->width;
	height = images[0]->height;
    }
    
    position = 0;
}

DummySource::~DummySource()
{
    for ( int i = 0 ; i < NUM_IMAGES ; i++ ) {
	free(images[i]->imageData);
	cvReleaseImageHeader(&images[i]);
    }
    cout << "Data structures freed." << endl;
}

IplImage* DummySource::getImage() {
    position = ++position % NUM_IMAGES;

    return images[position];
}

/* simple loader for 24bit bitmaps (data is in rgb-format) */
int DummySource::loadBMP(char *filename, IplImage **image)
{
	FILE *file;
	unsigned short int bfType;
	long int bfOffBits;
	short int biPlanes;
	short int biBitCount;
	long int biSizeImage;
	int i;
	unsigned char temp;
	int width, height;
	char* data;
	
	/* make sure the file is there and open it read-only (binary) */
	if ((file = fopen(filename, "rb")) == NULL)
	{
		printf("File not found : %s\n", filename);
		return 0;
	}
	if(!fread(&bfType, sizeof(short int), 1, file))
	{
		printf("Error reading file!\n");
		return 0;
	}
	/* check if file is a bitmap */
	if (bfType != 19778)
	{
		printf("Not a Bitmap-File!\n");
		return 0;
	}        
	/* get the file size */
	/* skip file size and reserved fields of bitmap file header */
	fseek(file, 8, SEEK_CUR);
	/* get the position of the actual bitmap data */
	if (!fread(&bfOffBits, sizeof(long int), 1, file))
	{
		printf("Error reading file!\n");
		return 0;
	}
	printf("Data at Offset: %ld\n", bfOffBits);
	/* skip size of bitmap info header */
	fseek(file, 4, SEEK_CUR);
	/* get the width of the bitmap */
	fread(&width, sizeof(int), 1, file);
	printf("Width of Bitmap: %d\n", width);
	/* get the height of the bitmap */
	fread(&height, sizeof(int), 1, file);
	printf("Height of Bitmap: %d\n", height);
	/* get the number of planes (must be set to 1) */
	fread(&biPlanes, sizeof(short int), 1, file);
	if (biPlanes != 1)
	{
		printf("Error: number of Planes not 1!\n");
		return 0;
	}
	/* get the number of bits per pixel */
	if (!fread(&biBitCount, sizeof(short int), 1, file))
	{	CvSize size;
	size.height = height;
	size.width = width;
		printf("Error reading file!\n");
		return 0;
	}
	printf("Bits per Pixel: %d\n", biBitCount);
	if (biBitCount != 24)
	{
		printf("Bits per Pixel not 24\n");
		return 0;
	}
	/* calculate the size of the image in bytes */
	biSizeImage = width * height * 3;
	printf("Size of the image data: %ld\n", biSizeImage);
	data = (char*) malloc(biSizeImage);
	/* seek to the actual data */
	fseek(file, bfOffBits, SEEK_SET);
	if (!fread(data, biSizeImage, 1, file))
	{
		printf("Error loading file!\n");
		return 0;
	}
	/* swap red and blue (bgr -> rgb) */
	for (i = 0; i < biSizeImage; i += 3)
	{
		temp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = temp;
	}
	
	CvSize size;
	size.height = height;
	size.width = width;
	
	*image = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
	(*image)->imageData = data;

	return 1;
}


