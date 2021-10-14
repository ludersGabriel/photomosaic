/* Feito por Gabriel Lüders
  GRR20190172 */

#ifndef IMAGE_H
#define IMAGE_H

#include <dirent.h>

// struct that defines a RGB Pixel
typedef struct Pixel{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
}pixel;

// struct that defines what my program understands
// as a ppm file
// dimensions[0] == width
// dimensions[1] == height
typedef struct Image{
  struct dirent* file; // only tiles will have this prop 
  char type[3];
  long dimensions[2];
  int max;
  struct Pixel averagePixel;
  struct Pixel** pixels;
}image;

// setts pointers to null and int variables to -1
void InitializeImage(image* img);

// iterates through the passed dir and returns an array
// of images containing all dirents
image* GetImages(char* dir, long* size);

// coordinates the processing of a single image
void ProcessImage(FILE* file, image* img);

// responsible for processing the ppms in an array of images
void ProcessImages(char* dir, image* images, long n);

// structures the final image
void ComposeFinalImage(image* images, long n, image* mainImage);

// save all modifications done to mainImage into an output
void WriteFinalImage(image* mainImage, FILE** output);

// frees all memory allocated by a single image struct
void FreeImageData(image* img);

// frees all memory allocated by an struct image array
void FreeImagesData(image** images, long n);

#endif