/* made by Gabriel Lüders
   GRR20190172 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "utils.h"

// opens the file given by path in the mode = mode
// returns a file pointer for the given path
FILE* OpenFile(char* path, char* mode){
  FILE* file = fopen(path, mode);
    
  if(!file){
    fprintf(stderr, "Error opening %s\n", path);
    exit(0);
  }

  return file;
}

// allocates space and concatanates two strings
// in the order string1+string2
// returns the resulting concatanation
char* AppendString(char* str1, char* str2){
  int size = strlen(str1) + strlen(str2) + 1;
  int i, j;

  // allocates space for the result string
  char* str = malloc(size * sizeof(char));
  if(!str)
      FireMemErrorException(__func__);
  
  // walts through str1 until \0
  // and copies it's content to the
  // resulting string
  for(i = 0; str1[i]; i++)
    str[i] = str1[i];

  // walts through str2 until \0
  // and copies it's content to the
  // resulting string
  for(j = 0; str2[j]; i++, j++)
    str[i] = str2[j];
  str[i] = '\0';

  return str;
}

// adds a \0 in the last 'c' found in the string
void FilterString(const char* str, char c){
  if(!str || !strlen(str)) return;

  char* p;
  p = strrchr(str, c); 
  if(p)   
    p[0] = '\0';
}

// checks the command line arguments and set default 
// values when needed
void ParseArgs(int argc, char** argv, const char *args, FILE** valueI, FILE** valueO, char** valueP){  
  opterr = 0;
  *valueI = stdin;
  *valueO = stdout;
  *valueP = AppendString("", "./tiles/");

  int option = getopt(argc, argv, args);

  if(option == -1){
    fprintf(stderr, "No argument was provided\n");
    exit(1);
  }

  while(option != -1){
    switch (option){
      case 'i':      
        *valueI = OpenFile(optarg, "r");
        break;
      case 'o':
        FilterString(optarg, '.');      
        char* outputName = AppendString(optarg, ".ppm");
        *valueO = OpenFile(outputName, "w");
        free(outputName);
        break;
      case 'p':      
        free(*valueP);
        *valueP = optarg;
        // this guarantees that the folder name will have a '/' at the end
        if(((*valueP)[strlen(*valueP) - 1] == '/'))
            (*valueP)[strlen(*valueP) - 1] = '\0';
        *valueP = AppendString(*valueP, "/");
        break;
      case 'h':
        fprintf(
          stderr, 
          "This program builds a mosaic from a given image using a collection of square tiles.\n"
        );
      default:
        fprintf (
          stderr, 
          "Usage: %s -i <input_file.ppm> -o <output_file.ppm> -p <tiles_folder/>\n", 
         argv[0]
        );
        fprintf(stderr, "default value for tiles_folder = ./tiles\n");
        exit (1) ;
    }

    option = getopt(argc, argv, args);
  }

  // checks if the dir passed exists
  DIR* dir = opendir(*valueP);
  if(!dir){
    fprintf(stderr, "Error opening dir %s\n", *valueP);
    exit(1);
  }
  closedir(dir);
  fprintf(stderr, "\n[+] Parsing command line arguments\n");
}

// throws a generic error
void FireErrorException(const char* funcName){
  fprintf(stderr, "Error at function %s\n", funcName);
  fprintf(stderr, "For your safety, program was aborted\n");
  exit(1);
}

// thows a memory error 
void FireMemErrorException(const char* funcName){
  fprintf(stderr, "Error alocating memory at %s\n", funcName);
  fprintf(stderr, "For your safety, program was aborted\n");
  exit(1);
}