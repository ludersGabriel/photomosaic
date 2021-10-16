/* Made by Gabriel Lüders
   GRR20190172 */

#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "utils.h"

int main(int argc, char** argv){
    FILE *input = NULL;
    FILE *output = NULL;
    image target;
    image *images;
    long n;
    char *dir = NULL;

    // parses the argumentents and loads the 
    // tile array from dir into images
    ParseArgs(argc, argv, "i:o:p:h", &input, &output, &dir);
    images = GetImages(dir, &n);

    // Processes the tiles and the input image
    ProcessImages(dir, images, n);
    InitializeImage(&target);
    ProcessImage(input, &target);

    // Structures and writes the final image in output
    ComposeFinalImage(images, n, &target);
    WriteFinalImage(&target, &output);

    // Frees all memory used by the program
    fprintf(stderr, "[+] Freeing all allocated memory\n");
    FreeImageData(&target);
    FreeImagesData(&images, n);
    free(dir);
    fclose(input);
    fclose(output);

    return 0;
}