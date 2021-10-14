/* Feito Por Gabriel Lüders
   GRR20190172 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "image.h"
#include "dir.h"
#include "utils.h"

// writes the data matrix of a P3 ppm into the output
void PrintPixels(image* img, FILE** output){
  if(!img) return;
  if(!img->pixels) return;

  // reminder: width is for columns and 
  //height is for rows
  long i, j;
  for(i = 0; i < img->dimensions[1]; i++){
    if(i % (img->dimensions[1] / 10) == 0 ){
      fprintf(stderr, "\r[+] Writing target P3 output: %ld%%", (i * 100/ img->dimensions[1]) );
      fflush(stderr);
    }

    for(j = 0; j < img->dimensions[0]; j++){
      fprintf(
        (*output),
        "%d %d %d ",
        img->pixels[i][j].red,
        img->pixels[i][j].green,
        img->pixels[i][j].blue
      );
    }
    
    fprintf((*output), "\n");
  }

  fprintf(stderr, "\r[+] Writing target P3 output: 100%%\n");
  fflush(stderr);
}

// writes the header of a ppm into the output
void PrintHeader(image* img, FILE** output){
  fprintf((*output), "%s\n", img->type);
  fprintf(
    (*output),
    "%ld %ld\n", 
    img->dimensions[0], 
    img->dimensions[1]
  );
  fprintf((*output), "%d\n", img->max);
}

// a debugging function that prints the
// average pixel of an img 
void PrintAveragePixel(image* img){
  printf(
    "%d %d %d",
    img->averagePixel.red,
    img->averagePixel.green,
    img->averagePixel.blue
  );
}

// sets default values for a pixel
void InitializePixel(pixel* p){
  if(!p) return;
    
  p->blue = 0;
  p->green = 0;
  p->red = 0;
}

// setts pointers to null and int variables to -1
void InitializeImage(image* img){
  if(!img) return;
    
  img->file = NULL;
  img->pixels = NULL;    
  img->dimensions[0] = -1;
  img->dimensions[1] = -1;
  img->max = -1;
  InitializePixel(&img->averagePixel);
}

// iterates through the passed dir and returns an array
// of images containing all dirents
image* GetImages(char* dir, long* n){
  fprintf(stderr, "[+] Getting tiles from %s\n", dir);

  struct dirent **list = GetDirEntries(dir, n);

  image* images = malloc((*n) * sizeof(image));
  if(!images)
    FireMemErrorException(__func__);

  for(long i = 0; i < (*n); i++){
    InitializeImage(&images[i]);
    images[i].file = list[i];  
  }

  free(list);
  list = NULL;

  return images;
}

// Calculates the average pixel
// the average is calculated with float values and 
// rounded up at the end
// it's also casted to unsigned char
pixel AveragePixel(image* img){
  long i, j, num = 0;
  pixel averagePixel;
  InitializePixel(&averagePixel);
  if(!img->pixels) return averagePixel;

  float red, green, blue;
  red = green = blue = 0;

  // reminder: width is for columns and
  // height is for rows
  for(i = 0; i < img->dimensions[1]; i++){
    for(j = 0; j < img->dimensions[0]; j++){
      red += powf(img->pixels[i][j].red, 2.0); 
      green += powf(img->pixels[i][j].green, 2.0); 
      blue += powf(img->pixels[i][j].blue, 2.0); 
      num++;
    }
  }

  averagePixel.red =  round(sqrtf(red/num));
  averagePixel.green =  round(sqrtf(green/num));
  averagePixel.blue =  round(sqrtf(blue/num));

  return averagePixel;
}

// allocates memory for the data matrix 
// and processes a P6 ppm
void ProcessP6(FILE* file, image* img){
  if(!img->file){
    fprintf(stderr, "\r[+] Processing target P6 image: 0%%");
    fflush(stderr);
  }
  
  long blocks;

  img->pixels = (pixel**) AllocateMatrix(
    sizeof(pixel),
    img->dimensions[1],
    img->dimensions[0]
  );

  // reads the entire data block ontop of the
  // data matrix of the P6 ppm
  blocks = fread(
    img->pixels[0],
    sizeof(pixel),
    img->dimensions[0] * img->dimensions[1],
    file
  );

  if(blocks != img->dimensions[0] * img->dimensions[1]){
    fprintf(stderr, "Falha ao executar fread em %s\n", __func__);
    exit(1);
  }

  if(!img->file){
    fprintf(stderr, "\r[+] Processing target P6 image: 100%%\n");
    fflush(stderr);
  }
}

// allocates memory for the data matrix 
// and processes a P3 ppm
void ProcessP3(FILE* file, image* img){
  unsigned char num;
  long i;
  char *buffer;

  long size = img->dimensions[0] * img->dimensions[1] * sizeof(pixel);
  unsigned char* v = malloc(size);
  if(!v){
    FireMemErrorException(__func__);
  }

  img->pixels = (pixel**) AllocateMatrix(
    sizeof(pixel), 
    img->dimensions[1],
    img->dimensions[0]
  );

  for(i = 0; i < size && fscanf(file, "%m[^\n]", &buffer) > 0; ){
    // checks if is a tile or main image
    if(!img->file && i % (size/10) == 0){
      fprintf(stderr, "\r[+] Processing target P3 image: %ld%%", i * 100 / size);
      fflush(stderr);
    }

    // filters \n after fscanf
    fgetc(file);

    FilterString(buffer, '#');

    // after filtering comments,
    // if buffer exists, iterate through
    // grabbing values
    if(strlen(buffer)){
      int n;
      char *p = buffer;
      for(n = 0; i < size && p[0];){
        sscanf(p, "%hhu %n", &num, &n);
        p += n;
        v[i] = num;
        i++;
      }
    }

    free(buffer);
  }

  if(!img->file){
    fprintf(stderr, "\r[+] Processing target P3 image: 100%%\n");
    fflush(stderr);
  }

  // copia o bloco de memória do vetor em cima da matriz de dados da struct image
  memcpy(img->pixels[0], v, size);
  free(v);
  v = NULL;
  buffer = NULL;
}

// treats the header of the file 
// and populates the image struct with the info
void ProcessHeader(FILE* file, image* img){
  long i;
  char *buffer = NULL;
  char elements[3][10];

  for(i = 0; i < 3 && fscanf(file, "%m[^\n]", &buffer) > 0;){
    // filters the \n after fscanf
    fgetc(file);

    FilterString(buffer, '#');

    // after filtering the comments
    // if buffer exists, copy it into the 
    // aux array elements
    if(strlen(buffer)){
      strcpy(elements[i], buffer);
      i++;
    }

    free(buffer);
  }

  // copies the elements of the aux array into
  // the struct image following the pattern: 
  // element[0] == magic number
  // element[1] == width height
  // elemnt[2] == max

  // removes spaces after the magic number
  FilterString(elements[0], ' ');
  if(strlen(elements[0]) <= 3)
    strcpy(img->type, elements[0]);
  else{
    fprintf(stderr, "Erro em %s\n", __func__);
    fprintf(stderr, "Número mágico deve ser P3 ou P6\n");
    exit(1);
  }

  // separates width and height by the space
  // and save them in the dimension array
  char *token;
  token = strtok(elements[1], " ");
  for(i = 0; i < 2 && token != NULL; i++){
    img->dimensions[i] = atoi(token);
    token = strtok(NULL, " ");
  }

  img->max = atoi(elements[2]);
}

// coordinates the processing of a single image
void ProcessImage(FILE* file, image* img){
  ProcessHeader(file, img);
  if(!strcmp(img->type, "P3"))
      ProcessP3(file, img);
  else if(!strcmp(img->type, "P6"))
      ProcessP6(file, img);
  else {
    fprintf(stderr, "Erro em %s\n", __func__);
    fprintf(stderr, "Tipo de imagem inválido\n");
    exit(1);
  }

  img->averagePixel = AveragePixel(img);
}

// responsible for processing the ppms in an array of images
void ProcessImages(char* dir, image* images, long n){
  long i;
  char* fullName = NULL;
  FILE* file = NULL;

  for(i = 0; i < n; i++){
    // formulates the path for the ppm
    // and opnes the file
    fullName = AppendString(dir, images[i].file->d_name);
    file = OpenFile(fullName, "r");

    if(i % (n/10) == 0){
      fprintf(stderr, "\r[+] Loading tiles into memory: %ld%%", i * 100 / n);
      fflush(stderr);
    }
    ProcessImage(file, &(images[i]));

    fclose(file);
    free(fullName);
  }
  fprintf(stderr, "\r[+] Loading tiles into memory: 100%%\n");
}

// returns the distance between two pixels
float RedMean(pixel* p1, pixel* p2){
  float r, redmean, dR, dG, dB;

  dR = p1->red - p2->red;
  dG = p1->green - p2->green;
  dB = p1->blue - p2->blue;
  r = (p1->red + p2->red)/2.0;

  redmean = sqrt(
    (2.0 + r/256.0)* powf(dR, 2.0)
     + 4.0*powf(dG, 2.0) 
     + (2.0 + (255.0 -r)/256.0)*powf(dB, 2.0)
  );

  return redmean; 
}

// returns the average pixel of a semi-matrix
// padded by initI + inc and initJ + inc
pixel LocalAverage(long initI, long initJ, int inc, image* img){
  long i, j, num = 0;
  pixel averagePixel;
  InitializePixel(&averagePixel);
  if(!img->pixels) return averagePixel;

  float red, green, blue;
  red = green = blue = 0;

  // reminder: width is for columns and
  // height is for rows

  for(i = initI; i < img->dimensions[1] && i < initI + inc; i++)
    for(j = initJ; j < img->dimensions[0] && j < initJ + inc; j++){
      red += powf(img->pixels[i][j].red, 2.0); 
      green += powf(img->pixels[i][j].green, 2.0); 
      blue += powf(img->pixels[i][j].blue, 2.0);
      num++;
    }

  averagePixel.red = round(sqrtf(red/num));
  averagePixel.green = round(sqrtf(green/num));
  averagePixel.blue =  round(sqrtf(blue/num));

  return averagePixel;
}

// searches for the closest average pixel in the image array
// returns the index of the image
long FindClosestImage(pixel* average, image* images, long n){
  long i, index;
  float minDistance, distance;

  index = 0;
  minDistance = RedMean(average, &images[0].averagePixel);
  for(i = 1; i < n; i++){
    distance = RedMean(average, &images[i].averagePixel);
    if(distance < minDistance){
      minDistance = distance;
      index = i;
    }
  }

  return index;
}

// copies a ppm tile ontop of a semi-matrix of the
// data matrix for the target mainImg 
void CopyMatrix(long initI, long initJ, int inc, image* mainImg, image* img){
  long i, j;
  long imgI, imgJ;

  // reminder: width é para as colunas e height é para as linhas
  for(i = initI, imgI = 0; i < mainImg->dimensions[1] && i < initI + inc && imgI < img->dimensions[1]; i++){
    for(j = initJ, imgJ = 0; j < mainImg->dimensions[0] && j < initJ + inc && imgJ < img->dimensions[0]; j++){
      mainImg->pixels[i][j].red = img->pixels[imgI][imgJ].red;
      mainImg->pixels[i][j].green = img->pixels[imgI][imgJ].green;
      mainImg->pixels[i][j].blue = img->pixels[imgI][imgJ].blue;
      imgJ++;
    }
    imgI++;
  }
}

// structures the final image
void ComposeFinalImage(image* images, long n, image* mainImage){
  int inc; 
  long i, j, index;
  pixel localAverage; 

  // grabs the size of the tiles, assuming they are 
  // all the same size
  inc = images[0].dimensions[0];

  // iterates through mainImage in a semi-matrix fashion
  // defined by inc + i and inc + j
  for(i = 0; i < mainImage->dimensions[1]; i += inc){
    if(i % (mainImage->dimensions[1] / 10) == 0){
      fprintf(stderr, "\r[+] Copying tiles into target image: %ld%%", i * 100 / mainImage->dimensions[1]);
      fflush(stderr);
    }

    for(j = 0; j < mainImage->dimensions[0]; j+= inc){
      localAverage = LocalAverage(i, j, inc, mainImage);
      index = FindClosestImage(&localAverage, images, n);
      CopyMatrix(i, j, inc, mainImage, &images[index]);
    }
  }
  fprintf(stderr, "\r[+] Copying tiles into target image: 100%%\n");
  fflush(stderr);
}

// save all modifications done to mainImage into an output
void WriteFinalImage(image* mainImage, FILE** output){
  PrintHeader(mainImage, output);

  if(!strcmp(mainImage->type, "P3"))
    PrintPixels(mainImage, output);
  else if(!strcmp(mainImage->type, "P6")){
    fprintf(stderr, "\r[+] Writing target P6 output: 0%%");
    fflush(stderr);

    long blocks;
    blocks = fwrite(mainImage->pixels[0], sizeof(pixel), mainImage->dimensions[0] * mainImage->dimensions[1], (*output));

    if(blocks != mainImage->dimensions[0] * mainImage->dimensions[1]){
      fprintf(stderr, "falha ao executar fwrite em %s\n", __func__);
      exit(1);
    }

    fprintf(stderr, "\r[+] Writing target P6 output: 100%%\n");
    fflush(stderr);
  }
}

// frees all memory allocated by a single image struct
void FreeImageData(image* img){
  if(!img) return;
    
  if(img->pixels)
    FreeMatrix((void***)&img->pixels);
  if(img->file)
    FreeDirEntries(&img->file);
}

// frees all memory allocated by an struct image array
void FreeImagesData(image** images, long n){
  // checa se images já é NULL, ou seja, se ja tomou free
  if(!(*images)) return;

  // libera a memoria da matriz de dados e do dirent 
  // presentes em cada img do vetor images
  long i;
  for(i = 0; i < n; i++){
    FreeImageData(&(*images)[i]);
  }

  // da free e nulifica o ponteiro do vetor de struct image
  free(*images);
  *images = NULL;
}