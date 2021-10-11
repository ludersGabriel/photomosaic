/* Feito Por Gabriel Lüders
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
    fprintf(stderr, "Erro ao abrir %s\n", path);
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
  fprintf(stderr, "[+] Parsing command line arguments\n");
  
  int option;
  int flagI, flagO, flagP;

  flagI = flagO = flagP = 0;
  opterr = 0;

  for(; (option = getopt(argc, argv, args)) != -1; ){
    switch (option){
      case 'i':      
        flagI = 1;
        *valueI = OpenFile(optarg, "r");
        break;
      case 'o':      
        flagO = 1;
        *valueO = OpenFile(optarg, "w");
        break;
      case 'p':      
        flagP = 1;
        *valueP = optarg;
        // garantir consistencia entre dir entrados com '/' no fim e 
        // dirs entrados sem '/' no fim
        if(((*valueP)[strlen(*valueP) - 1] == '/'))
            (*valueP)[strlen(*valueP) - 1] = '\0';
        *valueP = AppendString(*valueP, "/");
        break;
      default:
        fprintf (stderr, "Usage: %s -a -b -c value\n", argv[0]);
        exit (1) ;
    }
  }

  if(!flagI) *valueI = stdin;
  if(!flagO) *valueO = stdout;
  if(!flagP) *valueP = AppendString("", "./tiles/");

  // checa se o dir passado existe
  DIR* dir = opendir(*valueP);
  if(!dir){
    fprintf(stderr, "Erro ao abrir o dir %s\n", *valueP);
    exit(1);
  }
  closedir(dir);
}

// throws a generic error
void FireErrorException(const char* funcName){
  fprintf(stderr, "Erro na função %s\n", funcName);
  fprintf(stderr, "Para sua segurança, o programa foi abortado\n");
  exit(1);
}

// thows a memory error 
void FireMemErrorException(const char* funcName){
  fprintf(stderr, "Erro ao alocar memoria em %s\n", funcName);
  fprintf(stderr, "Para sua segurança, o programa foi abortado\n");
  exit(1);
}