/* Feito Por Gabriel Lüders
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
    ParseArgs(argc, argv, "i:o:p:", &input, &output, &dir);
    images = GetImages(dir, &n);

    // Processes the tiles and the input image
    fprintf(stderr, "[+] Processing tiles\n");
    ProcessImages(dir, images, n);
    fprintf(stderr, "[+] Initializing target image\n");
    InitializeImage(&target);
    fprintf(stderr, "[+] Processing target image\n");
    ProcessImage(input, &target);

    // Structures and writes the final image in output
    fprintf(stderr, "[+] Composing final image\n");
    ComposeFinalImage(images, n, &target);
    fprintf(stderr, "[+] Writing final image to output\n");
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